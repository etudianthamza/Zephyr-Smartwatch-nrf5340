#ifndef SENSORS_H
#define SENSORS_H

int sensors_init(void);
void sensors_update(void);
float sensors_get_temperature(void);
float sensors_get_humidity(void);
float sensors_get_pressure(void);
void sensors_get_magn(float *x, float *y, float *z);
void sensors_get_accel(float *x, float *y, float *z);
void sensors_get_gyro(float *x, float *y, float *z);
void sensors_update_motion(void);    // mouvement (accel/gyro/magn)
/**
 * @brief Convertit un angle (en degrés) en direction cardinale en français.
 * @param angle_deg Angle en degrés (0-360).
 * @return Pointeur vers une chaîne constante.
 */
const char* sensors_heading_to_cardinal(float angle_deg);

#endif