/* Compile:
    gcc cMessenger.c -o cMessenger
*/
/* Source code: https://www.geeksforgeeks.org/socket-programming-cc/ */
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
    
    /* Breaks the program on Mac; is not nessesary */
    /* Forcefully attaching socket to the port 8080*/
    /*if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }*/

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
        printf("enter IP:\n");
        printf("-> ");scanf("%s",ipServer);

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
main()
{
	/**/
	char menuChoice;
	/**/

	printf("Type in your name: ");
	scanf("%s", &USERNAME);

	printf("1: Open chat\n");
	printf("2: Join chat\n");
	printf("-> ");scanf("%s", &menuChoice);

	/* Open chat */
	if (49 == menuChoice)
	{
		CreateServer();
	}
    /* Join chat */
    else if (50 == menuChoice)
    {
        CreateClient();
    }

	return 0;
}