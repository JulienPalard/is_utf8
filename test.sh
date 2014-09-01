#!/bin/bash

utf8_test()
{
    to_test="$1"
    should_return="$2"
    should_print="$3"

    answer="$(echo "$1" | ./is_utf8 -)"
    error_level=$?
    if ! [ z"$error_level" == z"$should_return" ]
    then
        echo "Wrong error level, got $error_level should be $should_return"
        exit 1
    fi
    if ! [ z"$answer" == z"$should_print" ]
    then
        echo "Wrong answer, got $answer, should be $should_print"
        exit 1
    fi
}

utf8_test "$(echo Привет$(echo мир | iconv -f utf-8 -t cp1251))" 1 "Привет"
utf8_test "$(echo Привет$(echo мир | iconv -f utf-8 -t utf-8))" 0 "Приветмир"

echo "All tests are OK."