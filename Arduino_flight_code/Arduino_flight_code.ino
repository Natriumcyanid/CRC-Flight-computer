/*
  This code is being designed to run
  a rocket flight computer.

  It uses an Arduino Nano 33 IoT.

  created 24 May 2022
  by Jakub Kulhavý
*/

#include <Arduino_LSM6DS3.h>
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;
float accErrorX, accErrorY, gyroErrorX, gyroErrorY, gyroErrorZ;
int i = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(19200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    while (1);
  }

calculate_IMU_error(); // Device has to stay still
delay(20);
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(accX, accY, accZ); // Get current acceleration in G's in all axes

    // Calculate angles of two axes from acceleration
    accAngleX = (atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI) - accErrorX;
    accAngleY = (atan(-1 * accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI) - accErrorY;
    
  }
  if (IMU.gyroscopeAvailable()) {
    // Get elapsed time since last measurement in order to integrate
    // degrees per second to get current angular position
    previousTime = currentTime;
    currentTime = millis();
    elapsedTime = (currentTime - previousTime) / 1000;
    IMU.readGyroscope(gyroX, gyroY, gyroZ); // Get current angular speed in all axes

    gyroAngleX = gyroAngleX + (gyroX - gyroErrorX) * elapsedTime;
    gyroAngleY = gyroAngleY + (gyroY -gyroErrorY ) * elapsedTime;
    yaw = yaw + (gyroZ - gyroErrorZ) * elapsedTime;
    
// Complementary filter
    roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
    pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
    
  }
  // Test if rocket is tiping over
  if (abs(yaw) > 90 || abs(pitch) > 90) {
    digitalWrite(PIN_LED, HIGH);
    recovery_system();
  } else if (abs(yaw) < 90 || abs(pitch) < 90) {
    digitalWrite(PIN_LED, LOW);
  }
  // Used for external serial communication
  Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);
}

// Triggered if rocket tipover is detected
void recovery_system() {
  for (i = 0; i < 10; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
  }
  
  while (1) {
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
    delay(500);
    }
}

void calculate_IMU_error() {
  // Calculating error of measurements in order to filter
  // them out later. The IMU error is constant, so that way,
  // it can be completely filtered out.

  // Get 200 measurements from the accelerator and sum them up
  while (i < 200) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(accX, accY, accZ);
      accErrorX = accErrorX + ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
      accErrorY = accErrorY + ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
      i++;
    }
  }

  // Divide by 200 to get error value
  accErrorX = accErrorX / 200;
  accErrorY = accErrorY / 200;
  i = 0;

// Get 200 measurements from the gyroscope and sum them up
  while (i < 200) {
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gyroX, gyroY, gyroZ);
      gyroErrorX = gyroErrorX + gyroX;
      gyroErrorY = gyroErrorY + gyroY;
      gyroErrorZ = gyroErrorZ + gyroZ;
      i++;
    }
  }

  // Divide by 200 to get error value
  gyroErrorX = gyroErrorX / 200;
  gyroErrorY = gyroErrorY / 200;
  gyroErrorZ = gyroErrorZ / 200;
}
