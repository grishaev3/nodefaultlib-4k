#pragma once

#include <Windows.h>

HANDLE g_hHeap;

LPVOID __fastcall l_memset(LPVOID pvMem, int c, size_t dwBytes)
{
	BYTE* p = pvMem;
	while (dwBytes--)
		*p++ = (BYTE)c;
	return pvMem;
}

inline void __fastcall l_zeromem(LPVOID pvMem, size_t dwBytes)
{
	l_memset(pvMem, 0x0, dwBytes);
}

void __fastcall l_heap_create(size_t dwInitialSize, size_t dwHaximumSize)
{
	g_hHeap = HeapCreate(0, dwInitialSize, dwHaximumSize);
}

LPVOID __fastcall l_malloc(size_t dwBytes)
{
	return HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, dwBytes);
}

LPVOID __fastcall l_realloc(LPVOID pvMem, size_t dwBytes)
{
	return HeapReAlloc(g_hHeap, HEAP_ZERO_MEMORY, pvMem, dwBytes);
}

BOOL __fastcall l_free(LPVOID pvMem)
{
	return HeapFree(g_hHeap, 0, pvMem);
}

BOOL __fastcall l_heap_destroy()
{
	return HeapDestroy(g_hHeap);
}

