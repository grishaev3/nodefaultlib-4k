#pragma once

#include <Windows.h>

HANDLE hHeap;

inline void l_heap_create(size_t dwInitialSize, size_t dwHaximumSize)
{
	hHeap = HeapCreate(0, dwInitialSize, dwHaximumSize);
}

inline LPVOID l_malloc(size_t dwBytes)
{
	return HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwBytes);
}

inline LPVOID l_realloc(LPVOID pvMem, size_t dwBytes)
{
	return HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pvMem, dwBytes);
}

inline BOOL l_free(LPVOID pvMem)
{
	return HeapFree(hHeap, 0, pvMem);
}

inline BOOL l_heap_destroy()
{
	return HeapDestroy(hHeap);
}

