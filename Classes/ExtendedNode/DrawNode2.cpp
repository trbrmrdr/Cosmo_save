#include "stdafx.h"
#include "DrawNode2.h"
#include "renderer/CCGLProgramState.h"
#include "base/ccTypes.h"

DrawNode2::DrawNode2()
{}

DrawNode2* DrawNode2::create()
{
	DrawNode2 *pRet = new (std::nothrow) DrawNode2();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}

	CC_SAFE_DELETE( pRet );
	return nullptr;
}


static inline Tex2F __t( const Vec2 &v )
{
	return *(Tex2F*)&v;
}

static Vec2 v2fzero( 0.0f, 0.0f );

void DrawNode2::drawFaceCircle( const Vec2& center, float radius, float expand )
{

	const size_t segments = 8;

	Vec2 *vertices = new (std::nothrow) Vec2[segments];
	if (!vertices)
		return;

	//___________
	vertices[0].x = center.x - radius;
	vertices[0].y = center.y;

	vertices[1].x = center.x - radius;
	vertices[1].y = center.y + radius;

	vertices[2].x = center.x;
	vertices[2].y = center.y + radius;
	//___________
	vertices[3].x = center.x + radius;
	vertices[3].y = center.y + radius;

	vertices[4].x = center.x + radius;
	vertices[4].y = center.y;

	vertices[5].x = center.x + radius;
	vertices[5].y = center.y - radius;
	//___________
	vertices[6].x = center.x;
	vertices[6].y = center.y - radius;

	vertices[7].x = center.x - radius;
	vertices[7].y = center.y - radius;

	//vertices[9].x = center.x - radius;
	//vertices[9].y = center.y;
	//##1	= 1 - 1
	//## 0.7 = 0.25 - 1
	//##0.5 = 0 - 1
	//##0.2 = 0 - 0.25
	//##0.0 = 0 - 0
	//##
	const float exp = std::max( 0.f, std::min( 1.f, expand ) );
	const float a_s = std::max( 0.f, 2 * exp - 1 );
	const float a_e = exp >= 0.5f ? 1.f : 2 * exp;
	//CCLOG( "%f->%f == %f", a_s, a_e, exp );
	const Color4F& colorStart = Color4F( 0, 0, 0, a_e );
	const Color4F& colorEnd = Color4F( 0, 0, 0, a_s );
	//#####
	auto  triangle_count = segments;
	auto vertex_count = 3 * triangle_count;
	ensureCapacity( vertex_count );

	V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_buffer + _bufferCount);
	V2F_C4B_T2F_Triangle *cursor = triangles;

	for (int i = 0; i < segments - 1; i++)
	{
		V2F_C4B_T2F_Triangle tmp = {
			{ center, Color4B( colorStart ), __t( v2fzero ) },
			{ vertices[i + 0], Color4B( colorEnd ), __t( v2fzero ) },
			{ vertices[i + 1], Color4B( colorEnd ), __t( v2fzero ) },
		};

		*cursor++ = tmp;
	}

	V2F_C4B_T2F_Triangle tmp = {
		{ center, Color4B( colorStart ), __t( v2fzero ) },
		{ vertices[0], Color4B( colorEnd ), __t( v2fzero ) },
		{ vertices[segments - 1], Color4B( colorEnd ), __t( v2fzero ) },
	};

	*cursor++ = tmp;

	_bufferCount += vertex_count;

	_dirty = true;
	//############################

	CC_SAFE_DELETE_ARRAY( vertices );
}

