typedef	char s8;
typedef	unsigned char u8;
typedef	short s16;
typedef	unsigned short u16;
typedef	int s32;
typedef	unsigned int u32;
typedef	long long s64;
typedef	unsigned long long u64;

#define global_variable static
#define internal static

u32 rgba_to_hex(int r, int g, int b, int a)
{
	return (u32)((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

u32 rgb_to_hex(int r, int g, int b)
{
	return (u32)((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

inline int
clamp(int min, int val, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

inline float
clamp(float min, float val, float max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

struct int2 {
	int x, y;
};

struct vec2d {
	float u = 0.f;
	float v = 0.f;
	float w = 1.f; 

	vec2d()								: u(0.f), v(0.f), w(1.f) {}
	vec2d(float u, float v)				: u(u),   v(v),   w(1.f) {}
	vec2d(float u, float v, float w)	: u(u),   v(v),   w(w)	 {}
	vec2d(const vec2d& v)				: u(v.u), v(v.v), w(v.w) {}

	float  mag		   ()					{ return sqrt(u * u + v * v);						}
	vec2d  norm		   ()					{ float r = 1 / mag(); return vec2d(u * r, v * r);	}
	vec2d  perp		   ()					{ return vec2d(-v, u);								}
	float  dot	       (const vec2d& rhs)	{ return this->u * rhs.u + this->v * rhs.v;			}
	float  cross	   (const vec2d& rhs)	{ return this->u * rhs.v + this->v * rhs.u;			}

	vec2d  operator +  (const vec2d& rhs)	{ return vec2d(this->u + rhs.u, this->v + rhs.v);	}
	vec2d  operator -  (const vec2d& rhs)	{ return vec2d(this->u - rhs.u, this->v - rhs.v);	}
	vec2d  operator *  (const float& rhs)	{ return vec2d(this->u * rhs,   this->v * rhs);		}
	vec2d  operator /  (const float& rhs)	{ return vec2d(this->u / rhs,   this->v / rhs);		}

	vec2d& operator += (const vec2d& rhs)	{ this->u += rhs.u; this->v += rhs.v; return *this; }
	vec2d& operator -= (const vec2d& rhs)	{ this->u -= rhs.u; this->v -= rhs.v; return *this; }
	vec2d& operator *= (const float  rhs)	{ this->u *= rhs;   this->v *= rhs;	  return *this; } 
	vec2d& operator /= (const float  rhs)	{ this->u /= rhs;   this->v /= rhs;	  return *this; }
};

#define	vec2d_right	vec2d( 1.f,  0.f)
#define	vec2d_left	vec2d(-1.f,  0.f)
#define	vec2d_up	vec2d( 0.f,  1.f)
#define	vec2d_down	vec2d( 0.f, -1.f)

typedef struct vec3d vec3d;

struct vec3d {
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;  // Need a 4th term to perform sensible matrix vector multiplication

	vec3d()										: x(0.f), y(0.f), z(0.f), w(1.f) {}
	vec3d(float x, float y, float z)			: x(x),   y(y),   z(z),	  w(1.f) {}
	vec3d(float x, float y, float z, float w)	: x(x),   y(y),   z(z),   w(w)	 {}
	vec3d(const vec3d& v)						: x(v.x), y(v.y), z(v.z), w(v.w) {}

	float  mag		   ()					{ return sqrt(x * x + y * y + z * z);					              }
	vec3d  norm		   ()					{ float r = 1 / mag(); return vec3d(x * r, y * r, z * r);             }
	float  dot		   (const vec3d& rhs)	{ return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;		  }
	float  length      ()                   { float d = this->dot(*this); return sqrtf(d);						  }
	vec3d  cross	   (const vec3d& rhs)	{ return vec3d(this->y * rhs.z - this->z * rhs.y,
														   this->z * rhs.x - this->x * rhs.z,
														   this->x * rhs.y - this->y * rhs.x);					  }

	vec3d  operator +  (const vec3d& rhs)	{ return vec3d(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);	  }
	vec3d  operator -  (const vec3d& rhs)	{ return vec3d(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);	  }
	vec3d  operator *  (const float& rhs)	{ return vec3d(this->x * rhs,   this->y * rhs,   this->z * rhs);	  }
	vec3d  operator /  (const float& rhs)	{ return vec3d(this->x / rhs,   this->y / rhs,   this->z / rhs);	  }

	vec3d& operator += (const vec3d& rhs)	{ this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
	vec3d& operator -= (const vec3d& rhs)	{ this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
	vec3d& operator *= (const float  rhs)	{ this->x *= rhs;   this->y *= rhs;	  this->z *= rhs;	return *this; }
	vec3d& operator /= (const float  rhs)	{ this->x /= rhs;   this->y /= rhs;	  this->z /= rhs;	return *this; }
};

#define vec3d_right	  vec3d( 1.f,  0.f,  0.f)
#define vec3d_left	  vec3d(-1.f,  0.f,  0.f)
#define vec3d_up	  vec3d( 0.f,  1.f,  0.f)
#define vec3d_down	  vec3d( 0.f, -1.f,  0.f)
#define vec3d_forward vec3d( 0.f,  0.f,  1.f)
#define vec3d_back	  vec3d( 0.f,  0.f, -1.f)


struct triangle {
	vec3d p[3]; // Vert
	vec3d n[3]; // Norm
	vec2d t[3]; // UV
};

struct mesh {
	vector<triangle> tris;

	bool LoadFromObjectFile(string sFilename)
	{
		if (sFilename.substr(sFilename.size() - 4, 4) != ".obj")
			return false;

		bool bHasTexture = false;
		bool bHasNormal = false;

		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;
		vector<vec3d> norms;
		vector<vec2d> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			// Read in all the Vert, Tex and Norm coordinates
			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					bHasTexture = true;
					vec2d v;
					s >> junk >> junk >> v.u >> v.v;
					texs.push_back(v);
				} else if (line[1] == 'n')
				{
					bHasNormal = true;
					vec3d v;
					s >> junk >> junk >> v.x >> v.y >> v.z;
					norms.push_back(v);
				} else
				{
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			// Generate faces based on Vert, Tex and Norm information read
			if (line[0] == 'f')
			{
				s >> junk;

				string tokens[9];
				int nTokenCount = -1;

				// read in just the numbers
				while (!s.eof()) {
					char c = s.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();

				if (!bHasTexture && !bHasNormal) {

					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[1]) - 1], verts[stoi(tokens[2]) - 1] });	 // Verts

				} else if (!bHasTexture && bHasNormal) {

					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],     // Verts
									 norms[stoi(tokens[1]) - 1], norms[stoi(tokens[3]) - 1], norms[stoi(tokens[5]) - 1] });	 // Texs

				} else if (bHasTexture && !bHasNormal) {

					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],     // Verts
									 vec3d(),					 vec3d(),					 vec3d(),						 // Normals
									 texs [stoi(tokens[1]) - 1], texs [stoi(tokens[3]) - 1], texs [stoi(tokens[5]) - 1] });  // Texs

				} else if (bHasTexture && bHasNormal) {

					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[3]) - 1], verts[stoi(tokens[6]) - 1],     // Verts
									 norms[stoi(tokens[2]) - 1], norms[stoi(tokens[5]) - 1], norms[stoi(tokens[8]) - 1],     // Normals
									 texs [stoi(tokens[1]) - 1], texs [stoi(tokens[4]) - 1], texs [stoi(tokens[7]) - 1] });  // Texs
				}
			}
		}
		return true;
	}
};

