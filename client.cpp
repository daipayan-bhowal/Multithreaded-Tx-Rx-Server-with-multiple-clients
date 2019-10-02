
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>
#define PORT 8888 
#define MAXLINE 1024 
int main() 
{ 
    int sockfd; 
    char buffer[MAXLINE]; 
    char message[] = "Hello Server"; 
    struct sockaddr_in servaddr; 

    int n, len,ret; 
    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("socket creation failed"); 
        exit(0); 
    } 

    memset(&servaddr, 0, sizeof(servaddr)); 

    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    while (ret=connect(sockfd, (struct sockaddr*)&servaddr, 
                            sizeof(servaddr)) >= 0) { 
        
    memset(buffer, 0, sizeof(buffer)); 
    strcpy(buffer, "Hello Server"); 
    write(sockfd, buffer, sizeof(buffer)); 
    printf("Message from server: "); 
    read(sockfd, buffer, sizeof(buffer)); 
    printf("%s\n",buffer); 
  }
    close(sockfd); 
  if(ret< 0)
      printf("Connection Failed\n");
} 

