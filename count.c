#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* The number of command line arguments. */
#define NUM_ARGS 4
/* The maximum number of character for file names. */
#define FILENAME_LEN 20
/* The maximum number of character for error messages. */
#define ERROR_LEN 150
/* The maximum size (in bytes) of the search string. */
#define SEARCH_MAX 20
/* The buffer size (in bytes) for chunking the file.
 * Recommended 5x larger than the search string size to avoid rescan issues over multiple buffers. */
#define BUFFER_LEN 100

void handle_init_error(const char *msg, int errnoSet);
long int get_file_size(FILE *file);
int file_string_match(FILE *inputFile, const char *str);
int buffer_string_match(const char *buffer, const char *str, int buffLength, int strLength);
int string_match(const char *buffer, const char *str, int buffLength, int strLength, int *indx, int *strPos, int *rescanPos);
void print_stats(FILE *stream, long int size, int matches);

/**
 * @brief This program reads a binary file and prints the size of the file in bytes and number
 * of times the search-string specified in the second argument appeared in the file on the
 * screen as well to an output file. If the input-filename is incorrect, or the number of
 * arguments is incorrect, or the output-file cannot be created, the program prints appropriate
 * messages and shows how to correctly invoke it. 
 * 
 * @param argc Non-negative value representing the number of arguments passed to the program
 * from the environment in which the program is run.
 * @param argv Pointer to the first element of an array of argc + 1 pointers, of which the
 * last one is null and the previous ones, if any, point to strings that represent the
 * arguments passed to the program from the host environment. If argv[0] is not a null
 * pointer (or, equivalently, if argc > 0), it points to a string that represents the program
 * name, which is empty if the program name is not available from the host environment.
 * @return If the return statement is used, the return value is used as the argument to the
 * implicit call to exit(). The values zero and EXIT_SUCCESS indicate successful termination,
 * the value EXIT_FAILURE indicates unsuccessful termination.
 */
int main(int argc, char *argv[]) {
	char inputFilename[FILENAME_LEN], outputFilename[FILENAME_LEN], searchStr[SEARCH_MAX];
	FILE *inputFile, *outputFile;
	long int fileSize;
	int strMatches;

	/* Check for the correct number of command line arguments */
	if (argc == NUM_ARGS) {
		/* Extract arguments to their respective variables */
		strcpy(inputFilename, argv[1]);
		strcpy(searchStr, argv[2]);
		strcpy(outputFilename, argv[3]);
	} else {
		handle_init_error("Invalid number of command line arguments", errno);
	}
	/* Attempt to open the input and output files */
	if (!(inputFile = fopen(inputFilename, "rb"))) {
		char msg[ERROR_LEN];
		sprintf(msg, "open_input - \"%s\"", inputFilename);
		handle_init_error(msg, errno);
	}
	if (!(outputFile = fopen(outputFilename, "w"))) {
		char msg[ERROR_LEN];
		sprintf(msg, "open_output - \"%s\"", outputFilename);
		handle_init_error(msg, errno);
	}

	/* Get the file statistics (size and number of matches) */
	fileSize = get_file_size(inputFile);
	strMatches = file_string_match(inputFile, searchStr);

	/* Print the statistics to the terminal and the output file */
	print_stats(stdout, fileSize, strMatches);
	print_stats(outputFile, fileSize, strMatches);

	/* Attempt to close the input and output files */
	if (fclose(inputFile) < 0) {
		printf("ERROR: close_input - \"%s\": %s\n", inputFilename, strerror(errno));
	}
	if (fclose(outputFile) < 0) {
		printf("ERROR: close_output - \"%s\": %s\n", outputFilename, strerror(errno));
	}
	
	return 0;
}

/**
 * @brief Prints a string describing the initialization error and provided error number (if
 * nonzero), the correct command usage, and exits the process signaling unsuccessful termination. 
 * 
 * @param msg The error description message to display.
 * @param errnum This is the error number, usually errno.
 */
