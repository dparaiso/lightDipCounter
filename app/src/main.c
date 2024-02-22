// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal/button.h"
#include "sampler.h"

int main()
{
    Sampler_init(); 
    Sampler_cleanup(); 
    return 0; 

}