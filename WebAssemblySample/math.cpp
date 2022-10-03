#include "math.h"

float Sqrtf(const float& n) {
	if (n == 0.0f) {
		return 0.0f;
	}

	int i = 0x2035AD0C + (*(int*)&n >> 1);
	return n / *(float*)&i + *(float*)&i * 0.25f;
}

float Fabsf(const float& f) {
	if (f < 0.0f) {
		return f * -1.0f;
	}
	return f;
}

double Sin(const double& x) {
	int i = 1;
	double cur = x;
	double acc = 1;
	double fact = 1;
	double pow = x;
	while (acc > .00000001 && i < 100) {
		fact *= ((2 * i) * (2 * i + 1));
		pow *= -1 * x * x;
		acc = pow / fact;
		if (acc < 0.0) { // fabs
			acc *= -1.0;
		}
		cur += acc;
		i++;
	}
	return cur;
}

#define PI         (3.1415926535f)
#define HALF_PI    (0.5f * PI)
#define TWO_PI     (2.0f * PI)
#define TWO_PI_INV (1.0f / TWO_PI)

inline float Hill(float x)
{
	const float a0 = 1.0f;
	const float a2 = 2.0f / PI - 12.0f / (PI * PI);
	const float a3 = 16.0f / (PI * PI * PI) - 4.0f / (PI * PI);
	const float xx = x * x;
	const float xxx = xx * x;

	return a0 + a2 * xx + a3 * xxx;
}

float FastSin(float x)
{
	// wrap x within [0, TWO_PI)
	const float a = x * TWO_PI_INV;
	x -= static_cast<int>(a) * TWO_PI;
	if (x < 0.0f)
		x += TWO_PI;

	// 4 pieces of hills
	if (x < HALF_PI)
		return Hill(HALF_PI - x);
	else if (x < PI)
		return Hill(x - HALF_PI);
	else if (x < 3.0f * HALF_PI)
		return -Hill(3.0f * HALF_PI - x);
	else
		return -Hill(x - 3.0f * HALF_PI);
}

float FastCos(float x)
{
	return FastSin(x + HALF_PI);
}

float Tan(const float& d) {
	return FastSin(d) / FastCos(d);
}


vec3 operator+(const vec3& l, const vec3& r) {
	return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

vec3 operator-(const vec3& l, const vec3& r) {
	return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& v, float f) {
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& l, const vec3& r) {
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

float dot(const vec3& l, const vec3& r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

float lenSq(const vec3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return 0.0f;
	}
	return Sqrtf(lenSq);
}

void normalize(vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return;
	}
	float invLen = 1.0f / Sqrtf(lenSq);

	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

vec3 normalized(const vec3& v) {
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON) {
		return v;
	}
	float invLen = 1.0f / Sqrtf(lenSq);

	return vec3(
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
	);
}

vec3 project(const vec3& a, const vec3& b) {
	float magBSq = len(b);
	if (magBSq < VEC3_EPSILON) {
		return vec3();
	}
	float scale = dot(a, b) / magBSq;
	return b * scale;
}

vec3 reject(const vec3& a, const vec3& b) {
	vec3 projection = project(a, b);
	return a - projection;
}

vec3 reflect(const vec3& a, const vec3& b) {
	float magBSq = len(b);
	if (magBSq < VEC3_EPSILON) {
		return vec3();
	}
	float scale = dot(a, b) / magBSq;
	vec3 proj2 = b * (scale * 2);
	return a - proj2;
}

vec3 cross(const vec3& l, const vec3& r) {
	return vec3(
		l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x
	);
}

vec3 lerp(const vec3& s, const vec3& e, float t) {
	return vec3(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
}

vec3 nlerp(const vec3& s, const vec3& e, float t) {
	vec3 linear(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
	return normalized(linear);
}

bool operator==(const vec3& l, const vec3& r) {
	vec3 diff(l - r);
	return lenSq(diff) < VEC3_EPSILON;
}

bool operator!=(const vec3& l, const vec3& r) {
	return !(l == r);
}

bool operator==(const mat4& a, const mat4& b) {
	for (int i = 0; i < 16; ++i) {
		if (Fabsf(a.v[i] - b.v[i]) > MAT4_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const mat4& a, const mat4& b) {
	return !(a == b);
}

mat4 operator+(const mat4& a, const mat4& b) {
	return mat4(
		a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
		a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
		a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
		a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
	);
}

mat4 operator*(const mat4& m, float f) {
	return mat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
	);
}

#define M4D(aRow, bCol) \
    a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + \
    a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + \
    a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + \
    a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

mat4 operator*(const mat4& a, const mat4& b) {
	return mat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Column 0
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Column 1
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Column 2
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Column 3
	);
}

#define M4V4D(mRow, x, y, z, w) \
    x * m.v[0 * 4 + mRow] + \
    y * m.v[1 * 4 + mRow] + \
    z * m.v[2 * 4 + mRow] + \
    w * m.v[3 * 4 + mRow]

vec4 operator*(const mat4& m, const vec4& v) {
	return vec4(
		M4V4D(0, v.x, v.y, v.z, v.w),
		M4V4D(1, v.x, v.y, v.z, v.w),
		M4V4D(2, v.x, v.y, v.z, v.w),
		M4V4D(3, v.x, v.y, v.z, v.w)
	);
}

vec3 transformVector(const mat4& m, const vec3& v) {
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 0.0f),
		M4V4D(1, v.x, v.y, v.z, 0.0f),
		M4V4D(2, v.x, v.y, v.z, 0.0f)
	);
}

vec3 transformPoint(const mat4& m, const vec3& v) {
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 1.0f),
		M4V4D(1, v.x, v.y, v.z, 1.0f),
		M4V4D(2, v.x, v.y, v.z, 1.0f)
	);
}

