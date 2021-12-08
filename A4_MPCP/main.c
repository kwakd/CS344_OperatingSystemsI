#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm
#include <string.h>

/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/

// Size of the buffers
#define SIZE 6

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 6

#define BUFFERSIZE 1000

// Buffer 1, shared resource between input thread and square-root thread
char* buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between square root thread and output thread
char* buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the square-root thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){
  char buffer[BUFFERSIZE];
  //memset(buffer, '\0', BUFFERSIZE);
  fgets(buffer, BUFFERSIZE, stdin);
  printf("BUFFER: %s\n", buffer);
  return buffer;
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* item){

  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  while (count_1 != 0) {
    pthread_mutex_unlock(&mutex_1);
    sleep(1);
    pthread_mutex_lock(&mutex_1);
    //pthread_cond_wait(&empty_1, &mutex_1);
  }
  //printf("PUTBUFF1: %s, %s\n", item, buffer_1[prod_idx_1]);
  // Put the item in the buffer
  //buffer_1[prod_idx_1] = malloc(sizeof(char) * BUFFERSIZE);
  //memset(buffer_1[prod_idx_1], '\0', BUFFERSIZE);
  //printf("MEMSET: %s, %s\n", item, buffer_1[prod_idx_1]);
  printf("ITEM: %s\n", item);
  printf("PRODIDX: %d\n", prod_idx_1);
  buffer_1[prod_idx_1] = item;
  printf("PUTBuffer1 Contents at 0: %s\n", buffer_1[0]);
  printf("PUTBuffer1 Contents at %d: %s\n", prod_idx_1, buffer_1[prod_idx_1]);
  //strcpy(buffer_1[prod_idx_1], item);
  //printf("AFTER STRCPY: %s, %s\n", item, buffer_1[prod_idx_1]);
  // Increment the index where the next item will be put.
  prod_idx_1++;
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input(void *args)
{
    int i;
    char* items[NUM_ITEMS];
    for (i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      items[i] = get_user_input();
      //printf("ITEM: %s\n", item);
      put_buff_1(items[i]);
    }
    return;
}

/*
Get the next item from buffer 1
*/
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  printf("Buffer1 Contents at 0: %s\n", buffer_1[0]);
  char* item = buffer_1[con_idx_1];
  //printf("GETBUFF_1: %s, %d\n", item);
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  pthread_cond_signal(&empty_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return item;
}

/*
 Put an item in buff_2
*/

void put_buff_2(char* item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  //buffer_2[prod_idx_2] = malloc(sizeof(char) * BUFFERSIZE);
  //memset(buffer_2[prod_idx_2], '\0', BUFFERSIZE);
  buffer_2[prod_idx_2] = item;
  //strcpy(buffer_2[prod_idx_2], item);
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}


/*
 Function that the square root thread will run.
 Consume an item from the buffer shared with the input thread.
 Compute the square root of the item.
 Produce an item in the buffer shared with the output thread.

*/

void *compute_square_root(void *args)
{
    char* item;
    char* bN;
    int i;
    int f;
    //printf("BEFORE MEMSET SQUAREROOT\n");
    //memset(bN, '\0', BUFFERSIZE);
    for (i = 0; i < NUM_ITEMS; i++)
    {
      //printf("1\n");

      item = get_buff_1();
      printf("ITEM GETBUFF1: %s\n", item);
      //printf("2\n");
      printf("sizeof item: %d\n", strlen(item));
      for(f = 0; f < sizeof(item); f++)
      {
        //printf("%f\n", item[f]);
        if(item[f] == '\n')
        {
          bN[f] = ' ';
        }
        else
        {
          bN[f] = item[f];
        }
      }

      //printf("PUTBUFF2: %s,%s\n", item, bN);
      //printf()
      put_buff_2(bN);

    }
    return NULL;
}

/*
Get the next item from buffer 2
*/

char* get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  char* item = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return item;
}


/*
 Function that the output thread will run.
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/

void *write_output(void *args)
{
    char* item;
    int i;
    for ( i = 0; i < NUM_ITEMS; i++)
    {
      item = get_buff_2();
      printf("OUTPUT: %s\n", item);
    }
    return NULL;
}

int main()
{
    srand(time(0));

    pthread_mutex_init(&mutex_1, NULL);
    pthread_mutex_init(&mutex_2, NULL);
    pthread_t input_t, square_root_t, output_t;


    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&square_root_t, NULL, compute_square_root, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);

    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(square_root_t, NULL);
    pthread_join(output_t, NULL);

    pthread_mutex_destroy(&mutex_1);
    pthread_mutex_destroy(&mutex_2);
    return EXIT_SUCCESS;
}