void DrawNode2::drawCircle_ex( const Vec2& center, float radius, float expand )
{
	const float angle = 360.f;
	const size_t segments = 100;
	float scaleX = 1.f;
	float scaleY = 1.f;

	const float coef = 2.0f * (float)M_PI / segments;

	Vec2 *vertices = new (std::nothrow) Vec2[segments];
	if (!vertices)
		return;

	for (unsigned int i = 0; i < segments; i++)
	{
		float rads = i*coef;
		GLfloat j = radius * cosf( rads + angle ) * scaleX + center.x;
		GLfloat k = radius * sinf( rads + angle ) * scaleY + center.y;

		vertices[i].x = j;
		vertices[i].y = k;
	}
	//##1	= 1 - 1
	//## 0.7 = 0.25 - 1
	//##0.5 = 0 - 1
	//##0.2 = 0 - 0.25
	//##0.0 = 0 - 0
	//##
	const float exp = std::max( 0.f, std::min( 1.f, expand ) );
	const float a_s = std::max( 0.f, 2 * exp - 1 );
	const float a_e = exp >= 0.5f ? 1.f : 2 * exp;
	//CCLOG( "%f->%f == %f", a_s, a_e, exp );
	const Color4F& colorStart = Color4F( 0, 0, 0, a_e );
	const Color4F& colorEnd = Color4F( 0, 0, 0, a_s );
	//#####
	auto  triangle_count = segments;
	auto vertex_count = 3 * triangle_count;
	ensureCapacity( vertex_count );

	V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_buffer + _bufferCount);
	V2F_C4B_T2F_Triangle *cursor = triangles;

	for (int i = 0; i < segments - 1; i++)
	{
		V2F_C4B_T2F_Triangle tmp = {
			{ center, Color4B( colorStart ), __t( v2fzero ) },
			{ vertices[i + 0], Color4B( colorEnd ), __t( v2fzero ) },
			{ vertices[i + 1], Color4B( colorEnd ), __t( v2fzero ) },
		};

		*cursor++ = tmp;
	}

	V2F_C4B_T2F_Triangle tmp = {
		{ center, Color4B( colorStart ), __t( v2fzero ) },
		{ vertices[0], Color4B( colorEnd ), __t( v2fzero ) },
		{ vertices[segments - 1], Color4B( colorEnd ), __t( v2fzero ) },
	};

	*cursor++ = tmp;

	_bufferCount += vertex_count;

	_dirty = true;
	//############################

	CC_SAFE_DELETE_ARRAY( vertices );
}

static inline Vec2 v2f( float x, float y )
{
	Vec2 ret( x, y );
	return ret;
}

static inline Vec2 __v2f( const Vec2 &v )
{
	//#ifdef __LP64__
	return v2f( v.x, v.y );
	// #else
	//     return * ((Vec2*) &v);
	// #endif
}

static inline Vec2 v2fnormalize( const Vec2 &p )
{
	Vec2 r( p.x, p.y );
	r.normalize();
	return v2f( r.x, r.y );
}

static inline Vec2 v2fneg( const Vec2 &p0 )
{
	return v2f( -p0.x, -p0.y );
}

static inline Vec2 v2fmult( const Vec2 &v, float s )
{
	return v2f( v.x * s, v.y * s );
}

static inline Vec2 v2fperp( const Vec2 &p0 )
{
	return v2f( -p0.y, p0.x );
}

static inline Vec2 v2fadd( const Vec2 &v0, const Vec2 &v1 )
{
	return v2f( v0.x + v1.x, v0.y + v1.y );
}

static inline float v2fdot( const Vec2 &p0, const Vec2 &p1 )
{
	return  p0.x * p1.x + p0.y * p1.y;
}

static inline Vec2 v2fsub( const Vec2 &v0, const Vec2 &v1 )
{
	return v2f( v0.x - v1.x, v0.y - v1.y );
}

