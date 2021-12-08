#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int i;
    int length = atoi(argv[1]); //length of the key

    char  key; //Placeholder for Key to recieve a random value for ASCII

    srand(time(NULL)); //random key

    for(i = 0; i < length; i++)
    {
        key = (rand() % 27) + 65; //add 65 because the ASCII value for 'A' is 65 therefore in the case we get 0 we add 65 to get to the beginning of Capital Letters ASCII wise
        if(key == 91) //in the case we go beyond 91, 91 == '['
        {
          key = 32; //set it to ' ' instead
        }
        printf("%c", key);
    }

    printf("\n");
    return 0;
}
