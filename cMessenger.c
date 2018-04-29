/* Compile:
    gcc cMessenger.c -o cMessenger
*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

#define SYSTEMACTION "--------------------"

/*GLOBAL DECLARATION*/
/* Structures */
struct User
{
    char userName[16];
    int userColor;
};

struct Message
{
    struct User* sender;

    char* message;
    int indentation;

    struct Message* next;
};

struct MessageHistory
{
    struct Message* top;
};

/* Functions prototypes */
struct User* CreateUser();
int CreateServer();
int CreateClient();

char* ProcessMessage(int);
void AddMessage(struct MessageHistory*, struct User*, char*, int);
void PrintMessage(struct User*, char*, int, struct MessageHistory*);
void PrintHistory(struct Message*);

/* Global variables */
/* Reference: https://stackoverflow.com/questions/11709929/how-to-initialize-a-pointer-to-a-struct-in-c */
struct User* systemUser = &(struct User) { .userName = "cMessenger", .userColor = 100 };
struct User* currentUser = NULL;
struct MessageHistory* messageHistory = NULL;
/**/


/*FUNCTIONS*/
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
        PrintMessage(systemUser, "Type in your nickname", 1, messageHistory);
        strncpy(userPtr->userName, ProcessMessage(16), 16);

        currentUser = userPtr;
        
        PrintMessage(userPtr, userPtr->userName, 0, messageHistory);
    }

    /* Choose color */
    {
        int i;
        char* tempColor = (char*)malloc(sizeof(char[64]));

        /* Print various colors */
        PrintMessage(systemUser, "Choose color", 0, messageHistory);

        printf("%s\n", SYSTEMACTION);
        for (i = 1; i < 6; i++)
        {
            printf("\x1b[97;%dm %d - %s \x1b[0m ", i + 40, i, "message");
        }
        printf("\n%s\n", SYSTEMACTION);

        while (userColorInput[0] < 49 || userColorInput[0] > 53)
        {
            strncpy(userColorInput, ProcessMessage(1), 1);
        }

        userPtr->userColor = atoi(userColorInput) + 40;

        /* Reference: http://forums.codeguru.com/showthread.php?347081-itoa-error */
        sprintf(tempColor, "%d", userPtr->userColor - 40);

        PrintMessage(userPtr, tempColor, 0, messageHistory);
    }

    return userPtr;
}

/* A universal string processing method that includes systems calls.
    Allocates memory for a string with the size specified.
    By default, returns the pointer to the string.
    */
char* ProcessMessage(int size)
{
    char* messagePtr = (char*)malloc(sizeof(char[size]));

    /* Print Standard UI */
    printf("%s\n", SYSTEMACTION);
    printf("message > ");
    scanf("%s", messagePtr);
    printf("%s\n\n", SYSTEMACTION);

    /* Process the string */
    /* If it starts with "/", it contains a system call */
    if (47 == messagePtr[0])
    {
        /* If "/c", close the program */
        if (99 == messagePtr[1])
        {
            printf("Close the program\n");

            free(messagePtr);

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
        messagePtr = ProcessMessage(size);
    }
    else
    {
    }

    /* system("clear");
    PrintHistory(messageHistory->top);*/

    /* Return the string */
    return messagePtr;
}

void PrintMessage(struct User* user, char* messageStr, int woName, struct MessageHistory* historyPtr)
{
    const int cLAlign = 18;
    const int cRAlign = 64;
    int isRight = 0, indentation = 1;

    if (NULL != currentUser)
    {
        if (!strncmp(user->userName, currentUser->userName, 16))
        {
            isRight = 1;
        }
    }

    /* Print user name */
    if (0 == woName)
    {
        printf("%s: ", user->userName);
        indentation = strlen(user->userName) + 3;
    }

    while (indentation < cLAlign)
    {
        printf(" ");
        indentation++;
    }

    if (isRight)
    {
        while (indentation < (cRAlign - strlen(messageStr) - strlen(user->userName)))
        {
            printf(" ");
            indentation++;
        }
    }

    printf("\x1b[97;%dm %s \x1b[0m\n", user->userColor, messageStr);

    if (NULL != historyPtr)
    {
        AddMessage(historyPtr, user, messageStr, woName);
    }
}

void AddMessage(struct MessageHistory* historyPtr, struct User* userPtr, char* messageStr, int indentation)
{
    struct Message* iMessage;
    struct Message* tempMessage;

    iMessage = historyPtr->top;
    
    /* Fill out the new message */
    {
        tempMessage = (struct Message*)malloc(sizeof(struct Message));
        tempMessage->sender = userPtr;

        tempMessage->message = messageStr;
        tempMessage->indentation = indentation;

        tempMessage->next = NULL;
    }

    if (NULL == iMessage)
    {
        historyPtr->top = tempMessage;

        return;
    }
    else
    {
        while (NULL != iMessage->next)
        {
            iMessage = iMessage->next;
        }

        iMessage->next = tempMessage;
    }
}

void PrintHistory(struct Message* currentMessage)
{
    struct Message* iMessage;

    if (NULL == currentMessage)
        return;


    iMessage = currentMessage;
    while (NULL != iMessage)
    {
        PrintMessage(iMessage->sender, iMessage->message, iMessage->indentation, NULL);
        iMessage = iMessage->next;
    }
}


/* Main logic */
int main()
{
	/* Declaration */
	char menuChoice[1];
    char* strPtr;
	/**/

    messageHistory = (struct MessageHistory*)malloc(sizeof(struct MessageHistory));
    messageHistory->top = NULL;

    if (NULL == currentUser)
    {
        /* Welcome message */
        PrintMessage(systemUser, "Welcome!", 0, messageHistory);

        /* Creating a user */
        CreateUser();
        /**/
    }

    /* Printing main menu */
    {
        PrintMessage(systemUser, "Choose from the options", 0, messageHistory);

        printf("1 | Open chat\n");
        printf("2 | Join chat\n");
        printf("3 | Print history\n");
    }

    /* Processing the menu choice */
    while (menuChoice[0] < 49 || menuChoice[0] > 51)
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

    else if (51 == menuChoice[0])
    {
        PrintHistory(messageHistory->top);
    }

	main();
}