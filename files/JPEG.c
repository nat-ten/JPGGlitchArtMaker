#include "JPEG.h"

char* getSegmentName(unsigned char byte, int new_list) {
	int MAX_CHARACTERS = 6;

	static int SOF_count;
	static int DHT_count;
	static int JPG_count;
	static int DAC_count;
	static int RST_count;
	static int SOI_count;
	static int EOI_count;
	static int SOS_count;
	static int DQT_count;
	static int DNL_count;
	static int DRI_count;
	static int DHP_count;
	static int EXP_count;
	static int APP_count;
	static int COM_count;

	if (new_list) {
		SOF_count = 0;
		DHT_count = 0;
		JPG_count = 0;
		DAC_count = 0;
		RST_count = 0;
		SOI_count = 0;
		EOI_count = 0;
		SOS_count = 0;
		DQT_count = 0;
		DNL_count = 0;
		DRI_count = 0;
		DHP_count = 0;
		EXP_count = 0;
		APP_count = 0;
		COM_count = 0;
	}

	char* segment_name = malloc(MAX_CHARACTERS);
	if (segment_name == NULL) {
		printf("ERROR: Unable to allocate memory for segment name.\n");
		return NULL;
	}

	switch (byte)
	{
	case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC5: case 0xC6: case 0xC7: case 0xC9: case 0xCA: case 0xCB: case 0xCD: case 0xCE: case 0xCF:
		snprintf(segment_name, MAX_CHARACTERS, "SOF%d", SOF_count);
		SOF_count++;
		break;

	case 0xC4:
		snprintf(segment_name, MAX_CHARACTERS, "DHT%d", DHT_count);
		DHT_count++;
		break;

	case 0xC8: case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7: case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD:
		snprintf(segment_name, MAX_CHARACTERS, "JPG%d", JPG_count);
		JPG_count++;
		break;

	case 0xCC:
		snprintf(segment_name, MAX_CHARACTERS, "DAC%d", DAC_count);
		DAC_count++;
		break;

	case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
		snprintf(segment_name, MAX_CHARACTERS, "RST%d", RST_count);
		RST_count++;
		break;

	case 0xD8:
		snprintf(segment_name, MAX_CHARACTERS, "SOI%d", SOI_count);
		SOI_count++;
		break;

	case 0xD9:
		snprintf(segment_name, MAX_CHARACTERS, "EOI%d", EOI_count);
		EOI_count++;
		break;

	case 0xDA:
		snprintf(segment_name, MAX_CHARACTERS, "SOS%d", SOS_count);
		SOS_count++;
		break;

	case 0xDB:
		snprintf(segment_name, MAX_CHARACTERS, "DQT%d", DQT_count);
		DQT_count++;
		break;

	case 0xDC:
		snprintf(segment_name, MAX_CHARACTERS, "DNL%d", DNL_count);
		DNL_count++;
		break;

	case 0xDD:
		snprintf(segment_name, MAX_CHARACTERS, "DRI%d", DRI_count);
		DRI_count++;
		break;

	case 0xDE:
		snprintf(segment_name, MAX_CHARACTERS, "DHP%d", DHP_count);
		DHP_count++;
		break;

	case 0xDF:
		snprintf(segment_name, MAX_CHARACTERS, "EXP%d", EXP_count);
		EXP_count++;
		break;

	case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7: case 0xE8:case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
		snprintf(segment_name, MAX_CHARACTERS, "APP%d", APP_count);
		APP_count++;
		break;

	case 0xFE:
		snprintf(segment_name, MAX_CHARACTERS, "COM%d", COM_count);
		COM_count++;
		break;

	default:
		printf("Unable to find segment name.\n");
		break;
	}
	return segment_name;
}

struct Segment_Info* getSegments(char* file_contents, unsigned long file_size) {
	unsigned long index = 0;
	unsigned long start = 0;
	unsigned long end = 0;

	int segment_head_flag = 0;

	struct Segment_Info* head = NULL;
	struct Segment_Info* seg_ptr = NULL;

