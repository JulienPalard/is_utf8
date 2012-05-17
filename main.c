#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "is_utf8.h"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        fprintf(stderr, "USAGE: %s string\n", av[0]);
        return EXIT_FAILURE;
    }
    return is_utf8((unsigned char*)av[1], strlen(av[1]));
}
