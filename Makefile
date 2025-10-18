# Makefile для лабораторной работы №1 с main.c и child.c

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/child.c
TARGET = prime_checker
BUILDDIR = build

# Создаем директорию для сборки
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Сборка программы
build: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(SOURCES) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Запуск программы
run: build
	$(BUILDDIR)/$(TARGET)
	
# Очистка
clean:
	rm -rf $(BUILDDIR)

# Псевдонимы
all: build

.PHONY: build run clean all
.DEFAULT_GOAL := build