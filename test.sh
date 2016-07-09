#!/bin/bash

utf8_test_file()
{
    printf "."
    to_test="$1"
    should_return="$2"
    ./is_utf8 <(printf "%s" "$to_test") 2>/dev/null >/dev/null
    error_number=$?
    if ! [ z"$error_number" == z"$should_return" ]
    then
        [ $should_return == 1 ] &&
        printf "\nThis one should have failed:\n" ||
        printf "\nThis one should succeed:\n"
        printf "%s" "$to_test" | hexdump -C
        printf "%s" "$to_test" | ./is_utf8 <(printf "%s" "$to_test")
        exit 1
    fi
}

utf8_test_pipe()
{
    printf "."
    to_test="$1"
    should_return="$2"
    printf "%s" "$to_test" | ./is_utf8 - 2>/dev/null >/dev/null
    error_number=$?
    if ! [ z"$error_number" == z"$should_return" ]
    then
        [ $should_return == 1 ] &&
        printf "\nThis one should have failed:\n" ||
        printf "\nThis one should succeed:\n"
        printf "%s" "$to_test" | hexdump -C
        printf "%s" "$to_test" | ./is_utf8 -
        exit 1
    fi
}

utf8_test()
{
    utf8_test_pipe "$@"
    utf8_test_file "$@"
}

should_pass()
{
    while [ -n "$1" ]
    do
        utf8_test "$1" 0
        shift
    done
}

should_fail()
{
    while [ -n "$1" ]
    do
        utf8_test "$1" 1
        shift
    done
}

should_pass "Léa" "Maïté" "Pauline" "Élise"
should_fail $'\xc9lise' $'Elis\xc9'

# Tests from :
# Table 3-7. Well-Formed UTF-8 Byte Sequences
# -----------------------------------------------------------------------------
# |  Code Points        | First Byte | Second Byte | Third Byte | Fourth Byte |
# |  U+0000..U+007F     |     00..7F |             |            |             |
should_pass $'\01' $'\02' q w e r t y "~" "foo" "bar" "baz" "123" 1 2 3 "," $'\n' $'\x0E' $'\x0F' $'\x7F'
should_fail $'\x80' $'\x82' $'\x83' $'\xC0' $'\xC1'
should_fail $'|\x80' $'|\x82' $'|\x83' $'|\xC0' $'|\xC1'
should_fail $'\x80|' $'\x82|' $'\x83|' $'\xC0|' $'\xC1|'
# |  U+0080..U+07FF     |     C2..DF |      80..BF |            |             |
should_pass $'\xC2\x80' $'\xC2\xBF' $'\xDF\x80' $'\xDF\xBF'
should_fail $'\xC2\x79' $'\xC2\xC0' $'\xC2\xC3'
should_pass $'---\xC2\x80' $'---\xC2\xBF' $'---\xDF\x80' $'---\xDF\xBF'
should_fail $'\xC2\x79---' $'\xC2\xC0---' $'\xC2\xC3---'
# |  U+0800..U+0FFF     |         E0 |      A0..BF |     80..BF |             |
should_pass $'\xE0\xA0\x80' $'\xE0\xA0\xBF' $'\xE0\xBF\x80' $'\xE0\xBF\xBF'
should_fail $'\xE0\x99\x80' $'\xE0\xC5\xBF' $'\xE0\xBF\x78' $'\xE0\xBF\xEE'
should_pass $'~~~~\xE0\xA0\x80' $'~~~~\xE0\xA0\xBF' $'~~~~\xE0\xBF\x80' $'~~~~\xE0\xBF\xBF'
should_fail $'\xE0\x99\x80~~~~' $'\xE0\xC5\xBF~~~~' $'\xE0\xBF\x78~~~~' $'\xE0\xBF\xEE~~~~'
# |  U+1000..U+CFFF     |     E1..EC |      80..BF |     80..BF |             |
should_pass $'\xE1\x80\x80' $'\xE1\xBF\x80' $'\xE1\x80\xBF' $'\xE1\xBF\xBF'
should_pass $'\xEC\x80\x80' $'\xEC\xBF\x80' $'\xEC\x80\xBF' $'\xEC\xBF\xBF'
should_fail $'\xE1\x41\x80' $'\xE1\xC0\x80' $'\xE1\xC8\xBF' $'\xE1\xBF\xFE'
# |  U+D000..U+D7FF     |         ED |      80..9F |     80..BF |             |
should_pass $'\xED\x80\x80' $'\xED\x80\xBF' $'\xED\x9F\x80' $'\xED\x9F\xBF'
should_fail $'\xED\x80\x50' $'\xED\x80\xC1' $'\xED\xBF\x80' $'\xED\xBF\xBF'
# |  U+E000..U+FFFF     |     EE..EF |      80..BF |     80..BF |             |
should_pass $'\xEE\x80\x80' $'\xEE\x80\xBF' $'\xEE\xBF\x80' $'\xEE\xBF\xBF'
should_fail $'\xEE\x70\x80' $'\xEE\x80\x70' $'\xEE\xCF\x80' $'\xEE\xCF\xCF'
# |  U+10000..U+3FFFF   |         F0 |      90..BF |     80..BF |      80..BF |
should_pass $'\xF0\x90\x80\x80' $'\xF0\xBF\x80\x80' $'\xF0\x90\x80\xBF' $'\xF0\xBF\xBF\xBF'
should_fail $'\xF0\x70\x80\x80' $'\xF0\xCF\x70\x80' $'\xF0\xCF\x80\xCE' $'\xF0\xCF\xCF\xDF'
should_fail $'\xF0\x80\x80\x80'
# |  U+40000..U+FFFFF   |     F1..F3 |      80..BF |     80..BF |      80..BF |
should_pass $'\xF1\x80\x80\x80' $'\xF1\xBF\xBF\xBF'
should_pass $'\xF2\x80\x80\x80' $'\xF2\xBF\xBF\xBF'
should_pass $'\xF3\x80\x80\x80' $'\xF3\xBF\xBF\xBF'
# |  U+100000..U+10FFFF |         F4 |      80..8F |     80..BF |      80..BF |
should_pass $'\xF4\x80\x80\x80' $'\xF4\x8F\xBF\xBF'
should_fail $'\xF4\x80\x80\x79' $'\xF4\xBF\xBF\xBF'
# -----------------------------------------------------------------------------

should_fail $'\xf0-' $'\xf1-' $'\xf2-' $'\xf3-' $'\xE1-' $'\xEE-' $'\xED-' $'\xEF-'

printf "\nAll tests are OK.\n"
