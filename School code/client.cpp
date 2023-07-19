#include <iostream>
#include <queue>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main (int argc, char *argv[])
{
    // The port
    int myPort;

    // Initializing the buffer's size
    int bufferSize = 100;

    char buffer[bufferSize];

    // The client's socket
    int myClientSocket = -1;

    // The socket's address
    struct sockaddr_in address;

    // My message
    char* myMessage = "Brandon Tran:4261-8673";

    // Switch that uses the arguments
    switch (argc)
    {
        case 1:
            break;

        case 2:
            // Getting the port
            myPort = atoi(argv[1]);
        
            break;
            
        default:
            printf("Error: Too many arguments given.\n");
        
            break;
    }
    
    // Getting the client's socket
    myClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;

    address.sin_port = htons(myPort);

    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Error message if the client's socket can't be opened
    if (myClientSocket == -1)
    {
        printf("Error: Can't open socket.\n");
    }

    // Connecting the socket to the server
    if (connect(myClientSocket, (struct sockaddr*)& address, sizeof(address)) != 0)
    {
        printf("Error: Can't connect to server's port.\n");

        return 1;
    }

    // Writing my message
    write(myClientSocket, myMessage, 25);

    read(myClientSocket, buffer, bufferSize);

    printf("\n%s\n", buffer);

    bzero(buffer, bufferSize);

    // Closing the client's socket
    close(myClientSocket);
}