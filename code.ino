#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>

#define DHTPIN 5

DHT dht(DHTPIN, DHT11);

WiFiClient client;

long myChannelNumber = 2043087;
const char myWriteAPIKey[] = "DXUG8TG06C00D2Y3";


int measurePin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 4;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(ledPower,OUTPUT);
  
  WiFi.begin("Lyammmmmaaaa_2.4", "123@wueee@456");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  dht.begin();
  ThingSpeak.begin(client);
}

void loop() {
  // put your main code here, to run repeatedly:
  

  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);

  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
   float dustDensity = 50 * calcVoltage - 0.1;

  Serial.print("Current Dust Density Is: ");
  Serial.print(dustDensity); // unit: ug/m3
  Serial.print("ug/m3 \n");

  
  float h = 0.5 * dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("Temperature: " + (String) t);
  Serial.println("Humidity: " + (String) h);
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, dustDensity, myWriteAPIKey);

  
  delay(15000);
}
