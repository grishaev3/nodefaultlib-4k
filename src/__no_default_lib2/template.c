#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

HWND  g_hWnd = NULL;
HDC   g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void init(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	g_hDC = GetDC(g_hWnd);
	GLuint iPixelFormat = ChoosePixelFormat(g_hDC, &pfd);

	if (iPixelFormat != 0)	{
		PIXELFORMATDESCRIPTOR bestMatch_pfd;
		DescribePixelFormat(g_hDC, iPixelFormat, sizeof(pfd), &bestMatch_pfd);

		if (bestMatch_pfd.cDepthBits < pfd.cDepthBits)		{
			return;
		}

		if (SetPixelFormat(g_hDC, iPixelFormat, &pfd) == FALSE)	{
			DWORD dwErrorCode = GetLastError();
			return;
		}
	}
	else {
		DWORD dwErrorCode = GetLastError();
		return;
	}

	g_hRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hRC);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); 
}

void shutDown(void)
{
	if (g_hRC != NULL)	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}

	if (g_hDC != NULL)	{
		ReleaseDC(g_hWnd, g_hDC);
		g_hDC = NULL;
	}
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(g_hDC);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp";
	if (!RegisterClass(&wc))
		return 1;

	g_hWnd = CreateWindow(wc.lpszClassName,
		L"__4Kb",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 640, 480, 0, 0, hInstance, NULL);

	if (g_hWnd == NULL)
		return E_FAIL;


	init();

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			render();
	}

	shutDown();
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}