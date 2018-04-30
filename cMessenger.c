/* Compile:
    gcc cMessenger.c -o cMessenger
*/
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

#define SYSTEMACTION "--------------------"

/*GLOBAL DECLARATION*/
/* Structures */
struct User
{
    char userName[16];

    /* Reference: https://misc.flogisoft.com/bash/tip_colors_and_formatting */
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
void PrintMessage(struct User*, char*, int);
void PrintHistory(struct Message*);

/* Global variables */
/* Reference: https://stackoverflow.com/questions/11709929/how-to-initialize-a-pointer-to-a-struct-in-c */
struct User* systemUser = &(struct User){ .userName = "cMessenger", .userColor = 100 };
struct User* currentUser = NULL;

struct MessageHistory* messageHistory = NULL;
/**/


/*FUNCTIONS*/
/* Create a listening host using a socket */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ */
int CreateServer()
{
	/* Socket data */
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    /**/
      
    /* Create socket file descriptor */
    if (0 == (server_fd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
      
    /* Forcefully attach socket to the port 8080 */
    if (0 > bind(server_fd, (struct sockaddr*)&address, sizeof(address)))
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    /* Wait for a client to make a connection */
    if (0 > listen(server_fd, 3))
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    /* Create a new connected socket and establish the connection */
    if (0 > (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)))
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

     printf("Server got connection from client %s\n", inet_ntoa(address.sin_addr));

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    return 0;
}

/* Create a client by connecting to a listening socket at a specified IP address */
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ */
int CreateClient()
{
    /* Socket data */
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    char ipServer[64];
    /**/

    if (0 > (sock = socket(AF_INET, SOCK_STREAM, 0)))
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    while (0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr))
    {
    	/* Receive an IP address from the user */
    	{
	    	AddMessage(messageHistory, systemUser, "Enter IP", 0);
	    	{
		    	printf("%s\n", SYSTEMACTION);
			    PrintMessage(systemUser, "Type 'x' to connect to 127.0.0.1 (localhost)", 1);
		        strncpy(ipServer, ProcessMessage(64), 64);

		        if ('x' == ipServer[0])
		        {
		        	strncpy(ipServer, "127.0.0.1", 64);
		        }
	    	}


	        AddMessage(messageHistory, currentUser, ipServer, 0);
    	}

        if(0 >= inet_pton(AF_INET, ipServer, &serv_addr.sin_addr)) 
        {
            printf("\nInvalid address or address not supported \n");
        }
    }
  
    if (0 > connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        printf("\nPort is closed \n");
        return -1;
    }

    send(sock, hello, strlen(hello), 0);
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
    	AddMessage(messageHistory, systemUser, "Type in your nickname", 1);

        strncpy(userPtr->userName, ProcessMessage(16), 16);

        currentUser = userPtr;

        AddMessage(messageHistory, userPtr, userPtr->userName, 0);
    }

    /* Choose color */
    {
        int i;
        char* tempColor = (char*)malloc(sizeof(char[64]));

        /* Print various colors */
        AddMessage(messageHistory, systemUser, "Choose color", 0);

        /* Print system action */
    	{
	        printf("%s\n", SYSTEMACTION);
	        for (i = 1; i < 6; i++)
	        {
	            printf("\x1b[97;%dm %d - %s \x1b[0m ", i + 40, i, "message");
	        }
	        printf("\n%s\n", SYSTEMACTION);
    	}

        while (userColorInput[0] < 49 || userColorInput[0] > 53)
        {
            strncpy(userColorInput, ProcessMessage(1), 1);
        }

        userPtr->userColor = atoi(userColorInput) + 40;

        /* Reference: http://forums.codeguru.com/showthread.php?347081-itoa-error */
        sprintf(tempColor, "%d", userPtr->userColor - 40);

        AddMessage(messageHistory, userPtr, tempColor, 0);
    }

    return userPtr;
}

