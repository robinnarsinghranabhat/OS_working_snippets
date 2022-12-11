#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(void)
{
    int i = 1;

    while (1) {
        printf("Producer : At index : %d\n", i);
        fflush(stdout);
        sleep(3);
        i++;
    }

}