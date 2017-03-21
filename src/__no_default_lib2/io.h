#pragma once

#ifdef _DEBUG
#include <string.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t countlines(FILE *fp)
{
	uint32_t c, lines = 0;
	fseek(fp, 0, SEEK_SET);
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n') ++lines;
	}
	return lines;
}

char* read_file(const char *fileName)
{
	FILE * fp;
	fp = fopen(fileName, "r");
	if (fp == NULL)	return NULL;
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* result = (char*)malloc(fileSize);
	memset(result, 0, fileSize);
	size_t chars_read = fread(result, sizeof(char), fileSize, fp);
	fclose(fp);
	return result;
}

#endif // _DEBUG

