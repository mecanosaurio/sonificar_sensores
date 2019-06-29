import processing.serial.*;

int bgcolor;
int fgcolor;
Serial myPort;
int serialCount = 0;
byte[] dataIn = new byte[8];
int dSensA0, dSensA1, dSensA2;
float sA0,sA1, sA2;

int cX, cY;

void setup() {
  size(600, 400);
  background(bgcolor);
  frameRate(24);
  stroke(255);
  printArray(Serial.list());
  String portName = Serial.list()[4];
  myPort = new Serial(this, portName, 9600);
  cX = 0; cY =0;
  sA0 = 0;
  sA1 = 0;
  sA2 = 0;
}

void draw() {
  if (checkForNewData()){
    stroke(0);
    strokeWeight(5);
    line(cX+2, 0, cX+2, height);
    stroke(0,255,0, 200);
    strokeWeight(1);
    line(cX+1, 0, cX+1, height);

    stroke(255);
    strokeWeight(2);
    cY = 100 + int(constrain(map(sA0,0, 32, 10, 100),0, 128)*noise(millis()/100,1)*sin(10));
    point(cX, cY);
    cY = 200 + int(constrain(map(sA1,0, 32, 10, 100),0, 128)*noise(millis()/100,2)*sin(5));
    point(cX, cY);
    cY = 300 - int(constrain(map(sA2,0, 32, 10, 100),0, 128)*noise(millis()/100,3)*sin(3));
    point(cX, cY);
    
    cX=cX<width?cX+1:0;    
  }

}





boolean checkForNewData() {
  boolean isNew=false;
  while (myPort.available()>0){
    myPort.readBytes(dataIn);
    dSensA0 = 0;
    dSensA1 = 0;
    dSensA2 = 0;
    dSensA0 = ((dataIn[1] & 0xff) << 8) | (dataIn[0] & 0xff);
    dSensA1 = ((dataIn[3] & 0xff) << 8) | (dataIn[2] & 0xff);
    dSensA2 = ((dataIn[5] & 0xff) << 8) | (dataIn[4] & 0xff);
    sA0 = lerp(sA0, dSensA0, 0.78);
    sA1 = lerp(sA1, dSensA1, 0.75);
    sA2 = lerp(sA2, dSensA2, 0.75);
    myPort.clear();
    if (dataIn!=null){
      print(dSensA0); print('\t');
      print(dSensA1); print('\t');
      println(dSensA2); 
      isNew = true;
    }
  }
  return isNew;
}
