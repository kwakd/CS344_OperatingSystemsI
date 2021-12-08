#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>


int  accessBackground = 1;

void catch_signal(int);
void sh_execute(char *[], char[], char[], struct sigaction, int*, int*);
void status_printer(int);
void test_input(char *[], char [], char [], int*, int);


int main()
{
  int pid = getpid();
  int exitStatusChecker = 0, backgroundChecker = 0;
  char input[256] = "", output[256] = "";

  char* array[512];
  memset(array, '\0', 512);


	struct sigaction sa_sigint = {0};
	sa_sigint.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_sigint, NULL);

	struct sigaction sa_sigtstp = {0};
	sa_sigtstp.sa_handler = catch_signal;
	sigaction(SIGTSTP, &sa_sigtstp, NULL);

  while(1){
    test_input(array, input, output, &backgroundChecker, pid); //goes into the function in order to get the user's input
    //printf("%s", array);

    /*2.Comment & Blank Lines*/
    if(array[0][0] == '\0') //blank line handling
    {
      //printf("HELLO WORLD\n");
      backgroundChecker = 0; //resetting the values
  		input[0] = '\0'; //resetting the values
  		output[0] = '\0'; //resetting the values
      memset(array, '\0', 512); //resetting the values
      continue; //breaks out of this if statements
    }

    else if(array[0][0] == '#') //comment line handling "#THIS COMMENT SHOULD DO NOTHING"
    {
      backgroundChecker = 0; //same comments as above, resetting the values
  		input[0] = '\0';
  		output[0] = '\0';
      memset(array, '\0', 512);
      continue;
    }

    //4.Built in Commands: status
    else if(strcmp(array[0], "status") == 0 || strcmp(array[0], "STATUS") == 0) //for the input of STATUS or status
    {
      status_printer(exitStatusChecker); //goes into the status_printer function and prints the status
    }
    //4.Built in Commands: cd
    else if(strcmp(array[0], "cd") == 0 || strcmp(array[0], "CD") == 0)
    {
      if(array[1]) //we look for the second part of the array because 'cd xxxx' with xxxx being the filename its the second part of the argument
      {
        if(chdir(array[1]) == -1) //chdir checks the current directory, if it returns -1 that means it found nothing
        {
          printf("Directory not found.\n");
          fflush(stdout);
        }
      }

      else
      {
          chdir(getenv("HOME"));
      }
    }

    //4.Built in Commands: EXIT
    else if(strcmp(array[0], "exit") == 0 || strcmp(array[0], "EXIT") == 0) //strcmp brings back true or false in numerical value
    //in the case of 0 this means true which brings us to this else if
    {
      return 0; //exits the program
    }

    else
    {
      //printf("ELSE\n");
      sh_execute(array, input, output, sa_sigint, &exitStatusChecker, &backgroundChecker);
    }
    /*
    for (int i = 0; array[i]; i++) {
			array[i] = NULL;
		}*/
		backgroundChecker = 0; //reset values
		input[0] = '\0';
		output[0] = '\0';
    memset(array, '\0', 512);

  } //end of do while loop, the while condition is 1 so that this do while loop goes forever
  //the only way to exit the program is by typing 'exit' since this will also return 0
  return 0; //here just in case
}

void catch_signal(int sig_num) {

	// If it's 1, set it to 0 and display a message reentrantly
	if (accessBackground == 1) {
		printf("Now entering FOREGROUND-ONLY mode, & will be ignored\n");
		accessBackground = 0;
	}

	// If it's 0, set it to 1 and display a message reentrantly
	else {
		printf ("Now exiting FOREGROUND-ONLY mode\n");
		accessBackground = 1; //toggle
	}
}

