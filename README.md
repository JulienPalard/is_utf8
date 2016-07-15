isutf8 is a program and a c library to check if a given file (or stdin) contains only
valid utf-8 sequences.

# Compiling

    $ make

# Test a file

isutf8 returns 0 if the file is correctly encoded. Else, a verbose
error is printed:

    $ ./isutf8 main.c
    $ echo $?
    0

Some files here only contain ASCII or correctly encoded UTF8:

    $ ./isutf8 README.md
    $ ./isutf8 test.sh

But an ELF is clearly not UTF8:

    $ ./isutf8 isutf8
    isutf8: Encoding error on line 1, character 25
    \x08@
    ^ After a first byte between C2 and DF, expecting a 2nd byte between 80 and BF

Nor /dev/urandom:

    $ ./isutf8 /dev/urandom
    /dev/urandom:1:1: Expecting bytes in the following ranges: 00..7F C2..F4

Note that you can make `isutf8` to read on stdin by not giving a
filename, also, note that `bash` helps a lot with the `$''` syntax
allowing to give hexadecimal:

    $ echo $'\xe9' | ./isutf8 -
    (standard input):1:0: After a first byte between E1 and EC, expecting two following bytes.

    $ echo "Hellö world" | iconv -f utf8 -t latin1 | ./isutf8 -
    (standard input):1:4: Expecting bytes in the following ranges: 00..7F C2..F4

# Find non-UTF8 files

As `isutf8` can take multiple arguments it's easy to find
UTF8-compatible / non UTF8-compatible files:

List non-UTF8 compatible files:

    $ isutf8 *
    isutf8:1:40: Expecting bytes in the following ranges: 00..7F C2..F4
    is_utf8.o:1:65: Expecting bytes in the following ranges: 00..7F C2..F4
    libisutf8.so:1:96: Expecting bytes in the following ranges: 00..7F C2..F4
    main.o:1:41: After a first byte of E0, expecting a 2nd byte between A0 and BF.

List UTF8-compatible files:

    $ ./isutf8 --invert *
    COPYRIGHT
    is_utf8.c
    is_utf8.h
    main.c
    Makefile
    README.md
    test.sh

Just list non-UTF8 compatible files:

    $ ./isutf8 --list *
    isutf8
    is_utf8.o
    libisutf8.so
    main.o

Be verbose about the exact error:

    $ ./isutf8 --verbose *
    isutf8:1:40: Expecting bytes in the following ranges: 00..7F C2..F4
    \xB0.
    ^
    is_utf8.o:1:65: Expecting bytes in the following ranges: 00..7F C2..F4
    \x85\xF6H\xC7\x02
    ^
    libisutf8.so:1:96: Expecting bytes in the following ranges: 00..7F C2..F4
    \x8C\x11
    ^
    main.o:1:41: After a first byte of E0, expecting a 2nd byte between A0 and BF.
    \x14
    ^
