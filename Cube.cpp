#include <Windows.h>

#include <math.h>

#define CUBE_N 3

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
static bool global_left_mouse_button_down;

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
		case WM_LBUTTONDOWN:
		{
			global_left_mouse_button_down = true;
			break;
		}
		case WM_LBUTTONUP:
		{
			global_left_mouse_button_down = false;
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

static unsigned int
func GetPixelColorChecked(Bitmap *bitmap, int row, int col)
{
	unsigned int color = 0;
	if((row >= 0 && row < bitmap->height) && (col >= 0 && col < bitmap->width))
	{
		color = bitmap->memory[row * bitmap->width + col];
	}

	return color;
}

struct V2
{
	float x, y;
};

struct BresenhamContext
{
	int x1, y1;
	int x2, y2;
	int abs_x, abs_y;
	int add_x, add_y;
	int error, error2;
};

static int
func IntAbs(int x)
{
	int abs_x = (x > 0) ? x : (-x);
	return abs_x;
}

static BresenhamContext
func BresenhamInit(V2 p1, V2 p2)
{
	BresenhamContext context = {};
	context.x1 = (int)p1.x;
	context.y1 = (int)p1.y;
	context.x2 = (int)p2.x;
	context.y2 = (int)p2.y;

	context.abs_x = IntAbs(context.x1 - context.x2);
	context.abs_y = IntAbs(context.y1 - context.y2);

	context.add_x = 1;
	if(context.x1 > context.x2) context.add_x = -1;

	context.add_y = 1;
	if(context.y1 > context.y2) context.add_y = -1;

	context.error = 0;
	if(context.abs_x > context.abs_y) context.error = context.abs_x / 2;
	else context.error = -context.abs_y / 2;

	context.error2 = 0;

	return context;
}

static void
func BresenhamAdvance(BresenhamContext *context)
{
	context->error2 = context->error;
	if(context->error2 > -context->abs_x)
	{
		context->error -= context->abs_y;
		context->x1 += context->add_x;
	}
	if(context->error2 < context->abs_y)
	{
		context->error += context->abs_x;
		context->y1 += context->add_y;
	}
}

static void
func Bresenham(Bitmap *bitmap, V2 p1, V2 p2, unsigned int color)
{
	BresenhamContext context = BresenhamInit(p1, p2);
	while(1)
	{
		SetPixelColor(bitmap, context.y1, context.x1, color);
		if(context.x1 == context.x2 && context.y1 == context.y2)
		{
			break;
		}

		BresenhamAdvance(&context);
	}
}

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

static V3
operator-(V3 p1, V3 p2)
{
	V3 p = {};
	p.x = p1.x - p2.x;
	p.y = p1.y - p2.y;
	p.z = p1.z - p2.z;
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
func GetIdentityMatrix()
{
	M3x3 m =
	{
		{
			{1, 0, 0},
			{0, 1, 0},
			{0, 0, 1}
		}
	};

	return m;
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
func DrawLine3(Bitmap *bitmap, V3 p1, V3 p2, unsigned int color)
{
	V2 p1_projected = ProjectToScreen(p1);
	V2 p2_projected = ProjectToScreen(p2);
	Bresenham(bitmap, p1_projected, p2_projected, color);
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

enum CubeCorner
{
	CORNER_LUF,
	CORNER_LUB,
	CORNER_LDF,
	CORNER_LDB,
	CORNER_RUF,
	CORNER_RUB,
	CORNER_RDF,
	CORNER_RDB
};

V3 unit_cube_corners[8] = 
{
	{-1.0, +1.0, +1.0f},
	{-1.0, +1.0, -1.0f},
	{-1.0, -1.0, +1.0f},
	{-1.0, -1.0, -1.0f},
	{+1.0, +1.0, +1.0f},
	{+1.0, +1.0, -1.0f},
	{+1.0, -1.0, +1.0f},
	{+1.0, -1.0, -1.0f}
};

int cube_face_corners[6][4] = 
{
	{CORNER_LUB, CORNER_LUF, CORNER_LDF, CORNER_LDB},
	{CORNER_RUF, CORNER_RUB, CORNER_RDB, CORNER_RDF},
	{CORNER_LUF, CORNER_LUB, CORNER_RUB, CORNER_RUF},
	{CORNER_LDB, CORNER_LDF, CORNER_RDF, CORNER_RDB},
	{CORNER_LUF, CORNER_RUF, CORNER_RDF, CORNER_LDF},
	{CORNER_LUB, CORNER_LDB, CORNER_RDB, CORNER_RUB}
};

int cube_edges[12][2] = 
{
	{CORNER_LUF, CORNER_RUF}, {CORNER_RUF, CORNER_RUB}, 
	{CORNER_RUB, CORNER_LUB}, {CORNER_LUB, CORNER_LUF},
		
	{CORNER_LDF, CORNER_RDF}, {CORNER_RDF, CORNER_RDB}, 
	{CORNER_RDB, CORNER_LDB}, {CORNER_LDB, CORNER_LDF},

	{CORNER_LUF, CORNER_LDF}, {CORNER_RUF, CORNER_RDF},
	{CORNER_LUB, CORNER_LDB}, {CORNER_RUB, CORNER_RDB}
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

struct Cube
{
	V3 center;
	int id;
	float radius;
};

static unsigned int
func AugmentColorWithCubeIndex(unsigned int color, int cube_index)
{
	Assert(cube_index >= 0 && cube_index < 256);

	unsigned int result = color | (cube_index << 24);
	return result;
}

static int
func GetCubeIndexFromAugmentedColor(unsigned int color)
{
	int cube_index = (int)(color >> 24);
	return cube_index;
}

static void
func DrawCube(Bitmap *bitmap, Cube cube, M3x3 rotations)
{
	V3 cube_corners[8] = {};
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = cube.center + cube.radius * (rotations * unit_cube_corners[corner_id]);
	}

	for(int face_id = 0; face_id < 6; face_id++)
	{
		Quad3 q3 = {};
		for(int face_corner_id = 0; face_corner_id < 4; face_corner_id++)
		{
			int corner_id = cube_face_corners[face_id][face_corner_id];
			q3.p[face_corner_id] = cube_corners[corner_id];
		}

		unsigned int color = cube_face_colors[face_id];
		unsigned int augmented_color = AugmentColorWithCubeIndex(color, cube.id);
		DrawQuad3(bitmap, q3, augmented_color);
	}

	float min_corner_z = cube_corners[0].z;
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		V3 corner = cube_corners[corner_id];
		if(corner.z < min_corner_z) min_corner_z = corner.z;
	}

	unsigned int edge_color = 0x000000;
	for(int edge_id = 0; edge_id < 12; edge_id++)
	{
		int corner_id1 = cube_edges[edge_id][0];
		int corner_id2 = cube_edges[edge_id][1];

		V3 corner1 = cube_corners[corner_id1];
		V3 corner2 = cube_corners[corner_id2];

		if(corner1.z > min_corner_z && corner2.z > min_corner_z)
		{
			DrawLine3(bitmap, corner1, corner2, edge_color);
		}
	}
}

static void
func HighlightCube(Bitmap *bitmap, Cube cube, M3x3 rotations)
{
	unsigned int color = 0x800000;

	V3 cube_corners[8] = {};
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = cube.center + cube.radius * (rotations * unit_cube_corners[corner_id]);
	}

	for(int face_id = 0; face_id < 6; face_id++)
	{
		Quad3 q3 = {};
		for(int face_corner_id = 0; face_corner_id < 4; face_corner_id++)
		{
			int corner_id = cube_face_corners[face_id][face_corner_id];
			q3.p[face_corner_id] = cube_corners[corner_id];
		}

		DrawQuad3(bitmap, q3, color);
	}
}

static bool
func CubesAreInOrder(Cube first_cube, Cube second_cube)
{
	bool in_order = (first_cube.center.z < second_cube.center.z);
	return in_order;
}

static void
func SwapCubes(Cube *cube1, Cube *cube2)
{
	Cube tmp = *cube1;
	*cube1 = *cube2;
	*cube2 = tmp;
}

static void
func SortCubesByZ(Cube *cubes, int cube_n)
{
	int i = 1;
	while(i < cube_n)
	{
		int j = i;
		while(j > 0 && !CubesAreInOrder(cubes[j - 1], cubes[j]))
		{
			SwapCubes(&cubes[j - 1], &cubes[j]);
			j--;
		}
		i++;
	}
}

static void
func DrawScene(Bitmap *bitmap, V2 mouse_position)
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

	static V2 prev_mouse_position;
	static M3x3 transform = GetIdentityMatrix();

	V2 mouse_position_diff = mouse_position - prev_mouse_position;

	prev_mouse_position = mouse_position;

	if(global_left_mouse_button_down)
	{
		float theta_y = mouse_position_diff.x / 100.0f;
		M3x3 rotate_y = GetYAxisRotation(theta_y);

		float theta_x = (-mouse_position_diff.y) / 100.0f;
		M3x3 rotate_x = GetXAxisRotation(theta_x);

		transform = rotate_x * rotate_y * transform;
	}

	V3 screen_center = 0.5f * Point3((float)bitmap->width, (float)bitmap->height, 0.0f);
	float side_radius = 100.0f;
	float small_side_radius = side_radius / (float)CUBE_N;

	Cube cubes[CUBE_N * CUBE_N * CUBE_N];
	int cube_n = sizeof(cubes) / sizeof(cubes[0]);

	int cube_id = 0;
	for(int x = 0; x < CUBE_N; x++)
	{
		float x_offset = -(0.5f * CUBE_N + 0.5f) + (2.0f * x);
		for(int y = 0; y < CUBE_N; y++)
		{
			float y_offset = -(0.5f * CUBE_N + 0.5f) + (2.0f * y);
			for(int z = 0; z < CUBE_N; z++)
			{
				float z_offset = -(0.5f * CUBE_N + 0.5f) + (2.0f * z);

				V3 offset = small_side_radius * Vector3(x_offset, y_offset, z_offset);
				offset = transform * offset;

				V3 center = screen_center + offset;
				Cube *cube = &cubes[cube_id];
				cube_id++;

				cube->id = cube_id;

				cube->center = center;
				cube->radius = small_side_radius;
			}
		}
	}

	SortCubesByZ(cubes, cube_n);
	for(int i = 0; i < cube_n; i++) 
	{
		DrawCube(bitmap, cubes[i], transform);
	}

	unsigned int color_at_mouse = GetPixelColorChecked(bitmap, (int)mouse_position.y, (int)mouse_position.x);
	unsigned int cube_index_at_mouse = GetCubeIndexFromAugmentedColor(color_at_mouse);

	Cube *cube_at_mouse = 0;
	for(int i = 0; i < cube_n; i++)
	{
		if(cubes[i].id == cube_index_at_mouse)
		{
			cube_at_mouse = &cubes[i];
		}
	}

	if(cube_at_mouse)
	{
		HighlightCube(bitmap, *cube_at_mouse, transform);
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

		POINT cursor_point = {};
		GetCursorPos(&cursor_point);
		ScreenToClient(window, &cursor_point);

		V2 mouse_position = Point2((float)cursor_point.x, (float)(height - cursor_point.y));

		DrawScene(bitmap, mouse_position);

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