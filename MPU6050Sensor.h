#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MPU6050Sensor {
  public:
    MPU6050Sensor();
    bool begin();
    void update();
    sensors_event_t getAccel();
    sensors_event_t getGyro();
    float getTemperature();
    float getMotionMagnitude();
    bool detectFall(float threshold);
    bool detectNoMotion(unsigned long duration);

  private:
    Adafruit_MPU6050 mpu;
    sensors_event_t accel, gyro, temp;
    unsigned long lastMovementTime = 0;
};

#endif
