#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define INPUTLENGTH 2048

void testInput(char *[], int* , char [], char [], int);
void statusPrinter(int);

int main()
{
  int pid = getpid();
  int exitStatusChecker = 0;
  int backgroundChecker = 0;

  char input[256] = "";
  char output[256] = "";
  char* array[512];
  memset(array, '\0', 512);

  do{
    testInput(array, &backgroundChecker, input, output, pid);
    //printf("%s", array);

    /*2.Comment & Blank Lines*/
    if(array[0][0] == '\0' || array[0][0] == '#')
    {
      //printf("HELLO WORLD\n");
      continue;
    }

    switch(array[0])
    {
    //4.Built in Commands: EXIT
    case 'exit':
    case 'EXIT':
        return 0;

    //4.Built in Commands: cd
    case 'cd':
    case 'CD':
      if(array[1])
      {
        if(chdir(array[1]) == -1)
        {
          printf("Directory not found.\n");
          fflush(stdout);
        }
      }

      else
      {
          chdir(getenv("HOME"));
      }
      break;

    case 'status':
    case 'STATUS':
    //4.Built in Commands: status
      printf("HELLO\n");
      break;


  } //end of switch case
  } while(1);

}

void statusPrinter(int exitStatusChecker)
{
    if(WIFEXITED(exitStatusChecker))
    {
      printf("exit value %d\n", WEXITSTATUS(exitStatusChecker));
    }

    else
    {
      printf("terminated by signal %d\n", WTERMSIG(exitStatusChecker));
    }
}


void testInput(char *array[], int* backgroundChecker, char input[], char output[], int pid)
{
  char UserInput[INPUTLENGTH];
  char space[2] = " ";

  int i; //
  int j; //

  printf(": "); //prints the command line for the user
  fflush(stdout);
  fgets(UserInput, INPUTLENGTH, stdin);



  //looks for a new line or if the token is at a 0, or if there's a new line
  int token = 0;
  for(i = 0; !token && i < INPUTLENGTH; i++)
  {
      if(UserInput[i] == '\n')
      {
          UserInput[i] = '\0';
          token = 1;
      }
  }

  //checks to see if the argument user puts into the shell is blank or not
  if(!strcmp(UserInput, "")) //we use string compare to see if the userinput is equal to "" which is just hitting enter without anything
  {
    array[0] = strdup(""); //fill array[0] with a blank and return back to the loop, loop checks array[0] to see the command, ->
    return; //-> above there's a statement saying if array[0] is '\0' which will continue the program
  }

  //raw input is converted into an individual string
  char *tokenString = strtok(UserInput, space);

  for(i = 0; tokenString; i++)
  {
      if(!strcmp(tokenString, "<"))
      {
        tokenString = strtok(NULL, space);
        strcpy(input, tokenString);
                  printf("END OF TESTINPUT\n");
      }

      else if(!strcmp(tokenString, ">"))
      {
        tokenString = strtok(NULL, space);
        strcpy(output, tokenString);
                  printf("END OF \n");
      }

      else if(!strcmp(tokenString, "&"))
      {
        *backgroundChecker = 1;
                  printf("END \n");
      }

      else
      {

          array[i] = strdup(tokenString);
          for(j = 0; array[i][j]; j++)
          {
              if(array[i][j] == '$' && array[i][j+1] == '$')
              {
                array[i][j] = '\0';
                snprintf(array[i], 256, "%s%d", array[i], pid);
              }
          }
      }

      tokenString = strtok(NULL, space);//Moves onto the next token
  }
}
