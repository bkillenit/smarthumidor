#include <Servo.h> 
#include "DHT.h"
#define DHTTYPE DHT11   // DHT 11 sensor type 
#define DHTPIN 2 
 
Servo myservo;  // create servo object to control a servo 
int buttonClicked;
boolean lidOpen;
int degreesRotation = 22; //sets the initial degrees of rotation for the servo to begin at
int delayHumiditySensor;
float hi = 0.0;

//initialize connected pins for I/O  
int buttonPin = 7;
int DHTPin = 2;
int servoPin = 11;

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{ 
  delayHumiditySensor++;
  
  myservo.attach(servoPin);  // attaches the servo on pin 11 to the servo object 
  myservo.write(degreesRotation);
  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
} 
 
void loop() 
{ 
  buttonClicked = digitalRead(buttonPin);
  if (buttonClicked == HIGH) {
    if(lidOpen == true) {
      closeLid();
    } else {
      openLid();
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
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          // attempts to read the weather data from the sensor until it returns valid data
          float weatherData[4] = {0};
          
          while(wewatherData[3] = 0) {
            weatherData = readWeatherData;
          }
          
          client.println("<cke:html><cke:body bgcolor=#000000 text=#FFFFFF>DARK!</cke:body></cke:html>");

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
      degreesRotation--;
      delay(15);
   }
   lidOpen = false;
}
  
void openLid() {
    while( degreesRotation <= 132 ) {
      myservo.write(degreesRotation);
      degreesRotation++;
      delay(15);
   }
   lidOpen = true;
}

float[] readWeatherData() {
  float answer[4] = {0};
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Read temperature as Celsius
  answer[0] = dht.readTemperature();
  // Read temperature as Fahrenheit
  answer[1] = dht.readTemperature(true);
  // Read the humidity
  answer[2] = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float newHi = dht.computeHeatIndex(f, h);
  
  if (hi != newHi) {
      hi = newHi;
      answer[3] = hi
      
      Serial.print("Humidity: "); 
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: "); 
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hi);
      Serial.println(" *F");
   }
   
   return answer;
}
