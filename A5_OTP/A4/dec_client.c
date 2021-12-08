#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//Exploration: Client-Server Communication Via Sockets CANVAS
void error(const char *msg)
{
  perror(msg);
  exit(1);
} //ERROR FUNCTION for reporting issues

int main(int argc, char *argv[])
{
  // https://canvas.oregonstate.edu/courses/1784217/pages/exploration-sockets?module_item_id=19893124
  int   socketFD;
  int   portNumber;
  int   charsWritten;
  int   charsRead;
  int   i;

  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;

  char   cBuffer[70000]; //Cipher Buffer
  char   kBuffer[70000]; //Key Buffer

  FILE *tempFile;

  //Check usage & args
  if(argc < 4)
  {
    fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
    exit(0);
  }

  //Setting Up Server Address struct
  memset((char*) &serverAddress, '\0', sizeof(serverAddress));  //Clears up the address struct
  portNumber = atoi(argv[3]); //Retrieves Port Number, and converts to an integer from a string
  serverAddress.sin_family = AF_INET; //Address should be network capable
  serverAddress.sin_port = htons(portNumber); //Store Port portNumber
  serverHostInfo = gethostbyname("localhost"); //Get DNS entry for this host name
  if(serverHostInfo == NULL)
  {
      fprintf(stderr, "CLIENT: ERROR, no such host\n");
      exit(0);
  }
  memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);   // Copy the first IP address from the DNS entry to sin_addr.s_addr

  //Setting Up Sockets
  //https://canvas.oregonstate.edu/courses/1784217/pages/exploration-client-server-communication-via-sockets?module_item_id=19893125
  socketFD = socket(AF_INET, SOCK_STREAM, 0); //Socket is Created
  if(socketFD < 0)
  {
      error("CLIENT: ERROR opening socket");
  }

  //Connecting To serverAddress
  if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) //Cpnnecting socket to the address
  {
      error("CLIENT: ERROR connecting");
  }

  //Input From User
  memset(cBuffer, '\0', sizeof(cBuffer)); //Cipher Buffer is cleared
  memset(kBuffer, '\0', sizeof(kBuffer)); //Key Buffer is cleared

  tempFile = fopen(argv[1], "r"); // We have to open the file in order to access the content inside, set the content as TempFile
  fgets(cBuffer, sizeof(cBuffer) - 1, tempFile); //Cipher Text is Received
    cBuffer[strcspn(cBuffer, "\n")] = '\0'; //fgets adds a trailing \n this basically rids of it
  fclose(tempFile);

  tempFile = fopen(argv[2], "r"); // We have to open the file in order to access the content inside, set the content as TempFile
  fgets(kBuffer, sizeof(kBuffer) - 1, tempFile); //Key is Received
    kBuffer[strcspn(kBuffer, "\n")] = '\0'; //fgets adds a trailing \n this basically rids of it
  fclose(tempFile);

  if(strlen(kBuffer) < strlen(cBuffer)) //Checks to see if the Key Buffer is longer than Cipher Buffer, Do this in order to make sure the Key isn't wrong
  {
      error("Key is shorter than Cipher");
      return 1;
  }

  for(i = 0; i < strlen(cBuffer); i++) //For Loop Checker to see cases of Bad Characters
  {
                                                              //cBuffer[i] != 'SPACE' && cBuffer < A && cBuffer > Z
    if(cBuffer[i] != 32 && cBuffer[i] < 65 && cBuffer[i] > 90) //As we go through the cBuffer Array we check to see if the ASCII values are valid or not.
    {
      error("ERROR: Bad Characters! In cBuffer");
      return 1; //Returns an error basically
    }
  }

  //Write to the Server
  //https://repl.it/@cs344/83clientc#client.c
  charsWritten = send(socketFD, cBuffer, strlen(cBuffer), 0);
  if(charsWritten < 0)
  {
      error("CLIENT: ERROR writing to socket");
  }
  if(charsWritten < strlen(cBuffer))
  {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
  }
  sleep(1);

  charsWritten = send(socketFD, kBuffer, strlen(kBuffer), 0);
  if(charsWritten < 0)
  {
      error("CLIENT: ERROR writing to socket");
  }
  if(charsWritten < strlen(kBuffer))
  {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  //Get Return Message from Server
  //https://repl.it/@cs344/83clientc#client.c
  memset(cBuffer, '\0', sizeof(cBuffer)); //Clear Buffer for Reuse
  charsRead = recv(socketFD, cBuffer, sizeof(cBuffer) - 1, 0); //Read Data from the socket, leaving \0 at end
  if(charsRead < 0)
  {
    error("CLIENT: ERROR reading from socket");
  }
  printf("%s\n", cBuffer);

  close(socketFD); //close Socket
  return 0;



}
