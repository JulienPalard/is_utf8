is_utf8 is a program and a c library to check if a given file (or stdin) contains only
valid utf-8 sequences.

To compile (The program and the lib) :

    $ make

To run with a string, note how bash can help to forge string from
hexadecimal:

    $ echo $'\xe9' | ./is_utf8 -
    Encoding error on line 1, character 0
    \xE9\x0A
    ^ After a first byte between E1 and EC, expecting two following bytes.

Or to simply test a file, note that 0 is returned and nothing is
printed is the file is correctly encoded:

    $ ./is_utf8 main.c
    $ ./is_utf8 README.md
    $ ./is_utf8 test.sh

    $ ./is_utf8 is_utf8
    is_utf8: Encoding error on line 1, character 25
    \x08@
    ^ After a first byte between C2 and DF, expecting a 2nd byte between 80 and BF

    $ ./is_utf8 /dev/urandom
    Encoding error on line 1, character 2
    rO\xBFfI\xE2\xDBz}gM~
      ^ Expecting bytes in the following ranges: 00..7F C2..F4


Which prints nothing it it's OK (and returns 0), of prints a usefull
error message if an invalid byte sequence is found.

The lib and the program will return 0 (success) if the sequence is a
valid utf-8 string. In case an invalid UTF-8 sequence is found they
will return the position of the invalid byte and a verbose message
about why it's invalid.
