#ifndef _A2D_H_
#define _A2D_H_

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int getVoltage1Reading();
int getVoltage0Reading();
double convertA2D(double analogSignal);

#endif