#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "is_utf8.h"

#define BUFSIZE 4096

static void pretty_print_error_at(char *str, int pos)
{
    str[pos] = '\0';
    fprintf(stdout, "%s\n", str);
}

int main(int ac, char **av)
{
    char buffer[BUFSIZE];
    int  read_retval;
    size_t pos;

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

            if (is_utf8((unsigned char*)buffer, read_retval, &pos))
            {
                pretty_print_error_at(buffer, pos);
                return EXIT_FAILURE;
            }
            else
            {
                fprintf(stdout, "%s\n", buffer);
            }
        }
        return EXIT_SUCCESS;
    }
    return is_utf8((unsigned char*)av[1], strlen(av[1]), NULL);
}
