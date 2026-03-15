#ifndef SENSORS_H
#define SENSORS_H

int sensors_init(void);
void sensors_update(void);
float sensors_get_temperature(void);
float sensors_get_humidity(void);
float sensors_get_pressure(void);
void sensors_get_magn(float *x, float *y, float *z);
void sensors_get_accel(float *x, float *y, float *z);

#endif