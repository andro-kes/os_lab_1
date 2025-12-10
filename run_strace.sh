#!/bin/bash
set -euo pipefail

# 1) Сборка
make build

# 2) Запуск strace: следим за форками (-f), печатаем timestamps (-tt),
#    увеличиваем длину строк (-s 200), декодируем файловые дескрипторы (-y),
#    результат в strace.log
printf "numbers.txt\n" | strace -f -tt -s 200 -y -o strace.log ./build/prime_checker

# 3) Получить краткую статистику системных вызовов
printf "numbers.txt\n" | strace -c -f -s 200 ./build/prime_checker 2> strace_summary.txt || true

echo "Готово. Файлы: strace.log, strace_summary.txt"