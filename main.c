#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "is_utf8.h"

#define BUFSIZE 4096

int showstr(const char *str, unsigned int max_length)
{
    size_t out;

    out = 0;
    while (*str)
    {
        if (max_length <= 0)
            return out;
        out += printf((*str < ' ' || *str > '~') ? "\\x%.2X" : "%c",
                      (unsigned char)*str);
        str++;
        max_length -= 1;
    }
    return out;
}


static void pretty_print_error_at(char *str, int pos)
{
    int chars_to_error;
    char *error;

    error = "^ This byte is not UTF8.";

    if (pos > 10)
    {
        /* Ok, we got some context to print ... */
        str = str + pos - 10; /* Print from 10 char before error. */
        chars_to_error = showstr(str, 10); /* Print up to error. */
        showstr(str + 10, 10); /* Print after error, to get context. */
    }
    else
    {
        /* Error is around the start of the string, we don't have context */
        chars_to_error = showstr(str, pos); /* Print up to error. */
        showstr(str + pos, 10); /* Print after error, to get context. */
    }
    printf("\n%*s\n", chars_to_error + strlen(error), error);
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
                fprintf(stdout, "UTF-8 compliant ! Maybe ASCII !\n");
            }
        }
        return EXIT_SUCCESS;
    }
    return is_utf8((unsigned char*)av[1], strlen(av[1]), NULL);
}
