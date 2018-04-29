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
/* Structures */
struct User
{
    char userName[32];
    int userColor;
};

/* Functions prototypes */
char* ProcessMessage(int);
struct User* CreateUser();

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
    printf("message > ");
    scanf("%s", messagePtr);
    printf("--------\n\n");

    /* Process the string */
    /* If it starts with "/", it contains a system call */
    if (47 == messagePtr[0])
    {
        /* If "/c", close the program */
        if (99 == messagePtr[1])
        {
            printf("Close the program\n");

            free(messagePtr);

            /* Reference: https://stackoverflow.com/questions/7973583/how-can-i-immediately-close-a-program-in-c?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa */
            exit(0);
        }
        /* If "/h", print help commands */
        else if (104 == messagePtr[1])
        {
            printf("System commands: \n");
            printf("%s | %s\n", "/c", "Close the program");
            printf("%s | %s\n", "/h", "Help");
        }

        free(messagePtr);
        ProcessMessage(size);
    }
    else
    {
    }

    /* Return the string */
    return messagePtr;
}

/* Create a listening host using a socket */
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
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Server: Hello message sent\n");
    return 0;
}

/* Create a client by connecting to a listening socket at a specified IP address */
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
  
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nPort is closed \n");
        return -1;
    }


    send(sock, hello, strlen(hello), 0);
    printf("Client: Hello message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
    return 0;
}

struct User* CreateUser()
{
    struct User* userPtr = (struct User*)malloc(sizeof(struct User));
    char userColorInput[1];

    /* Choose name */
    {
        printf("Type in your nickname:\n");
        strncpy(userPtr->userName, ProcessMessage(32), 32);
    }

    /* Choose color */
    {
        int i;

        /* Print various colors */
        printf("Choose color:\n");
        for (i = 1; i < 7; i++)
        {
            printf("\x1b[97;%dm %d - %s \x1b[0m ", i + 40, i, "message");
        }
        printf("\n");

        while (userColorInput[0] < 49 || userColorInput[0] > 54)
        {
            strncpy(userColorInput, ProcessMessage(1), 1);
        }

        userPtr->userColor = atoi(userColorInput);
    }

    return userPtr;
}


/* Main logic */
int main()
{
	/* Declaration */
	char menuChoice[1];
    char* strPtr;

    struct User* userPtr = NULL;
	/**/

    /* Creating a user */
    userPtr = CreateUser();

    /* Printing main menu */
    {
        printf("User: \x1b[97;%dm%s\x1b[0m\n", userPtr->userColor + 40, userPtr->userName);
        printf("1 | Open chat\n");
        printf("2 | Join chat\n");
    }

    /* Processing the menu choice */
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