void DrawNode2::drawPolygon( const std::vector<Vec2>& verts, const Color4F &fillColor, float borderWidth, const Color4F &borderColor )
{

	size_t count = verts.size();
	if (count <= 0)
		return;

	bool outline = (borderColor.a > 0.0 && borderWidth > 0.0);

	auto  triangle_count = outline ? (3 * count - 2) : (count - 2);
	auto vertex_count = 3 * triangle_count;
	ensureCapacity( vertex_count );

	V2F_C4B_T2F_Triangle *triangles = (V2F_C4B_T2F_Triangle *)(_buffer + _bufferCount);
	V2F_C4B_T2F_Triangle *cursor = triangles;

	for (size_t i = 0; i < count - 2; i++)
	{
		V2F_C4B_T2F_Triangle tmp = {
			{ verts[0], Color4B( fillColor ), __t( v2fzero ) },
			{ verts[i + 1], Color4B( fillColor ), __t( v2fzero ) },
			{ verts[i + 2], Color4B( fillColor ), __t( v2fzero ) },
		};

		*cursor++ = tmp;
	}

	if (outline)
	{
		struct ExtrudeVerts { Vec2 offset, n; };
		struct ExtrudeVerts* extrude = (struct ExtrudeVerts*)malloc( sizeof( struct ExtrudeVerts )*count );
		memset( extrude, 0, sizeof( struct ExtrudeVerts )*count );

		for (size_t i = 0; i < count; i++)
		{
			Vec2 v0 = __v2f( verts[(i - 1 + count) % count] );
			Vec2 v1 = __v2f( verts[i] );
			Vec2 v2 = __v2f( verts[(i + 1) % count] );

			Vec2 n1 = v2fnormalize( v2fperp( v2fsub( v1, v0 ) ) );
			Vec2 n2 = v2fnormalize( v2fperp( v2fsub( v2, v1 ) ) );

			Vec2 offset = v2fmult( v2fadd( n1, n2 ), 1.0 / (v2fdot( n1, n2 ) + 1.0) );
			struct ExtrudeVerts tmp = { offset, n2 };
			extrude[i] = tmp;
		}

		for (size_t i = 0; i < count; i++)
		{
			int j = (i + 1) % count;
			Vec2 v0 = __v2f( verts[i] );
			Vec2 v1 = __v2f( verts[j] );

			Vec2 n0 = extrude[i].n;

			Vec2 offset0 = extrude[i].offset;
			Vec2 offset1 = extrude[j].offset;

			Vec2 inner0 = v2fsub( v0, v2fmult( offset0, borderWidth ) );
			Vec2 inner1 = v2fsub( v1, v2fmult( offset1, borderWidth ) );
			Vec2 outer0 = v2fadd( v0, v2fmult( offset0, borderWidth ) );
			Vec2 outer1 = v2fadd( v1, v2fmult( offset1, borderWidth ) );

			V2F_C4B_T2F_Triangle tmp1 = {
				{ inner0, Color4B( borderColor ), __t( v2fneg( n0 ) ) },
				{ inner1, Color4B( borderColor ), __t( v2fneg( n0 ) ) },
				{ outer1, Color4B( borderColor ), __t( n0 ) }
			};
			*cursor++ = tmp1;

			V2F_C4B_T2F_Triangle tmp2 = {
				{ inner0, Color4B( borderColor ), __t( v2fneg( n0 ) ) },
				{ outer0, Color4B( borderColor ), __t( n0 ) },
				{ outer1, Color4B( borderColor ), __t( n0 ) }
			};
			*cursor++ = tmp2;
		}

		free( extrude );
	}

	_bufferCount += vertex_count;

	_dirty = true;
}

void DrawNode2::drawCardinalSpline(PointArray2 *config, float tension, unsigned int segments, const Color4F &color) {
	Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
	if (!vertices)
		return;

	ssize_t p;
	float lt;
	float deltaT = 1.0f / config->count();

	for (unsigned int i = 0; i < segments + 1; i++) {

		float dt = (float)i / segments;

		// border
		if (dt == 1) {
			p = config->count() - 1;
			//p = config->count();
			lt = 1;
		}
		else {
			p = dt / deltaT;
			lt = (dt - deltaT * (float)p) / deltaT;
		}

		// Interpolate
		Vec2 pp0 = config->getControlPointAtIndex2(p - 1);
		Vec2 pp1 = config->getControlPointAtIndex2(p + 0);
		Vec2 pp2 = config->getControlPointAtIndex2(p + 1);
		Vec2 pp3 = config->getControlPointAtIndex2(p + 2);

		Vec2 newPos = ccCardinalSplineAt(pp0, pp1, pp2, pp3, tension, lt);
		vertices[i].x = newPos.x;
		vertices[i].y = newPos.y;
	}

	drawPoly(vertices, segments + 1, false, color);

	CC_SAFE_DELETE_ARRAY(vertices);
}
