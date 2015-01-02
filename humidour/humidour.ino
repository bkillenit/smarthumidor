#include <Servo.h>
#include "DHT.h"
#include <SPI.h>
#include <Ethernet.h>
#define DHTTYPE DHT11   // DHT 11 sensor type 
#define DHTPIN 2 

Servo myservo;  // create servo object to control a servo 
int buttonClicked;
boolean lidOpen;
int degreesRotation = 22; //sets the initial degrees of rotation for the servo to begin at
int delayHumiditySensor;
float hi;

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 0, 20); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

//initialize connected pins for I/O  
int buttonPin = 7;
int DHTPin = 2;
int servoPin = 9;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  
  delayHumiditySensor++;
  
  myservo.attach(servoPin);  // attaches the servo on pin 11 to the servo object 
  myservo.write(degreesRotation);
  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);
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
  
  // constructing a blank array and passing in a pointer to the first address to the readWeatherData method
  float weatherData[4] = {};
  readWeatherData( weatherData );
    
//  if(*weatherData != NULL) {
//    Serial.println("'{\"weather\": { \"temp\": \"");
//    Serial.print(weatherData[0]);
//    Serial.print("\", \"tempf\": \"");
//    Serial.print(weatherData[1]);
//    Serial.print("\", \"humidity\": \"");
//    Serial.print(weatherData[2]);
//    Serial.print("\", \"heatindex\": \"");
//    Serial.print(weatherData[3]);
//    Serial.print("\"} }'");
//  }
  Serial.println("weather Data outside of scope is");
  Serial.println(weatherData[2]);
    
  // listen for incoming clients
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
//          // attempts to read the weather data from the sensor until it returns valid data
//          float *weatherData = readWeatherData();
//          
//          while(weatherData == NULL) {
//            weatherData = readWeatherData;
//          }
//          
//          client.println("{'weather': { 'temp': ");
//          client.println(weatherData[0]);
//          client.println(", tempf: ");
//          client.println(weatherData[1]);
//          client.println(", humidity: ");
//          client.println(weatherData[2]);
//          client.println(", heatindex: ");
//          client.println(weatherData[3]);
//          client.println("} }");
//            
//          if(weatherData) {
//            free(weatherData);
//          }
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
