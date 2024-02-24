#ifndef _UDPLISTENER_H_
#define _UDPLISTENER_H_

void UDP_init();
void UDP_cleanup(pthread_t tid);
void* UDP_startListening(void* args);
void UDP_parseMessage(char* buff, int bytesRead, char* msg);

#endif