vec3 transformPoint(const mat4& m, const vec3& v, float& w) {
	float _w = w;
	w = M4V4D(3, v.x, v.y, v.z, _w);

	return vec3(
		M4V4D(0, v.x, v.y, v.z, _w),
		M4V4D(1, v.x, v.y, v.z, _w),
		M4V4D(2, v.x, v.y, v.z, _w)
	);
}

#define M4SWAP(x, y) \
    {float t = x; x = y; y = t; }

void transpose(mat4& m) {
	M4SWAP(m.yx, m.xy);
	M4SWAP(m.zx, m.xz);
	M4SWAP(m.tx, m.xw);
	M4SWAP(m.zy, m.yz);
	M4SWAP(m.ty, m.yw);
	M4SWAP(m.tz, m.zw);
}

mat4 transposed(const mat4& m) {
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
	);
}

#define M4_3X3MINOR(c0, c1, c2, r0, r1, r2) \
    (m.v[c0 * 4 + r0] * (m.v[c1 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c1 * 4 + r2] * m.v[c2 * 4 + r1]) - \
     m.v[c1 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c2 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c2 * 4 + r1]) + \
     m.v[c2 * 4 + r0] * (m.v[c0 * 4 + r1] * m.v[c1 * 4 + r2] - m.v[c0 * 4 + r2] * m.v[c1 * 4 + r1]))

float determinant(const mat4& m) {
	return  m.v[0] * M4_3X3MINOR(1, 2, 3, 1, 2, 3)
		- m.v[4] * M4_3X3MINOR(0, 2, 3, 1, 2, 3)
		+ m.v[8] * M4_3X3MINOR(0, 1, 3, 1, 2, 3)
		- m.v[12] * M4_3X3MINOR(0, 1, 2, 1, 2, 3);
}

