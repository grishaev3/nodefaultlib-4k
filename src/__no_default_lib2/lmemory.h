#pragma once

#include <Windows.h>

HANDLE g_hHeap;

void l_heap_create(size_t dwInitialSize, size_t dwHaximumSize)
{
	g_hHeap = HeapCreate(0, dwInitialSize, dwHaximumSize);
}

LPVOID l_malloc(size_t dwBytes)
{
	return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwBytes);
}

LPVOID l_realloc(LPVOID pvMem, size_t dwBytes)
{
	return HeapReAlloc(g_hHeap, HEAP_ZERO_MEMORY, pvMem, dwBytes);
}

BOOL l_free(LPVOID pvMem)
{
	return HeapFree(g_hHeap, 0, pvMem);
}

BOOL l_heap_destroy()
{
	return HeapDestroy(g_hHeap);
}

