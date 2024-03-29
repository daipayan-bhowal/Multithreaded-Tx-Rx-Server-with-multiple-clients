#include <stdio.h> 
#include <string.h>  
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>  
#include <pthread.h> 
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 


class RxTxEntity
{
    public:
      int socket;
      char message[1024];
};
void *Rxfunc(void *msg)
{
     char buffer[1025] = {0}; //data buffer of 1K 
      
    //  RxTxEntity *rx = (RxTxEntity*) msg; 
      int *sock = (int*) msg;
      if(recv(*sock,buffer , 1024, 0) < 0)
      {
           printf("Receive failed\n");
      }
      printf("Received Message: %s\n", buffer);
}
void *Txfunc(void *msg)
{
  //    RxTxEntity *tx= (RxTxEntity*) msg;
      int *sock = (int*) msg;
     char message[1025] = "Tx Function"; //data buffer of 1K 

      if( send(*sock, message, strlen(message), 0) != strlen(message) )
      {
           perror("send");
      }

}
    //set of socket descriptors 
int main(int argc , char *argv[]) 
{ 
    int opt = TRUE; 
    int master_socket , addrlen , new_socket , client_socket[30] , 
        max_clients = 30 , activity, i , valread , sd; 
    int max_sd; 
    struct sockaddr_in address; 
       // static once; 
    char buffer[1025]; //data buffer of 1K 
        
    //set of socket descriptors 
    fd_set readfds; 
        
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++) 
    { 
        client_socket[i] = 0; 
    } 
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
        sizeof(opt)) < 0 ) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    //type of socket created 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
        
        
    //a message 
    char message[] = "5G NR DU \r\n";

    pthread_t TxMsg[30], RxMsg[30]; 
   
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Listener on port %d \n", PORT); 
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
        
    //accept the incoming connection 
    addrlen = sizeof(address); 
    puts("Waiting for connections ..."); 
        
    while(TRUE) 
    { 
        //clear the socket set 
        FD_ZERO(&readfds); 
    
        //add master socket to set 
        FD_SET(master_socket, &readfds); 
        max_sd = master_socket; 
            
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++) 
        { 
            //socket descriptor 
            sd = client_socket[i]; 
                
            //if valid socket descriptor then add to read list 
            if(sd > 0) 
                FD_SET( sd , &readfds); 
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd) 
                max_sd = sd; 
        } 
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
    
        if ((activity < 0) && (errno!=EINTR)) 
        { 
            printf("select error"); 
        } 
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds)) 
        { 
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
            { 
                perror("accept"); 
                exit(EXIT_FAILURE); 
            } 
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port)); 
      
            if( pthread_create(&RxMsg[new_socket], NULL, Rxfunc, &new_socket) != 0 )
                printf("Receive thread could not be created\n");

            //send new connection greeting message 
            if( pthread_create(&TxMsg[new_socket], NULL, Txfunc, &new_socket) != 0 )
                printf("Send thread could not be created\n");   

 
            puts("Welcome message sent successfully"); 
                
             pthread_join(RxMsg[new_socket], NULL); 
             pthread_join(TxMsg[new_socket], NULL);
            
             //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++) 
            { 
                //if position is empty 
                if( client_socket[i] == 0 ) 
                { 
                    client_socket[i] = new_socket; 
                    printf("Adding to list of sockets as %d\n" , i); 
                        
                    break; 
                } 
            } 
        } 
            
    } 
    return 0; 
} 