/* A universal string processing method that includes systems calls.
    Allocates memory for a string with the size specified.
    By default, returns the pointer to the string.
    */
char* ProcessMessage(int size)
{
    /* Declaration */
    char* messageStr = (char*)malloc(sizeof(char[size]));
    const int cLAlign = 18;
    int i;
    /**/

    /* Print standard system action UI */
    {
	    printf("%s\n", SYSTEMACTION);
	    printf("message >");

	    for (i = strlen("message >"); i < cLAlign; i++)
	        printf(" ");

	    scanf("%s", messageStr);
	    printf("%s\n", SYSTEMACTION);
	}

    /* Process the string */
    /* If it starts with "/", it contains a system call */
    if (47 == messageStr[0])
    {
        /* If "/c", close the program */
        if (99 == messageStr[1])
        {
        	AddMessage(messageHistory, systemUser, "Goodbye!", 0);

            free(messageStr);

            exit(0);
        }
        /* If "/h", print help commands */
        else if (104 == messageStr[1])
        {
        	printf("%s\n", SYSTEMACTION);
	    	PrintMessage(systemUser, "List of system commands", 0);
	    	PrintMessage(systemUser, "/c | Close the program", 1);
	    	PrintMessage(systemUser, "/h | Help", 1);
	    	printf("%s\n", SYSTEMACTION);
        }

        free(messageStr);
        messageStr = ProcessMessage(size);
    }
    else
    {
    }

    /* Return the string */
    return messageStr;
}

/* Print a message */
void PrintMessage(struct User* user, char* messageStr, int woName)
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

    /* Fill out the space before left alignment */
    while (indentation < cLAlign)
    {
        printf(" ");
        indentation++;
    }

    /* If the message is right-aligned, align according to right alignment */
    if (isRight)
    {
        while (indentation < (cRAlign - strlen(messageStr) - strlen(user->userName)))
        {
            printf(" ");
            indentation++;
        }
    }

    /* Print the message */
    printf("\x1b[97;%dm %s \x1b[0m\n", user->userColor, messageStr);
}

/* Add a new node to the dynamic structure */
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
    }
    else
    {
        while (NULL != iMessage->next)
        {
            iMessage = iMessage->next;
        }

        iMessage->next = tempMessage;
    }

    PrintHistory(messageHistory->top);
}

/* Traverse the dynamic strucre of messages */
void PrintHistory(struct Message* currentMessage)
{
    struct Message* iMessage;

    system("clear");

    iMessage = currentMessage;
    while (NULL != iMessage)
    {
        PrintMessage(iMessage->sender, iMessage->message, iMessage->indentation);
        iMessage = iMessage->next;
    }
}


/* Main logic */
int main()
{
	/* Declaration */
	char menuChoice[1];
    char* strPtr;

    messageHistory = (struct MessageHistory*)malloc(sizeof(struct MessageHistory));
	messageHistory->top = NULL;
	/**/

	while (1)
	{
		strncpy(menuChoice, "\0", 1);

	    if (NULL == currentUser)
	    {
	        /* Welcome message */
	       	AddMessage(messageHistory, systemUser, "Welcome!", 0);

	        /* Creating a user */
	        CreateUser();
	        /**/
	    }

	    /* Printing main menu */
	    {
	    	AddMessage(messageHistory, systemUser, "Choose from the options", 0);

	    	{
	    		printf("%s\n", SYSTEMACTION);
		    	PrintMessage(systemUser, "1 | Open chat", 1);
		    	PrintMessage(systemUser, "2 | Join chat", 1);
		    	printf("%s\n", SYSTEMACTION);
		    }
	    }

	    /* Processing the menu choice */
	    while (menuChoice[0] < 49 || menuChoice[0] > 50)
	    {
	        strncpy(menuChoice, ProcessMessage(1), 1);
	    }

	    AddMessage(messageHistory, currentUser, menuChoice, 0);

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
	}
}