#include <Servo.h>
#include "DHT.h"
#include <SPI.h>
#include <EthernetV2_0.h> // custom ethernet library for the seeed eth shield v2
#define DHTTYPE DHT11   // DHT 11 sensor type 
#define DHTPIN 2 

Servo myservo;  // create servo object to control a servo 
int buttonClicked;
boolean lidOpen;
int degreesRotation = 22; //sets the initial degrees of rotation for the servo to begin at
int delayHumiditySensor;

// these are test values that will be dynamically set from the web in later versions
float desiredHumidity = 40;
const int humidityTolerance = 5;

float hi;

// Initialization of internet configuration constants
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
EthernetServer server(80);  // create a server at port 80
EthernetClient client;
#define W5200_CS  10
#define SDCARD_CS 4

//initialize connected pins for I/O
// Note: pins 10, 11, 12, and 13 are 
const int buttonPin = 7;
const int DHTPin = 2;
const int servoPin = 9;
const int pumpPin = 6;

// initializing the analog values for high and low voltages that control the air pump
// the numbers are merely test numbers and have not been calibrated yet
const int highVoltage = 250;
const int lowVoltage = 210;

DHT dht(DHTPIN, DHTTYPE);

// using an array instead of a more explicit struct because Arduino makes prototypes for functions before 
// it generates custom structs.This keeps us from passing structs in to functions.
// an explanation for this problem can be found at http://forum.arduino.cc/index.php?topic=41848.0
// index structure of the array is: [0] = tempC, [1] = tempF, [2] = humidityLevel, [3] = heat index
float weatherData[4] = {};

void setup() {
  Serial.begin(9600);
    
  // start the Ethernet connection:
//  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
//    // no point in carrying on, so do nothing forevermore:
//    for(;;)
//      ;
//  }
//  
//  // print your local IP address:
//  Serial.print("My IP address: ");
//  for (byte thisByte = 0; thisByte < 4; thisByte++) {
//    // print the value of each byte of the IP address:
//    Serial.print(Ethernet.localIP()[thisByte], DEC);
//    Serial.print("."); 
//  }
//  Serial.println();
  
  // start to listen for clients once the web server is running and configured
//  server.begin();   
  
  delayHumiditySensor++;
  
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  myservo.write(degreesRotation);
  pinMode(buttonPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  analogWrite(pumpPin, lowVoltage);
  
  Serial.println("DHTxx test!");
  dht.begin();
} 
 
void loop() {   
  buttonClicked = digitalRead(buttonPin);
  if (buttonClicked == HIGH) {
    if(lidOpen == true) {
      closeLid();
    } else {
      openLid();
    }
  }  
  
  readWeatherData( weatherData );
  
  float currentHumidity = weatherData[2];
  
  // if the current humidity is less than the desired humidity accounting for tolerance,
  // switch the motor pump's voltage to high resulting in higher air flow rate
  if( currentHumidity < (desiredHumidity - humidityTolerance) ) {
      analogWrite(pumpPin, highVoltage);
      while(currentHumidity != (desiredHumidity + humidityTolerance)) {
          ;
      }
      analogWrite(pumpPin, lowVoltage);
  }
        
//  // listen for incoming clients
//  EthernetClient client = server.available();
//  if (client) {
//    // an http request ends with a blank line
//    boolean currentLineIsBlank = true;
//    while (client.connected()) {
//      if (client.available()) {
//        char c = client.read();
//        // if you've gotten to the end of the line (received a newline
//        // character) and the line is blank, the http request has ended,
//        // so you can send a reply
//        if (c == '\n' && currentLineIsBlank) {
//          // send a standard http response header
//          client.println("HTTP/1.1 200 OK");
//          client.println("Content-Type: application/json");
//          client.println();
//          
//          client.println("<html><p>Hello World!</p></html>");  
//          
//          // constructing a blank array and passing in a pointer to the first address to the readWeatherData method
//          float weatherData[4] = {};
//          readWeatherData( weatherData );
//          
//          while(weatherData == NULL) {
//            readWeatherData( weatherData );          
//          }
//          
//          // respond to the JSON request using JSON-P extension
////          client.println("{'weather': { 'temp': ");
////          client.println(weatherData[0]);
////          client.println(", tempf: ");
////          client.println(weatherData[1]);
////          client.println(", humidity: ");
////          client.println(weatherData[2]);
////          client.println(", heatindex: ");
////          client.println(weatherData[3]);
////          client.println("} }");
//          
//          break;
//        }
//        if (c == '\n') {
//          // you're starting a new line
//          currentLineIsBlank = true;
//        } 
//        else if (c != '\r') {
//          // you've gotten a character on the current line
//          currentLineIsBlank = false;
//        }
//      }
//    }
//    // give the web browser time to receive the data
//    delay(1);
//    // close the connection:
//    client.stop();
//  }
} 

void closeLid() {
   while( degreesRotation >= 22 ) {
      myservo.write(degreesRotation);
      // rotating by 2 degrees with a longer time delay to smoothen out the rotation
      degreesRotation--;
      delay(15);
   }
   lidOpen = false;
}
  
void openLid() {
    while( degreesRotation <= 132 ) {
      myservo.write(degreesRotation);
      // rotating by 2 degrees with a longer time delay to smoothen out the rotation
      degreesRotation++;
      delay(15);
   }
   lidOpen = true;
}

void readWeatherData(float *answer) {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Read temperature as Celsius
  answer[0] = dht.readTemperature();
  // Read temperature as Fahrenheit
  answer[1] = dht.readTemperature(true);
  // Read the humidity
  answer[2] = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(answer[0]) || isnan(answer[1]) || isnan(answer[2])) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float newHi = dht.computeHeatIndex(answer[1], answer[2]);
  
  if (hi != newHi) {
      hi = newHi;
      answer[3] = hi;
      
      Serial.print("Humidity: "); 
      Serial.print(answer[2]);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(answer[0]);
      Serial.print(" *C ");
      Serial.print(answer[1]);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hi);
      Serial.println(" *F");
   }   
}
