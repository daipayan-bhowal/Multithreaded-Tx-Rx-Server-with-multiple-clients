# Multithreaded-Tx-Rx-Server-with-multiple-clients
Socket Programming: Receive and Send message are multithreaded with multiple clients handling

Server compilation:
g++ -g -pthread Multithreaded-server.cpp -o Multithreaded-server

Client compilation:
g++ client.cpp -o client

Run ./Multithreaded-server on one terminal
open another terminal and run ./client

