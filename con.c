#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
In this program :
### Run Normally ### 
>>> gcc -o con con.c && ./con

### Run with stdin replaced by test.txt file. 
Assuming such file exists. ###
>>> gcc -o con con.c && ./con < test.txt
*/

int main(void)
{
    char *buffer;
    size_t bufsize = 100;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }


    while (1) {
        characters = getline(&buffer,&bufsize, stdin);
        if (characters == -1){
            break;
        }
        printf("Character Len : %d", characters);
        printf("--- Consumed :: %s", buffer);
        sleep(1);
    }

    free(buffer);
    return 0;
}