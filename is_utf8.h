#ifndef _IS_UTF8_H
#define _IS_UTF8_H

#include <stdlib.h>

int is_utf8(unsigned char *str, size_t len, size_t * first_invalid_pos);

#endif /* _IS_UTF8_H */
