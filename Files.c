#include "Files.h"

struct File_Info* openFile(char* file_path) {
	int length = strlen(file_path);
	if (file_path[length - 3] != 'j' && file_path[length - 2] != 'p' && file_path[length - 1] != 'g') {
		printf("ERROR: File is not a JPG file.\n");
		return NULL;
	}

	FILE* file = fopen(file_path, "rb"); //open file and read binary
	if (file == NULL) {
		printf("ERROR: Unable to read/find file: \"%s\".\n", file_path);
		return NULL;
	}

	//get size of file
	fseek(file, 0L, SEEK_END);
	unsigned long file_sz = ftell(file);
	rewind(file);
	if (file_sz == 0) {
		printf("ERROR: File: \"%s\" is empty.\n", file_path);
		return NULL;
	}

	struct File_Info* file_info = malloc(sizeof(struct File_Info*));
	if (file_info == NULL) {
		printf("ERROR: unable to allocate memory.\n");
		return NULL;
	}

	file_info->fileptr = file;
	file_info->file_size = file_sz;

	return file_info;
}

char* readFile(struct File_Info* file) {
	char* buffer = malloc(file->file_size);
	if (buffer == NULL) {
		printf("ERROR: Failed to allocate memory for buffer.\n");
		return NULL;
	}

	unsigned long count = fread(buffer, sizeof(char), file->file_size, file->fileptr);
	if (count <= 0) {
		printf("ERROR: File pointer was unable to be read.\n");
		return NULL;
	}

	if ((unsigned char)buffer[0] != 0xFF && (unsigned char)buffer[1] != 0xD8) {
		printf("ERROR: File is not a jpg file, or has been corrupted.\n");
		return NULL;
	}

	return buffer;
}

FILE* createNewFile(char* file_path) {
	int length = strlen(file_path);
	char* new_file = malloc(length + 1);
	if (new_file == NULL) {
		printf("ERROR: Unable to allocate memory for new file.\n");
	}

	//create new file of the same name but with a 'c' at the start
	int index = 0;
	new_file[0] = 'c';
	while (file_path[index] != '\0') {
		new_file[index + 1] = file_path[index];
		index++;
	}
	new_file[index + 1] = '\0';

	FILE* file = fopen(new_file, "wb"); //open file with writing permission
	if (file == NULL) {
		printf("ERROR: Unable to create new file.\n");
		return NULL;
	}

	return file;
}