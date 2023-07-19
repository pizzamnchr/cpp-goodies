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

// Status string's getter
std::string getStatusString(int _sizeOfQueue, int _myPort)
{
    std::string result = "Wall server running on port ";

    result.append(std::to_string(_myPort));

    result.append(" with queue size ");

    result.append(std::to_string(_sizeOfQueue));

    // Return the result
    return result;
}

// Buffer length's getter
int getLengthOfBuffer(char* _myBuffer, int _maximumSize)
{
    for (int i = 0; i < _maximumSize; i++)
    {
        if (_myBuffer[i] == '\n')
        {
            return i;
        }
    }

    // Failed
    return -1;
}

int main(int argc, char *argv[])
{
    // Setting the default port
    int port = 5514;

    // The socket's address
    struct sockaddr_in address;

    // Initializing the server
    int serverFD = -1;

    // Initiliazing the backlog
    int myBacklog = 10;

    // Initializing the client
    int clientFD = -1;

    // The client's address
    struct sockaddr_in addressClient;

    // Initializing size of read buffer
    int sizeOfReadBuffer = 100;

    // Using the size of read buffer
    char readBuffer[sizeOfReadBuffer];

    // Initializing the maximum length
    const unsigned int MAXIMUM_LEN = 80;

    // Using the maximum length for the name
    char nameInput[MAXIMUM_LEN];

    // Using the maximum length for the actual post
    char postInput[MAXIMUM_LEN];

    //int success = 0;

    int lengthOfInput = 0;

    char* welcomeMessage = "Welcome to the server running on REPTILIAN";

    // Switch that uses the arguments
    switch (argc)
    {
        case 1:
            break;

        case 2:
            port = atoi(argv[1]);

            break;
        default:
            printf("Error: Too many arguments given\n");

            break;
    }

    // Getting the socket
    serverFD = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;

    address.sin_port = htons(port);
    
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Error message if the socket can't be opened
    if (serverFD < 0)
    {
        printf("Error: Can't open socket.\n");
    }

    // Binding the socket error
    if(bind(serverFD, (struct sockaddr*)& address, sizeof(address)) == -1)
    {
        printf("Error: Can't bind socket.\n");
    }

    // Listening to the socket error
    if(listen(serverFD, myBacklog) == -1)
    {
        printf("Error: Can't listen to socket.\n");
    }

    // Getting the client's address length
    socklen_t clientLength = sizeof(addressClient);

    // Accepting the client
    clientFD = accept(serverFD, (struct sockaddr*)& addressClient, &clientLength);

    // Accepting the client error
    if(clientFD == -1)
    {
        printf("Error: Can't accept the client.\n");
    }

    // Reading
    read(clientFD, readBuffer, sizeOfReadBuffer);

    printf("%s\n", readBuffer);

    bzero(readBuffer, sizeOfReadBuffer);

    // Writing the welcome message
    write(clientFD, welcomeMessage, 43);
    
    // Closing the client
    close(clientFD);

    return 0;

    // Keeping the server up
    while (1)
    {
        bzero(readBuffer, sizeOfReadBuffer);

        // Reading
        read(clientFD, readBuffer, sizeOfReadBuffer);
        
        printf("\nClient: %s\n", readBuffer);

        printf("To Client: ");

        bzero(readBuffer, sizeOfReadBuffer);

        int i = 0;

        // Until we reach '\n'
        while ((readBuffer[i++] = getchar()) != '\n') {}

        readBuffer[i] = '\n';

        write(clientFD, readBuffer, sizeOfReadBuffer);

        // Quitting the loop
        if (strncmp("quit", readBuffer, 4) == 0)
        {
            break;
        }
    }

    // Closing the client
    close(clientFD);

    return 0;
}
