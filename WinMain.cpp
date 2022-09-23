#pragma warning(disable : 28251)
#pragma warning(disable : 28159)

#include "gl.h"
#undef APIENTRY
#include <Windows.h>
#include "Graphics.h"
#include "math.h"
#include "FileLoaders.h"

 extern "C" int _fltused = 1;
 extern float ambientOnly;
 extern bool ShowDepth;
 extern bool enablePCM;

char buf[1024];
Graphics::Device* gfx;

void Initialize(Graphics::Dependencies& p, Graphics::Device*);
void Update(Graphics::Device*, float deltaTime);
void Render(Graphics::Device*, int x, int y, int w, int h);
void Shutdown(Graphics::Device*);

static void WinAssert(bool condition) {
#if _WASM32
	if (condition == false) {
		__builtin_trap();
	}
#else
	char* data = (char*)((void*)0);
	if (condition == false) {
		*data = '\0';
	}
#endif
}

#if 0
#pragma intrinsic(memcpy)
#pragma function(memcpy)
extern "C" void* memcpy(void* dst, const void* src, unsigned long long bytes) {
	unsigned char* d = (unsigned char*)dst;
	unsigned char* s = (unsigned char*)src;
	for (unsigned int i = 0; i < bytes; ++i) {
		d[i] = s[i];
	}
	return dst;
}

#pragma intrinsic(memset)
#pragma function(memset)
extern "C" void* memset(void* str, int c, unsigned long long n) {
	unsigned char* s = (unsigned char*)str;
	for (unsigned int i = 0; i < n; ++i) {
		s[i] = c;
	}

	return str;
}
#endif


void* RequestMem(u32 bytes) {
	return VirtualAlloc(0, bytes, MEM_COMMIT, PAGE_READWRITE);
}

void ReleaseMem(void* mem) {
	VirtualFree(mem, 0, MEM_RELEASE);
}

#pragma comment(lib, "opengl32.lib")

void printf(const char* pszFormat, ...) {
    va_list argList;
    va_start(argList, pszFormat);
    wvsprintfA(buf, pszFormat, argList);
    va_end(argList);
    DWORD done;
    unsigned int len = 0;
    for (char* c = buf; *c != '\0'; ++c, ++len) {
        if (len >= 1024) {
            len = 1024;
            break;
        }
    }
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)len, &done, NULL);
}

const char* FindStringInString(const char* string, const char*substring) {
	const char* a, * b;

	b = substring;
	if (*b == 0) {
		return string;
	}
	for (; *string != 0; string += 1) {
		if (*string != *b) {
			continue;
		}
		a = string;
		while (1) {
			if (*b == 0) {
				return string;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = substring;
	}
	return NULL;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int run() {
    printf("Hello, world\n");
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
	return 0;
}

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int*);

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int);
typedef int (WINAPI* PFNWGLGETSWAPINTERVALEXTPROC) (void);

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = 0;
	wndclass.lpszClassName = L"Win32 Game Window";
	RegisterClassEx(&wndclass);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int clientWidth = 800;
	int clientHeight = 600;
	RECT windowRect;
	SetRect(&windowRect, (screenWidth / 2) - (clientWidth / 2), (screenHeight / 2) - (clientHeight / 2), (screenWidth / 2) + (clientWidth / 2), (screenHeight / 2) + (clientHeight / 2));

	DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // WS_THICKFRAME to resize
	AdjustWindowRectEx(&windowRect, style, FALSE, 0);
	HWND hwnd = CreateWindowEx(0, wndclass.lpszClassName, L"Game Window", style, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, szCmdLine);
	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	{ // memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		unsigned char* target = (unsigned char*)&pfd;
		for (unsigned int i = 0; i < sizeof(PIXELFORMATDESCRIPTOR); ++i) {
			target[i] = 0;
		}
	}
	
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	HGLRC tempRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC hglrc = wglCreateContextAttribsARB(hdc, 0, attribList);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempRC);
	wglMakeCurrent(hdc, hglrc);

	/*if (!gladLoadGL()) {
		std::cout << "Could not initialize GLAD\n";
	}
	else {
		std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded\n";
	}*/

	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	bool swapControlSupported = FindStringInString(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != 0;

	int vsynch = 0;
	if (swapControlSupported) {
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

		if (wglSwapIntervalEXT(1)) {
			printf("Enabled vsynch\n");
			vsynch = wglGetSwapIntervalEXT();
		}
		else {
			printf("Could not enable vsynch\n");
		}
	}
	else { // !swapControlSupported
		printf("WGL_EXT_swap_control not supported\n");
	}

	Graphics::Dependencies platform;
	platform.Request = RequestMem;
	platform.Release = ReleaseMem;
	gfx = (Graphics::Device*)RequestMem(sizeof(Graphics::Device));

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	Initialize(platform, gfx);

	DWORD lastTick = GetTickCount();
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DWORD thisTick = GetTickCount();
		float deltaTime = float(thisTick - lastTick) * 0.001f;
		lastTick = thisTick;

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		clientWidth = clientRect.right - clientRect.left;
		clientHeight = clientRect.bottom - clientRect.top;

		Update(gfx, deltaTime);
		Render(gfx, 0, 0, clientWidth, clientHeight);

		SwapBuffers(hdc);
		if (vsynch != 0) {
			glFinish();
		}
	} // End of game loop

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	switch (iMsg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		{
			Shutdown(gfx);
			ReleaseMem(gfx);
			
			HDC hdc = GetDC(hwnd);
			HGLRC hglrc = wglGetCurrentContext();

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hglrc);
			ReleaseDC(hwnd, hdc);

			PostQuitMessage(0);
		}
		return 0;
	case WM_PAINT:
	case WM_ERASEBKGND:
		return 0;
	case WM_KEYUP:
		if (wParam == VK_NUMPAD1 || wParam == 0x31) {
			ambientOnly = 0.0f;
		}
		else if (wParam == VK_NUMPAD2 || wParam == 0x32) {
			ambientOnly = 1.0f;
		}
		else if (wParam == VK_NUMPAD3 || wParam == 0x33) {
			enablePCM = !enablePCM;
		}
		else if (wParam == VK_SPACE) {
			ShowDepth = !ShowDepth;
		}
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

///

static void Assert(bool condition) {
#if _WASM32
	if (condition == false) {
		__builtin_trap();
	}
#else
	char* data = (char*)((void*)0);
	if (condition == false) {
		*data = '\0';
	}
#endif
}