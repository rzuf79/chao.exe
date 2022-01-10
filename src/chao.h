#ifndef CHAO_H
#define CHAO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <algorithm>
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include "keyboard.h"
#include "base64.h"
#include "upng.h"


#define MAX_IMAGES 256


enum ScreenScalingMode {
	SSM_NONE = 0,
	SSM_STRETCH,
	SSM_EXTEND,
	SSM_END,
};


class Vector2 {
	public:

	float x, y;

	Vector2() {
		x = y = 0.f;
	}
	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	Vector2& operator = (const Vector2 &a) { x = a.x; y = a.y; return *this; }
	Vector2 operator += (const Vector2 &a) { x += a.x; y += a.y; return *this; }
	Vector2 operator -= (const Vector2 &a) { x -= a.x; y -= a.y; return *this; }
	Vector2 operator *= (const Vector2 &a) { x *= a.x; y *= a.y; return *this; }
	Vector2 operator /= (const Vector2 &a) { x /= a.x; y /= a.y; return *this; }
	Vector2 operator + (const Vector2 &a) { return Vector2(x + a.x, y + a.y); }
	Vector2 operator - (const Vector2 &a) { return Vector2(x - a.x, y - a.y); }
	Vector2 operator * (const Vector2 &a) { return Vector2(x * a.x, y * a.y); }
	Vector2 operator / (const Vector2 &a) { return Vector2(x / a.x, y / a.y); }
	Vector2 operator + (const float &a) { return Vector2(x + a, y + a); }
	Vector2 operator - (const float &a) { return Vector2(x - a, y - a); }
	Vector2 operator * (const float &a) { return Vector2(x * a, y * a); }
	Vector2 operator / (const float &a) { return Vector2(x / a, y / a); }

	void print() {
		printf("%.1fx%.1f\n", x, y);
	}
};

class Rect {
	public:
	float x, y, w, h;
	Rect() {
		x = y = w = h = 0.f;
	}
	Rect(float x, float y, float w, float h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	Rect operator + (const float &a) { return Rect(x + a, y + a, w + a, h + a); }
	Rect operator - (const float &a) { return Rect(x - a, y - a, w - a, h - a); }
	Rect operator * (const float &a) { return Rect(x * a, y * a, w * a, h * a); }
	Rect operator / (const float &a) { return Rect(x / a, y / a, w / a, h / a); }

};

class Color {
	public:

	float r, g, b, a = 1.f;

	Color() {
		r = g = b = a = 1.f;
	};
	Color(float r, float g, float b, float a = 1.f) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color& operator = (const Color &other) {
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		return *this;
	}
};

class Image {
	public:
	Image() {
		name = NULL;
		tex = -1;
	}

	char* name;
	GLuint tex;
	Vector2 size;
};


void chao_main();
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);


HINSTANCE chao_h_instance;
int chao_cmd_show;
HWND chao_hwnd;
void on_resize();

Image images[MAX_IMAGES];

ScreenScalingMode screen_scaling_mode;
Vector2 screen_size; // size of the viewport
Vector2 base_screen_size; // the values passed in chao_init; base for screen scaling
Vector2 window_size; // size of the actual window

Color clear_color = Color(0,0,0,1);
bool image_smoothing_enabled = false;

bool pressed[256];
bool just_pressed[256];
bool just_released[256];

float delta_time;
float max_delta_time = 0.05;
float time_scale = 1.f;
float last_system_time;
float fps_timer = 0.f;
int fps_counter = 0;


void update_delta_time() {
	float current_time = (float)GetTickCount() / 1000.0f;
	delta_time = std::min(current_time - last_system_time, max_delta_time);
	last_system_time = current_time;

	// fps_counter ++;
	// fps_timer += delta_time;
	// if(fps_timer >= 1.f){
	// 	printf("FPS: %d %f\n", fps_counter, delta_time);
	// 	fps_timer -= 1.f;
	// 	fps_counter = 0;
	// } 
}


float get_delta_time() {
	return delta_time * time_scale;
}


void chao_init(const char* window_name, int screen_width, int screen_height, ScreenScalingMode scaling_mode, void(*init_func_pointer)(), void(*update_func_pointer)()){
	WNDCLASSEX wcex;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL bQuit = FALSE;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = chao_h_instance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "ChaoWindow";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;
	
	if (!RegisterClassEx(&wcex)) {
		return;
	}

	chao_hwnd = CreateWindowEx(0,
						  "ChaoWindow",	
						  window_name,
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  screen_width,
						  screen_height,
						  NULL,
						  NULL,
						  chao_h_instance,
						  NULL);

	ShowWindow(chao_hwnd, chao_cmd_show);

	// EnableOpenGL
	{
		PIXELFORMATDESCRIPTOR pfd;
		int iFormat;
		hDC = GetDC(chao_hwnd);
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;
		pfd.cDepthBits = 16;
		pfd.iLayerType = PFD_MAIN_PLANE;
		iFormat = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, iFormat, &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		// for 2D
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}

	// init stuff

	screen_scaling_mode = scaling_mode;

	screen_size = Vector2(screen_width, screen_height);
	base_screen_size = Vector2(screen_width, screen_height);

	on_resize();

	if (init_func_pointer){
		(*init_func_pointer)();
	}

	// main loopz

	update_delta_time();

	while (!bQuit) {

		update_delta_time();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bQuit = TRUE;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else{
			glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
			glClear(GL_COLOR_BUFFER_BIT);


			if (update_func_pointer){
				(*update_func_pointer)();
			}
			for(int i = 0; i < 256; ++i) {
				just_pressed[i] = false;
				just_released[i] = false;
			}

			SwapBuffers(hDC);
		}
	}

	// DisableOpenGL
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(chao_hwnd, hDC);
	}

	DestroyWindow(chao_hwnd);
}


