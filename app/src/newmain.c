// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/i2c.h>
// #include <linux/i2c-dev.h>
// #include <errno.h>

// #define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
// #define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
// #define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

// #define I2C_DEVICE_ADDRESS 0x20

// #define REG_DIRA 0x02
// #define REG_DIRB 0x03
// #define REG_OUTA 0x00
// #define REG_OUTB 0x01

// #define GPIO61_VAL "/sys/class/gpio/gpio61/value"
// #define GPIO44_VAL "/sys/class/gpio/gpio44/value"

// #define GPIO61_DIR "/sys/class/gpio/gpio61/direction"
// #define GPIO44_DIR "/sys/class/gpio/gpio44/direction"
// #define GPIO61_EXP "/sys/class/gpio/export"
// #define GPIO44_EXP "/sys/class/gpio/export"

// static int initI2cBus(char* bus, int address);
// static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
// static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);
// static void segDisplay_write(char *filename, char* param);
// static void sleepForMs(long long delayInMs);

// int outa[10] = {0xD0, 0xC0, 0x98, 0xD8, 0xC8, 0x58, 0x58, 0xC0, 0xD8, 0xC8}; 
// int outb[10] = {0xA1, 0x00, 0x83, 0x03, 0x22, 0X23, 0XA3, 0x01, 0xA3, 0x23}; 

// int main()
// {
// 	printf("Drive display (assumes GPIO #61 and #44 are output and 1\n");
//     // segDisplay_write(GPIO61_EXP, "61"); 
//     // sleepForMs(300); 

//     // segDisplay_write(GPIO61_EXP, "44"); 
//     // sleepForMs(300); 
//     // segDisplay_write(GPIO61_DIR, "out"); 
//     // sleepForMs(300); 
// 	// segDisplay_write(GPIO44_DIR, "out"); 
//     // sleepForMs(300); 
//     // segDisplay_write(GPIO61_VAL, "1"); 
//     // sleepForMs(300); 
// 	// segDisplay_write(GPIO44_VAL, "1"); 
// 	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

// 	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
//     sleepForMs(300); 
// 	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
//     sleepForMs(300); 
// 	// writeI2cReg(i2cFileDesc, REG_OUTA, 0x00);
//     // sleepForMs(300); 
// 	// Drive an hour-glass looking character (Like an X with a bar on top & bottom)
// 	writeI2cReg(i2cFileDesc, REG_OUTA, 0xC8);
//     sleepForMs(300);
// 	writeI2cReg(i2cFileDesc, REG_OUTB, 0X23);
//     sleepForMs(300);
// 	// writeI2cReg(i2cFileDesc, REG_OUTB, 0x02);

// 	// Read a register:
// 	unsigned char regVal = readI2cReg(i2cFileDesc, REG_OUTA);
// 	printf("Reg OUT-A = 0x%04x\n", regVal);

// 	// Cleanup I2C access;
// 	close(i2cFileDesc);
// 	return 0;
// }

// static int initI2cBus(char* bus, int address)
// {
// 	int i2cFileDesc = open(bus, O_RDWR);
// 	if (i2cFileDesc < 0) {
// 		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
// 		perror("Error is:");
// 		exit(-1);
// 	}

// 	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
// 	if (result < 0) {
// 		perror("Unable to set I2C device to slave address.");
// 		exit(-1);
// 	}
// 	return i2cFileDesc;
// }

// static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
// {
// 	unsigned char buff[2];
// 	buff[0] = regAddr;
// 	buff[1] = value;
// 	int res = write(i2cFileDesc, buff, 2);
// 	if (res != 2) {
// 		perror("Unable to write i2c register");
// 		exit(-1);
// 	}
// }

// static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
// {
// 	// To read a register, must first write the address
// 	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
// 	if (res != sizeof(regAddr)) {
// 		perror("Unable to write i2c register.");
// 		exit(-1);
// 	}

// 	// Now read the value and return it
// 	char value = 0;
// 	res = read(i2cFileDesc, &value, sizeof(value));
// 	if (res != sizeof(value)) {
// 		perror("Unable to read i2c register");
// 		exit(-1);
// 	}
// 	return value;
// }

// static void segDisplay_write(char *filename, char* param){
//     FILE *pLedTriggerFile = fopen(filename, "w");
//     if(pLedTriggerFile == NULL){
//         printf("ERROR: Unable to open file (%s) for read\n", filename);
//         printf("fopen failed, errno = (%s)\n", strerror(errno)); 
//         exit(1);
//     }
//     int charWritten = fprintf(pLedTriggerFile, param);
//     // printf("charWritten: %d\tparam: %lld\n",charWritten, param);
//     if(charWritten <= 0){
//         printf("ERROR WRITING DATA\n");
//         exit(1);
//     }

//     fclose(pLedTriggerFile);
// }

// static void sleepForMs(long long delayInMs){
//     const long long NS_PER_MS = 1000 * 1000; 
//     const long long NS_PER_SECOND = 1000000000;
    
//     long long delayNs = delayInMs * NS_PER_MS; 
//     int seconds = delayNs / NS_PER_SECOND;
//     int nanoseconds = delayNs % NS_PER_SECOND; 

//     struct timespec reqDelay = {seconds, nanoseconds}; 
//     nanosleep(&reqDelay, (struct timespec *) NULL);

// }