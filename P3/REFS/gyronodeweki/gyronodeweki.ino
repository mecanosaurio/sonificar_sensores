/*
* send gyro and accel data via udp
*/


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define PIXN 12
#define PIXPIN 2
Adafruit_NeoPixel pixels(PIXN, PIXPIN, NEO_GRB + NEO_KHZ800);

char ssid[] = "000_000";          // your network SSID (name)
char pass[] = "elbichojake"; 

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192,168,1,66);        // remote IP of your computer
const unsigned int outPort = 6448;          // remote port to receive OSC
const unsigned int localPort = 2390;        // local port to listen for OSC packets (not used/tested)


// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
uint16_t cR, cG, cB;



void setup() {
  Serial.begin(115200);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

  // other hardware
  Wire.begin(sda, scl);
  MPU6050_Init();

  // pixels
  pixels.begin();
  pixels.clear();

  for (int i=0; i<4; i++){ pixels.setPixelColor(i, pixels.Color(255,0,0)); }
  pixels.show();
  delay(400);
  for (int i=0; i<4; i++){ pixels.setPixelColor(i, pixels.Color(0,0,0)); }
  pixels.show();
  delay(100);

  for (int i=4; i<12; i++){ pixels.setPixelColor(i, pixels.Color(0,255,0)); }
  pixels.show();
  delay(400);
  for (int i=4; i<12; i++){ pixels.setPixelColor(i, pixels.Color(0,0,0)); }
  pixels.show();
  delay(100);

  // wait
  delay(500);
}




void loop() {
  double Ax, Ay, Az, T, Gx, Gy, Gz;
  
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
  Az = (double)AccelZ/AccelScaleFactor;
  T = (double)Temperature/340+36.53; //temperature formula
  Gx = (double)GyroX/GyroScaleFactor;
  Gy = (double)GyroY/GyroScaleFactor;
  Gz = (double)GyroZ/GyroScaleFactor;

  Serial.print("Ax: "); Serial.print(Ax);
  Serial.print(" Ay: "); Serial.print(Ay);
  Serial.print(" Az: "); Serial.print(Az);
  Serial.print(" T: "); Serial.print(T);
  Serial.print(" Gx: "); Serial.print(Gx);
  Serial.print(" Gy: "); Serial.print(Gy);
  Serial.print(" Gz: "); Serial.println(Gz);


  float ro=0.6;
  float nGx = lerp(nGx, float(Gx), ro);
  float nGy = lerp(nGy, float(Gy), ro);
  float nGz = lerp(nGz, float(Gz), ro);

  //set lights
  if (Gx>=5){cR = floor(map(nGx, 2, 100, 0, 255));} 
  else if (Gx<=-5){ cR = floor(map(nGx, -2, -100, 0, 255)); }
  else {cR = 0;}
  if (Gy>5){ cG = floor(map(nGy, 2, 100, 0, 255));}
  else if (Gy<-5){ cG = floor(map(nGy, -2, -100, 0, 255)); }
  else {cG = 0;}
  if (Gz>5){ cB = floor(map(nGz, 2, 100, 0, 255)); }
  else if (Gz<-7){ cB = floor(map(nGz, -4, -100, 0, 255)); }
  else {cB = 0;}
 
  Serial.print(" R: "); Serial.print(cR);
  Serial.print(" G: "); Serial.print(cG);
  Serial.print(" B: "); Serial.println(cB);
  

  for (int i=0; i<12; i++){ pixels.setPixelColor(i, pixels.Color(cR,cG,cB)); }
  pixels.show();

  // send oscs
  OSCMessage msg("/node/data");
  msg.add(float(Gx));
  msg.add(float(Gy));
  msg.add(float(Gz));  
  msg.add(float(Ax));
  msg.add(float(Ay));
  msg.add(float(Az));  
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  delay(100);
}





void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}


float lerp(float a, float b, float x){
  return a + x * (b - a); 
}
