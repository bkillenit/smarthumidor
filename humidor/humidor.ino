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
float desiredHumidity = 72;
const int humidityTolerance = 5;

float hi;

// Initialization of internet configuration constants
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
EthernetServer server(80);  // create a server at port 80
EthernetClient client;
String callback = "ArduinoWeatherDataCallback";

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

// variable to track the pwm amount written to the pump pin. keeps from 
// continuously writing the same pwm value to the pin in the loop method
// when adjusting the humidity in the box. initialized to the lowVoltage value
int currPumpVoltage = lowVoltage;

void setup() {
  Serial.begin(9600);
  
  Serial.begin(9600);
  pinMode(SDCARD_CS,OUTPUT);
  
  // deselect the SD card, required for the pins from the ethernet shield to 
  // communicate properly with base board in the absence of an SD card
  digitalWrite(SDCARD_CS,HIGH);
  
  // start the Ethernet connection and ensure that the Arduino correctly obtains
  // and IP address. If not, skip setting up the server and move on to the 
  // rest of the setup for pin initialization
  if ( Ethernet.begin(mac) == 0 ) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  
  // start to listen for clients once the web server is running and configured
  server.begin(); 
  
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  } 
  Serial.print("\n");
  
  delayHumiditySensor++;
  
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  myservo.write(degreesRotation);
  pinMode(buttonPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  analogWrite(pumpPin, lowVoltage);
  
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
  // add conditional for handling breaking of the while loop after certain time
  // consider running this concurrently ( if there is a way )  
  if( currentHumidity < (desiredHumidity - humidityTolerance) ) {
      // the humidity is low and thus the pump must pump faster
      // set the voltage supplied to the pump to our high voltage if
      // the voltage currently being supplied is low
      if( currPumpVoltage == lowVoltage) {
        analogWrite(pumpPin, highVoltage);
        currPumpVoltage = highVoltage;
      }
  } else if( currentHumidity >= (desiredHumidity + humidityTolerance) ) {
      // the humidity is just right, set the pump to its cruising flow rate
      // set the voltage supplied to the pump to our low voltage if
      // the voltage currently being supplied is high
      if( currPumpVoltage == highVoltage) {
        analogWrite(pumpPin, lowVoltage);
        currPumpVoltage = lowVoltage;
      }
  }
        
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          respondJSON( client );
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
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

void respondJSON(EthernetClient client) {
   // Send a standard http response header.
   client.println("Content-Type: application/json");
   client.println("Connnection: close");
   client.println();
    
    // Respond with data in JSONP format
    client.println(callback + "({ \"weather\": {");
    client.println("  \"temperature\": {");
    client.print("    \"celcius\": ");
    client.print(weatherData[0]);
    client.println(",");
    client.print("    \"fahrenheit\": ");
    client.print(weatherData[1]);
    client.println(",");
    client.print("    \"heatIndex\": ");
    client.println(weatherData[3]);
    client.println("  }, ");
    client.print(" \"humidity\": ");
    client.println(weatherData[2]);
    client.println("} });");
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
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float newHi = dht.computeHeatIndex(answer[1], answer[2]);
  
  if ((int) hi != (int) newHi) {
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
