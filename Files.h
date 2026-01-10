#pragma once
#include <stdlib.h>
#include <stdio.h>

struct File_Info {
	FILE* fileptr;
	unsigned long file_size;
};

struct File_Info* openFile(char* file_path);

char* readFile(struct File_Info* file);

FILE* createNewFile(char* file_path);