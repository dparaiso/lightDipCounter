#ifndef _UDPLISTENER_H_
#define _UDPLISTENER_H_

void UDP_init();
void UDP_cleanup();
void* UDP_startListening(void* args);

#endif