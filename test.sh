#!/bin/bash

utf8_test()
{
    to_test="$1"
    should_return="$2"
    should_print="$3"

    answer="$(printf "$1" | ./is_utf8 -)"
    error_level=$?
    if ! [ z"$error_level" == z"$should_return" ]
    then
        printf "Wrong error level, got: %s should be: %s" $error_level $should_return
        exit 1
    fi
    if [ ! -z "$3" ]
    then
        if ! [ z"$answer" == z"$should_print" ]
        then
            printf "Wrong answer, got:\n%s\nshould be:\n%s\n" "$answer" "$should_print"
            exit 1
        fi
    fi
}

utf8_test "Léa" 0
utf8_test "Maïté" 0
utf8_test "Pauline" 0
utf8_test "Élise" 0
utf8_test $'\xc9\x6c\x69\x73\x65' 1 "\xC9lise
^ This byte is not UTF8."
utf8_test $'Elis\xc9' 1 "Elis\xC9
    ^ This byte is not UTF8."

utf8_test "$(echo Привет$(echo мир | iconv -f utf-8 -t cp1251))" 1 "\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82\xEC\xE8\xF0
                                        ^ This byte is not UTF8."
utf8_test "$(echo Привет$(echo мир | iconv -f utf-8 -t utf-8))" 0 "UTF-8 compliant ! Maybe ASCII !"

echo "All tests are OK."