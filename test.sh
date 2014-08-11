
answer=$(echo Привет$(echo мир | iconv -f utf-8 -t koi8-r) | ./is_utf8 -)
error_level=$?
[ $error_level == 1 ] || exit 1
[ $answer == "Привет" ] || exit 1

answer=$(echo Привет$(echo мир | iconv -f utf-8 -t cp1251) | ./is_utf8 -)
error_level=$?
[ $error_level == 1 ] || exit 1
[ $answer == "Привет" ] || exit 1

answer=$(echo Привет$(echo мир | iconv -f utf-8 -t utf-8) | ./is_utf8 -)
error_level=$?
[ $error_level == 0 ] || exit 1
[ $answer == "Приветмир" ] || exit 1

echo "All tests are OK"
exit 0