mat4 adjugate(const mat4& m) {
	// Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	mat4 cofactor;

	cofactor.v[0] = M4_3X3MINOR(1, 2, 3, 1, 2, 3);
	cofactor.v[1] = -M4_3X3MINOR(1, 2, 3, 0, 2, 3);
	cofactor.v[2] = M4_3X3MINOR(1, 2, 3, 0, 1, 3);
	cofactor.v[3] = -M4_3X3MINOR(1, 2, 3, 0, 1, 2);

	cofactor.v[4] = -M4_3X3MINOR(0, 2, 3, 1, 2, 3);
	cofactor.v[5] = M4_3X3MINOR(0, 2, 3, 0, 2, 3);
	cofactor.v[6] = -M4_3X3MINOR(0, 2, 3, 0, 1, 3);
	cofactor.v[7] = M4_3X3MINOR(0, 2, 3, 0, 1, 2);

	cofactor.v[8] = M4_3X3MINOR(0, 1, 3, 1, 2, 3);
	cofactor.v[9] = -M4_3X3MINOR(0, 1, 3, 0, 2, 3);
	cofactor.v[10] = M4_3X3MINOR(0, 1, 3, 0, 1, 3);
	cofactor.v[11] = -M4_3X3MINOR(0, 1, 3, 0, 1, 2);

	cofactor.v[12] = -M4_3X3MINOR(0, 1, 2, 1, 2, 3);
	cofactor.v[13] = M4_3X3MINOR(0, 1, 2, 0, 2, 3);
	cofactor.v[14] = -M4_3X3MINOR(0, 1, 2, 0, 1, 3);
	cofactor.v[15] = M4_3X3MINOR(0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

mat4 inverse(const mat4& m) {
	float det = determinant(m);

	if (det == 0.0f) { // Epsilon check would need to be REALLY small
		return mat4();
	}
	mat4 adj = adjugate(m);

	return adj * (1.0f / det);
}

void invert(mat4& m) {
	float det = determinant(m);

	if (det == 0.0f) {
		m = mat4();
		return;
	}

	m = adjugate(m) * (1.0f / det);
}

mat4 frustum(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return mat4(); // Error
	}
	return mat4(
		(2.0f * n) / (r - l), 0, 0, 0,
		0, (2.0f * n) / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
		0, 0, (-2 * f * n) / (f - n), 0
	);
}

mat4 perspective(float fov, float aspect, float znear, float zfar) {
	float ymax = znear * (float)Tan(fov * 3.14159265359f / 360.0f);
	float xmax = ymax * aspect;

	return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 ortho(float l, float r, float b, float t, float n, float f) {
	if (l == r || t == b || n == f) {
		return mat4(); // Error
	}
	return mat4(
		2.0f / (r - l), 0, 0, 0,
		0, 2.0f / (t - b), 0, 0,
		0, 0, -2.0f / (f - n), 0,
		-((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
	);
}

mat4 lookAt(const vec3& position, const vec3& target, const vec3& up) {
	// Remember, forward is negative z
	vec3 f = normalized(target - position) * -1.0f;
	vec3 r = cross(up, f); // Right handed
	if (r == vec3(0, 0, 0)) {
		return mat4(); // Error
	}
	normalize(r);
	vec3 u = normalized(cross(f, r)); // Right handed

	vec3 t = vec3(
		-dot(r, position),
		-dot(u, position),
		-dot(f, position)
	);

	return mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x, u.x, f.x, 0,
		r.y, u.y, f.y, 0,
		r.z, u.z, f.z, 0,
		t.x, t.y, t.z, 1
	);
}

quat angleAxis(float angle, const vec3& axis) {
	vec3 norm = normalized(axis);
	float s = FastSin(angle * 0.5f);

	return quat(
		norm.x * s,
		norm.y * s,
		norm.z * s,
		FastCos(angle * 0.5f)
	);
}

quat fromTo(const vec3& from, const vec3& to) {
	vec3 f = normalized(from);
	vec3 t = normalized(to);

	if (f == t) {
		return quat();
	}
	else if (f == t * -1.0f) {
		vec3 ortho = vec3(1, 0, 0);
		if (Fabsf(f.y) < Fabsf(f.x)) {
			ortho = vec3(0, 1, 0);
		}
		if (Fabsf(f.z) < Fabsf(f.y) && Fabsf(f.z) < Fabsf(f.x)) {
			ortho = vec3(0, 0, 1);
		}

		vec3 axis = normalized(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = normalized(f + t);
	vec3 axis = cross(f, half);

	return quat(
		axis.x,
		axis.y,
		axis.z,
		dot(f, half)
	);
}

vec3 getAxis(const quat& quat) {
	return normalized(vec3(quat.x, quat.y, quat.z));
}

float getAngle(const quat& quat) {
	return 2.0f * (1.0f / FastCos(quat.w));
}

quat operator+(const quat& a, const quat& b) {
	return quat(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	);
}

quat operator-(const quat& a, const quat& b) {
	return quat(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	);
}

quat operator*(const quat& a, float b) {
	return quat(
		a.x * b,
		a.y * b,
		a.z * b,
		a.w * b
	);
}

quat operator-(const quat& q) {
	return quat(
		-q.x,
		-q.y,
		-q.z,
		-q.w
	);
}

bool operator==(const quat& left, const quat& right) {
	return (Fabsf(left.x - right.x) <= QUAT_EPSILON &&
		Fabsf(left.y - right.y) <= QUAT_EPSILON &&
		Fabsf(left.z - right.z) <= QUAT_EPSILON &&
		Fabsf(left.w - left.w) <= QUAT_EPSILON);
}

bool operator!=(const quat& a, const quat& b) {
	return !(a == b);
}

bool sameOrientation(const quat& left, const quat& right) {
	return (Fabsf(left.x - right.x) <= QUAT_EPSILON && Fabsf(left.y - right.y) <= QUAT_EPSILON &&
		Fabsf(left.z - right.z) <= QUAT_EPSILON && Fabsf(left.w - left.w) <= QUAT_EPSILON)
		|| (Fabsf(left.x + right.x) <= QUAT_EPSILON && Fabsf(left.y + right.y) <= QUAT_EPSILON &&
			Fabsf(left.z + right.z) <= QUAT_EPSILON && Fabsf(left.w + left.w) <= QUAT_EPSILON);
}

float dot(const quat& a, const quat& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float lenSq(const quat& q) {
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float len(const quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON) {
		return 0.0f;
	}
	return Sqrtf(lenSq);
}

void normalize(quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON) {
		return;
	}
	float i_len = 1.0f / Sqrtf(lenSq);

	q.x *= i_len;
	q.y *= i_len;
	q.z *= i_len;
	q.w *= i_len;
}

quat normalized(const quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON) {
		return quat();
	}
	float i_len = 1.0f / Sqrtf(lenSq);

	return quat(
		q.x * i_len,
		q.y * i_len,
		q.z * i_len,
		q.w * i_len
	);
}

quat conjugate(const quat& q) {
	return quat(
		-q.x,
		-q.y,
		-q.z,
		q.w
	);
}

quat inverse(const quat& q) {
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON) {
		return quat();
	}
	float recip = 1.0f / lenSq;

	// conjugate / norm
	return quat(
		-q.x * recip,
		-q.y * recip,
		-q.z * recip,
		q.w * recip
	);
}

#if 1
quat operator*(const quat& Q1, const quat& Q2) {
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
	);
}
#else
quat operator*(const quat& Q1, const quat& Q2) {
	quat result;
	result.scalar = Q2.scalar * Q1.scalar - dot(Q2.vector, Q1.vector);
	result.vector = (Q1.vector * Q2.scalar) + (Q2.vector * Q1.scalar) + cross(Q2.vector, Q1.vector);
	return result;
}
#endif

vec3 operator*(const quat& q, const vec3& v) {
	return q.vector * 2.0f * dot(q.vector, v) +
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
		cross(q.vector, v) * 2.0f * q.scalar;
}

quat mix(const quat& from, const quat& to, float t) {
	return from * (1.0f - t) + to * t;
}

quat nlerp(const quat& from, const quat& to, float t) {
	return normalized(from + (to - from) * t);
}

quat operator^(const quat& q, float f) {
	float angle = 2.0f * (1.0f / FastCos(q.scalar));
	vec3 axis = normalized(q.vector);

	float halfCos = FastCos(f * angle * 0.5f);
	float halfSin = FastSin(f * angle * 0.5f);

	return quat(
		axis.x * halfSin,
		axis.y * halfSin,
		axis.z * halfSin,
		halfCos
	);
}

quat slerp(const quat& start, const quat& end, float t) {
	if (Fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON) {
		return nlerp(start, end, t);
	}

	return normalized(((inverse(start) * end) ^ t) * start);
}

quat lookRotation(const vec3& direcion, const vec3& up) {
	// Find orthonormal basis vectors
	vec3 f = normalized(direcion);
	vec3 u = normalized(up);
	vec3 r = cross(u, f);
	u = cross(f, r);

	// From world forward to object forward
	quat f2d = fromTo(vec3(0, 0, 1), f);

	// what direction is the new object up?
	vec3 objectUp = f2d * vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	quat result = f2d * u2u;
	// Don�t forget to normalize the result
	return normalized(result);
}

mat4 quatToMat4(const quat& q) {
	vec3 r = q * vec3(1, 0, 0);
	vec3 u = q * vec3(0, 1, 0);
	vec3 f = q * vec3(0, 0, 1);

	return mat4(
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1
	);
}

quat mat4ToQuat(const mat4& m) {
	vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
	vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
	vec3 right = cross(up, forward);
	up = cross(forward, right);

	return lookRotation(forward, up);
}