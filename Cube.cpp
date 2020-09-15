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
static bool global_right_mouse_button_down;

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
		case WM_RBUTTONDOWN:
		{
			global_right_mouse_button_down = true;
			break;
		}
		case WM_RBUTTONUP:
		{
			global_right_mouse_button_down = false;
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
operator-(V3 v)
{
	V3 r = {};
	r.x = -v.x;
	r.y = -v.y;
	r.z = -v.z;
	return r;
}

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

static float
func Abs(float x)
{
	float abs_x = (x > 0.0f) ? x : -x;
	return abs_x;
}

static M3x3
func GetRotationAroundAxis(V3 axis, float theta)
{
	Assert(Abs(axis.x + axis.y + axis.z) == 1.0f);

	M3x3 rotation = {};
	if(axis.x == 1.0f) rotation = GetXAxisRotation(theta);
	else if(axis.x == -1.0f) rotation = GetXAxisRotation(-theta);
	else if(axis.y == 1.0f) rotation = GetYAxisRotation(theta);
	else if(axis.y == -1.0f) rotation = GetYAxisRotation(-theta);
	else if(axis.z == 1.0f) rotation = GetZAxisRotation(theta);
	else if(axis.z == -1.0f) rotation = GetZAxisRotation(-theta);
	else
	{
		DebugBreak();
	}

	return rotation;
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

enum CubeFace
{
	FACE_L,
	FACE_R,
	FACE_U,
	FACE_D,
	FACE_F,
	FACE_B
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
	M3x3 rotations;
	V3 center_base;
	V3 center_final;
	int id;
	float radius;
	bool is_rotating;
};

static V3
func GetCubeFaceNormalVector(int face_id)
{
	V3 normal = {};

	switch(face_id)
	{
		case FACE_L:
		{
			normal = Vector3(-1, 0, 0);
			break;
		}
		case FACE_R:
		{
			normal = Vector3(+1, 0, 0);
			break;
		}
		case FACE_U:
		{
			normal = Vector3(0, +1, 0);
			break;
		}
		case FACE_D:
		{
			normal = Vector3(0, -1, 0);
			break;
		}
		case FACE_F:
		{
			normal = Vector3(0, 0, +1);
			break;
		}
		case FACE_B:
		{
			normal = Vector3(0, 0, -1);
			break;
		}
		default:
		{
			DebugBreak();
		}
	}

	return normal;
}

static float
func Dot2(V2 v1, V2 v2)
{
	float prod = v1.x * v2.x + v1.y * v2.y;
	return prod;
}

static float
func Dot3(V3 v1, V3 v2)
{
	float prod = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return prod;
}

static unsigned int
func AugmentColor(unsigned int color, int augment_value)
{
	Assert(augment_value >= 0 && augment_value < 256);

	unsigned int result = color | (augment_value << 24);
	return result;
}

static int
func GetColorAugmentValue(unsigned int color)
{
	int augment_value = (int)(color >> 24);
	return augment_value;
}

static void
func DrawCube(Bitmap *bitmap, Cube cube, M3x3 rotations)
{
	V3 cube_corners[8] = {};
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = cube.center_final +
			cube.radius * (rotations * (cube.rotations * unit_cube_corners[corner_id]));
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

		unsigned int cube_face_id = 6 * cube.id + face_id;
		unsigned int augmented_color = AugmentColor(color, cube_face_id);

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

static V3
func CrossProduct(V3 v1, V3 v2)
{
	/*
	   x    y    z
	v1.x v1.y v1.z
	v2.x v2.y v2.z
	*/

	V3 result = {};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

static bool
func IsRightHandedSystem(V3 x, V3 y, V3 z)
{
	V3 cross_xy = CrossProduct(x, y);
	float dot = Dot3(cross_xy, z);
	bool is_right_handed = (dot > 0.0f);
	return is_right_handed;
}

static V3
func ShiftVector(V3 v)
{
	V3 r = {};
	r.y = v.x;
	r.z = v.y;
	r.x = v.z;
	return r;
}

static void
func HighlightCube(Bitmap *bitmap, Cube cube, M3x3 rotations)
{
	unsigned int color = 0x800000;

	V3 cube_corners[8] = {};
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = cube.center_final + cube.radius * (rotations * unit_cube_corners[corner_id]);
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

/*
static void
func HighlightCubeFace(Bitmap *bitmap, Cube cube, M3x3 rotations, int face_id)
{
	unsigned int side_color = 0x800000;

	V3 cube_corners[8] = {};
	for(int corner_id = 0; corner_id < 8; corner_id++)
	{
		cube_corners[corner_id] = cube.center_final + cube.radius * (rotations * unit_cube_corners[corner_id]);
	}

	Quad3 q3 = {};
	for(int face_corner_id = 0; face_corner_id < 4; face_corner_id++)
	{
		int corner_id = cube_face_corners[face_id][face_corner_id];
		q3.p[face_corner_id] = cube_corners[corner_id];
	}

	DrawQuad3(bitmap, q3, side_color);

	unsigned int line_color = 0x400000;

	V3 normal = GetCubeFaceNormalVector(face_id);

	V3 p1 = cube.center_final + cube.radius * (rotations * normal);
	V3 p2 = cube.center_final + (2.0f * cube.radius) * (rotations * normal);

	DrawLine3(bitmap, p1, p2, line_color);

	unsigned int rotation_line_color = 0xFF00FF;

	V3 rotation_vector1 = ShiftVector(normal);
	V3 rotation_vector2 = ShiftVector(rotation_vector1);

	float rotation_line_length = (2.0f * cube.radius);

	V3 rotation_line1_p1 = p1 - rotation_line_length * (rotations * rotation_vector1);
	V3 rotation_line1_p2 = p1 + rotation_line_length * (rotations * rotation_vector1);

	V3 rotation_line2_p1 = p1 - rotation_line_length * (rotations * rotation_vector2);
	V3 rotation_line2_p2 = p1 + rotation_line_length * (rotations * rotation_vector2);

	DrawLine3(bitmap, rotation_line1_p1, rotation_line1_p2, rotation_line_color);
	DrawLine3(bitmap, rotation_line2_p1, rotation_line2_p2, rotation_line_color);
}
*/

static bool
func CubesAreInOrder(Cube first_cube, Cube second_cube)
{
	bool in_order = (first_cube.center_final.z < second_cube.center_final.z);
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

struct BigCube
{
	M3x3 rotations;

	Cube cubes[CUBE_N * CUBE_N * CUBE_N];
	int cube_n;
};

static BigCube
func InitBigCube()
{
	float side_radius = 100.0f;
	float small_side_radius = side_radius / (float)CUBE_N;

	BigCube big_cube = {};
	big_cube.cube_n = sizeof(big_cube.cubes) / sizeof(big_cube.cubes[0]);
	big_cube.rotations = GetIdentityMatrix();

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

				V3 offset_base = small_side_radius * Vector3(x_offset, y_offset, z_offset);

				Cube *cube = &big_cube.cubes[cube_id];
				cube_id++;

				cube->id = cube_id;

				cube->rotations = GetIdentityMatrix();
				cube->center_base = offset_base;
				cube->radius = small_side_radius;
			}
		}
	}

	return big_cube;
}

static M3x3
func NormalizeMatrixValues(M3x3 m)
{
	M3x3 r = {};
	for(int row = 0; row < 3; row++)
	{
		for(int col = 0; col < 3; col++)
		{
			float m_value = m.v[row][col];
			float r_value = 0.0f;
			if(m_value < -0.5f) r_value = -1.0f;
			if(m_value > +0.5f) r_value = +1.0f;

			r.v[row][col] = r_value;
		}
	}

	return r;
}

static float 
func RoundToHalfPi(float x)
{
	float half_pi = 0.5f * 3.141592653589793238462643383f;
	float rounded_x = x;

	int times = (int)(x / half_pi);

	float low = (float)times * half_pi;
	if(x < 0) low -= half_pi;

	float high = low + half_pi;
	Assert(low <= x);
	Assert(x <= high);

	float low_abs = x - low;
	float high_abs = high - x;
	if(low_abs < high_abs) rounded_x = low;
	else rounded_x = high;

	return rounded_x;
}

static void
func DrawScene(Bitmap *bitmap, BigCube *big_cube, V2 mouse_position)
{
	static bool is_rotating = false;
	static bool big_cube_rotation = false;

	static int rotating_cube_id;
	static int rotating_face_id;
	static V2 clicked_pixel;
	static V3 rotation1_vector;
	static V3 rotation2_vector;
	static V2 rotation1_vector_pixel;
	static V2 rotation2_vector_pixel;

	bool is_side_rotating = (is_rotating && !big_cube_rotation);

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

	V2 mouse_position_diff = mouse_position - prev_mouse_position;

	prev_mouse_position = mouse_position;

	if(is_rotating && big_cube_rotation)
	{
		float theta_y = mouse_position_diff.x / 100.0f;
		M3x3 rotate_y = GetYAxisRotation(theta_y);

		float theta_x = (-mouse_position_diff.y) / 100.0f;
		M3x3 rotate_x = GetXAxisRotation(theta_x);

		big_cube->rotations = rotate_x * rotate_y * big_cube->rotations;
	}

	V3 screen_center = 0.5f * Point3((float)bitmap->width, (float)bitmap->height, 0.0f);
	for(int i = 0; i < big_cube->cube_n; i++)
	{
		Cube *cube = &big_cube->cubes[i];
		cube->center_final = screen_center + big_cube->rotations * cube->center_base;
	}

	for(int i = 0; i < big_cube->cube_n; i++)
	{
		big_cube->cubes[i].is_rotating = false;
	}

	M3x3 side_rotation = GetIdentityMatrix();
	if(is_side_rotating)
	{
		if(global_right_mouse_button_down)
		{
			DebugBreak();
			global_right_mouse_button_down = false;
		}

		V2 mouse_diff = mouse_position - clicked_pixel;
		float rotation1_distance = Dot2(mouse_diff, rotation1_vector_pixel);
		float rotation2_distance = Dot2(mouse_diff, rotation2_vector_pixel);

		bool use_rotation1 = (Abs(rotation1_distance) > Abs(rotation2_distance));
		
		V3 rotation_vector_base = use_rotation1 ? rotation1_vector : rotation2_vector;
		V3 rotation_perp_vector_base = use_rotation1 ? rotation2_vector : -rotation1_vector;

		V3 rotation_vector = big_cube->rotations * rotation_vector_base;
		V3 rotation_perp_vector = big_cube->rotations * rotation_perp_vector_base;

		float rotation_distance = use_rotation1 ? rotation1_distance : rotation2_distance;
		float theta = rotation_distance / 50.0f;
		
		side_rotation = GetRotationAroundAxis(rotation_perp_vector_base, theta);
	
		Cube *rotating_cube = 0;
		for(int i = 0; i < big_cube->cube_n; i++)
		{
			if(big_cube->cubes[i].id == rotating_cube_id) rotating_cube = &big_cube->cubes[i];
		}
		Assert(rotating_cube);

		float cube_radius = rotating_cube->radius;

		float base_perp_distance = Dot3(rotating_cube->center_final, rotation_perp_vector);
		for(int i = 0; i < big_cube->cube_n; i++)
		{
			Cube *cube = &big_cube->cubes[i];
			float perp_distance = Dot3(cube->center_final, rotation_perp_vector);
			if(Abs(perp_distance - base_perp_distance) < cube_radius)
			{
				cube->is_rotating = true;
				V3 center_base_rotated = side_rotation * cube->center_base;
				cube->center_final = screen_center + big_cube->rotations * center_base_rotated;
			}
			else
			{
				cube->is_rotating = false;
			}
		}
		
		if(!global_left_mouse_button_down)
		{
			float rounded_theta = RoundToHalfPi(theta);
			M3x3 rotation_to_apply = GetRotationAroundAxis(rotation_perp_vector_base, rounded_theta);
			rotation_to_apply = NormalizeMatrixValues(rotation_to_apply);
			for(int i = 0; i < big_cube->cube_n; i++)
			{
				Cube *cube = &big_cube->cubes[i];
				if(cube->is_rotating)
				{
					cube->rotations = rotation_to_apply * cube->rotations;
					cube->center_base = rotation_to_apply * cube->center_base;
					cube->is_rotating = false;
				}
			}

			side_rotation = GetIdentityMatrix();
			is_rotating = false;
		}
	}

	SortCubesByZ(big_cube->cubes, big_cube->cube_n);

	M3x3 side_rotation_transform = big_cube->rotations * side_rotation;
	for(int i = 0; i < big_cube->cube_n; i++) 
	{
		Cube cube = big_cube->cubes[i];

		M3x3 cube_transform = big_cube->rotations;

		if(cube.is_rotating) cube_transform = side_rotation_transform;

		DrawCube(bitmap, cube, cube_transform);
	}

	unsigned int picked_color = GetPixelColorChecked(bitmap, (int)mouse_position.y, (int)mouse_position.x);
	int picked_cube_face_id = GetColorAugmentValue(picked_color);
	int picked_cube_id = picked_cube_face_id / 6;
	int picked_face_id = picked_cube_face_id % 6;

	Cube *cube_at_mouse = 0;
	for(int i = 0; i < big_cube->cube_n; i++)
	{
		if(big_cube->cubes[i].id == picked_cube_id)
		{
			cube_at_mouse = &big_cube->cubes[i];
		}
	}

	if(!global_left_mouse_button_down)
	{
		is_rotating = false;
	}
	else if(!is_rotating)
	{
		is_rotating = true;
		if(cube_at_mouse == 0)
		{
			big_cube_rotation = true;
		}
		else
		{
			big_cube_rotation = false;
			rotating_cube_id = picked_cube_id;
			rotating_face_id = picked_face_id;

			clicked_pixel = mouse_position;

			float rotation_line_length = cube_at_mouse->radius;
			V3 face_normal = cube_at_mouse->rotations * GetCubeFaceNormalVector(rotating_face_id);

			rotation1_vector = ShiftVector(face_normal);
			rotation2_vector = ShiftVector(rotation1_vector);

			if(!IsRightHandedSystem(face_normal, rotation1_vector, rotation2_vector))
			{
				rotation2_vector = -rotation2_vector;
			}
			Assert(IsRightHandedSystem(face_normal, rotation1_vector, rotation2_vector));

			rotation1_vector_pixel = ProjectToScreen(big_cube->rotations * rotation1_vector);
			rotation2_vector_pixel = ProjectToScreen(big_cube->rotations * rotation2_vector);
		}
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

	BigCube big_cube = InitBigCube();

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

		DrawScene(bitmap, &big_cube, mouse_position);

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