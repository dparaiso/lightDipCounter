#ifndef _LED_H_
#define _LED_H_

#define LED_config "config-pin p9_21 pwm"
#define LED_period "/dev/bone/pwm/0/b/period"
#define LED_enable "/dev/bone/pwm/0/b/enable"
#define LED_duty_cycle "/dev/bone/pwm/0/b/duty_cycle"

void LED_init(); 
void LED_cleanup();
void* LED_setPWM();
void LED_write(char *filename, long long param);
void LED_updateFrequency(int a2dsignal);
void runCommand(char* command);
void sleepForMs(long long delayInMs);
long long getTimeInMs(void);

#endif