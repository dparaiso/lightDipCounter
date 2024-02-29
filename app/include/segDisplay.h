// segDisplay.h
// Module to write to 14-seg display in the background
//
// It continuously samples the number of dips and displays it on the 14-seg display. 
//
// The application will do a number of actions each second which must
// be synchronized (such as computing dips and printing to the screen).
// It also checks that the pins are exported and writes to the pins to make them 
// ready to display output. 
#ifndef _SEGDISPLAY_H_
#define _SEGDISPLAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <stdbool.h>

#define i2c_config1 "config-pin P9_18 i2c"
#define i2c_config2 "config-pin P9_17 i2c"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x02
#define REG_DIRB 0x03
#define REG_OUTA 0x00
#define REG_OUTB 0x01

// green zencape registers
// #define REG_DIRA 0x00
// #define REG_DIRB 0x01
// #define REG_OUTA 0x14
// #define REG_OUTB 0x15

#define GPIO61_VAL "/sys/class/gpio/gpio61/value"
#define GPIO44_VAL "/sys/class/gpio/gpio44/value"
#define GPIO61_DIR "/sys/class/gpio/gpio61/direction"
#define GPIO44_DIR "/sys/class/gpio/gpio44/direction"
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO61 "/sys/class/gpio/gpio61"
#define GPIO44 "/sys/class/gpio/gpio44"

void segDisplay_init();
void segDisplay_cancel();
void* displayNum();
void initI2cBus();
void writeI2cReg(unsigned char regAddr, unsigned char value);
unsigned char readI2cReg( unsigned char regAddr);
void segDisplay_write(char *filename, char* param);
bool checkIfExported(char *gpioPin);

#endif