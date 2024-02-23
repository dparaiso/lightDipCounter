#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "udpListener.h"

#define PORT 12345
#define BUFFER_SIZE 1024
pthread_t tid; 

void UDP_init() {
  pthread_create(&tid, NULL, &UDP_startListening, NULL); 
  pthread_join(tid, NULL);
} 

void UDP_cleanup(){
    pthread_cancel(tid); 
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
  do {    
    int bytesRead = recvfrom(sockId, buff, BUFFER_SIZE-1, 0, (struct sockaddr*)&client, (socklen_t *)&(clientLen));
    printf("%d\n", bytesRead);
    buff[bytesRead-1] = '\0';
    if(strcmp(buff, "stop") == 0) {
      break;
    }
    if(connect(sockId, (struct sockaddr*)&client, clientLen) == -1) {
      perror("socket connect failed");
      exit(EXIT_FAILURE);
    }
    char str[] = "Hello from the server\n";
    send(sockId, str, strlen(str)+1, 0);
  }
  while(strcmp(buff, "stop") != 0);
  
  close(sockId);
  return NULL;
}