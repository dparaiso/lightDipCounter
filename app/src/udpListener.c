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
  int sockId = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockId < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  if (bind(sockId, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("socket bind failed");
    exit(EXIT_FAILURE);
  }

  printf("Listening on port: %d\n", PORT);
  while(true) {
    char buff[BUFFER_SIZE];
    int bytesRead = read(sockId, buff, BUFFER_SIZE-1);
    buff[bytesRead-1] = '\0';
    printf("%s\n", buff);

    // for(int i = 0; i < BUFFER_SIZE; i++) {
    //   if (buff[i] == 0)
    //         printf("\\0");
    //   else
    //     printf("%c", buff[i]);
    // }
    // printf("\n");

    printf("%d\n", strcmp(buff, "stop"));
    if(strcmp(buff, "stop") == 0) {
      break;
    }
    send(sockId, "Hello from the server", 22, 0);
    printf("Hello message sent\n");
  }
  
  close(sockId);
  return NULL;
}