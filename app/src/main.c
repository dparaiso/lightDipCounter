// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal/button.h"
#include "sampler.h"
#include "udpListener.h"
#include "hal/led.h"

int main()
{
    Sampler_init();
    UDP_init();
    Sampler_cleanup(); 
    UDP_cleanup();

    return 0; 

}