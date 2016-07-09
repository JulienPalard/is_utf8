#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
        out += fprintf(stderr, (*str < ' ' || *str > '~') ? "\\x%.2X" : "%c",
                       (unsigned char)*str);
        str++;
        max_length -= 1;
    }
    return out;
}


static void pretty_print_error_at(char *str, int pos, const char *message)
{
    int chars_to_error;

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
    fprintf(stderr, "\n%*s^ %s\n", (int)(chars_to_error), "", message);
}

int main(int ac, char **av)
{
    char buffer[BUFSIZE];
    int read_retval;
    int pos;
    int source_fd;
    char *message;

    if (ac != 2)
    {
        fprintf(stderr, "USAGE: %s FILE\n    Use '-' as a FILE to read from stdin.\n", av[0]);
        return EXIT_FAILURE;
    }
    if (strcmp(av[1], "-") == 0)
        source_fd = 0;
    else
        source_fd = open(av[1], O_RDONLY);
    if (source_fd == -1)
    {
        perror("open");
        return EXIT_FAILURE;
    }
    while (42)
    {
        read_retval = read(source_fd, buffer, BUFSIZE);
        if (read_retval == 0)
            return EXIT_SUCCESS;
        if (read_retval == -1)
        {
            perror("read");
            return EXIT_FAILURE;
        }
        pos = is_utf8((unsigned char*)buffer, read_retval, &message);
        if (message != NULL)
        {
            pretty_print_error_at(buffer, pos, message);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
