#pragma once


#include <Windows.h>
#include <string.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


// evil C:
void l_strrev(char *p)
{
	char *q = p;
	while (q && *q) ++q;
	for (--q; p < q; ++p, --q) {
		*p = *p ^ *q;
		*q = *p ^ *q;
		*p = *p ^ *q;
	}
}

// Implementation of itoa()
char* l_itoa(int num, char* str, int base)
{
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; // Append string terminator
	l_strrev(str);	// Reverse the string
	return str;
}

#ifdef _DEBUG
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

size_t read_file(char buffer[], const char *fileName)
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
	return chars_read;
	//free(result);
}

BOOL load_shaders(const char *vhlEnding, const char *fhlEnding,
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
		return FALSE;
}

#endif // _DEBUG

