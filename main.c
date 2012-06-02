#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "is_utf8.h"

#define BUFSIZE 4096

int main(int ac, char **av)
{
    char buffer[BUFSIZE];
    int  read_retval;

    if (ac != 2)
    {
        fprintf(stderr, "USAGE: %s STRING or - for stdin.\n", av[0]);
        return EXIT_FAILURE;
    }
    if (strcmp(av[1], "-") == 0)
    {
        while (42)
        {
            read_retval = read(0, buffer, BUFSIZE);
            if (read_retval == 0)
                return EXIT_SUCCESS;
            if (read_retval == -1)
            {
                perror("read");
                return EXIT_FAILURE;
            }
            if (is_utf8((unsigned char*)buffer, read_retval) != 0)
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return is_utf8((unsigned char*)av[1], strlen(av[1]));
}
