# Простой Makefile для лабораторной работы №1

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

SRCDIR = src
SOURCES = $(SRCDIR)/main.c
TARGET = prime_checker
BUILDDIR = build

# Создаем директорию для сборки
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Сборка программы
build: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(SOURCES)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Запуск программы
run: $(BUILDDIR)/$(TARGET)
	$(BUILDDIR)/$(TARGET)
	
# Очистка
clean:
	rm -rf $(BUILDDIR)

# Псевдонимы
all: build

.PHONY: build run clean all
.DEFAULT_GOAL := build