struct mat4x4 {
	float m[4][4] = { 0 };

	mat4x4 makeIdentity () {
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	/// <summary>
	/// Only for Rotation/Translation Matrices
	/// </summary>
	mat4x4 inverse() {
		mat4x4 matrix;
		matrix.m[0][0] = this->m[0][0]; matrix.m[0][1] = this->m[1][0]; matrix.m[0][2] = this->m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = this->m[0][1]; matrix.m[1][1] = this->m[1][1]; matrix.m[1][2] = this->m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = this->m[0][2]; matrix.m[2][1] = this->m[1][2]; matrix.m[2][2] = this->m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(this->m[3][0] * matrix.m[0][0] + this->m[3][1] * matrix.m[1][0] + this->m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(this->m[3][0] * matrix.m[0][1] + this->m[3][1] * matrix.m[1][1] + this->m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(this->m[3][0] * matrix.m[0][2] + this->m[3][1] * matrix.m[1][2] + this->m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	mat4x4 transpose() {
		mat4x4 matrix;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				matrix.m[j][i] = this->m[i][j];
		return matrix;
	}

	mat4x4 operator * (mat4x4& m) {
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = this->m[r][0] * m.m[0][c] + this->m[r][1] * m.m[1][c] + this->m[r][2] * m.m[2][c] + this->m[r][3] * m.m[3][c];
		return matrix;
	}

	vec3d operator * (vec3d& i)	{
		return vec3d(i.x * this->m[0][0] + i.y * this->m[1][0] + i.z * this->m[2][0] + i.w * this->m[3][0],
					 i.x * this->m[0][1] + i.y * this->m[1][1] + i.z * this->m[2][1] + i.w * this->m[3][1],
					 i.x * this->m[0][2] + i.y * this->m[1][2] + i.z * this->m[2][2] + i.w * this->m[3][2],
					 i.x * this->m[0][3] + i.y * this->m[1][3] + i.z * this->m[2][3] + i.w * this->m[3][3]); 
	}

	mat4x4& operator *= (mat4x4& m) {
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				this->m[r][c] = this->m[r][0] * m.m[0][c] + this->m[r][1] * m.m[1][c] + this->m[r][2] * m.m[2][c] + this->m[r][3] * m.m[3][c];
		return *this;
	}
};

mat4x4 Matrix_MakeRotationX(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationY(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeRotationZ(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 Matrix_MakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

mat4x4 Matrix_PointAt(vec3d& pos, vec3d& target, vec3d& up)
{
	// Calculate new forward direction
	vec3d newForward = target - pos;
	newForward = newForward.norm();

	// Calculate new Up direction
	vec3d a = newForward * up.dot(newForward);
	vec3d newUp = up - a;
	newUp = newUp.norm();

	// New Right direction is easy, its just cross product
	vec3d newRight = newUp.cross(newForward);

	// Construct Dimensioning and Translation Matrix	
	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

vec3d Vector_IntersectPlane(vec3d& plane_p, vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd, float& t)
{
	plane_n = plane_n.norm();
	float plane_d = -plane_n.dot(plane_p);
	float ad = lineStart.dot(plane_n);
	float bd = lineEnd.dot(plane_n);
	t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = lineEnd - lineStart;
	vec3d lineToIntersect = lineStartToEnd * t;
	return lineStart + lineToIntersect;
}

int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n = plane_n.norm();

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](vec3d& p)
	{
		vec3d n = p.norm();
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.dot(plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d* inside_points[3];  int nInsidePointCount = 0;
	vec3d* outside_points[3]; int nOutsidePointCount = 0;
	vec2d* inside_tex[3]; int nInsideTexCount = 0;
	vec2d* outside_tex[3]; int nOutsideTexCount = 0;


	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; } else {
		outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
	} else {
		outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
	} else {
		outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];
		out_tri1.t[0] = *inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[1].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[1].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.t[2].u = t * (outside_tex[1]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[1]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.t[0] = *inside_tex[0];
		out_tri1.t[1] = *inside_tex[1];

		float t;
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[2].u = t * (outside_tex[0]->u - inside_tex[0]->u) + inside_tex[0]->u;
		out_tri1.t[2].v = t * (outside_tex[0]->v - inside_tex[0]->v) + inside_tex[0]->v;
		out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.t[0] = *inside_tex[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.t[1] = out_tri1.t[2];
		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.t[2].u = t * (outside_tex[0]->u - inside_tex[1]->u) + inside_tex[1]->u;
		out_tri2.t[2].v = t * (outside_tex[0]->v - inside_tex[1]->v) + inside_tex[1]->v;
		out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;
		return 2; // Return two newly formed triangles which form a quad
	}
}

