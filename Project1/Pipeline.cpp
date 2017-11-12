//
//  Pipeline.cpp
//
//  Created by Warren R. Carithers on 2016/10/19.
//  Based on a version created by Joe Geigel on 11/30/11.
//  Copyright 2016 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Paul Zenie, pxz5572
//

#include <iostream>

#include "Pipeline.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Clipper.h"
#include "Rasterizer.h"

#include "GL/freeglut.h"

///
// Simple wrapper class for midterm assignment
//
// Only methods in the implementation file whose bodies
// contain the comment
//
//    // YOUR IMPLEMENTATION HERE
//
// are to be modified by students.
///

int arrayIndex = 0;
std::vector<Vertex> polyList;
glm::mat3 transformMatrix; //holds current transformations
int viewPort[4]; //holds viewport cords
Vertex emptyVert;//empty vertex to seperate polygons
Vertex ll;		//clip window lower left
Vertex ur;		//clip window upper right
int c_height;	//canvas height
///
// Constructor
//
// @param w width of canvas
// @param h height of canvas
///
Pipeline::Pipeline( int w, int h ) :  Canvas(w,h)
    // YOUR IMPLEMENTATION HERE if you need to add initializers

{
    // YOUR IMPLEMENTATION HERE if you need to modify the constructor
	emptyVert.x = -1;
	emptyVert.y = -1;
	c_height = h;
}

///
// addPoly - Add a polygon to the canvas.  This method does not draw
//           the polygon, but merely stores it for later drawing.
//           Drawing is initiated by the drawPoly() method.
//
//           Returns a unique integer id for the polygon.
//
// @param p - Array containing the vertices of the polygon to be added.
// @param n - Number of vertices in polygon
//
// @return a unique integer identifier for the polygon
///
int Pipeline::addPoly( const Vertex p[], int n )
{
    // YOUR IMPLEMENTATION HERE
	int identifier = arrayIndex;
	for (int i = 0; i < n; i++)
	{
		polyList.push_back(p[i]);
	}
	polyList.push_back(emptyVert);
	arrayIndex += n + 1;
    // REMEMBER TO RETURN A UNIQUE ID FOR THE POLYGON
    return identifier;
}

///
// drawPoly - Draw the polygon with the given id.  The polygon should
//            be drawn after applying the current transformation to
//            the vertices of the polygon.
//
// @param polyID - the ID of the polygon to be drawn.
///
void Pipeline::drawPoly( int polyID )
{
    // YOUR IMPLEMENTATION HERE
	Vertex *polygon = new Vertex[arrayIndex];
	int polygonSize = 0;
	glm::vec3 coords;
	Vertex vert;

	//model transformations and vertex array initiation
	for (int i = polyID; i < arrayIndex; i++)
	{
		if (polyList.at(i).x == -1 && polyList.at(i).y == -1)
		{
			break;
		}
		else 
		{
			vert = polyList.at(i);
			coords = glm::vec3(vert.x, vert.y, 1.0f);//convert coordinates using
			coords = coords * transformMatrix;		 //using transform matrix
			vert.x = coords[0];
			vert.y = coords[1];
			polygon[polygonSize] = vert;
			polygonSize++;
		}
	}

	//Clip polygon
	Clipper clip;
	Vertex *outV = new Vertex[polygonSize];
	int out = clip.clipPolygon(polygonSize, polygon, outV, ll, ur);

	//Viewport transformations
	for (int i = 0; i < out; i++)
	{
		outV[i].x = ((outV[i].x - ll.x) / (ur.x - ll.x) * viewPort[2] + viewPort[0]);
		outV[i].y = ((outV[i].y - ll.y) / (ur.y - ll.y) * viewPort[3] + viewPort[1]);
	}

	//draw the polygons
	int *x = new int[out];
	int *y = new int[out];
	for (int i = 0; i < out; i++)
	{
		x[i] = outV[i].x;
		y[i] = outV[i].y;
	}
	Rasterizer raster = Rasterizer(c_height, *this);
	raster.drawPolygon(out, x, y);
	delete[] x, y, outV, polygon;
}

///
// clearTransform - Set the current transformation to the identity matrix.
///
void Pipeline::clearTransform( void )
{
    // YOUR IMPLEMENTATION HERE
	transformMatrix = glm::mat3(1.0f);//sets the transform matrix to identity
}

///
// translate - Add a translation to the current transformation by
//             premultiplying the appropriate translation matrix to
//             the current transformation matrix.
//
// @param x - Amount of translation in x.
// @param y - Amount of translation in y.
///
void Pipeline::translate( float tx, float ty )
{
    // YOUR IMPLEMENTATION HERE
	glm::mat3 translateMatrix = glm::mat3(1.f, 0.f, tx, 0.f, 1.f, ty, 0.f, 0.f, 1.f);
	transformMatrix = transformMatrix * translateMatrix;
}

///
// rotate - Add a rotation to the current transformation by premultiplying
//          the appropriate rotation matrix to the current transformation
//          matrix.
//
// @param degrees - Amount of rotation in degrees.
///
void Pipeline::rotate( float degrees )
{
    // YOUR IMPLEMENTATION HERE
	float radians = glm::radians(degrees);
	glm::mat3 rotationMatrix = glm::mat3(cos(radians), -sin(radians), 0.f, sin(radians), cos(radians), 0.f, 0.f, 0.f, 1.f);
	transformMatrix = transformMatrix * rotationMatrix;
}

///
// scale - Add a scale to the current transformation by premultiplying
//         the appropriate scaling matrix to the current transformation
//         matrix.
//
// @param x - Amount of scaling in x.
// @param y - Amount of scaling in y.
///
void Pipeline::scale( float sx, float sy )
{
    // YOUR IMPLEMENTATION HERE
	glm::mat3 scaleMatrix = glm::mat3(sx, 0.f, 0.f, 0.f, sy, 0.f, 0.f, 0.f, 1.f);
	transformMatrix = transformMatrix * scaleMatrix;
}

///
// setClipWindow - Define the clip window.
//
// @param bottom - y coord of bottom edge of clip window (in world coords)
// @param top - y coord of top edge of clip window (in world coords)
// @param left - x coord of left edge of clip window (in world coords)
// @param right - x coord of right edge of clip window (in world coords)
///
void Pipeline::setClipWindow( float bottom, float top, float left, float right )
{
    // YOUR IMPLEMENTATION HERE
	ll.x = left;
	ll.y = bottom;
	ur.x = right;
	ur.y = top;
}

///
// setViewport - Define the viewport.
//
// @param xmin - x coord of lower left of view window (in screen coords)
// @param ymin - y coord of lower left of view window (in screen coords)
// @param width - width of view window (in world coords)
// @param height - width of view window (in world coords)
///
void Pipeline::setViewport( int x, int y, int width, int height )
{
    // YOUR IMPLEMENTATION HERE
	viewPort[0] = x;
	viewPort[1] = y;
	viewPort[2] = width;
	viewPort[3] = height;
}
