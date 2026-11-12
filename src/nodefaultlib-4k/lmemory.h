#pragma once

#include <Windows.h>
#include <intrin.h>

HANDLE g_hHeap;

#ifndef _DEBUG
/* Without the CRT the compiler still emits memset calls for "= { 0 }" initializers. */
#pragma function(memset)
void *__cdecl memset(void *dst, int c, size_t count)
{
	__stosb((unsigned char*)dst, (unsigned char)c, count);
	return dst;
}
#endif // _DEBUG

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

LPVOID __fastcall l_malloc(size_t dwBytes)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);
}

LPVOID __fastcall l_realloc(LPVOID pvMem, size_t dwBytes)
{
	return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pvMem, dwBytes);
}

BOOL __fastcall l_free(LPVOID pvMem)
{
	return HeapFree(GetProcessHeap(), 0, pvMem);
}

BOOL __fastcall l_heap_destroy()
{
	return HeapDestroy(GetProcessHeap());
}

