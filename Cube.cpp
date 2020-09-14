#include <Windows.h>

#include <math.h>

#define Assert(condition) if(!(condition)) DebugBreak();
#define func

struct Bitmap
{
	unsigned int *memory;
	int width;
	int height;
};

static Bitmap global_bitmap;
static bool global_running;

static void
func ResizeBitmap(Bitmap *bitmap, int width, int height)
{
	delete bitmap->memory;

	bitmap->width = width;
	bitmap->height = height;

	bitmap->memory = new unsigned int[width * height];
}

static LRESULT CALLBACK
func WinCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch(message)
	{
		case WM_SIZE:
		{
			RECT rect = {};
			GetClientRect(window, &rect);
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			ResizeBitmap(&global_bitmap, width, height);

			break;
		}
		case WM_DESTROY:
		case WM_CLOSE:
		{
			global_running = false;
			break;
		}
		default:
		{
			result = DefWindowProc(window, message, wparam, lparam);
			break;
		}
	}

	return result;
}

static void
func SetPixelColor(Bitmap *bitmap, int row, int col, unsigned int color)
{
	Assert(row >= 0 && row < bitmap->height);
	Assert(col >= 0 && col < bitmap->width);

	bitmap->memory[row * bitmap->width + col] = color;
}

struct V2
{
	float x, y;
};

static V2
func Point(float x, float y)
{
	V2 p = {};
	p.x = x;
	p.y = y;
	return p;
}

static V2
func Vector(float x, float y)
{
	V2 v = {};
	v.x = x;
	v.y = y;
	return v;
}

static V2
operator*(float a, V2 p)
{
	V2 r = {};
	r.x = a * p.x;
	r.y = a * p.y;
	return r;
}

static V2
operator+(V2 p1, V2 p2)
{
	V2 p = {};
	p.x = p1.x + p2.x;
	p.y = p1.y + p2.y;
	return p;
}

static V2
operator-(V2 p1, V2 p2)
{
	V2 p = {};
	p.x = p1.x - p2.x;
	p.y = p1.y - p2.y;
	return p;
}

static bool
func TurnsRight(V2 p0, V2 p1, V2 p2)
{
	V2 d0 = p1 - p0;
	V2 d1 = p2 - p1;

	float det = (d0.x * d1.y) - (d0.y * d1.x);
	bool turns_right = (det < 0.0f);
	return turns_right;
}

struct Quad
{
	V2 p[4];
};

static bool
func IsValidQuad(Quad q)
{
	bool is_valid = true;
	is_valid &= TurnsRight(q.p[0], q.p[1], q.p[2]);
	is_valid &= TurnsRight(q.p[1], q.p[2], q.p[3]);
	is_valid &= TurnsRight(q.p[2], q.p[3], q.p[0]);
	is_valid &= TurnsRight(q.p[3], q.p[0], q.p[1]);
	return is_valid;
}

static bool
func IsPointInQuad(V2 p, Quad q)
{
	bool is_inside = true;
	is_inside &= TurnsRight(q.p[0], q.p[1], p);
	is_inside &= TurnsRight(q.p[1], q.p[2], p);
	is_inside &= TurnsRight(q.p[2], q.p[3], p);
	is_inside &= TurnsRight(q.p[3], q.p[0], p);
	return is_inside;
}

static void
func DrawQuad(Bitmap *bitmap, Quad quad, unsigned int color)
{
	Assert(IsValidQuad(quad));

	float min_x = quad.p[0].x;
	float max_x = quad.p[0].x;
	float min_y = quad.p[0].y;
	float max_y = quad.p[0].y;
	for(int i = 1; i < 4; i++)
	{
		float x = quad.p[i].x;
		float y = quad.p[i].y;
		if(x < min_x) min_x = x;
		if(x > max_x) max_x = x;
		if(y < min_y) min_y = y;
		if(y > max_y) max_y = y;
	}

	for(int row = (int)(min_y); row < (int)(max_y) + 1; row++)
	{
		for(int col = (int)(min_x); col < (int)(max_x) + 1; col++)
		{
			V2 p = Point((float)col, (float)row);
			if(IsPointInQuad(p, quad)) SetPixelColor(bitmap, row, col, color);
		}
	}
}

static void
func DrawScene(Bitmap *bitmap)
{
	unsigned int color = 0xAAAAAA;
	unsigned int *pixel = bitmap->memory;
	for(int row = 0; row < bitmap->height; row++)
	{
		for(int col = 0; col < bitmap->width; col++)
		{
			*pixel = color;
			pixel++;
		}
	}

	unsigned int quad_color = 0xFF0000;

	static float theta = 0.0f;
	theta += 0.01f;

	V2 center = Point(0.5f * (float)bitmap->width, 0.5f * (float)bitmap->height);
	
	V2 x_axis = Vector(cosf(theta), sinf(theta));
	V2 y_axis = Vector(-sinf(theta), cosf(theta));

	float x_radius = 50.0f;
	float y_radius = 80.0f;

	V2 p_x = x_radius * x_axis;
	V2 p_y = y_radius * y_axis;

	Quad quad = {};
	quad.p[0] = center - p_x + p_y;
	quad.p[1] = center + p_x + p_y;
	quad.p[2] = center + p_x - p_y;
	quad.p[3] = center - p_x - p_y;
	DrawQuad(bitmap, quad, quad_color);

	/*
	for(int row = bottom; row <= top; row++)
	{
		for(int col = left; col <= right; col++)
		{
			SetPixelColor(bitmap, row, col, rect_color);
		}
	}
	*/
}

int CALLBACK
func WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
	WNDCLASS win_class = {};
	win_class.style = CS_OWNDC;
	win_class.lpfnWndProc = WinCallback;
	win_class.hInstance = instance;
	win_class.lpszClassName = "CubeWC";

	RegisterClass(&win_class);
	HWND window = CreateWindowEx(
		0,
		win_class.lpszClassName,
		"Cube",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		instance,
		0
	);
	Assert(window != 0);

	Bitmap *bitmap = &global_bitmap;
	global_running = true;
	while(global_running)
	{
		MSG message = {};
		while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		HDC context = GetDC(window);
		BITMAPINFO bitmap_info = {};
		BITMAPINFOHEADER *header = &bitmap_info.bmiHeader;
		header->biSize = sizeof(*header);
		header->biWidth = bitmap->width;
		header->biHeight = bitmap->height;
		header->biPlanes = 1;
		header->biBitCount = 32;
		header->biCompression = BI_RGB;

		RECT rect = {};
		GetClientRect(window, &rect);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		DrawScene(bitmap);

		StretchDIBits(context,
					  0, 0, bitmap->width, bitmap->height,
					  0, 0, width, height,
					  bitmap->memory,
					  &bitmap_info,
					  DIB_RGB_COLORS,
					  SRCCOPY
		);
		ReleaseDC(window, context);
	}

	return 0;
}