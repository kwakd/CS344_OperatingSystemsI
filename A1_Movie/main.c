// If you are not compiling with the gcc option --std=gnu99, then
// uncomment the following line or you might get a compiler warning
//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
struct movie *processFile(char *filePath, int *n)
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
        n += 1;
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

/*
* Print data for the given movie year
*/
void printMovieYear(struct movie* aMovie, int n, int user_Input_Year){
  int flag = 0;
  struct movie* tempMovie = aMovie;

  while(tempMovie != NULL)
  {
      //printf("USERINPUTYEAR: %d\n",user_Input_Year);
      //printf("AMOVIE: %d\n",tempMovie->year);
      if(tempMovie->year == user_Input_Year)
      {
          printf("%s\n", tempMovie->title);
          flag = 1;
      }
      tempMovie = tempMovie->next;
  }

  if(flag == 0){
    printf("No data about movies released in the year %d\n", user_Input_Year);
  }
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

/*Prints the movie rating with bubblesort, the linked list is sorted out first by Year
afterwards goes through the list, IF the year is different from the year thats next in the linked LIST
the code will then decide there are no more movies left in that year and display the rating that was saved*/
void printMovieRating(struct movie* aMovie)
{
  int currentYear = 0; //tempYear
  float currentRating = 0.0; //tempRating
  char* currentMovieName; //tempMovie
  //all of these variables are tempHolders for when sorting the MovieRating

  bubbleSort(aMovie);//linked list is now sorted by year

  struct movie* tempMovie = aMovie;

  while(tempMovie != NULL)
  {
    if (tempMovie->year > currentYear && currentYear != 0) //Once tempMovie Year is bigger than the currentYear it'll print the statement due to how currenYear is the temp holder for the biggest value
    {
      printf("%d %.2f %s\n", currentYear, currentRating, currentMovieName);
      currentRating = 0.0;
    }

    if (tempMovie->rating > currentRating) //in the case it's the same year and there are diff ratings this one will hold the biggest rating then move onto the other to compare
    {
      currentYear = tempMovie->year;
      currentRating = tempMovie->rating;
      currentMovieName = tempMovie->title;
    }

      //printf("%s\n",tempMovie->title);
      tempMovie = tempMovie->next;
  }
  printf("%d %.2f %s\n", currentYear, currentRating, currentMovieName); //prints the last values
}

/* Simply prints the movie languages, by comparing the language in the struct with the user's input for language
using the strstr it simply see's if the string has the user's INPUT within the string if it does it'll show the user the movie */

void printMovieLanguage(struct movie* aMovie, int n, char user_Input_Language[])
{
    int flag = 0; /*flag to indicate if there's a match if flag is at 0 there were no matches and tell the user there was nothing*/
    struct movie* tempMovie = aMovie;

    while(tempMovie != NULL)
    {
        if(strstr(tempMovie->lang, user_Input_Language) != NULL) /*using strstr instead of strcmp because lang values are right now at EX: [ENGLISH,FRENCH] as one whole word, therefore to seperate it, it's only looking for the key word now in the string*/

        {
            printf("%d %s\n", tempMovie->year, tempMovie->title);
            flag = 1; //indicate how there was a match
        }
      tempMovie = tempMovie->next;
    }
    if(flag == 0)
    {
        printf("No data about movies released in %s\n", user_Input_Language);
    }
}

void printMainMenu()
{
    printf("1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of release of all movies in a specific language\n");
    printf("4. Exit from the program\n\n");
    printf("Enter a choice from 1 to 4: ");
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
    int user_Input_Year;
    char user_Input_Language[1000];

    int n = 0; //TOTAL LIST COUNTER

    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./students student_info1.txt\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1], n);

    printf("Processed file movie_sample_1.csv and parsed data for 24 movies\n\n");


    /* only way to exit this while loop is to enter 4 otherwise it'll keep looping
    the main menu, in the case the user puts in any other number other than 1,2,3,4 the
    program will tell the user they made an incorrect choice and to try again, however
    in the case of a character such as 'x' will crash the program */
    while(1)
    {
      printMainMenu();
      scanf("%d", &user_Input_MainMenu);

      switch(user_Input_MainMenu)
      {
          case 1:
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &user_Input_Year);
            printMovieYear(list, n, user_Input_Year);
            printf("\n");
            break;

          case 2:
            printMovieRating(list);
            printf("\n");
            break;

          case 3:
            printf("Enter the language for which you want to see movies: ");
            scanf("%s", &user_Input_Language);
            printMovieLanguage(list, n, user_Input_Language);
            printf("\n");
            break;

          case 4:
            return EXIT_SUCCESS;

          default:
            printf("You entered an incorrect choice. Try again.\n\n");
            break;
      }
    }




}
