/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2008-2010 Ricardo Quesada
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#import <math.h>
#import <stdlib.h>
#import <string.h>

#import "CCDrawingPrimitives.h"
#import "ccTypes.h"
#import "ccMacros.h"
#import "Platforms/CCGL.h"

//цепочка линий если флаг=1
bool _FlagPotokLine;

GLfloat prevt1,prevt2,prevt3,prevt4,prevt5,prevt6,prevt7,prevt8,prevt9,prevt10,prevt11,prevt12,prevt13,prevt14,prevt15,prevt16,prevt17,prevt18;

void ccDrawPoint( CGPoint point )
{
	ccVertex2F p = (ccVertex2F) {point.x * CC_CONTENT_SCALE_FACTOR(), point.y * CC_CONTENT_SCALE_FACTOR() };
	
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, &p);	
	glDrawArrays(GL_POINTS, 0, 1);

	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}

void ccDrawPoints( const CGPoint *points, NSUInteger numberOfPoints )
{
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	ccVertex2F newPoints[numberOfPoints];

	// iPhone and 32-bit machines optimization
	if( sizeof(CGPoint) == sizeof(ccVertex2F) ) {

		// points ?
		if( CC_CONTENT_SCALE_FACTOR() != 1 ) {
			for( NSUInteger i=0; i<numberOfPoints;i++)
				newPoints[i] =  (ccVertex2F){ points[i].x * CC_CONTENT_SCALE_FACTOR(), points[i].y * CC_CONTENT_SCALE_FACTOR() };

			glVertexPointer(2, GL_FLOAT, 0, newPoints);

		} else
			glVertexPointer(2, GL_FLOAT, 0, points);
		
		glDrawArrays(GL_POINTS, 0, (GLsizei) numberOfPoints);
		
	} else {
		
		// Mac on 64-bit
		for( NSUInteger i=0; i<numberOfPoints;i++)
			newPoints[i] = (ccVertex2F) { points[i].x, points[i].y };
			
		glVertexPointer(2, GL_FLOAT, 0, newPoints);
		glDrawArrays(GL_POINTS, 0, (GLsizei) numberOfPoints);

	}

	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}


void ccDrawLine( CGPoint origin, CGPoint destination )
{
	ccVertex2F vertices[2] = {
		{origin.x * CC_CONTENT_SCALE_FACTOR(), origin.y * CC_CONTENT_SCALE_FACTOR() },
		{destination.x * CC_CONTENT_SCALE_FACTOR(), destination.y * CC_CONTENT_SCALE_FACTOR() }
	};

	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);	
	glDrawArrays(GL_LINES, 0, 2);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);		
}

