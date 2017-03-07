#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG

int countlines(FILE *fp)
{
	int c;
	int lines = 0;
	fseek(fp, 0, SEEK_SET);
	while ((c = fgetc(fp)) != EOF)
	{
		if (c == '\n') ++lines;
	}
	return lines;
}

char* read_file(const char *fileName)
{
	FILE * fp;
	fp = fopen(fileName, "r");
	if (fp == NULL)	return 0;
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* result = (char*)malloc(fileSize);
	fread(result, sizeof(char), fileSize, fp);
	fclose(fp);
	return result;
}

#endif // _DEBUG

