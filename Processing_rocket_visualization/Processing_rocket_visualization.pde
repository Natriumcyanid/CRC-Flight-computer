/*
  This code was designed for 3D
  visualization of rocket orientation.
  
  created 24 May 2022
  by Jakub Kulhavý
*/
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;
Serial serialPort;
String data = "";
float rotate;
float roll, pitch, yaw;
PShape rocket;

public void setup() {
  size(960, 960, P3D);
  rocket = loadShape("rocket.obj");
  try {
    serialPort = new Serial(this, "/dev/ttyACM0", 19200);
  } catch (Throwable exc) {
    serialPort = new Serial(this, "/dev/ttyACM1", 19200);
  }
  serialPort.bufferUntil('\n');
}

public void draw() {
  background(255,255,255);
  fill(0);
  lights();
  textSize(22);
  textAlign(BOTTOM);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch)  + "     Yaw: " + int(yaw), 100, 265);
  translate(width/2, height/2 + 100, -200);
  scale(1.4);
  rotateX(-180);

  rotateX(radians(-pitch));
  rotateY(radians(yaw));
  rotateZ(radians(roll));
  
  rocket.setFill(color(255, 0, 255));
  shape(rocket);
}

public void serialEvent (Serial serialPort) {
  data = serialPort.readStringUntil('\n');
  if (data != null) {
    data = trim(data);
    String items[] = split(data, '/');
    if (items.length > 1) {
      roll = float(items[0]);
      pitch = float(items[1]);
      yaw = float(items[2]);
    }
  }
}
