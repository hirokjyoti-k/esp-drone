#include <Wire.h>

// MPU6050 Data Variables
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

// ESC PWM output pins for NodeMCU (D5, D6, D7, D8)
#define ESC1 D5
#define ESC2 D6
#define ESC3 D7
#define ESC4 D8

int throttle = 1200;  // Base throttle (PWM range: 1000-2000)

// Setup MPU6050
void setupMPU() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1B); Wire.write(0x00); Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x1C); Wire.write(0x00); Wire.endTransmission();
}

// Read Accelerometer Data
void recordAccelRegisters() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  while (Wire.available() < 6);
  accelX = Wire.read() << 8 | Wire.read();
  accelY = Wire.read() << 8 | Wire.read();
  accelZ = Wire.read() << 8 | Wire.read();
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0;
  gForceZ = accelZ / 16384.0;
}

// Read Gyroscope Data
void recordGyroRegisters() {
  Wire.beginTransmission(0x68);
  Wire.write(0x43); Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  while (Wire.available() < 6);
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0;
  rotZ = gyroZ / 131.0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // SDA = D2, SCL = D1

  pinMode(ESC1, OUTPUT);
  pinMode(ESC2, OUTPUT);
  pinMode(ESC3, OUTPUT);
  pinMode(ESC4, OUTPUT);

  setupMPU();

  // ESC Calibration: Send minimum throttle
  analogWrite(ESC1, map(1000, 1000, 2000, 0, 1023));
  analogWrite(ESC2, map(1000, 1000, 2000, 0, 1023));
  analogWrite(ESC3, map(1000, 1000, 2000, 0, 1023));
  analogWrite(ESC4, map(1000, 1000, 2000, 0, 1023));
  delay(2000);
}

void loop() {
  recordAccelRegisters();
  recordGyroRegisters();

  // Basic Flight Stabilization
  int esc1_speed = throttle + rotX - rotY + rotZ;
  int esc2_speed = throttle - rotX - rotY - rotZ;
  int esc3_speed = throttle - rotX + rotY + rotZ;
  int esc4_speed = throttle + rotX + rotY - rotZ;

  // Limit motor speeds between 1000-2000 µs
  esc1_speed = constrain(esc1_speed, 1000, 2000);
  esc2_speed = constrain(esc2_speed, 1000, 2000);
  esc3_speed = constrain(esc3_speed, 1000, 2000);
  esc4_speed = constrain(esc4_speed, 1000, 2000);

  // Convert to ESP8266 PWM scale (0-1023)
  analogWrite(ESC1, map(esc1_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC2, map(esc2_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC3, map(esc3_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC4, map(esc4_speed, 1000, 2000, 0, 1023));

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
