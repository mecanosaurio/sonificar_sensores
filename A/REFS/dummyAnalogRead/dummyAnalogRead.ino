uint16_t dato0;
uint16_t dato1;
uint16_t dato2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  dato0 = analogRead(A0);
  dato1 = analogRead(A1);
  dato2 = analogRead(A2);
  sendInt(dato0);
  sendInt(dato1);
  sendInt(dato2);
  delay(50);
}


void sendInt(uint16_t u){
  byte * b = (byte *) &u;
  Serial.write(b, 2);
  }
