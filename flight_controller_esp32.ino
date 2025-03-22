#include <Wire.h>
// #include <Servo.h>

// MPU6050 Data Variables
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

// ESC PWM output pins
// Servo esc1, esc2, esc3, esc4;
int throttle = 1200;  // Base throttle (1000-2000µs)

// Setup MPU6050
void setupMPU() {
  Wire.beginTransmission(0b1101000);
  Wire.write(0x6B); Wire.write(0b00000000); Wire.endTransmission();  
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1B); Wire.write(0b00000000); Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000);
  Wire.write(0x1C); Wire.write(0b00000000); Wire.endTransmission(); 
}

// Read Accelerometer Data
void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000);
  Wire.write(0x3B); Wire.endTransmission();
  Wire.requestFrom(0b1101000,6);
  while(Wire.available() < 6);
  accelX = Wire.read()<<8 | Wire.read();
  accelY = Wire.read()<<8 | Wire.read();
  accelZ = Wire.read()<<8 | Wire.read();
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

// Read Gyroscope Data
void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000);
  Wire.write(0x43); Wire.endTransmission();
  Wire.requestFrom(0b1101000,6);
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8 | Wire.read();
  gyroY = Wire.read()<<8 | Wire.read();
  gyroZ = Wire.read()<<8 | Wire.read();
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  setupMPU();

  // esc1.attach(5); esc2.attach(6);
  // esc3.attach(9); esc4.attach(10);

  // ESC Calibration: Send minimum throttle
  // esc1.writeMicroseconds(1000);
  // esc2.writeMicroseconds(1000);
  // esc3.writeMicroseconds(1000);
  // esc4.writeMicroseconds(1000);
  delay(2000);
}

void loop() {
  recordAccelRegisters();
  recordGyroRegisters();

  // // Basic Flight Stabilization
  int esc1_speed = throttle + rotX - rotY + rotZ;
  int esc2_speed = throttle - rotX - rotY - rotZ;
  int esc3_speed = throttle - rotX + rotY + rotZ;
  int esc4_speed = throttle + rotX + rotY - rotZ;

  // Limit motor speeds between 1000-2000 µs
  esc1_speed = constrain(esc1_speed, 1000, 2000);
  esc2_speed = constrain(esc2_speed, 1000, 2000);
  esc3_speed = constrain(esc3_speed, 1000, 2000);
  esc4_speed = constrain(esc4_speed, 1000, 2000);

  // // Send values to ESCs
  // esc1.writeMicroseconds(esc1_speed);
  // esc2.writeMicroseconds(esc2_speed);
  // esc3.writeMicroseconds(esc3_speed);
  // esc4.writeMicroseconds(esc4_speed);

  // Debugging
  Serial.print("Roll: "); Serial.print(rotX);
  Serial.print(" | Pitch: "); Serial.print(rotY);
  Serial.print(" | Yaw: "); Serial.print(rotZ);
  Serial.print(" | ESC1: "); Serial.print(esc1_speed);
  Serial.print(" | ESC2: "); Serial.print(esc2_speed);
  Serial.print(" | ESC3: "); Serial.print(esc3_speed);
  Serial.print(" | ESC4: "); Serial.println(esc4_speed);

  delay(20);  // 50Hz update rate
}