	while (index < file_size) {
		unsigned char first_byte = file_contents[index];
		unsigned char second_byte = file_contents[index + 1];
		int valid_marker = validateMarker(first_byte, second_byte);

		if (valid_marker) {
			valid_marker = 0;

			if (second_byte != 0xD8 && second_byte != 0xD9 && second_byte != 0xDA) {
				unsigned char s1 = file_contents[index + 2];
				unsigned char s2 = file_contents[index + 3];
				unsigned short marker_size = s1 + s2 - 2;

				start = index + 4;
				index = start;
				end = start + marker_size - 1;

				seg_ptr = createNewSegment_Info(getSegmentName(second_byte, 0), start, end, seg_ptr);

				start = 0;
				end = 0;
			}

			else {
				start = index;
				if (second_byte == 0xDA) {
					start = index + 14; //the immediate 14 bytes following the marker are important and should not be modified
					index = start;
				}
				
				while (!valid_marker && index < file_size - 1) {
					unsigned char first_byte_temp = file_contents[index + 2];
					unsigned char second_byte_temp = file_contents[index + 3];
					valid_marker = validateMarker(first_byte_temp, second_byte_temp);
					index++;
				}

				if (!segment_head_flag) {
					end = index;
					head = createNewSegment_Info(getSegmentName(second_byte, 1), start, end, NULL);

					seg_ptr = head;
					segment_head_flag = 1;
				}

				else if (segment_head_flag) {
					end = index;
					seg_ptr = createNewSegment_Info(getSegmentName(second_byte, 0), start, end, seg_ptr);

				}

				start = 0;
				end = 0;
			}
		}

		index++;
	}

	return head;
}

int validateMarker(unsigned char first_byte, unsigned char second_byte) {
	if ((first_byte != 0xFF)) {
		return 0;
	}

	else {
		if ((second_byte != 0x00) && (second_byte != 0xFF)) {
			return 1;
		}
	}
	return 0;
}

struct Segment_Info* createNewSegment_Info(char* segment_name, unsigned long start, unsigned long end, struct Segment_Info* previous_segment) {
	struct Segment_Info* new_segment_info = malloc(sizeof(struct Segment_Info*));
	if (new_segment_info == NULL) {
		printf("ERROR: Unable to allocate memory for new segment_info struct.\n");
		return NULL;
	}

	int length = strlen(segment_name);
	if (length == 0) {
		printf("ERROR: Length of segment name is 0.\n");
		return NULL;
	}

	new_segment_info->segment_name = malloc(length + 1);
	if (new_segment_info->segment_name == NULL) {
		printf("ERROR: Unable to allocate memory for segment_info segment_name.\n");
		return NULL;
	}

	new_segment_info->segment_name = segment_name;

	new_segment_info->start = start;
	new_segment_info->end = end;

	if (previous_segment != NULL) {
		previous_segment->next_segment = new_segment_info;
	}
	
	return new_segment_info;
}

struct RNG_Options* createNewOptions(unsigned int divisor, unsigned char min_byte_value, unsigned char max_byte_value) {
	struct RNG_Options* new_options = malloc(sizeof(struct RNG_Options*));
	if (new_options == NULL) {
		printf("ERROR: Unable to allocate memory for new options.\n");
		return NULL;
	}

	new_options->divisor = divisor;
	new_options->min_byte_value = min_byte_value;
	new_options->max_byte_value = max_byte_value;

	return new_options;
}

void printList(struct Segment_Info* head_of_list) {
	struct Segment_Info* seg_ptr = head_of_list;
	if (seg_ptr == NULL) {
		printf("ERROR: head of list is NULL.\n");
		return;
	}

	while (seg_ptr != NULL) {
		printf("segment_name: \"%s\". start: %lu. end: %lu.\n", seg_ptr->segment_name, seg_ptr->start, seg_ptr->end);
		seg_ptr = seg_ptr->next_segment;
	}
}

struct Segment_Info* matchSegment(struct Segment_Info* head_of_list, char* segment_name) {
	struct Segment_Info* seg_ptr = head_of_list;
	int match_found = 0;

	if (head_of_list == NULL) {
		printf("ERROR: Head of list is NULL.\n");
		return NULL;
	}

	while (seg_ptr != NULL) {
		if (!strcmp(seg_ptr->segment_name, segment_name)) {
			match_found = 1;
			break;
		}
		seg_ptr = seg_ptr->next_segment;
	}

	if (match_found) {
		return seg_ptr;
	}

	else {
		printf("No match with segment \"%s\" found.\n", segment_name);
		return NULL;
	}
}

char* modifySegment(char* file_contents, struct Segment_Info* segment, struct RNG_Options* options) {
	unsigned long index = segment->start;

	for (index; index < segment->end; index++) {
		unsigned int divisor = options->divisor;
		unsigned long value = rand() % divisor + 1;
		if (value <= 1) {
			unsigned char new_byte = rand() % (options->max_byte_value + 1 - options->min_byte_value) + options->min_byte_value;
			file_contents[index] = new_byte;
		}
	}
	return file_contents;
}