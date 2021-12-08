// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>

#define SUFFIX ".csv"

/* struct for movie information */
struct movie
{
    char *title;
    int year;
    char *lang;
    float rating;
    struct movie *next;
};

/* Parse the current line which is space delimited and create a
*  student struct with the data in this line
*/
struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the Year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);
    //strcpy(currMovie->year, token);

    // The next token is the Languages
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->lang = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->lang, token);

    // The last token is the Rating
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = atof(token);
    //strcpy(currMovie->rating, token);

    // Set the next node to NULL in the newly created student entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of students by parsing data from
* each line of the specified file.
*/
struct movie *processFile(char *filePath)
{
    /* Open the specified file for reading only*/
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Get a new student node corresponding to the current line
        struct movie *newNode = createMovie(currLine);
        // Is this the first node in the linked list?
        if (head == NULL)
        {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
    }
    free(currLine);
    fclose(movieFile);
    return head;
}


/*BubbleSort Code from geeksforgeeks https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/ */
void bubbleSort(struct movie* start)
{
    //geeks for geeks
    int swapped, i;
    struct movie *ptr1;
    struct movie *lptr = NULL;

    if(start == NULL)
    {
      return;
    }
    do
    {
        swapped = 0;
        ptr1 = start;
        while(ptr1->next != lptr)
        {
            if(ptr1->year > ptr1->next->year)
            {
              swap(ptr1, ptr1->next);
              swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while(swapped);

}

/*BubbleSort Code from geeksforgeeks https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/ */
void swap(struct movie* a, struct movie* b)
{
    //year
    int tempYear = a->year;
    a->year = b->year;
    b->year = tempYear;

    //title
    char* tempTitle = a->title;
    a->title = b->title;
    b->title = tempTitle;

    //Languages
    char* tempLang = a->lang;
    a->lang = b->lang;
    b->lang = tempLang;

    //rating
    float tempRating = a->rating;
    a->rating = b->rating;
    b->rating = tempRating;

}

void createMovieFiles(struct movie* aMovie, char* directoryPath)
{
  int fd;
  int currentYear = 0; //tempYear
  char* currentMovieName; //tempMovie
  char filePath[1000];
  char year[1000];
  //all of these variables are tempHolders for when sorting the MovieRating


  bubbleSort(aMovie);//linked list is now sorted by year

  struct movie* tempMovie = aMovie;

  while(tempMovie != NULL)
  {
    if (tempMovie->year != 0)
    {
      /* Referencing Exploration: Directories - Getting File and Directory Meta-Data */
      memset(filePath, '\0', sizeof(filePath));

      strcpy(filePath, directoryPath);
      strcat(filePath, "/");

      sprintf(year, "%d", tempMovie->year);

      strcat(filePath, year);
      strcat(filePath, SUFFIX);

      //printf("%s\n", tempMovie->title);

      /*Referencing Exploration:Files- Reading and Writing Files */
      fd = open(filePath, O_WRONLY | O_CREAT | O_APPEND, 0640);
      int howMany = write(fd, tempMovie->title, strlen(tempMovie->title) + 1); //writes movie title
        //printf("%d\n", howMany);
      howMany = write(fd, "\n", 1); //writes a new line
        //printf("%d\n", howMany);
      close(fd);
    }
      //printf("%s\n",tempMovie->title);
      tempMovie = tempMovie->next;
  }

}


void printMainMenu()
{
    printf("1. Select file to process\n");
    printf("2. Exit the program\n");
    printf("Enter a choice 1 or 2: ");
}

void printSelectFileMenu()
{
  printf("Which file you want to process?\n");
  printf("Enter 1 to pick the largest file\n");
  printf("Enter 2 to pick the smallest file\n");
  printf("Enter 3 to specify the name of a file\n");
  printf("Enter a choice from 1 to 3: ");
}

void processFileMovie(char *tempLargeNameHolder)
{
  int randomNumber;
  randomNumber = rand() % 100000;

  char onidName[10000]; //destination
  char randomNumberSource[10000]; //source

  strcpy(onidName, "kwakd.movie."); //copies string pointed to by src to dest, in this case kwakd.movie -> destination and onidName is source so kwakd.movie -> onidName

  sprintf(randomNumberSource, "%d", randomNumber);

  strcat(onidName, randomNumberSource); //appends the two so it'll be written as one

  mkdir(onidName, 0750); //Exploration:Permissions
  printf("Created directory with name %s\n\n", onidName);

  opendir(onidName); //opens file

  struct movie *list = processFile(tempLargeNameHolder);
  //printf("%s\n", onidName);
  createMovieFiles(list, onidName);
  //printf("%s\n", onidName);
  //printf("%d\n", randomNumber);
}

void LargeFileD()
{
    int tempLargeFileHolder = 0;
    char tempLargeNameHolder[1000];

    struct stat dirStat;

    char *string;

  //Exploration: Directories
      DIR *currDir = opendir(".");
      struct dirent *aDir;

      while((aDir = readdir(currDir)) != NULL)
      {
        string = strrchr(aDir->d_name, '.');
        if(string != NULL && strcmp(string, ".csv") == 0)
        {
          stat(aDir->d_name, &dirStat);
          if(dirStat.st_size > tempLargeFileHolder)
          {
            memset(tempLargeNameHolder, '\0', sizeof(tempLargeNameHolder));
            strcpy(tempLargeNameHolder, aDir->d_name);
            tempLargeFileHolder = dirStat.st_size;
          }
        }
      }
      printf("Now processing the chosen file named %s\n\n", tempLargeNameHolder);
      processFileMovie(tempLargeNameHolder);


      closedir(currDir);
}

void SmallFileD()
{
    int tempLargeFileHolder = INT_MAX;
    char tempLargeNameHolder[1000];

    struct stat dirStat;

    char *string;

  //Exploration: Directories
      DIR *currDir = opendir(".");
      struct dirent *aDir;

      while((aDir = readdir(currDir)) != NULL)
      {
        string = strrchr(aDir->d_name, '.');
        if(string != NULL && strcmp(string, ".csv") == 0)
        {
          stat(aDir->d_name, &dirStat);
          if(dirStat.st_size < tempLargeFileHolder)
          {
            memset(tempLargeNameHolder, '\0', sizeof(tempLargeNameHolder));
            strcpy(tempLargeNameHolder, aDir->d_name);
            tempLargeFileHolder = dirStat.st_size;
          }
        }
      }
      printf("Now processing the chosen file named %s\n\n", tempLargeNameHolder);
      processFileMovie(tempLargeNameHolder);


      closedir(currDir);
}


void SameFileD(char* user_Input_FileName)
{
      DIR *currDir = opendir(".");
      struct dirent *aDir;

      int check = 0;

      while((aDir = readdir(currDir)) != NULL)
      {
        if(strcmp(user_Input_FileName, aDir->d_name) == 0)
        {
            printf("Now processing the chosen file named %s\n\n", user_Input_FileName);
            processFileMovie(user_Input_FileName);
            check = 1;
        }
      }

      if(check == 0)
      {
        printf("The file %s was not found. Try again.\n\n", user_Input_FileName);
      }

      closedir(currDir);
}

/*
*   Process the file provided as an argument to the program to
*   create a linked list of student structs and print out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o students main.c
*/

int main(int argc, char *argv[])
{

    int user_Input_MainMenu;
    int user_Input_SelectFileMenu;
    int user_Input_Loop_Checker = 1;

    char user_Input_Language[1000];
    char user_Input_FileName[1000];

    srand(time(0)); //random number based on time



    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./students student_info1.txt\n");
        return EXIT_FAILURE;
    }


    do
    {
        printMainMenu();
        scanf("%d", &user_Input_MainMenu);
        printf("\n");

        if(user_Input_MainMenu == 1)
        {
          do {
            printSelectFileMenu();
            scanf("%d", &user_Input_SelectFileMenu);
            printf("\n");

            if(user_Input_SelectFileMenu == 1)
            {
              LargeFileD();
              user_Input_Loop_Checker = 0;
            }
            else if(user_Input_SelectFileMenu == 2)
            {
              SmallFileD();
              user_Input_Loop_Checker = 0;
            }
            else if(user_Input_SelectFileMenu == 3)
            {
              printf("Enter the complete file name: ");
              scanf("%s", &user_Input_FileName);
              SameFileD(user_Input_FileName);
              user_Input_Loop_Checker = 0;
            }
            else
            {
              printf("You entered an incorrect choice. Try again.\n\n");
            }
          } while(user_Input_Loop_Checker == 1);
        }
        else if(user_Input_MainMenu == 2)
        {
          return EXIT_SUCCESS;
        }
        else
        {
          printf("You entered an incorrect choice. Try again.\n\n");
        }
    } while(1);
}