void ccDrawLineTriangles( ccVertex2F origin, ccVertex2F destination, ccColor4B incol,float linew,float lineblurw)
{
    NSUInteger numberOfPoints=18;
    
    ccColor4F col=ccc4FFromccc4B(incol);
    
    const GLfloat Colors[] = { 
        col.r,col.g,col.b,0,
		col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,0,
        
		col.r,col.g,col.b,col.a,
		col.r,col.g,col.b,0,
        col.r,col.g,col.b,col.a,
        
        col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,col.a,
        
        col.r,col.g,col.b,col.a,
		col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,col.a,
        
        col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,0,
        col.r,col.g,col.b,col.a,
        
        col.r,col.g,col.b,0,
		col.r,col.g,col.b,col.a,
        col.r,col.g,col.b,0,
	};
    
    linew=linew/2;
    
    //уравнение прямой
    float A=origin.y-destination.y;
    float B=destination.x-origin.x;
    /////////////
    float mu=1/sqrtf(A*A+B*B);
    float mu2=linew*mu;
    mu*=lineblurw;
    
    GLfloat newprevt1,newprevt2,newprevt3,newprevt4,newprevt5,newprevt6,newprevt7,newprevt8,newprevt9,newprevt10,newprevt11,newprevt12,newprevt13,newprevt14,newprevt15,newprevt16,newprevt17,newprevt18; 
    
    if(_FlagPotokLine)
    {
        newprevt1=prevt1;
        newprevt2=prevt2;
        
        newprevt3=prevt3;
        newprevt4=prevt4;
        
        newprevt5=newprevt3;
        newprevt6=newprevt4;
        //////////
        newprevt7=prevt7;
        newprevt8=prevt8;
        
        newprevt9=prevt9;
        newprevt10=prevt10;
        
        newprevt11=newprevt9;
        newprevt12=newprevt10;
        //////////        
        newprevt13=prevt13;
        newprevt14=prevt14;
        
        newprevt15=prevt15;
        newprevt16=prevt16;
        
        newprevt17=newprevt15;
        newprevt18=newprevt16;
    }
    else
    {
        newprevt1=origin.x+A*(mu2+mu);
        newprevt2=origin.y+B*(mu2+mu);
        
        newprevt3=origin.x+A*mu2;
        newprevt4=origin.y+B*mu2;
        
        newprevt5=newprevt3;
        newprevt6=newprevt4;
        //////////
        newprevt7=origin.x+A*mu2;
        newprevt8=origin.y+B*mu2;
        
        newprevt9=origin.x-A*mu2;
        newprevt10=origin.y-B*mu2;
        
        newprevt11=newprevt9;
        newprevt12=newprevt10;
        //////////        
        newprevt13=origin.x-A*mu2;
        newprevt14=origin.y-B*mu2;
        
        newprevt15=origin.x-A*(mu2+mu);
        newprevt16=origin.y-B*(mu2+mu);
        
        newprevt17=newprevt15;
        newprevt18=newprevt16;
        
        
    }
    
    prevt1=destination.x+A*(mu2+mu);
    prevt2=destination.y+B*(mu2+mu);
    
    prevt3=destination.x+A*mu2;
    prevt4=destination.y+B*mu2;
    
    prevt5=prevt1;
    prevt6=prevt2;
    ///////////
    prevt7=destination.x+A*mu2;
    prevt8=destination.y+B*mu2;
    
    prevt9=destination.x-A*mu2;
    prevt10=destination.y-B*mu2;
    
    prevt11=prevt7;
    prevt12=prevt8;
    ////////////
    prevt13=destination.x-A*mu2;
    prevt14=destination.y-B*mu2;
    
    prevt15=destination.x-A*(mu2+mu);
    prevt16=destination.y-B*(mu2+mu);
    
    prevt17=prevt13;
    prevt18=prevt14;
    
    const GLfloat newPoint[] = {
        newprevt1,newprevt2,
        newprevt3,newprevt4,
        prevt1,prevt2,
        
        prevt3,prevt4,
        prevt5,prevt6,
        newprevt5,newprevt6,

        
        newprevt7,newprevt8,
        newprevt9,newprevt10,
        prevt7,prevt8,
        
        prevt9,prevt10,
        prevt11,prevt12,
        newprevt11,newprevt12,
        
        
        newprevt13,newprevt14,
        newprevt15,newprevt16,
        prevt13,prevt14,
        
        prevt15,prevt16,
        prevt17,prevt18,
        newprevt17,newprevt18,     
    };
    
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, GL_COLOR_ARRAY
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    //commented because error 20.10.2011
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     
    glColorPointer(4, GL_FLOAT, 0, Colors);
    glVertexPointer(2, GL_FLOAT, 0, newPoint);
            
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) numberOfPoints);
    
    //glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
	
	// restore default state
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
    
    glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);
}

void ccDrawPoly( const CGPoint *poli, NSUInteger numberOfPoints, BOOL closePolygon )
{
	ccVertex2F newPoint[numberOfPoints];

	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	
	// iPhone and 32-bit machines
	if( sizeof(CGPoint) == sizeof(ccVertex2F) ) {

		// convert to pixels ?
		if( CC_CONTENT_SCALE_FACTOR() != 1 ) {
			memcpy( newPoint, poli, numberOfPoints * sizeof(ccVertex2F) );
			for( NSUInteger i=0; i<numberOfPoints;i++)
				newPoint[i] = (ccVertex2F) { poli[i].x * CC_CONTENT_SCALE_FACTOR(), poli[i].y * CC_CONTENT_SCALE_FACTOR() };

			glVertexPointer(2, GL_FLOAT, 0, newPoint);

		} else
			glVertexPointer(2, GL_FLOAT, 0, poli);

		
	} else {
		// 64-bit machines (Mac)
		
		for( NSUInteger i=0; i<numberOfPoints;i++)
			newPoint[i] = (ccVertex2F) { poli[i].x, poli[i].y };

		glVertexPointer(2, GL_FLOAT, 0, newPoint );

	}
		
	if( closePolygon )
		glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
	else
		glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}

/** draws a filled polygon given a pointer to CGPoint coordinates and the number of vertices measured in points.
 The polygon can be closed or open
 */
