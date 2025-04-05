#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// MPU6050 object
Adafruit_MPU6050 mpu;

// ESC pins for NodeMCU
#define ESC1 D5
#define ESC2 D6
#define ESC3 D7
#define ESC4 D8

int throttle = 1200; // base throttle (1000-2000)

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // SDA = D2, SCL = D1 (NodeMCU)

  // Setup ESC pins
  pinMode(ESC1, OUTPUT);
  pinMode(ESC2, OUTPUT);
  pinMode(ESC3, OUTPUT);
  pinMode(ESC4, OUTPUT);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1) delay(10);
  }
  Serial.println("MPU6050 initialized!");

  // Configurations
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  delay(100);

  // ESC calibration (send min throttle)
  for (int i = 0; i < 4; i++) {
    analogWrite(ESC1, map(1000, 1000, 2000, 0, 1023));
    analogWrite(ESC2, map(1000, 1000, 2000, 0, 1023));
    analogWrite(ESC3, map(1000, 1000, 2000, 0, 1023));
    analogWrite(ESC4, map(1000, 1000, 2000, 0, 1023));
    delay(500);
  }
}

void loop() {
  // Get sensor events
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Use gyroscope data (rotation) for basic stabilization
  float rotX = gyro.gyro.x * 57.2958; // convert rad/s to deg/s
  float rotY = gyro.gyro.y * 57.2958;
  float rotZ = gyro.gyro.z * 57.2958;

  // Basic flight stabilization math
  int esc1_speed = throttle + rotX - rotY + rotZ;
  int esc2_speed = throttle - rotX - rotY - rotZ;
  int esc3_speed = throttle - rotX + rotY + rotZ;
  int esc4_speed = throttle + rotX + rotY - rotZ;

  // Clamp values to 1000-2000
  esc1_speed = constrain(esc1_speed, 1000, 2000);
  esc2_speed = constrain(esc2_speed, 1000, 2000);
  esc3_speed = constrain(esc3_speed, 1000, 2000);
  esc4_speed = constrain(esc4_speed, 1000, 2000);

  // Convert to 0-1023 for ESP8266 PWM
  analogWrite(ESC1, map(esc1_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC2, map(esc2_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC3, map(esc3_speed, 1000, 2000, 0, 1023));
  analogWrite(ESC4, map(esc4_speed, 1000, 2000, 0, 1023));

  // Debug output
  Serial.print("Roll: "); Serial.print(rotX);
  Serial.print(" | Pitch: "); Serial.print(rotY);
  Serial.print(" | Yaw: "); Serial.print(rotZ);
  Serial.print(" | ESC1: "); Serial.print(esc1_speed);
  Serial.print(" | ESC2: "); Serial.print(esc2_speed);
  Serial.print(" | ESC3: "); Serial.print(esc3_speed);
  Serial.print(" | ESC4: "); Serial.println(esc4_speed);

  delay(20); // 50 Hz update rate
}
