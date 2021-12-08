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
  int   i;
  int   ascii_c;
  int   ascii_k;
  int   ascii_p;

  char  cBuffer[70000]; //Cipher Buffer
  char  kBuffer[70000]; //Key Buffer
  char  dMessage[70000]; //decrypted Message

  struct sockaddr_in serverAddress;
  struct sockaddr_in clientAddress;

  pid_t  spawnpid = -5;

  socklen_t   sizeOfClientInfo = sizeof(clientAddress); //Get the size of the address for when the client will connect

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

    if (connectionCounter == 5)
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
          //Get the message from the client and display it
          memset(cBuffer, '\0', sizeof(cBuffer));
          memset(kBuffer, '\0', sizeof(kBuffer));

          //Reads the client's message from the socket
          //https://repl.it/@cs344/83serverc#server.c
          charsRead = recv(connectionSocket, cBuffer, sizeof(cBuffer) - 1, 0);
          if(charsRead < 0)
          {
            error("ERROR reading from socket");
          }
            charsRead = recv(connectionSocket, kBuffer, sizeof(cBuffer) - 1, 0);
            if(charsRead < 0)
            {
              error("ERROR reading from socket");
            }
            memset(dMessage, '\0', sizeof(dMessage));

            for(i = 0; i < strlen(cBuffer); i++) //DECRYPTION OF EXAMPLE BASICALLY IN REVERSE
            {
              ascii_c = cBuffer[i];
              if(ascii_c == 32) // ' '
              {
                ascii_c = 91; //[
              }
              ascii_c -= 65;
              ascii_k = kBuffer[i] - 65;
              ascii_c = ascii_c - ascii_k;
              ascii_p = ascii_c;
              while(ascii_p < 65 && ascii_p != 32)
              {
                ascii_p += 27;
              }
              dMessage[i] = ascii_p;
            } //FOR LOOP
            //Send a Success Message back to the client
            charsRead = send(connectionSocket, dMessage, sizeof(dMessage) - 1, 0); //Success message sent back
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
