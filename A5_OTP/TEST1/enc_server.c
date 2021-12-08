#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

//Exploration: Client-Server Communication Via Sockets CANVAS
void error(const char *msg)
{
  perror(msg);
  exit(0);
} //ERROR FUNCTION for reporting issues

int main(int argc, char* argv[])
{
  //https://repl.it/@cs344/83serverc#server.c
  int   listenSocketFD;
  int   connectionSocket;
  int   portNumber;
  int   charsRead;
  int   childExitMethod = -5;
  int   connectionCounter = 0;


  char  pBuffer[70000]; //Plain Buffer
  char  kBuffer[70000]; //Key Buffer
  char  eMessage[70000]; //Encrypted Message

  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;

  pid_t  spawnpid = -5;

  socklen_t   sizeOfClientInfo; //Get the size of the address for when the client will connect

  // Check usage & args
  //https://repl.it/@cs344/83serverc#server.c
  if (argc < 2)
  {
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }

  //Set up the address struct for the server socket
  memset((char *) &serverAddress, '\0', sizeof(serverAddress)); //clear out the address struct
  portNumber = atoi(argv[1]); //Get the portNumber, converts into an integer from a string
  serverAddress.sin_family = AF_INET; //The address should be network capable
  serverAddress.sin_port = htons(portNumber); //Store the port Number
  serverAddress.sin_addr.s_addr = INADDR_ANY; //Allow a client at any address to connect to this server

  //Create the socket that will listen for connections
  listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); //socket created
  if(listenSocketFD < 0)
  {
    error("ERROR opening socket");
  }

  //Associate the socket to the port
  if(bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) //sockett to port
  {
    error("ERROR on binding");
  }

  //start listening for connections. Allow up to 5 connections to queue up
  listen(listenSocketFD, 5); //flips the socket on for 5 connections

  //Accept a connection, blocking if one is not available until one connects
  while(1)
  {
  //Accept the connection request which creates a connection socket
    sizeOfClientInfo = sizeof(clientAddress);
    connectionSocket = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    if(connectionSocket < 0)
    {
      error("ERROR on accept");
    }

    if (connectionCounter == 5) //only allowed 5 connections
    {
      error("TOO MANY CONNECTIONS");
    }
    else
    {
      //Exploration: Pipes & FIFO
      spawnpid = fork();
      connectionCounter++;

      switch(spawnpid)
      {
        case -1:
          perror("FAILED FORK\n");
          exit(1);
          break;


        case 0:
        //printf("START OF CASE 0\n");
          //Get the message from the client and display it

          memset(pBuffer, '\0', sizeof(pBuffer));
          memset(kBuffer, '\0', sizeof(kBuffer));

          //Reads the client's message from the socket
          //https://repl.it/@cs344/83serverc#server.c
          charsRead = recv(connectionSocket, pBuffer, sizeof(pBuffer) - 1, 0);
          if(charsRead < 0)
          {
            error("ERROR reading from socket");
          }
          //printf("SERVER RECIEVE: %d %s\n", strlen(pBuffer), pBuffer);

          charsRead = recv(connectionSocket, kBuffer, sizeof(kBuffer) - 1, 0);
          if(charsRead < 0)
          {
            error("ERROR reading from socket");
          }
          int   i;
          int   ascii_p;
          int   ascii_k;
          int   ascii_e;
          memset(eMessage, '\0', sizeof(eMessage));

          for(i = 0; i < strlen(pBuffer); i++) //alg for encryption EXAMPLE
          {
            ascii_p = pBuffer[i];
            ascii_k = kBuffer[i];
            ascii_e = ascii_p + ascii_k - 65;
            ascii_e = (ascii_e % 27) + 65;
            if(ascii_e == 91)
            {
              ascii_e = 32;
            }
            eMessage[i] = ascii_e;
          } //FOR LOOP
            //Send a Success Message back to the client
          //printf("SERVER SEND: %d %s\n", strlen(eMessage), eMessage);
          charsRead = send(connectionSocket, eMessage, sizeof(eMessage) - 1, 0); //Success message sent back
          if(charsRead < 0)
          {
            error("ERROR writing to socket");
          }
            //close the connection socket for this client
          close(connectionSocket);
          break;

      default:
        childExitMethod = waitpid(spawnpid, &childExitMethod, WNOHANG);
        if(childExitMethod != 0)
        {
          connectionCounter--;
        }
    }

  }

  } //END OF WHILE LOOP
  //close the listening socket
  close(listenSocketFD);
  return 0;

}
