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
        out += printf((*str < ' ' || *str > '~') ? "\\x%.2X" : "%c",
                      (unsigned char)*str);
        str++;
        max_length -= 1;
    }
    return out;
}

static void print_utf8_error(
    const char* file_path, int error_line, int error_column, char *str, int pos,
    const char *message, int quiet, int verbose, int list_only, int invert)
{
    int chars_to_error;

    if (quiet)
        return;
    if (message && !invert)
    {
        if (list_only)
            printf("%s\n", file_path);
        else
            printf("%s:%d:%d: %s\n", file_path, error_line, error_column, message);
        if (verbose && !list_only)
        {
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
            printf("\n%*s^\n", (int)(chars_to_error), "");
        }
    }
    if (!message && invert)
    {
        printf("%s\n", file_path);
    }
}

#define handle_error(msg, target)                                   \
    do {retval = EXIT_FAILURE; perror(msg); goto target;} while (0)

static int is_utf8_readline(FILE *stream, const char *file_path,
                            int quiet, int verbose, int list_only, int invert)
{
    char *string = NULL;
    size_t size = 0;
    ssize_t str_length;
    char *message = NULL;
    int lineno = 1;
    int pos = 0;

    while ((str_length = getline(&string, &size, stream)) != -1)
    {
        pos = is_utf8((unsigned char*)string, str_length, &message);
        if (message != NULL)
        {
            break;
        }
        lineno += 1;
    }
    print_utf8_error(file_path, lineno, pos, string, pos, message,
                     quiet, verbose, list_only, invert);
    if (string != NULL)
        free(string);
    return message == NULL ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void count_lines(const char *string, int length, int up_to, int *line, int *column)
{
    int pos = 0;
    int line_start_at = 0;

    *line = 1;
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

static int is_utf8_mmap(const char *file_path, int quiet, int verbose,
                        int list_only, int invert)
{
    char *addr;
    struct stat sb;
    int fd;
    int pos = 0;
    char *message;
    int retval = EXIT_SUCCESS;
    int error_column = 0;
    int error_line = 0;

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
        return is_utf8_readline(fopen(file_path, "r"), file_path,
                                quiet, verbose, list_only, invert);
    }
    pos = is_utf8((unsigned char*)addr, sb.st_size, &message);
    if (message != NULL)
        count_lines(addr, sb.st_size, pos, &error_line, &error_column);
    print_utf8_error(file_path, error_line, error_column, addr, pos, message,
                     quiet, verbose, list_only, invert);
    if (message != NULL)
        retval = EXIT_FAILURE;
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
           "  -i, --invert     list valid UTF-8 files instead of invalid ones\n"
           "  -v, --verbose    print detailed error (multiple lines)\n"
           "\n"
           "This is version %s.\n",
           program_name, VERSION);
}

int main(int ac, char **av)
{
    int quiet = 0;
    int exit_value = EXIT_SUCCESS;
    int i;
    int list_only = 0;
    int invert = 0;
    int verbose = 0;
    int opt;
    struct option options[] = {
        { "help", no_argument, NULL, 'h' },
        { "quiet", no_argument, &quiet, 1 },
        { "list-only", no_argument, &list_only, 1 },
        { "invert", no_argument, &invert, 1 },
        { "verbose", no_argument, &verbose, 1 },
        { 0, 0, 0, 0 }
    };

    while ((opt = getopt_long(ac, av, "hqliv", options, NULL)) != -1) {
        switch (opt) {
            case 0:
                break;

            case 'h':
                usage(av[0]);
                return EXIT_SUCCESS;

            case 'q':
                quiet = 1;
                break;

            case 'l':
                list_only = 1;
                break;

            case 'i':
                invert = 1;
                break;

            case 'v':
                verbose = 1;
                break;

            case '?':
                usage(av[0]);
                return EXIT_FAILURE;

            default:
                usage(av[0]);
                return EXIT_FAILURE;
        }
    }
    if (optind == ac)
    {
        return is_utf8_readline(stdin, "(standard input)", quiet, verbose,
                                list_only, invert);
    }
    else
    {
        for (i = optind; i < ac; ++i)
        {
            if (is_utf8_mmap(av[i], quiet, verbose,
                             list_only, invert) == EXIT_FAILURE)
                exit_value = EXIT_FAILURE;
        }
        return exit_value;
    }
}
