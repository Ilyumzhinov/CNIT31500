/* Compile:
    gcc cMessenger.c -o cMessenger
*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

/*GLOBAL DECLARATION*/
/* Global variables */
char USERNAME[32];

/* Functions prototypes */
int CreateServer();
int CreateClient();
/**/


/*FUNCTIONS*/
/* A universal string processing method for systems calls
    Allocates memory for a string with the size specified
    By default, returns the pointer to the string 
    */
char* ProcessMessage(int size)
{
    char* messagePtr = (char*)malloc(sizeof(char[size]));

    /* Print Standard UI */
    printf("--------\n");
    printf("message | ");
    scanf("%s", messagePtr);
    printf("--------\n\n");

    /* Process the string */
    if (47 != messagePtr[0])
    {
    }
    else
    {
        /* If "\o" close the program */
        if (99 == messagePtr[1])
        {
            /* Reference: https://stackoverflow.com/questions/7973583/how-can-i-immediately-close-a-program-in-c?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */
            printf("Close the program\n");

            free(messagePtr);

            exit(0);
        }
        /* If "\h" close the program */
        else if (104 == messagePtr[1])
        {
            printf("System commands: \n");
            printf("%s | %s\n", "/c", "Close the program");
            printf("%s | %s\n", "/h", "Help");

            free(messagePtr);
            ProcessMessage(size);
        }
    }

    /* Return the string */
    return messagePtr;
}

/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ */
int CreateServer()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    return 0;
}

/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ */
int CreateClient()
{
    /* socket data */
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    char ipServer[64];
    /**/

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    while (0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
    {
        printf("Enter IP:\n");

        strncpy(ipServer, ProcessMessage(64), 64);

        if(inet_pton(AF_INET, ipServer, &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
        }
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nPort is closed \n");
        return -1;
    }


    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    return 0;
}


/* Main logic */
int main()
{
	/**/
	char menuChoice[1];
    char* strPtr;
	/**/

	printf("Type in your name:\n");
    strncpy(USERNAME, ProcessMessage(32), 32);

    printf("Named as \x1b[97;42m%s\x1b[0m\n", USERNAME);
	printf("1: Open chat\n");
	printf("2: Join chat\n");

    while (menuChoice[0] < 49 || menuChoice[0] > 50)
    {
        strncpy(menuChoice, ProcessMessage(1), 1);
    }

	/* Open chat */
	if (49 == menuChoice[0])
	{
		CreateServer();
	}
    /* Join chat */
    else if (50 == menuChoice[0])
    {
        CreateClient();
    }

	return 0;
}