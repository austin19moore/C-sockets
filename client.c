/*
 * File: client.c
 * Austin Moore
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// handle signal
static int sigrecieved = 0;
void my_sig_handler(int signo) {
	sigrecieved = 1;
}

main(argc, argv)
     int     argc;
     char    *argv[];
{ 
  struct  sockaddr_in sad;
  int     clientSocket;
  struct  hostent  *ptrh;
  
  char    *host;
  int     port;
  
  char    Sentence[128]; 
  char    modifiedSentence[128]; 
  char    buff[128];
  int     n;
  
  if (argc != 3) {
    fprintf(stderr,"Usage: %s server-name port-number\n",argv[0]);
    exit(1);
  }
  
  // wait for SIGINT
  struct sigaction act;
  sigset_t blockmask, sigmask;
  act.sa_handler = my_sig_handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaction(SIGINT, &act, NULL);
  sigemptyset(&blockmask);
  sigemptyset(&sigmask);
  sigaddset(&blockmask, SIGINT);
  sigprocmask(SIG_BLOCK, &blockmask, NULL);
  printf("Waiting for Ctrl-C...\n");
  while (sigrecieved == 0) {
	  sigsuspend(&sigmask);
  }
  printf("\nCtrl-C pressed.\n");
  
  // get hostname and port num from command line args
  host = argv[1];
  port = atoi(argv[2]);
  
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    fprintf(stderr, "couldn't make socket\n");
    exit(1);
  }

  // connect

  memset((char *)&sad,0,sizeof(sad));
  sad.sin_family = AF_INET;
  sad.sin_port = htons((u_short)port);
  ptrh = gethostbyname(host);
  if ( ((char *)ptrh) == NULL ) {
    fprintf(stderr,"bad host: %s\n", host);
    exit(1);
  }
  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
  
  if (connect(clientSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    fprintf(stderr,"connect failed\n");
    exit(1);
  }
  
  // Read a salaryID
  
  printf("Enter a salary ID (or STOP): "); gets(Sentence);
  
  // Send the salaryID to server
  
  write(clientSocket, Sentence, strlen(Sentence)+1);
  
  if (strcmp(Sentence, "STOP") == 0) {
	  exit(0);
  }
  
  // Get the info from the server
  modifiedSentence[0]='\0';
  n=read(clientSocket, buff, sizeof(buff));
  while(n > 0){
    strncat(modifiedSentence,buff,n);
    if (buff[n-1]=='\0') break;
    n=read(clientSocket, buff, sizeof(buff));
  }
  printf("(Server sent: %s)\n", modifiedSentence);
  if (strcmp(modifiedSentence, "ERROR") == 0) {
	  printf("ERROR: ID not in range.\n");
	  close(clientSocket);
  } else {
  printf("Employee salary: %s\n", modifiedSentence);
  }
  /* Close the socket. */
  
  close(clientSocket);
  
}

