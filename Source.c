#include "JPEG.h"

void argHandler(int argc, char** argv);
void printHelp();

int main(int argc, char** argv) {
	srand(time(NULL));
	argHandler(argc, argv);
	return 0;
}

void argHandler(int argc, char** argv) {
	char* file_path = argv[1];
	struct File_Info* file_info = openFile(file_path);
	if (file_info == NULL || file_info->fileptr == NULL || file_info->file_size == 0) {
		return;
	}

	char* buffer = readFile(file_info);
	if (buffer == NULL) {
		return;
	}

	struct Segment_Info* head = getSegments(buffer, file_info->file_size);
	if (head == NULL) {
		printf("ERROR: Unable to detect any markers in file.\n");
		return;
	}

	if (head == NULL) {
		return;
	}
	struct RNG_Options* ops = createNewOptions(10000, 0, 255);
	if (ops == NULL) {
		return;
	}

	for (int i = 2; i < argc; i++) {
		if (!(strcmp(argv[i], "-h"))) { //print help
			printHelp();
			break;
		}

		else if (!(strcmp(argv[i], "-p"))) { //print list of markers from file
			printList(head);
			break;
		}

		else if (!(strcmp(argv[i], "-o"))) { //modify RNG variables 
			long int divisor = strtol(argv[i + 1], NULL, 10);
			long int min_byte = strtol(argv[i + 2], NULL, 10);
			long int max_byte = strtol(argv[i + 3], NULL, 10);

			if (divisor < 0 || min_byte < 0 || max_byte < 0) {
				printf("ERROR: Divisor/min byte/max byte must be greater than 0.\n");
				return;
			}
			if (min_byte > 255 || max_byte > 255) {
				printf("ERROR: Min byte/max byte must be greater than or equal to 0 and less than or equal to 255.\n");
				return;
			}
			if (min_byte > max_byte) {
				printf("ERROR: Min byte must be less than max byte.\n");
				return;
			}

			ops = createNewOptions(divisor, min_byte, max_byte);
			i += 3;
		}

		else if (!(strcmp(argv[i], "-m"))) { //find matching markers
			/*struct Segment_Info* new_head = matchSegment(head, argv[i + 1]);
			struct Segment_Info* seg_ptr = new_head;
			i += 2;*/
			i++;
			while (i < argc && argv[i][0] != '-') {
				struct Segment_Info* seg_ptr = matchSegment(head, argv[i]);
				if (seg_ptr != NULL) {
					buffer = modifySegment(buffer, seg_ptr, ops);
				}

				i++;
			}
				
			FILE* file = createNewFile(file_path);
			if (file == NULL) {
				printf("ERROR: File pointer to new file is NULL.\n");
				return 1;
			}

			fwrite(buffer, sizeof(unsigned char), file_info->file_size, file);
			fclose(file);
			i--;
		}

		else {
			printf("ERROR: \"%s\" is not a valid option.\n", argv[i]);
			return;
		}
	}
}

void printHelp() {
	printf("This program produces glitch art of a JPEG file by randomly modifying its data. The file must be the first argument in the command line.\n\n");
	printf("OPTIONS\n");
	printf("-h: prints help and information about program.\n");
	printf("-p: prints the list of markers found in JPEG file and their modifiable byte range.\n");
	printf("-m: takes any number of names of markers. These arguments must be viable markers found in supplied JPEG file. The program will use the ranges of these markers to modify the file's data.\n");
	printf("-o: takes three arguments - the divisor, min value, and max value.\nThe divisor specifies the percentage a byte will be modified (i.e 1/divisor).\nThe min value is the minimum value a byte can be modified to, and the max is the maximum value a byte can be modified to.\n If this option is left empty, the default options are: 1/10000, 0, 255\n");
}