void ccFillPoly( const CGPoint *poli, NSUInteger numberOfPoints, BOOL closePolygon )
{
	ccVertex2F newPoint[numberOfPoints];
    
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
    
	
	// iPhone and 32-bit machines
	if( sizeof(CGPoint) == sizeof(ccVertex2F) ) {
        
		// convert to pixels ?
		if( CC_CONTENT_SCALE_FACTOR() != 1 ) {
			memcpy( newPoint, poli, numberOfPoints * sizeof(ccVertex2F) );
			for( NSUInteger i=0; i<numberOfPoints;i++)
				newPoint[i] = (ccVertex2F) { poli[i].x * CC_CONTENT_SCALE_FACTOR(), poli[i].y * CC_CONTENT_SCALE_FACTOR() };
            
			glVertexPointer(2, GL_FLOAT, 0, newPoint);
            
		} else
			glVertexPointer(2, GL_FLOAT, 0, poli);
        
		
	} else {
		// 64-bit machines (Mac)
		
		for( NSUInteger i=0; i<numberOfPoints;i++)
			newPoint[i] = (ccVertex2F) { poli[i].x, poli[i].y };
        
		glVertexPointer(2, GL_FLOAT, 0, newPoint );
        
	}
    
	if( closePolygon )
		glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) numberOfPoints);
	else
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei) numberOfPoints);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}


void ccDrawCircle( CGPoint center, float r, float a, NSUInteger segs, BOOL drawLineToCenter)
{
	int additionalSegment = 1;
	if (drawLineToCenter)
		additionalSegment++;

	const float coef = 2.0f * (float)M_PI/segs;
	
	GLfloat *vertices = calloc( sizeof(GLfloat)*2*(segs+2), 1);
	if( ! vertices )
		return;

	for(NSUInteger i=0;i<=segs;i++)
	{
		float rads = i*coef;
		GLfloat j = r * cosf(rads + a) + center.x;
		GLfloat k = r * sinf(rads + a) + center.y;
		
		vertices[i*2] = j * CC_CONTENT_SCALE_FACTOR();
		vertices[i*2+1] =k * CC_CONTENT_SCALE_FACTOR();
	}
	vertices[(segs+1)*2] = center.x * CC_CONTENT_SCALE_FACTOR();
	vertices[(segs+1)*2+1] = center.y * CC_CONTENT_SCALE_FACTOR();
	
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);	
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segs+additionalSegment);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
	
	free( vertices );
}

void ccDrawQuadBezier(CGPoint origin, CGPoint control, CGPoint destination, NSUInteger segments)
{
	ccVertex2F vertices[segments + 1];
	
	float t = 0.0f;
	for(NSUInteger i = 0; i < segments; i++)
	{
		GLfloat x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
		GLfloat y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
		vertices[i] = (ccVertex2F) {x * CC_CONTENT_SCALE_FACTOR(), y * CC_CONTENT_SCALE_FACTOR() };
		t += 1.0f / segments;
	}
	vertices[segments] = (ccVertex2F) {destination.x * CC_CONTENT_SCALE_FACTOR(), destination.y * CC_CONTENT_SCALE_FACTOR() };
	
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);	
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}

void ccDrawQuadBezierTriangles(CGPoint origin, CGPoint control, CGPoint destination, NSUInteger segments,ccColor4B col,float linew,float lineblurw)
{
	ccVertex2F vertices[segments + 2];
	
	float t = 0.0f;
	for(NSUInteger i = 0; i < segments+1; i++)
	{
		GLfloat x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
		GLfloat y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
		vertices[i] = (ccVertex2F) {x, y};
		t += 1.0f / segments;
	}
    for(NSUInteger i = 0; i < segments; i++)
	{
        ccDrawLineTriangles(vertices[i],vertices[i+1],col,linew,lineblurw);
        _FlagPotokLine=true;
	}
}

void ccDrawCubicBezier(CGPoint origin, CGPoint control1, CGPoint control2, CGPoint destination, NSUInteger segments)
{
	ccVertex2F vertices[segments + 1];
	
	float t = 0;
	for(NSUInteger i = 0; i < segments; i++)
	{
		GLfloat x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
		GLfloat y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
		vertices[i] = (ccVertex2F) {x * CC_CONTENT_SCALE_FACTOR(), y * CC_CONTENT_SCALE_FACTOR() };
		t += 1.0f / segments;
	}
	vertices[segments] = (ccVertex2F) {destination.x * CC_CONTENT_SCALE_FACTOR(), destination.y * CC_CONTENT_SCALE_FACTOR() };
	
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_VERTEX_ARRAY, 
	// Unneeded states: GL_TEXTURE_2D, GL_TEXTURE_COORD_ARRAY, GL_COLOR_ARRAY	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, vertices);	
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
	
	// restore default state
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);	
}
