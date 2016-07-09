is_utf8 is a program and a c library to check if a given file (or stdin) contains only
valid utf-8 sequences.

# Compiling

    $ make

# Test a file

is_utf8 returns 0 if the file is correctly encoded. Else, a verbose
error is printed:

    $ ./is_utf8 main.c
    $ echo $?
    0

Some files here only contain ASCII or correctly encoded UTF8:

    $ ./is_utf8 README.md
    $ ./is_utf8 test.sh

But an ELF is clearly not UTF8:

    $ ./is_utf8 is_utf8
    is_utf8: Encoding error on line 1, character 25
    \x08@
    ^ After a first byte between C2 and DF, expecting a 2nd byte between 80 and BF

Nor /dev/urandom:

    $ ./is_utf8 /dev/urandom
    Encoding error on line 1, character 2
    rO\xBFfI\xE2\xDBz}gM~
      ^ Expecting bytes in the following ranges: 00..7F C2..F4

Note that you can make `is_utf8` to read on stdin by giving `-` as a
filename, also, note that `bash` helps a lot with the `$''` syntax
allowing to give hexadecimal:

    $ echo $'\xe9' | ./is_utf8 -
    Encoding error on line 1, character 0
    \xE9\x0A
    ^ After a first byte between E1 and EC, expecting two following bytes.

    $ echo "Hellö world" | iconv -f utf8 -t latin1 | ./is_utf8 -
    Encoding error on line 1, character 4
    Hell\xF6 world\x0A
        ^ Expecting bytes in the following ranges: 00..7F C2..F4
