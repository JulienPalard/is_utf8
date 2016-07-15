#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/mman.h>

#include "is_utf8.h"

#define VERSION "1.2"

static int showstr(const char *str, unsigned int max_length)
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

#define handle_error(msg, target)                                   \
    do {retval = EXIT_FAILURE; perror(msg); goto target;} while (0)

static int is_utf8_readline(FILE *stream, int quiet)
{
    char *string;
    size_t size;
    ssize_t str_length;
    char *message;
    int lineno;
    int pos;

    lineno = 1;
    string = NULL;
    size = 0;
    while ((str_length = getline(&string, &size, stream)) != -1)
    {
        pos = is_utf8((unsigned char*)string, str_length, &message);
        if (message != NULL)
        {
            if (!quiet)
            {
                fprintf(stderr, "Encoding error on line %d, character %d\n", lineno, pos);
                pretty_print_error_at(string, pos, message);
            }
            free(string);
            return EXIT_FAILURE;
        }
        lineno += 1;
    }
    if (string != NULL)
        free(string);
    return EXIT_SUCCESS;
}

static void count_lines(const char *string, int length, int up_to, int *line, int *column)
{
    int pos;
    int line_start_at;

    pos = 0;
    *line = 1;
    line_start_at = 0;
    while (pos < length && pos < up_to)
    {
        if (string[pos] == '\n')
        {
            line_start_at = pos + 1;
            *line += 1;
        }
        pos += 1;
    }
    *column = up_to - line_start_at;
}

static int is_utf8_mmap(const char *file_path, int quiet)
{
    char *addr;
    struct stat sb;
    int fd;
    int pos;
    char *message;
    int retval;
    int error_column;
    int error_line;

    retval = EXIT_SUCCESS;
    fd = open(file_path, O_RDONLY);
    if (fd == -1)
        handle_error("open", err_open);
    if (fstat(fd, &sb) == -1)           /* To obtain file size */
        handle_error("fstat", err_fstat);
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED)
    {
        /* Can't nmap, maybe a pipe or whatever, let's try readline. */
        close(fd);
        return is_utf8_readline(fopen(file_path, "r"), quiet);
    }
    pos = is_utf8((unsigned char*)addr, sb.st_size, &message);
    if (message != NULL)
    {
        if (!quiet)
        {
            count_lines(addr, sb.st_size, pos, &error_line, &error_column);
            fprintf(stderr, "%s: Encoding error on line %d, character %d\n",
                    file_path, error_line, error_column);
            pretty_print_error_at(addr, pos, message);
        }
        retval = EXIT_FAILURE;
    }
    munmap(addr, sb.st_size);
err_fstat:
    close(fd);
err_open:
    return retval;
}

static void usage(const char *program_name) {
    printf("Usage: %s [OPTION]... [FILE]...\n"
           "Check whether input files are valid UTF-8.\n"
           "\n"
           "  -h, --help       display this help text and exit\n"
           "  -q, --quiet      suppress all normal output\n"
           "  -l, --list       print only names of FILEs containing invalid UTF-8\n"
           "  -i, --invert     list valid UTF-8 files instead of invalid ones"
           ""
           "This is version %s.\n",
           program_name, VERSION);
}

int main(int ac, char **av)
{
    int quiet;
    int exit_value;
    int i;
    int list_only;
    int invert;
    struct option options[] = {
        { "help", no_argument, NULL, 'h' },
        { "quiet", no_argument, &quiet, 1 },
        { "list-only", no_argument, &list_only, 1 },
        { "invert", no_argument, &invert, 1 },
        { 0, 0, 0, 0 }
    };
    int opt;

    quiet = 0;
    list_only = 0;
    invert = 0;
    while ((opt = getopt_long(ac, av, "hqli", options, NULL)) != -1) {
        switch (opt) {
            case 0:
                break;

            case 'h':
                usage(av[0]);
                return EXIT_SUCCESS;
                break;

            case 'q':
                quiet = 1;
                break;

            case 'l':
                list_only = 1;
                break;

            case 'i':
                invert = 1;
                break;

            case '?':
                return EXIT_FAILURE;

            default:
                return EXIT_FAILURE;
        }
    }
    if (list_only != 0)
        quiet = 1;
    if (optind == ac)
    {
        return is_utf8_readline(stdin, quiet);
    }
    else
    {
        exit_value = EXIT_SUCCESS;
        for (i = optind; i < ac; ++i)
        {
            if (is_utf8_mmap(av[i], quiet) == EXIT_FAILURE)
            {
                if (list_only && !invert)
                    printf("%s\n", av[i]);
                exit_value = EXIT_FAILURE;
            }
            else
            {
                if (list_only && invert)
                    printf("%s\n", av[i]);
            }
        }
        return exit_value;
    }
}
