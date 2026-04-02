#include <Arduino_LSM6DS3.h>
#include "Adafruit_VL53L0X.h"
#include <math.h>
#include <avr/dtostrf.h>

#define BUFF_SIZE_BIG 100
#define BUFF_SIZE_SMALL 20

// For NANO 33 IoT:
// - VIN to VIN
// - GND to GND
// - SDA to A4
// - SCL to A5

// gyro - x axis
// left - minus
// right - plus

// acc - y and z axis
// left - minus
// right - plus

Adafruit_VL53L0X sensor = Adafruit_VL53L0X();
float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, dt, acc_deg, acc_rad, gyro_deg, real_distance;
float angle_deg = 0.0;
float angle_rad = 0.0;
unsigned long current_time = 0;
unsigned long previous_time = 0;
uint16_t measured_distance;
const float alpha = 0.96;
VL53L0X_RangingMeasurementData_t measure;
char output[BUFF_SIZE_BIG];
char buffer_distance[BUFF_SIZE_SMALL];
char buffer_angle[BUFF_SIZE_SMALL];

void setup() 
{
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  if (!sensor.begin()) 
  {
    Serial.println("Failed to initialize ToF sensor!");
    while(1); 
  }
}

void loop()
{
  current_time = micros();

  // 50 Hz when dt = 20000
  // 1 Hz when dt = 1000000
  if (current_time - previous_time < 20000)
  {
    return;
  }

  dt = float((current_time - previous_time) / 1000000.0f);
  previous_time = current_time;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())  
  {
    IMU.readAcceleration(acc_x, acc_y, acc_z);
    IMU.readGyroscope(gyro_x, gyro_y, gyro_z);

    acc_rad = atan2(acc_y, acc_z);
    acc_deg = acc_rad * 180.0 / M_PI;
    gyro_deg =  angle_deg + gyro_x * dt;

    angle_deg = alpha * gyro_deg + (1.0 - alpha) * acc_deg;
    angle_rad = angle_deg * M_PI / 180.0;
  }

  sensor.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) 
  {  
    measured_distance = measure.RangeMilliMeter;
    real_distance = float(measured_distance * cos(angle_rad));
  } 
  else 
  {
    real_distance = 500.0;
  }
  
  dtostrf(real_distance, 1, 2, buffer_distance);
  dtostrf(angle_deg, 1, 2, buffer_angle);
  sprintf(output, "$AD,%s,%s\n", buffer_distance, buffer_angle);
  Serial.print(output);
}