void set_window_size(int width, int height) {
	RECT current_window_rect;
	GetWindowRect(chao_hwnd, &current_window_rect);

	SetWindowPos(chao_hwnd, NULL, current_window_rect.left, current_window_rect.top, width, height, 0);

	on_resize();
}


struct Image* get_image(const char* image_name) {
	for (int i = 0; i < MAX_IMAGES; ++i) {
		if (images[i].name != NULL && strcmp(images[i].name, image_name) == 0) {
			return &images[i];
		}
	}

	return NULL;
}


void draw_image_part(Image img, Vector2 pos, Rect rect,
		Color color = Color(1,1,1), Vector2 scale = Vector2(1, 1),
		float angle = 0.f, Vector2 pivot = Vector2(0.5f, 0.5f),
		bool flip_x = false, bool flip_y = false) 
{
	int size_x = rect.w * scale.x;
	int size_y = rect.h * scale.y;
	Vector2 pivot_point = Vector2(size_x*pivot.x, size_y*pivot.y);
	rect.x /= img.size.x;
	rect.y /= img.size.y;
	rect.w /= img.size.x;
	rect.h /= img.size.y;

	pos -= pivot_point;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslated(pos.x+pivot_point.x, pos.y+pivot_point.y, 0); 
	glRotatef(angle, 0, 0, 1);
	glTranslated(-pivot_point.x, -pivot_point.y, 0); 
	glBindTexture(GL_TEXTURE_2D, img.tex);
	glBegin(GL_QUADS);
	glColor4ub(color.r * 255.f,color.g * 255.f,color.b * 255.f,color.a * 255.f);
	glTexCoord2f(rect.x, rect.y); 				glVertex2d(flip_x ? size_x : 0, flip_y ? size_y : 0);
	glTexCoord2f(rect.x+rect.w, rect.y); 		glVertex2d(flip_x ? 0 : size_x, flip_y ? size_y : 0);
	glTexCoord2f(rect.x+rect.w, rect.y+rect.h); glVertex2d(flip_x ? 0 : size_x, flip_y ? 0 : size_y);
	glTexCoord2f(rect.x, rect.y+rect.h); 		glVertex2d(flip_x ? size_x : 0, flip_y ? 0 : size_y);
	glEnd();
}

void draw_image(Image img, Vector2 pos, Color color = Color(1,1,1),
		Vector2 scale = Vector2(1, 1), float angle = 0.f,
		Vector2 pivot = Vector2(0.5f, 0.5f),
		bool flip_x = false, bool flip_y = false)
{
	draw_image_part(img, pos, Rect(0, 0, img.size.x, img.size.y), color, scale, angle, pivot, flip_x, flip_y);
}

void draw_image(const char* image_name, Vector2 pos, Color color = Color(1,1,1),
		Vector2 scale = Vector2(1, 1), float angle = 0.f,
		Vector2 pivot = Vector2(0.5f, 0.5f),
		bool flip_x = false, bool flip_y = false)
{
	struct Image* img = get_image(image_name);
	if (img == NULL) {
		printf("no such image! \"%s\"\n",image_name );
		return;
	}

	draw_image(*img, pos, color, scale, angle, pivot, flip_x, flip_y);
}


void draw_text(const char* font_image_name, Vector2 pos, Vector2 glyph_size, const char* text) {
	Image img = *get_image(font_image_name);
	int h_char_count = img.size.x / glyph_size.x;
	int n = strlen(text);
	Vector2 current_pos = pos;

	for (int i = 0 ; i < n; ++i) {
		if (text[i] == '\n') {
			current_pos.x = pos.x;
			current_pos.y += glyph_size.y;
			continue;
		}

		int idx = (int)text[i];
		int glyph_x = idx % h_char_count; // idx - (glyph_y * h_char_count);
		int glyph_y = floor(idx / h_char_count);
		Rect rect = Rect(glyph_x*glyph_size.x, glyph_y*glyph_size.y, glyph_size.x, glyph_size.y);

		draw_image_part(img, current_pos, rect);

		current_pos.x += glyph_size.x;
	}
}


