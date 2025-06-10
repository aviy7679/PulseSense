#include "MPU6050Sensor.h"

MPU6050Sensor::MPU6050Sensor(){}

bool MPU6050Sensor::begin() {
  if(!mpu.begin()) return false;
    //רמת הדיוק של טווח מדידת התאוצה
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  //טווח מדידה של הג'יירוסקופ - 
  //כמה מעלות לשניה יוכל החיישן לקלוט
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  //מהירות התגובה וסינון רעשים - מסנן תדרים מעל 5 הרץ
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  return true;
}

void MPU6050Sensor::update(){
  mpu.getEvent(&accel, &gyro, &temp);
}

sensors_event_t MPU6050Sensor::getAccel(){
  return accel;
}

sensors_event_t MPU6050Sensor::getGyro(){
  return gyro;
}

float MPU6050Sensor::getTemperature(){
  return temp.temperature;
}

bool MPU6050Sensor::detectFall(float threshold) {
  float totalAccel = sqrt(
    accel.acceleration.x * accel.acceleration.x +
    accel.acceleration.y * accel.acceleration.y +
    accel.acceleration.z * accel.acceleration.z
  );

  return totalAccel > threshold;
}

bool MPU6050Sensor::detectNoMotion(unsigned long duration) {
  float totalAccel = sqrt(
    accel.acceleration.x * accel.acceleration.x +
    accel.acceleration.y * accel.acceleration.y +
    accel.acceleration.z * accel.acceleration.z
  );

  if (totalAccel > 10) {
    lastMovementTime = millis();
    return false;
  }

  return (millis() - lastMovementTime) > duration;
}

float MPU6050Sensor::getMotionMagnitude() {
  return sqrt(
    accel.acceleration.x * accel.acceleration.x +
    accel.acceleration.y * accel.acceleration.y +
    accel.acceleration.z * accel.acceleration.z
  );
}