void sh_execute(char *array[], char input[], char output[], struct sigaction sa, int* childExitStatusChecker, int* backgroundChecker)
{
    int   inputFile, outputFile, result;

    pid_t pid = -5;
    //printf("SH_EXECUTE\n");
    pid = fork(); //creates a new process, a value is returned, and the cases are listed below


    if (pid == -1) //if we get -1 from fork() that means there was an error
    {
      perror("Ruh roh!\n"); //Ruh roh!
      exit(1); //abnormal termination of program
    }

    else if (pid == 0) //if we get 0 from fork() we were able to return t
    {
      sa.sa_handler = SIG_DFL; //setting the signal handler from ignore to default
      sigaction(SIGINT, &sa, NULL); //binding SIGINT again
      if(strcmp(input, "")) //makes sure file name isnt blank
      {
        input = open(input, O_RDONLY); //Opens the file with O_RDONLY
        if(inputFile == -1) //file fails to open it sends back -1
        {
            perror("Can't open input file\n");
            exit(1); //exits
        }

        result = dup2(inputFile, 0); //Resource 6: Input & Output Redirection, makes sure input file can be read
        if(result == -1) //input file cant be assigned
        {
            perror("Can't assign input file\n");
            exit(2);
        }
        fcntl(input, F_SETFD, FD_CLOEXEC); //performs SETFD action //https://man7.org/linux/man-pages/man2/fcntl.2.html
      }

      if(strcmp(outputFile, ""))
      {
          outputFile = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666); //Opens the file with O_WRONLY
          if(outputFile == -1) //file cant be opened
          {
            perror("Can't open output file\n");
            exit(1);
          }
          result = dup2(outputFile, 1); //setting up for output //https://man7.org/linux/man-pages/man2/dup.2.html
          if(result == -1) //output file cant be assigned
          {
            perror("Can't assign output file\n");
            exit(2);
          }

          fcntl(output, F_SETFD, FD_CLOEXEC);
      }

      if(execvp(array[0], (char* const*)array)) //executes command //https://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/e/execvp.html
      {
        printf("%s: no such file or directory\n", array[0]);
        exit(2);
      }
    }
    else //for the parent
    {
      if(*backgroundChecker && accessBackground) //checks to see if its background mode
      {
        pid_t realPid = waitpid(pid, childExitStatusChecker, WNOHANG); //not hanging, allows you to run in the background
        printf("background pid is %d\n", pid);
      }
      else
      {
        pid_t realPid = waitpid(pid, childExitStatusChecker, 0); //this essentially waits for the process of the PID to finish, not running in the background
      }

      while((pid = waitpid(-1, childExitStatusChecker, WNOHANG)) > 0) //waiting for any child process to finish //https://linux.die.net/man/2/waitpid
      {
        printf("child %d terminated\n", pid);
        status_printer(*childExitStatusChecker);
      }
    }
}

void status_printer(int exitStatusChecker)
{
    if(WIFEXITED(exitStatusChecker)) //if the child process was terminated normally this will return a nonzero value
    {
      printf("exited with value: %d\n", WEXITSTATUS(exitStatusChecker)); //If WIFEXITED is true, WEXITSTATUS will return a low-order 8bit exist tatus value from the child
    }

    else
    {
      printf("signal %d terminated\n", WTERMSIG(exitStatusChecker)); //in the case it wasn't handled properly sends a signal to WTERMSIG saying how the child process terminated was not handled
    }
}


void test_input(char *array[], char input[], char output[], int* backgroundChecker, int pid)
{
  char UserInput[2048];
  char space[2] = " ";

  int i, j; //for loop for 2d array


  printf(": "); //prints the command line for the user
  fflush(stdout);
  fgets(UserInput, 2048, stdin); //gets the user input, puts it into userInput



  //looks for a new line or if the token is at a 0, or if there's a new line
  int token = 0; //token is the checker
  for(i = 0; !token && i < 2048; i++) //goes through the length of the user's input
  {
      if(UserInput[i] == '\n') //loops through the input until it finds the 'enter' or new line
      {
          UserInput[i] = '\0'; //we set the userinput to nothing essentially resetting it
          token = 1; //set the token to 1 to break out of this for loop
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
        tokenString = strtok(NULL, space); //moves the tokenString one over to make space for the file name
        strcpy(input, tokenString); //copies file name into INPUT
                  //printf("END OF TESTINPUT\n");
      }

      else if(!strcmp(tokenString, ">"))
      {
        tokenString = strtok(NULL, space); //same thing as above
        strcpy(output, tokenString);
                  //printf("END OF \n");
      }

      else if(!strcmp(tokenString, "&"))
      {
        *backgroundChecker = 1; //set to 1 to indicate background mode
                  //printf("END \n");
      }

      else
      {
          array[i] = strdup(tokenString); //https://man7.org/linux/man-pages/man3/strdup.3.html#:~:text=The%20strdup()%20function%20returns,copies%20at%20most%20n%20bytes.
          for(j = 0; array[i][j]; j++)
          {
              if(array[i][j] == '$' && array[i][j+1] == '$') //if the argument is $$ in the argument line
              {
                array[i][j] = '\0';
                snprintf(array[i], 256, "%s%d", array[i], pid); //changing the $ char into a pid and then replacing the $ in the array with the pid
              }
          }
      }
      tokenString = strtok(NULL, space);//Moves onto the next token
  }
}
