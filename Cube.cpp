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
func Point2(float x, float y)
{
	V2 p = {};
	p.x = x;
	p.y = y;
	return p;
}

static V2
func Vector2(float x, float y)
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

struct V3
{
	float x, y, z;
};

static V3
func Point3(float x, float y, float z)
{
	V3 p = {};
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}

static V3
func Vector3(float x, float y, float z)
{
	V3 v = {};
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

static V3
operator*(float a, V3 p)
{
	V3 r = {};
	r.x = a * p.x;
	r.y = a * p.y;
	r.z = a * p.z;
	return r;
}

static V3
operator+(V3 p1, V3 p2)
{
	V3 p = {};
	p.x = p1.x + p2.x;
	p.y = p1.y + p2.y;
	p.z = p1.z + p2.z;
	return p;
}

struct M3x3
{
	float v[3][3];
};

static M3x3
func ColumnMatrix(V3 col1, V3 col2, V3 col3)
{
	M3x3 m = 
	{
		{
			{col1.x, col2.x, col3.x},
			{col1.y, col2.y, col3.y},
			{col1.z, col2.z, col3.z}
		}
	};

	return m;
}

static V3
operator*(M3x3 m, V3 v)
{
	V3 r = {};
	r.x = m.v[0][0] * v.x + m.v[0][1] * v.y + m.v[0][2] * v.z;
	r.y = m.v[1][0] * v.x + m.v[1][1] * v.y + m.v[1][2] * v.z;
	r.z = m.v[2][0] * v.x + m.v[2][1] * v.y + m.v[2][2] * v.z;
	return r;
}

static M3x3
operator*(M3x3 m1, M3x3 m2)
{
	M3x3 r = {};
	for(int row = 0; row < 3; row++)
	{
		for(int col = 0; col < 3; col++)
		{
			r.v[row][col] = 0;
			for(int i = 0; i < 3; i++)
			{
				r.v[row][col] += m1.v[row][i] * m2.v[i][col];
			}
		}
	}

	return r;
}

static M3x3
func GetZAxisRotation(float theta)
{
	M3x3 m =
	{
		{
			{cosf(theta), -sinf(theta), 0},
			{sinf(theta),  cosf(theta), 0},
			{           0,           0, 1}
		}
	};

	return m;
}

static M3x3
func GetYAxisRotation(float theta)
{
	M3x3 m =
	{
		{
			{cosf(theta),  0, sinf(theta)},
			{          0,  1,           0},
			{-sinf(theta), 0, cosf(theta)}
		}
	};

	return m;
}

static M3x3
func GetXAxisRotation(float theta)
{
	M3x3 m =
	{
		{
			{1,           0,           0},
			{0, cosf(theta), -sinf(theta)},
			{0, sinf(theta),  cosf(theta)}
		}
	};

	return m;
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

struct Quad2
{
	V2 p[4];
};

struct Quad3
{
	V3 p[4];
};

static bool
func IsValidQuad2(Quad2 q)
{
	bool is_valid = true;
	is_valid &= TurnsRight(q.p[0], q.p[1], q.p[2]);
	is_valid &= TurnsRight(q.p[1], q.p[2], q.p[3]);
	is_valid &= TurnsRight(q.p[2], q.p[3], q.p[0]);
	is_valid &= TurnsRight(q.p[3], q.p[0], q.p[1]);
	return is_valid;
}

static bool
func IsPointInQuad2(V2 p, Quad2 q)
{
	bool is_inside = true;
	is_inside &= TurnsRight(q.p[0], q.p[1], p);
	is_inside &= TurnsRight(q.p[1], q.p[2], p);
	is_inside &= TurnsRight(q.p[2], q.p[3], p);
	is_inside &= TurnsRight(q.p[3], q.p[0], p);
	return is_inside;
}

static void
func DrawQuad2(Bitmap *bitmap, Quad2 quad, unsigned int color)
{
	Assert(IsValidQuad2(quad));

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
			V2 p = Point2((float)col, (float)row);
			if(IsPointInQuad2(p, quad)) SetPixelColor(bitmap, row, col, color);
		}
	}
}

static V2
func ProjectToScreen(V3 p3)
{
	V2 p2 = Point2(p3.x, p3.y);
	return p2;
}

