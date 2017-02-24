#ifndef _IS_UTF8_H
#define _IS_UTF8_H

#include <stdlib.h>

size_t is_utf8(unsigned char *str, size_t len, char **message, int *faulty_bytes);

#endif /* _IS_UTF8_H */
