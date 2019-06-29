#include "DHT.h"

#define DHTTYPE DHT22

DHT dht (2, DHTTYPE);
float hum, temp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
  //makeContact();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(3000);
  //if(Serial.available()>0){
    //int inByte = Serial.read();
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    //Serial.print("Humedad: ");
    //Serial.print(hum);
    sendFloat(hum);
    //Serial.print("  Temperatura: ");
    //Serial.print(',');
    //Serial.print(tem);
    sendFloat(temp);
    Serial.flush();
}


void makeContact(){
  while(Serial.available()<=0){
    Serial.print('A');
    delay(300);
  }
}


void sendFloat(float f){
  byte * b = (byte *) &f;
  Serial.write(b, 4);
  }
