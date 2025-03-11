struct V2
{
	float x, y;
};

static int
func IntAbs(int x)
{
	int abs_x = (x > 0) ? x : (-x);
	return abs_x;
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

union Quat
{
	struct
	{
		float w;
		float x;
		float y;
		float z;
	};
};

// i * i = j * j = k * k = i * j * k = -1

static Quat
func PositionToQuat(V3 pos)
{
	Quat q = {};
	q.w = 0.0f;
	q.x = pos.x;
	q.y = pos.y;
	q.z = pos.z;
	return q;
}

static V3
func QuatToPosition(Quat q)
{
	V3 p = {};
	p.x = q.x;
	p.y = q.y;
	p.z = q.z;
	return p;
}

static Quat
func QuatConjugate(Quat q)
{
	Quat q1 = {};
	q1.w = q.w;
	q1.x = -q.x;
	q1.y = -q.y;
	q1.z = -q.z;
	return q1;
}

static Quat
operator *(Quat q1, Quat q2)
{
	Quat q = {};

	int I = 1;
	int J = 2;
	int K = 3;

	q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	q.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	q.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;

	return q;
}

static Quat
func GetIdentityRotationQuat()
{
	Quat q = {};
	q.w = 1.0f;
	q.x = 0.0f;
	q.y = 0.0f;
	q.z = 0.0f;
	return q;
}

static V3
func QuatRotate(Quat rotations, V3 position)
{
	Quat pq = PositionToQuat(position);

	Quat rotations_inv = QuatConjugate(rotations);
	Quat rq = rotations * pq * rotations_inv;

	return QuatToPosition(rq);
}

static Quat
func GetRotationQuat(V3 axis, float angle)
{
	float s = sinf(angle * 0.5f);
	float c = cosf(angle * 0.5f);

	Quat q = {};
	q.w = c;
	q.x = s * axis.x;
	q.y = s * axis.y;
	q.z = s * axis.z;
	return q;
}