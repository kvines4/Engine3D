#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <fstream>
#include <strstream>

using namespace std;

#include "color.cpp"
#include "utils.cpp"
#include "bitmap.cpp"
#include <windows.h>

global_variable bool running = true;
global_variable float* pDepthBuffer = nullptr;
int start_width = 1024;
int start_height = 768;

// Timing
int frameCount = 0;
int fps = 0;
double delta_time = 0.f;
double counter_time = 0.f;
__int64 frameTimeOld = 0;
double frameTime;
LARGE_INTEGER frame_begin_time;
LARGE_INTEGER counter_begin_time;

/// <summary>
/// Framebuffer. Contains all display Information.
/// </summary>
struct Render_State {
	int height, width;		// Window width and height
	void* memory;			// Pixel buffer
	BITMAPINFO bitmap_info;	// Bitmap info
};

global_variable Render_State render_state;

#include "platform_common.cpp"
#include "renderer.cpp"
#include "engine3D.cpp"
// #include "game.cpp"

Engine3D engine;

// An application-defined function that processes messages sent to a window. The WNDPROC type defines a pointer to this callback function.
// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)
LRESULT CALLBACK window_callback(
	HWND hwnd, 
	UINT uMsg, 
	WPARAM wParam,
	LPARAM lParam) {

	LRESULT result = 0;

	// Handle Windows messages
	// https://docs.microsoft.com/en-us/windows/win32/winmsg/window-notifications
	switch (uMsg) { 
		// Window close event
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;
		// Window resize event
		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			// pixel buffer is width * height pixels each represented as an insigned integer
			int size = render_state.width * render_state.height * sizeof(unsigned int);

			// Ask windows to allocate memory on the Heap
			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			// Information on how pixels should be displayed so windows knows how to copy it to the video card
			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = -render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;
		// init
		default: {
			// default window procedure
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	return result;
}

// The user-provided entry point for a graphical Windows-based application.
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, 
	_In_ int nShowCmd) {

	//ShowCursor(false);
	// Create Window Class
	// https://docs.microsoft.com/en-us/previous-versions/ms942860(v=msdn.10)
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;
	window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	// Register Class
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa
	RegisterClass(&window_class);

	// Create Window
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowa
	HWND window = CreateWindow(window_class.lpszClassName,
		"GAME!",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Windows Styles: https://docs.microsoft.com/en-us/windows/win32/winmsg/window-styles
		CW_USEDEFAULT, CW_USEDEFAULT,
		start_width, start_height, 0, 0,
		hInstance,
		0);

	HDC hdc = GetDC(window);

	Input input = {};

	// Setup for calculating Delta Time
	QueryPerformanceCounter(&frame_begin_time);
	counter_begin_time = frame_begin_time;

	double performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (double)perf.QuadPart;
	}

	engine.game_init();

	while (running) {
		// Input
		MSG message;

		// Reset all button state changed values at start of frame
		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		// Check all messages windows has sent to the window
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			// Handle new button state
			switch (message.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vk_code = (u32)message.wParam;
					bool is_down = ((message.lParam & (static_cast <unsigned __int64>(1) << 31)) == 0);

					#define process_button(b, vk)\
					case vk: {\
						input.buttons[b].changed = is_down != input.buttons[b].is_down;\
						input.buttons[b].is_down = is_down;\
					} break;

					switch (vk_code) {
						process_button(BUTTON_ENTER, VK_RETURN);
						process_button(BUTTON_SPACE, VK_SPACE);
						process_button(BUTTON_CONTROL, VK_CONTROL);
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_A, 'A');
						process_button(BUTTON_S, 'S');
						process_button(BUTTON_D, 'D');
					}

				} break;
				default: {
					TranslateMessage(&message);
					DispatchMessage(&message); // To the callback
				}
			}
		}

		// Simulate
		engine.game_update(&input, delta_time);

		// Render
		// The StretchDIBits function copies the color data for a rectangle of pixels to the specified destination (Application Window)
		StretchDIBits(hdc,
			0, 0, render_state.width, render_state.height,
			0, 0, render_state.width, render_state.height,
			render_state.memory,
			&render_state.bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY);
	
		// Calculate times
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (double)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		counter_time = (double)(frame_end_time.QuadPart - counter_begin_time.QuadPart) / performance_frequency;

		frame_begin_time = frame_end_time;

		// Calculate fps
		frameCount++;
		if (counter_time > 1.0f)
		{
			fps = frameCount;
			frameCount = 0;

			QueryPerformanceCounter(&counter_begin_time);
		}

		// Updated title to show FPS
		string title = "Engine3D FPS: " + to_string(fps);
		SetWindowText(window, title.c_str());
	}
}