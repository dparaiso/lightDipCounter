#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "udpListener.h"
#include "sampler.h"
#include "hal/A2D.h"

pthread_t tid; 
char lastBuff [BUFFER_SIZE];

void UDP_init() {
  pthread_create(&tid, NULL, &UDP_startListening, NULL);
  pthread_join(tid, NULL); 
} 

void UDP_cleanup(pthread_t tid){
    pthread_cancel(tid); 
}

static void UDP_convertVoltage(float*newHist, double* history, int len) {
  for(int i = 0; i < len; i++) {
    newHist[i] = convertA2D(history[i]);
  }
}

static int UDP_receiveAndConnect(int sockId, char* buff, struct sockaddr_in client, int* clientLen) {
  int bytesRead = recvfrom(sockId, buff, BUFFER_SIZE-1, 0, (struct sockaddr*)&client, (socklen_t *) clientLen);
  buff[bytesRead-1] = '\0';
  if(connect(sockId, (struct sockaddr*)&client, *clientLen) == -1) {
    perror("socket connect failed");
    exit(EXIT_FAILURE);
  }
  return bytesRead;
}

static void UDP_helpMsg(char* msg) {
  char newMsg[] = "Accepted command examples:\n"
        "help -- return a brief summary/list of supported commands.\n"
        "? -- same as help\n"
        "count -- get the total number of samples taken.\n"
        "length -- get the number of samples taken in the previously completed second.\n"
        "dips -- get the number of dips in the previously completed second.\n"
        "history -- get all the samples in the previously completed second.\n"
        "stop -- cause the server program to end.\n"
        "<enter> -- repeat last command.\n\n";
    strncpy(msg, newMsg, strlen(newMsg)+1);
}
static void UDP_countMsg(char* msg) {
  long long count = Sampler_getNumSamplesTaken();
  snprintf(msg, BUFFER_SIZE, "samples taken total: %lld\n\n", count);
}
static void UDP_lengthMsg(char* msg) {
  int length = Sampler_getHistorySize();
  snprintf(msg, BUFFER_SIZE, "samples taken last second: %d\n\n", length);
}
static void UDP_dipsMsg(char* msg) {
  int dips = getNumDips();
  snprintf(msg, BUFFER_SIZE, "Dips: %d\n\n", dips);
}
static void UDP_historyMsg(char* msg) {
  msg[0] = '\0';
  int len = Sampler_getHistorySize();
  double* doubleHist = Sampler_getHistory(&len);
  float history[len];
  UDP_convertVoltage(history, doubleHist, len); 
  
  for(int i = 0; i < len; i++) {
    char str[50];
    sprintf(str,"%.3f, ", history[i]);
    if((i+1) % 10 == 0) { 
      strncat(str, "\n", 1);
    }
    strncat(msg, str, strlen(str));
  }
  strncat(msg, "\n\n", 2);
  free(doubleHist);
}
static void UDP_stopMsg(char* msg) {
  char newMsg[] = "Program terminating\n\n";
  strncpy(msg, newMsg, strlen(newMsg)+1);
}
static void UDP_enterMsg(char* msg) {
  // strncpy(msg, lastMsg, strlen(lastMsg)+1);
  UDP_parseMessage(lastBuff, (int) strlen(lastBuff)+1, msg);
}

void UDP_parseMessage(char* buff, int bytesRead, char* msg) { 
  char* possibleCommands[] = {"help", "?", "count", "length", "dips", "history", "stop"};
  char recvMsg[bytesRead];
  for(int i = 0; i < bytesRead; i++) {
    recvMsg[i] = tolower(buff[i]);
  }
  if(strncmp(recvMsg, possibleCommands[0], strlen(possibleCommands[0])) == 0 || strncmp(recvMsg, possibleCommands[1], strlen(possibleCommands[1])) == 0) {
    UDP_helpMsg(msg);
  }
  else if(strncmp(recvMsg, possibleCommands[2], strlen(possibleCommands[2])) == 0) {
    UDP_countMsg(msg);
  }
  else if(strncmp(recvMsg, possibleCommands[3], strlen(possibleCommands[3])) == 0) {
    UDP_lengthMsg(msg);
  }
  else if(strncmp(recvMsg, possibleCommands[4], strlen(possibleCommands[4])) == 0) {
    UDP_dipsMsg(msg);
  }
  else if(strncmp(recvMsg, possibleCommands[5], strlen(possibleCommands[5])) == 0) {
    UDP_historyMsg(msg);
  }
  else if(strncmp(recvMsg, possibleCommands[6], strlen(possibleCommands[6])) == 0) {
    UDP_stopMsg(msg);
  }
  else if(bytesRead == 1 && recvMsg[0] == '\0' && !(strcmp(lastBuff, buff) == 0)) {
    UDP_enterMsg(msg);
  }                            
  else {
    char newMsg[] = "unknown command\n\n"; 
    strncpy(msg, newMsg, strlen(newMsg)+1);
  }
}

static void UDP_parseAndSend(int sockId, char* buff, int bytesRead) {
  char msg[BUFFER_SIZE];
  UDP_parseMessage(buff, bytesRead, msg);
  strncpy(lastBuff, buff, bytesRead);

  // strncpy(lastMsg, msg, strlen(msg)+1);
  send(sockId, msg, sizeof(char)*(strlen(msg)+1), 0);
}

void* UDP_startListening() {
  struct sockaddr_in addr;
  struct sockaddr_in client;
  int sockId = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockId < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  } 

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  if(bind(sockId, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("socket bind failed");
    exit(EXIT_FAILURE);
  }

  char buff[BUFFER_SIZE];
  int clientLen = sizeof(client);

  char defaultLastBuff[] = "";
  strncpy(lastBuff, defaultLastBuff, strlen(defaultLastBuff));
  do {    
    int bytesRead = UDP_receiveAndConnect(sockId, buff, client, &clientLen);
    UDP_parseAndSend(sockId, buff, bytesRead);
  }
  while(strcmp(buff, "stop") != 0);

  close(sockId);
  return NULL;
}