void handle_init_error(const char *msg, int errnum) {
	char errorMsg[ERROR_LEN];
	/* Check for valid error code and generate error message */
	if (errnum) {
		sprintf(errorMsg, "ERROR: %s: %s\n", msg, strerror(errnum));
	} else {
		sprintf(errorMsg, "ERROR: %s\n", msg);
	}
	/* Print error messages to the terminal */
	printf(errorMsg);
	printf("Usage is: count <input-filename> <search-string> <output-filename>\n");
	/* Exits the process signaling unsuccessful termination */
	exit(EXIT_FAILURE);
}

/**
 * @brief Gets the size (in bytes) of the provided file and sets the current position to
 * the beginning of the file.
 * 
 * @param file The pointer to a FILE object.
 * @return The file size in bytes.
 */
long int get_file_size(FILE *file) {
	long int size;

	/* Set position to end of file */
	fseek(file, 0, SEEK_END);
	/* Get current byte position */
	size = ftell(file);
	/* Reset position to beginning of file */
	rewind(file);

	return size;
}

/**
 * @brief TODO file_string_match
 * 
 * @param file The pointer to a FILE object.
 * @param str The search string to match.
 * @return The number of matches found in the file.
 */
int file_string_match(FILE *file, const char *str) {
	int rc, strLength, matches = 0;
	char buffer[BUFFER_LEN];
	
	/* Gets the length of the search string */
	strLength = (rc = strlen(str)) < SEARCH_MAX ? rc : SEARCH_MAX;
	/* Reads the file in chunks and counts the total string matches found */
	do {
		rc = fread(buffer, sizeof(char), BUFFER_LEN, file);
		matches += buffer_string_match(buffer, str, rc, strLength);
	} while (rc == BUFFER_LEN);

	return matches;
}

/**
 * @brief TODO buffer_string_match
 * 
 * @param buffer The pointer to the character buffer containing the file chunk to search.
 * @param str The string to match.
 * @param buffLength The length of the character buffer.
 * @param strLength The length of the search string.
 * @return The number of matches found in the buffer.
 */
int buffer_string_match(const char *buffer, const char *str, int buffLength, int strLength) {
	static int strPos = 0;
	int i = 0, matches = 0;

	while (i < buffLength) {
		/* TODO */
		int rescanPos = i + 1;
		/* TODO */
		if (string_match(buffer, str, buffLength, strLength, &i, &strPos, &rescanPos)) {
			matches++;
		}
		/* TODO */
		if (i != buffLength) {
			strPos = 0;
			i = rescanPos;
		} else {
			if (strPos == strLength || buffer[i-1] != (str[strPos-1] & 0xff)) strPos = 0;
		}

	}

	return matches;
}

/**
 * @brief TODO string_match
 * 
 * @param buffer The pointer to the character buffer containing the file chunk to search.
 * @param str The search string to match.
 * @param buffLength The length of the character buffer.
 * @param strLength The length of the search string.
 * @param indx The current index in the character buffer.
 * @param strPos TODO
 * @param rescanPos TODO
 * @return TODO
 */
int string_match(const char *buffer, const char *str, int buffLength, int strLength, int *indx, int *strPos, int *rescanPos) {
	int newRescanPos = 0;
	int possibleMatch = (buffer[(*indx)++] == (str[(*strPos)++] & 0xff)) ? 1 : 0;

	while (possibleMatch && *strPos < strLength) {
		if (*indx == buffLength) {
			possibleMatch = 0;
		} else {
			if (!newRescanPos && (buffer[*indx] == (str[0] & 0xff))) newRescanPos = *indx;
			if (buffer[(*indx)++] != (str[(*strPos)++] & 0xff)) possibleMatch = 0;
		}
		if (!possibleMatch || (*strPos == strLength)) {
			*rescanPos = (newRescanPos) ? newRescanPos : *indx;
		}
	}
	
	return possibleMatch;
}

/**
 * @brief Prints the requested statistics to the provided output stream.
 * 
 * @param stream The pointer to a FILE object that identifies the stream.
 * @param size The file size statistic.
 * @param matches The number of matches statistic.
 */
void print_stats(FILE *stream, long int size, int matches) {
	fprintf(stream, "Size of file is %ld\n", size);
	fprintf(stream, "Number of matches = %d\n", matches);
}
