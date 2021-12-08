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
  int   socketFD;
  int   portNumber;
  int   charsWritten;
  int   charsRead;
  int   i;

  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;

  char   pBuffer[70000]; //Plain Buffer
  char   kBuffer[70000]; //Key Buffer

  FILE*  tempFile;

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

  //setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int)); //make socket reusable
  //Connecting To serverAddress
  if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) //Cpnnecting socket to the address
  {
      error("CLIENT: ERROR connecting");
  }

  //Input From User
  memset(pBuffer, '\0', sizeof(pBuffer)); //Cipher Buffer is cleared
  memset(kBuffer, '\0', sizeof(kBuffer)); //Key Buffer is cleared

  tempFile = fopen(argv[1], "r"); // We have to open the file in order to access the content inside, set the content as TempFile
  fgets(pBuffer, sizeof(pBuffer) - 1, tempFile); //Cipher Text is Received
    pBuffer[strcspn(pBuffer, "\n")] = '\0'; //fgets adds a trailing \n this basically rids of it
  fclose(tempFile);

  tempFile = fopen(argv[2], "r"); // We have to open the file in order to access the content inside, set the content as TempFile
  fgets(kBuffer, sizeof(kBuffer) - 1, tempFile); //Key is Received
    kBuffer[strcspn(kBuffer, "\n")] = '\0'; //fgets adds a trailing \n this basically rids of it
  fclose(tempFile);
  //printf("BEFORE FOR LOOP\n");
  for(i = 0; i < strlen(pBuffer); i++) //For Loop Checker to see cases of Bad Characters
  {
    if(pBuffer[i] != 32 && (pBuffer[i] < 65 || pBuffer[i] > 90)) //As we go through the cBuffer Array we check to see if the ASCII values are valid or not.
    {
      error("ERROR: Bad Characters! In pBuffer");
      return 1; //Returns an error basically
    }
  }

  if(strlen(kBuffer) < strlen(pBuffer)) //Checks to see if the Key Buffer is longer than Cipher Buffer, Do this in order to make sure the Key isn't wrong
  {
      error("Key is shorter than plaintext");
      return 1;
  }



  //Write to the Server
  //https://repl.it/@cs344/83clientc#client.c
  //printf("CLIENT SEND: %d %s\n", strlen(pBuffer),pBuffer);
  charsWritten = send(socketFD, pBuffer, strlen(pBuffer), 0);
  if(charsWritten < 0)
  {
      error("CLIENT: ERROR writing to socket");
  }
  if(charsWritten < strlen(pBuffer))
  {
      printf("CLIENT: WARNING: Not all data written to socket!\n");
  }
  //printf("BETWEEN SENDS\n");
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
  //sleep(1);
  //printf("BEFORE RECIEVE\n");
  //Get Return Message from Server
  //https://repl.it/@cs344/83clientc#client.c
  memset(pBuffer, '\0', sizeof(pBuffer)); //Clear Buffer for Reuse
  charsRead = recv(socketFD, pBuffer, sizeof(pBuffer) - 1, 0); //Read Data from the socket, leaving \0 at end
  if(charsRead < 0)
  {
    error("CLIENT: ERROR reading from socket");
  }
  //printf("CLIENT RECIEVE: %d %s\n", strlen(pBuffer),pBuffer);
  printf("%s\n", pBuffer);

  close(socketFD); //close Socket
  return 0;
}
