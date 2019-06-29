/*
* 1. usar la biblioteca de comunicación serie de processing
* 2. parsear los datos que lleguen por el puerto serie 
* 3. realizar acciones con los datos recibidos
        - ej mostrarlo graficamente
        - ej guardarlo en un archivo
        - ej producir otro tipo de datos nuevos
*/


import processing.serial.*;

Serial thePort;
byte[] dataIn = new byte[30];
int serialCount = 0;
boolean isConnected = false;

PrintWriter archivo;

float t1 = 0;
float h1 = 0;
float t2 = 0;
float h2 = 0;
float t3 = 0;
float h3 = 0;

float cx1, cyt1, cyh1;
float cx2, cyt2, cyh2;
float cx3, cyt3, cyh3;
int dv1, dv2, dv3;
float cyv1, cyv2, cyv3;

float tt;

void setup(){
  size(1000, 700);
  background(0);
  //frameRate(12);
  
  String nomArch = year()+month()+day()+"_"+hour()+minute()+second()+".csv";
  archivo = createWriter(nomArch);
  archivo.println("temperatura1, humedad1, movimiento1, temperatura2, humedad2, movimiento2, temperatura3, humedad3, movimiento3");

  println(Serial.list());
  String portName = Serial.list()[1];
  thePort = new Serial(this, portName, 115200);
  
  h1 = 0;  h2 = 0;   h3 = 0;
  t1 = 0;  t2 = 0;   t3 = 0;
  cx1 = 0;  cx2 = 0;  cx3 = 0;
  cyt1 = 0;  cyt2 = 0;  cyt3 = 0; 
  cyh1 = 0;  cyh2 = 0;  cyh3 = 0;
  dv1 = 0; dv2 = 0; dv3 =0;
  cyv1 = 0; cyv2 = 0; cyv3 =0;
}

void draw(){
  tt = millis();
  // actualiza cy
  if (checkForNewData()){
    cyt1 = map(t1, 0, 50, 0, -100);
    cyt2 = map(t2, 0, 50, 0, -100);
    cyt3 = map(t3, 0, 50, 0, -100);
    cyh1 = -1*h1;
    cyh2 = -1*h2;
    cyh3 = -1*h3;
    cyv1 = constrain(map(dv1, 0, 64, 1, -100), -100, 1)*noise(tt/100);
    cyv2 = constrain(map(dv2, 0, 64, 1, -100), -100, 1)*noise(tt/250);
    cyv3 = constrain(map(dv3, 0, 64, 1, -100), -100, 1)*noise(tt/500);
    archivo.print(t1); archivo.print(',');
    archivo.print(h1); archivo.print(',');
    archivo.print(cyv1); archivo.print(',');
    archivo.print(t2); archivo.print(',');
    archivo.print(h2); archivo.print(',');
    archivo.print(cyv2); archivo.print(',');
    archivo.print(t3); archivo.print(',');
    archivo.print(h3); archivo.print(',');
    archivo.println(cyv3); 
  }
  // dibuja la gráfica
  stroke(0);
  strokeWeight(5);
  line(cx1+2, 0, cx1+2, height);
  stroke(0,0,255);
  strokeWeight(1);
  line(cx1+1, 0, cx1+1, height);
  
  stroke(255, 0, 0);
  strokeWeight(2);
  point(cx1, 200 + cyt1);
  point(cx2, 400 + cyt2);
  point(cx3, 600 + cyt3);
  stroke(0, 255, 0);
  point(cx1, 200 + cyh1);
  point(cx2, 400 + cyh2);
  point(cx3, 600 + cyh3);
  stroke(255);
  strokeWeight(1);
  //point(cx1, 200 + cyv1);
  //point(cx2, 400 + cyv2);
  //point(cx3, 600 + cyv3);

  // actualiza cx
  cx1=cx1<width?cx1+1:0;
  cx2=cx2<width?cx2+1:0;
  cx3=cx3<width?cx3+1:0;
} //<>//


void keyPressed(){
  if(key=='x'){
    archivo.flush();
    archivo.close();
    exit();
  }
  if(key==' '){
    saveFrame();
  }
}



boolean checkForNewData(){
  boolean isNew=false;
  while(thePort.available()>0){
    thePort.readBytes(dataIn);

    int intbit = 0;
    
    intbit = (dataIn[3] << 24) | ((dataIn[2] & 0xff) << 16) | ((dataIn[1] & 0xff) << 8) | (dataIn[0] & 0xff);
    t1 = Float.intBitsToFloat(intbit);
    intbit = (dataIn[7] << 24) | ((dataIn[6] & 0xff) << 16) | ((dataIn[5] & 0xff) << 8) | (dataIn[4] & 0xff);
    h1 = Float.intBitsToFloat(intbit);

    intbit = (dataIn[11] << 24) | ((dataIn[10] & 0xff) << 16) | ((dataIn[9] & 0xff) << 8) | (dataIn[8] & 0xff);
    t2 = Float.intBitsToFloat(intbit);
    intbit = (dataIn[15] << 24) | ((dataIn[14] & 0xff) << 16) | ((dataIn[13] & 0xff) << 8) | (dataIn[12] & 0xff);
    h2 = Float.intBitsToFloat(intbit);

    intbit = (dataIn[19] << 24) | ((dataIn[18] & 0xff) << 16) | ((dataIn[17] & 0xff) << 8) | (dataIn[16] & 0xff);
    t3 = Float.intBitsToFloat(intbit);
    intbit = (dataIn[23] << 24) | ((dataIn[22] & 0xff) << 16) | ((dataIn[21] & 0xff) << 8) | (dataIn[20] & 0xff);
    h3 = Float.intBitsToFloat(intbit);

    dv1 = ((dataIn[25] & 0xff)<<8 | dataIn[24] & 0xff);
    dv2 = ((dataIn[27] & 0xff)<<8 | dataIn[26] & 0xff);
    dv3 = ((dataIn[29] & 0xff)<<8 | dataIn[28] & 0xff);
    
    thePort.clear();
    if (dataIn!=null){
      String str = new String(dataIn);
      print(t1); print(',');print(h1); print('\t');
      print(t2); print(',');print(h2); print('\t');
      print(t3); print(',');println(h3);
      print(dv1); print(", "); print(dv2); print(", "); println(dv3);  //<>//

      isNew = true;
    }
  }
  return isNew;
}
