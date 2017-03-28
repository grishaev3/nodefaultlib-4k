#pragma once

#ifdef _DEBUG
#include <string.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t count_lines(FILE *fp)
{
	uint32_t c, lines = 0;
	fseek(fp, 0, SEEK_SET);
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n') ++lines;
	}
	return lines;
}

int32_t last_index_of(const char *src, const char ch, int level)
{
	char buf[260] = { 0 };
	int32_t len = -1;
	strcpy(buf, src);
	while (level--) {
		const char *dst = strrchr(buf, ch);
		len = dst - buf;
		buf[len] = '\0';
	}
	return len;
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

