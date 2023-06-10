/*
 * File: server.c
 * Austin Moore
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct employee_info {
	
	char ID[7];
	char name[10];
	double salary;
} employee_info;

int main(argc, argv)
     int     argc;
     char    *argv[];
{
	
  struct employee_info DB[10] = {
  {"abc123", "Tony", 100000.0}, 
  {"def456", "Bob", 200000.0}, 
  {"ghi789", "Chris", 300000.0},
  {"jkl012", "Don", 400000.0},
  {"mno345", "Chris", 500000.0},
  {"pqrs678", "John", 600000.0},
  {"tuv901", "Stacy", 700000.0},
  {"wxy234", "Steve", 800000.0},
  {"zab567", "Emily", 900000.0},
  {"xay937", "Joey", 1000000.0}
  };
  
  int pid;
  struct  sockaddr_in sad;
  int     port;
  struct  sockaddr_in cad;
  int     alen;
  int     welcomeSocket, connectionSocket;
  char    clientSentence[128]; 
  char    employeeData[128]; 
  char    buff[128];
  int     i, n;
  
  /* check command-line argument */
  if (argc > 1) {
    port = atoi(argv[1]);
  } else { 
    printf("Invalid number of command line arguements\n");
    exit(1);
  }
  /* Create socket and bind/listen*/

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  if (welcomeSocket < 0) {
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }
  
  memset((char *)&sad,0,sizeof(sad));
  sad.sin_family = AF_INET;
  sad.sin_addr.s_addr = INADDR_ANY;
  sad.sin_port = htons((u_short)port);
  
  if (bind(welcomeSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    fprintf(stderr,"bind failed\n");
    exit(1);
  }
  
  if (listen(welcomeSocket, 10) < 0) {
    fprintf(stderr,"listen failed\n");
    exit(1);
  }
  
  while (1) {
    alen = sizeof(cad);
    if ( (connectionSocket=accept(welcomeSocket, (struct sockaddr *)&cad, &alen)) < 0) {
      fprintf(stderr, "accept failed\n");
      exit(1);
    }
    
	// fork
	pid = fork();
	
	if (pid < 0) {
		exit(1);
	}
	
	if (pid == 0) {
	
    /* Get the sentence from the client */
    
    clientSentence[0]='\0';
    n=read(connectionSocket,buff,sizeof(buff));
    while(n > 0){
      strncat(clientSentence,buff,n);
      if (buff[n-1]=='\0') break;
      n=read(connectionSocket,buff,sizeof(buff));
    }
    printf("(client sent GETUSERID: %s)\n", clientSentence);
	
	if (strcmp(clientSentence, "STOP") == 0) {
		printf("STOPPING\n");
		close(connectionSocket);
		// exit parent
		if (kill(getppid(), SIGKILL) == -1) {
			printf("couldn't to stop parent process\n");
		}
		exit(0);
	} else {
		
		int salaryID = -1;
		// if no ID
		if (atoi(clientSentence) < 0 || atoi(clientSentence) > 9) {
			// printf("out of bounds");
			char error[6] = "ERROR";
			write(connectionSocket, error, strlen(error)+1);
			close(connectionSocket);
			exit(0);
		} else {
		
		//else
		salaryID = atoi(clientSentence);
		strcpy(employeeData, DB[salaryID].name);
		char salary[34];
		sprintf(salary, ", $%.02f", DB[salaryID].salary);
		strcat(employeeData,salary);
		write(connectionSocket, employeeData, strlen(employeeData)+1);
		
		}
	}
    // exit
    close(connectionSocket);
	exit(0);
	}
  }
}




