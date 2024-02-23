// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal/button.h"
#include "sampler.h"
#include "udpListener.h"

int main()
{
    Sampler_init();
    UDP_init();
    // while(1);   //keep thread going  
    Sampler_cleanup(); //use later for shutdown
    UDP_cleanup();

    return 0; 

}