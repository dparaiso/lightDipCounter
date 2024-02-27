#include "hal/led.h"
#include "hal/A2D.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 

static pthread_t ptid;  
static double frequency = 0; 
long long duty_cycle;
long long enable = 1; 

void LED_init(){
    runCommand(LED_config);
    LED_write(LED_enable, enable);

    if(pthread_create(&ptid, NULL, &LED_setPWM, NULL ) < 0){
        perror("pthread_create() error in Sampler"); 
        exit(1); 
    } 
}

void LED_cleanup(){
    pthread_cancel(ptid); 
}

void* LED_setPWM(){
    while(1){
        int a2dsignal = getVoltage0Reading();
        LED_updateFrequency(a2dsignal); 
        sleepForMs(100);
    }
}

void LED_updateFrequency(int a2dsignal){
    double newfrequency = (double)a2dsignal/40;
    long long period = 0;  

    if(newfrequency == 0){
        newfrequency = 0; 
        if(newfrequency == frequency){
            return; 
        }
        period = 0;
        duty_cycle = 0;
    }else{
        if(newfrequency == frequency){
            return; 
        }
         
        period = (1/newfrequency) *1000000000;
        duty_cycle = period/2; 
    }

    frequency = newfrequency; 

    LED_write(LED_duty_cycle, duty_cycle); 
    LED_write(LED_period, period); 

}

void LED_write(char *filename, long long param){
    FILE *pLedTriggerFile = fopen(filename, "w");
    if(pLedTriggerFile == NULL){
        printf("ERROR: Unable to open file (%s) for read\n", filename);
        exit(1);
    }
    int charWritten = fprintf(pLedTriggerFile, "%lld", param);
    // printf("charWritten: %d\tparam: %lld\n",charWritten, param);
    if(charWritten <= 0){
        printf("ERROR WRITING DATA\n");
        exit(1);
    }

    fclose(pLedTriggerFile);
}


void runCommand(char* command){
    //Execute shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    //Ignore output of the command; but consume it 
    //so we don't get an error when closing the pipe 
    char buffer[1024];
    while(!feof(pipe) && !ferror(pipe)){
        if(fgets(buffer, sizeof(buffer), pipe) == NULL)
        break; 
        // printf("--> %s", buffer); // uncomment for debugging
    }

    // get the exit code from the pipe; non-zero is an error: 
    int exitCode =  WEXITSTATUS(pclose(pipe));
    if (exitCode != 0){
        perror("Unable to execute command: ");
        printf("    command: %s\n", command); 
        printf("    exit code: %d\n", exitCode);
    }

}

void sleepForMs(long long delayInMs){
    const long long NS_PER_MS = 1000 * 1000; 
    const long long NS_PER_SECOND = 1000000000;
    
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND; 

    struct timespec reqDelay = {seconds, nanoseconds}; 
    nanosleep(&reqDelay, (struct timespec *) NULL);

}

long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds /1000000;
    return milliSeconds; 
}