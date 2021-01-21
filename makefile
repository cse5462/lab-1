# Makefile for Lab 1

# Setup for system
# Macros for C compiler and flags
CC = gcc
CFLAGS = -g -Wall

# Process to build application
all: count

count: count.c
	$(CC) $(CFLAGS) -o $@ $^

# Target to open lab source code files
openCode: README.md makefile count.c
	code $^
	
# Target to open lab files
openData: example1 example2 example3 example4 *.jpg
	code $^

# Remove executables for clean build
clean:
	rm count
