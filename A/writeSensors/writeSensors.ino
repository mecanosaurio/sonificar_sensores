#include "DHT.h"

#define DHTTYPE DHT22

DHT dht1 (2, DHTTYPE);
DHT dht2 (3, DHTTYPE);
DHT dht3 (4, DHTTYPE);

float h1, t1;
float h2, t2;
float h3, t3;

uint16_t datoVib1, datoVib2, datoVib2;

long t0, t1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  dht3.begin();

  time0 = 0;
  time1 = 0;

  t1 = 0; h1 = 0;
  t2 = 0; h2 = 0;
  t3 = 0; h3 = 0;

  delay(2000);
}

void loop() {
  time1 = millis();

  if(time1-time0>3000){
    h1 = dht1.readHumidity();
    t1 = dht1.readTemperature();
    h2 = dht2.readHumidity();
    t2 = dht2.readTemperature();
    h3 = dht3.readHumidity();
    t3 = dht3.readTemperature();
    time0 = millis();
  } else {
    datoVib1 = analogRead(A0);
    datoVib2 = analogRead(A1);
    datoVib3 = analogRead(A2);
    sendFloat(t1);
    sendFloat(h1);
    sendFloat(t2);
    sendFloat(h2);
    sendFloat(t3);
    sendFloat(h3);
    sendInt(datoVib1);
    sendInt(datoVib2);
    sendInt(datoVib3);
    Serial.flush();  
    delay(50);
  }
}





void sendFloat(float f){
  byte * b = (byte *) &f;
  Serial.write(b, 4);
  }

void sendInt(uint16_t u){
  byte * b = (byte *) &u;
  Serial.write(b, 2);
  }