static void
func DrawQuad3(Bitmap *bitmap, Quad3 quad3, unsigned int color)
{
	Quad2 quad2 = {};
	for(int i = 0; i < 4; i++)
	{
		quad2.p[i] = ProjectToScreen(quad3.p[i]);
	}

	if(IsValidQuad2(quad2)) DrawQuad2(bitmap, quad2, color);
}

enum CubeEdge
{
	CUBE_EDGE_LUF,
	CUBE_EDGE_LUB,
	CUBE_EDGE_LDF,
	CUBE_EDGE_LDB,
	CUBE_EDGE_RUF,
	CUBE_EDGE_RUB,
	CUBE_EDGE_RDF,
	CUBE_EDGE_RDB
};

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

	V3 unit_cube_corners[8] = {};
	unit_cube_corners[CUBE_EDGE_LUF] = Point3(-1.0, +1.0, +1.0f);
	unit_cube_corners[CUBE_EDGE_LUB] = Point3(-1.0, +1.0, -1.0f);
	unit_cube_corners[CUBE_EDGE_LDF] = Point3(-1.0, -1.0, +1.0f);
	unit_cube_corners[CUBE_EDGE_LDB] = Point3(-1.0, -1.0, -1.0f);
	unit_cube_corners[CUBE_EDGE_RUF] = Point3(+1.0, +1.0, +1.0f);
	unit_cube_corners[CUBE_EDGE_RUB] = Point3(+1.0, +1.0, -1.0f);
	unit_cube_corners[CUBE_EDGE_RDF] = Point3(+1.0, -1.0, +1.0f);
	unit_cube_corners[CUBE_EDGE_RDB] = Point3(+1.0, -1.0, -1.0f);

	int cube_face_corners[6][4] = 
	{
		{CUBE_EDGE_LUB, CUBE_EDGE_LUF, CUBE_EDGE_LDF, CUBE_EDGE_LDB},
		{CUBE_EDGE_RUF, CUBE_EDGE_RUB, CUBE_EDGE_RDB, CUBE_EDGE_RDF},
		{CUBE_EDGE_LUF, CUBE_EDGE_LUB, CUBE_EDGE_RUB, CUBE_EDGE_RUF},
		{CUBE_EDGE_LDB, CUBE_EDGE_LDF, CUBE_EDGE_RDF, CUBE_EDGE_RDB},
		{CUBE_EDGE_LUF, CUBE_EDGE_RUF, CUBE_EDGE_RDF, CUBE_EDGE_LDF},
		{CUBE_EDGE_LUB, CUBE_EDGE_LDB, CUBE_EDGE_RDB, CUBE_EDGE_RUB}
	};

	unsigned int cube_face_colors[6] =
	{
		0xFF8800,
		0xFF0000,
		0xFFFFFF,
		0xFFFF00,
		0x00FF00,
		0x0000FF
	};

	static float theta = 0.0f;
	theta += 0.01f;

#ifndef M_PI
	#define M_PI 3.141592653589797f
#endif

	if(theta > 2.0 * M_PI) theta -= 2.0f * M_PI;

	V3 screen_center = 0.5f * Point3((float)bitmap->width, (float)bitmap->height, 0.0f);
	float side_radius = 50.0f;

	V3 cube_corners[8] = {};

	M3x3 transform_x = GetXAxisRotation(theta);
	M3x3 transform_y = GetYAxisRotation(theta);
	M3x3 transform_z = GetZAxisRotation(theta);

	M3x3 transform = transform_x * transform_y * transform_z;

	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = screen_center + side_radius * (transform * unit_cube_corners[corner_id]);
	}

	/*
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = 
			screen_center + 
			side_radius * unit_cube_corners[corner_id].x * x_axis +
			side_radius * unit_cube_corners[corner_id].y * y_axis +
			side_radius * unit_cube_corners[corner_id].z * z_axis;
	}
	*/

	for(int face_id = 0; face_id < 6; face_id++)
	{
		Quad3 q3 = {};
		for(int corner_id = 0; corner_id < 4; corner_id++)
		{
			int edge_id = cube_face_corners[face_id][corner_id];
			q3.p[corner_id] = cube_corners[edge_id];
		}

		unsigned int color = cube_face_colors[face_id];
		DrawQuad3(bitmap, q3, color);
	}
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