void load_base64_image(const char* name, const char* encoded) {

	size_t out_len;
	unsigned char* decoded = base64_decode(encoded, strlen(encoded), &out_len);
	if(decoded == NULL) {
		printf("Couldn't decode %s.\n", name);
		return;
	}
	upng_t* upng = upng_new_from_bytes(decoded, out_len);
	upng_decode(upng);

	int i = 0;
	while (images[i].name != NULL) {
		i ++;
	}

	images[i].name = strdup(name);
	images[i].size.x = upng_get_width(upng);
	images[i].size.y = upng_get_height(upng);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &(images[i].tex));
	glBindTexture(GL_TEXTURE_2D, images[i].tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, image_smoothing_enabled ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, image_smoothing_enabled ? GL_LINEAR : GL_NEAREST);

	switch (upng_get_components(upng)) {
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, images[i].size.x, images[i].size.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, upng_get_buffer(upng));
		break;
	case 2:	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, images[i].size.x, images[i].size.y, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, upng_get_buffer(upng));
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, images[i].size.x, images[i].size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, upng_get_buffer(upng));
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i].size.x, images[i].size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, upng_get_buffer(upng));
		break;
	}

	upng_free(upng);
}


// DWORD play_midi_file(HWND hWndNotify, LPSTR lpszMIDIFileName){
//     UINT wDeviceID;
//     DWORD dwReturn;
//     MCI_OPEN_PARMS mciOpenParms;
//     MCI_PLAY_PARMS mciPlayParms;
//     MCI_STATUS_PARMS mciStatusParms;
//     MCI_SEQ_SET_PARMS mciSeqSetParms;

//     mciOpenParms.lpstrDeviceType = "sequencer";
//     mciOpenParms.lpstrElementName = lpszMIDIFileName;
//     if (dwReturn = mciSendCommand(NULL, MCI_OPEN,
//         MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
//         (DWORD)(LPVOID) &mciOpenParms)){
//         return (dwReturn);
//     }

//     wDeviceID = mciOpenParms.wDeviceID;

//     mciStatusParms.dwItem = MCI_SEQ_STATUS_PORT;
//     if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, 
//         MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms)){
//         mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
//         return (dwReturn);
//     }

//     if (LOWORD(mciStatusParms.dwReturn) != MIDI_MAPPER){
//         if (MessageBox(chao_hwnd,
//             "The MIDI mapper is not available. Continue?",
//             "", MB_YESNO) == IDNO){
//             mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
//             return (0L);
//         }
//     }

//     mciPlayParms.dwCallback = (DWORD) hWndNotify;
//     if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, 
//         (DWORD)(LPVOID) &mciPlayParms)) {
//         mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
//         return (dwReturn);
//     }

//     return (0L);
// }

void on_resize() {
	RECT window_rect;
	GetClientRect(chao_hwnd, &window_rect);
	window_size.x = window_rect.right - window_rect.left;
	window_size.y = window_rect.bottom - window_rect.top;

	glViewport(0, 0, window_size.x, window_size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	screen_size = Vector2(window_size.x, window_size.y);
	float scale_x = screen_size.x / base_screen_size.x;
	float scale_y = screen_size.y / base_screen_size.y;
	float scale = std::min(scale_x, scale_y);
	
	switch (screen_scaling_mode) {
		case SSM_STRETCH: {
			screen_size = Vector2(base_screen_size.x, base_screen_size.y);
			break;
		}
		case SSM_EXTEND: {
			if (base_screen_size.x * scale < window_size.x) {
				// extend the viewport horizontally
				int spare_width = window_size.x - (base_screen_size.x * scale);
				screen_size.x = base_screen_size.x + (spare_width / scale);
				screen_size.y = base_screen_size.y;
			} else {
				// extend the viewport vertically
				int spare_height = screen_size.y - (base_screen_size.y * scale);
				screen_size.x = base_screen_size.x;
				screen_size.y = base_screen_size.y + (spare_height / scale);
			}
			break;
		}
	}

    glOrtho(0, screen_size.x, screen_size.y, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int cmd_show) {
	chao_h_instance = h_instance;
	chao_cmd_show = cmd_show;
	chao_main();
	return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_CLOSE: {
			PostQuitMessage(0);
			break;
		}

		case WM_DESTROY:
			return 0;

		case WM_KEYDOWN: {
			if(!pressed[wParam]) just_pressed[wParam] = true;
			pressed[wParam] = true;
			switch (wParam) {
				case VK_ESCAPE:
					PostQuitMessage(0);
				break;
			}
			break;
		}

		case WM_KEYUP: {
			pressed[wParam] = false;
			just_released[wParam] = true;
			break;
		}

		case WM_SIZE: {
			window_size.x = LOWORD(lParam);
			window_size.y = HIWORD(lParam);
			on_resize();
			break;
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}


#endif //CHAO_H