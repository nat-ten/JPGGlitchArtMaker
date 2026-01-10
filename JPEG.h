#pragma once
#include "Files.h"
#include <time.h>

struct Segment_Info {
	char* segment_name;
	unsigned long start;
	unsigned long end;
	struct Segment_Info* next_segment;
};

struct RNG_Options {
	unsigned int divisor;
	unsigned char min_byte_value;
	unsigned char max_byte_value;
};

struct Segment_Info* getSegments(char* file_contents, unsigned long file_size);

char* getSegmentName(unsigned char byte, int new_list);

int validateMarker(unsigned char first_byte, unsigned char second_byte);

struct Segment_Info* createNewSegment_Info(char* segment_name, unsigned long start, unsigned long end, struct Segment_Info* previous_segment);

struct RNG_Options* createNewOptions(unsigned int divisor, unsigned char min_byte_value, unsigned char max_byte_value);

void printList(struct Segment_Info* head_of_list);

struct Segment_Info* matchSegment(struct Segment_Info* head_of_list, char* segment_name);

char* modifySegment(char* file_contents, struct Segment_Info* segment, struct RNG_Options* options);