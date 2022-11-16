#include "contiki.h"
#include "dev/light-sensor.h"
#include <stdio.h>

// initiate sensors
void start_light()
{
  SENSORS_ACTIVATE(light_sensor);
}

// gets adc value and converts it to useable light value in lux
float get_light()
{
  float lightData = (float)light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
//  0.625*1e6*(((1.5f*adc)/4096)/100000)*1000
  float voltage = (1.5f*lightData)/4096;
  float current = voltage/100000;
  float light = 0.625*1e6*current*1000;
  return light;
}