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

char* read_file(char buffer[], const char *fileName)
{
	FILE * fp;
	fp = fopen(fileName, "r+");
	if (fp == NULL)	return NULL;
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	//char* result = (char*)malloc(fileSize);
	//memset(result, 0, fileSize);

	size_t chars_read = fread(buffer, sizeof(char), fileSize, fp);


	fclose(fp);
	//free(result);
	//return result;
}

bool load_shaders(const char *vhlEnding, const char *fhlEnding,
	char my_vertex_shader_source[], char my_fragment_shader_source[])
{
	char buffer[MAX_PATH];
	char sourcePath[MAX_PATH];

	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	size_t sourcePathLength = last_index_of(buffer, '\\', 3);
	memcpy(sourcePath, buffer, sourcePathLength);
	strcpy(sourcePath + sourcePathLength, vhlEnding);
	read_file(my_vertex_shader_source, sourcePath);
	strcpy(sourcePath + sourcePathLength, fhlEnding);
	read_file(my_fragment_shader_source, sourcePath);
	if (!my_vertex_shader_source || !my_fragment_shader_source)
		return false;
}

#endif // _DEBUG

