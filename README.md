This is the README file for Lab_1

NAME: Conner Graham
DATE: 01/21/2021

DESCRIPTION:
This lab contains a program called "count" which reads a
binary file (in chunks) as input and prints the following statistics to the screen, as well as to a specified output-file:
- size of the input-file in bytes
- number of times the search-string specified appeares in
  the intput-file

If the input-filename is incorrect, or the number of
arguments is incorrect, or the output-file cannot be
created, the program prints appropriate messages and
shows how to correctly invoke the program. 

USAGE: ($ denotes the command prompt)
$ count <input-filename> <search-string> <output-filename>

If any of the argument strings contain whitespace, those
arguments will need to be enclosed in quotes.

ASSUMPTIONS:
- It is assumed that each argument string retrieved from
  the command line will be NULL terminated with no other
  NULL characters present within the string.
- It is required that the length of the search-string be
  <= the file chunking size, however it is recommended
  that the chunks be 5x larger than the search string.
- It is assumed that if a file has been opened
  successfully, there will be no read/write issues that
  occur when performing these operations.
- It is assumed that the number of matches found is <=
  the max SIGNED INT value.
