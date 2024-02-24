#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "udpListener.h"
#include "sampler.h"

#define PORT 12345
#define BUFFER_SIZE 16384 //16384
pthread_t tid; 
char lastMsg [BUFFER_SIZE];

void UDP_init() {
  pthread_create(&tid, NULL, &UDP_startListening, NULL); 
  pthread_join(tid, NULL);
} 

void UDP_cleanup(){
    pthread_cancel(tid); 
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

static void UDP_parseMessage(char* buff, int bytesRead, char* msg, int msgLen) {
  char* possibleCommands[] = {"help", "?", "count", "length", "dips", "history", "stop"};
  char recvMsg[bytesRead];
  for(int i = 0; i < bytesRead; i++) {
    recvMsg[i] = tolower(buff[i]);
  }
  if(strncmp(recvMsg, possibleCommands[0], strlen(possibleCommands[0])) == 0 || strncmp(recvMsg, possibleCommands[1], strlen(possibleCommands[1])) == 0) {
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
  else if(strncmp(recvMsg, possibleCommands[2], strlen(possibleCommands[2])) == 0) {
    long long count = Sampler_getNumSamplesTaken();
    snprintf(msg, BUFFER_SIZE, "samples taken total: %lld\n\n", count);
  }
  else if(strncmp(recvMsg, possibleCommands[3], strlen(possibleCommands[3])) == 0) {
    int length = Sampler_getHistorySize();
    snprintf(msg, BUFFER_SIZE, "samples taken last second: %d\n\n", length);
  }
  else if(strncmp(recvMsg, possibleCommands[4], strlen(possibleCommands[4])) == 0) {

  }
  else if(strncmp(recvMsg, possibleCommands[5], strlen(possibleCommands[5])) == 0) {
    msg[0] = '\0';
    int len = Sampler_getHistorySize();
    double* history = Sampler_getHistory(&len);
    
    for(int i = 0; i < len; i++) {
      char str[50];
      sprintf(str,"%d, ", (int)history[i]);
      strncat(msg, str, strlen(str));
    }
    strncat(msg, "\n\n", 3);
    free(history);
  }
  else if(strncmp(recvMsg, possibleCommands[6], strlen(possibleCommands[6])) == 0) {
    char newMsg[] = "Program terminating\n\n";
    strncpy(msg, newMsg, strlen(newMsg)+1);
  }
  else if(bytesRead == 1 && recvMsg[0] == '\0') {
    strncpy(msg, lastMsg, strlen(lastMsg)+1);
  }                            
  else {
    char newMsg[] = "unknown command\n\n"; 
    strncpy(msg, newMsg, strlen(newMsg)+1);
  }
}

static void UDP_parseAndSend(int sockId, char* buff, int bytesRead) {
  char msg[BUFFER_SIZE];
  UDP_parseMessage(buff, bytesRead, &msg, BUFFER_SIZE);
  strncpy(lastMsg, msg, strlen(msg)+1);
  send(sockId, msg, sizeof(char)*(strlen(msg)+1), 0);
}

void* UDP_startListening(void* args) {
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

  printf("Listening on port: %d\n", PORT);
  char buff[BUFFER_SIZE];
  int clientLen = sizeof(client);

  char defaultLastMsg[] = "unknown command\n";
  strncpy(lastMsg, defaultLastMsg, strlen(defaultLastMsg));
  do {    
    int bytesRead = UDP_receiveAndConnect(sockId, buff, client, &clientLen);
    UDP_parseAndSend(sockId, buff, bytesRead);
  }
  while(strcmp(buff, "stop") != 0);

  close(sockId);
  return NULL;
}