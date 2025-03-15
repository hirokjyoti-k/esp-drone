#include <Wire.h>

#define MPU6050_ADDR 0x68  // Default I2C address
#define PWR_MGMT_1 0x6B    // Power management register
#define ACCEL_XOUT_H 0x3B  // Accelerometer data register
#define GYRO_XOUT_H 0x43   // Gyroscope data register

void setup() {
    Serial.begin(115200);
    Wire.begin(); // Start I2C communication

    // Wake up MPU6050
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(PWR_MGMT_1);
    Wire.write(0x00);  // Set to 0 (disable sleep mode)
    Wire.endTransmission();
    
    Serial.println("MPU6050 Initialized!");
}

void loop() {
    int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ;

    // Read accelerometer and gyroscope data
    accelX = readMPU6050(ACCEL_XOUT_H);
    accelY = readMPU6050(ACCEL_XOUT_H + 2);
    accelZ = readMPU6050(ACCEL_XOUT_H + 4);
    gyroX = readMPU6050(GYRO_XOUT_H);
    gyroY = readMPU6050(GYRO_XOUT_H + 2);
    gyroZ = readMPU6050(GYRO_XOUT_H + 4);

    // Print raw values
    Serial.print("Accel X: "); Serial.print(accelX);
    Serial.print(" | Y: "); Serial.print(accelY);
    Serial.print(" | Z: "); Serial.println(accelZ);

    Serial.print("Gyro X: "); Serial.print(gyroX);
    Serial.print(" | Y: "); Serial.print(gyroY);
    Serial.print(" | Z: "); Serial.println(gyroZ);

    Serial.println("-----------------------------");
    delay(500);
}

// Function to read 16-bit values from MPU6050
int16_t readMPU6050(uint8_t reg) {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false); // Restart condition
    Wire.requestFrom(MPU6050_ADDR, 2, true);

    int16_t value = Wire.read() << 8 | Wire.read(); // Merge high and low byte
    return value;
}
