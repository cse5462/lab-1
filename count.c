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
 * @brief TODO main
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

	/* TODO */
	if (argc != NUM_ARGS) {
		handle_init_error("Invalid number of command line arguments", errno);
	} else {
		strcpy(inputFilename, argv[1]);
		strcpy(searchStr, argv[2]);
		strcpy(outputFilename, argv[3]);
	}
	if (!(inputFile = fopen(inputFilename, "rb"))) {
		char msg[ERROR_LEN];
		sprintf(msg, "open_input - \"%s\"", inputFilename);
		handle_init_error(msg, errno);
	}
	if (!(outputFile = fopen(outputFilename, "wb"))) {
		char msg[ERROR_LEN];
		sprintf(msg, "open_output - \"%s\"", outputFilename);
		handle_init_error(msg, errno);
	}

	/* TODO */
	fileSize = get_file_size(inputFile);
	strMatches = file_string_match(inputFile, searchStr);

	/* TODO */
	print_stats(stdout, fileSize, strMatches);
	print_stats(outputFile, fileSize, strMatches);

	/* TODO */
	if (fclose(inputFile) < 0) {
		perror("ERROR: close_input");
	}
	if (fclose(outputFile) < 0) {
		perror("ERROR: close_output");
	}
	
	return 0;
}

/**
 * @brief TODO handle_init_error
 * 
 * @param msg 
 * @param errnum 
 */
void handle_init_error(const char *msg, int errnum) {
	char errorMsg[ERROR_LEN];
	if (errnum) {
		sprintf(errorMsg, "ERROR: %s: %s\n", msg, strerror(errnum));
	} else {
		sprintf(errorMsg, "ERROR: %s\n", msg);
	}
	printf(errorMsg);
	printf("Usage is: count <input-filename> <search-string> <output-filename>\n");
	exit(EXIT_FAILURE);
}

/**
 * @brief Get the file size object
 * 
 * @param file 
 * @return TODO
 */
long int get_file_size(FILE *file) {
	long int size;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	return size;
}

/**
 * @brief TODO file_string_match
 * 
 * @param file 
 * @param str 
 * @return int 
 */
int file_string_match(FILE *file, const char *str) {
	int rc, strLength, matches = 0;
	char buffer[BUFFER_LEN];
	
	strLength = (rc = strlen(str)) < SEARCH_MAX ? rc : SEARCH_MAX;

	do {
		rc = fread(buffer, sizeof(char), BUFFER_LEN, file);
		matches += buffer_string_match(buffer, str, rc, strLength);
	} while (rc == BUFFER_LEN);

	return matches;
}

/**
 * @brief TODO buffer_string_match
 * 
 * @param buffer 
 * @param str 
 * @param buffLength 
 * @param strLength 
 * @return int 
 */
int buffer_string_match(const char *buffer, const char *str, int buffLength, int strLength) {
	static int strPos = 0;
	int i = 0, matches = 0;

	while (i < buffLength) {
		int rescanPos = i + 1;
		if (string_match(buffer, str, buffLength, strLength, &i, &strPos, &rescanPos)) {
			matches++;
		}
		/* TODO */
		if (i != buffLength) {
			strPos = 0;
			i = rescanPos;
		} else {
			if (buffer[i-1] != (str[strPos-1] & 0xff)) strPos = 0;
		}

	}
	return matches;
}

/**
 * @brief TODO string_match
 * 
 * @param buffer 
 * @param str 
 * @param buffLength 
 * @param strLength 
 * @param indx 
 * @param strPos 
 * @param rescanPos 
 * @return int 
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
 * @brief TODO print_stats
 * 
 * @param stream 
 * @param size 
 * @param matches 
 */
void print_stats(FILE *stream, long int size, int matches) {
	fprintf(stream, "Size of file is %ld\n", size);
	fprintf(stream, "Number of matches = %d\n", matches);
}
