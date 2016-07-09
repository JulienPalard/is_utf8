is_utf8 is a program and a c library to check if a given file (or stdin) contains only
valid utf-8 sequences.

To compile (The program and the lib) :
 $ make

To run with a string, note how bash can help to forge string from
hexadecimal:

    $ echo $'\xe9' | ./is_utf8 - && echo OK || echo KO

Or to simply test a file:

    $ ./is_utf8 FILE

Which prints nothing it it's OK (and returns 0), of prints a usefull
error message if an invalid byte sequence is found.

The lib and the program will return 0 (success) if the sequence is a
valid utf-8 string. In case an invalid UTF-8 sequence is found they
will return the position of the invalid byte and a verbose message
about why it's invalid.
