//
//  Clipper.cpp
//
//  Created by Warren R. Carithers on 01/15/14.
//  Based on a version created by Joe Geigel on 11/30/11.
//  Copyright 2014 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Paul Zenie, pxz5572
//

#include "Vertex.h"
#include "Clipper.h"

///
// Simple module that performs clipping
///

///
// Constructor
///
Clipper::Clipper() {
}
/*
Finds if a point is inside in reference to an edge
*/
bool inside(Vertex s, Vertex bound0, Vertex bound1)
{
	if (bound0.x == bound1.x)
	{
		if (bound1.y > bound0.y)
		{
			return s.x <= bound0.x;
		}
		else if(bound0.y > bound1.y)
		{
			return s.x >= bound0.x;
		}
	}
	else
	{
		if (bound1.x > bound0.x)
		{
			return s.y >= bound0.y;
		}
		else if (bound0.x > bound1.x)
		{
			return s.y <= bound0.y;
		}
	}
	return false;
}
/*
Finds the point where the line between p and s instersects the edge
Which is the line from bound0 to bound1
*/
Vertex intersect(Vertex p, Vertex s, Vertex bound0, Vertex bound1)
{
	Vertex intersect;
	if (bound0.x == bound1.x)
	{
		intersect.x = bound0.x;
		intersect.y = p.y + (bound0.x - p.x) * (s.y - p.y) / (s.x - p.x);
	}
	else 
	{
		intersect.y = bound0.y;
		intersect.x = p.x + (bound0.y - p.y) * (s.x - p.x) / (s.y - p.y);
	}
	return intersect;
}

void SHCP(const Vertex inV[], Vertex outV[], int in, int &out, Vertex bound0, Vertex bound1)
{
	//out = 0;
	Vertex p = inV[in - 1];
	for (int j = 0; j < in; j++)
	{
		Vertex s = inV[j];
		if (inside(s, bound0, bound1))
		{
			if (inside(p, bound0, bound1)) // if point inside in ref to edge keep it
			{
				outV[out] = s;
				out++;
			}
			else	// if point comes inside find intersect and add it to vector array
			{
				Vertex i = intersect(p, s, bound0, bound1);
				outV[out] = i;
				out++;
				outV[out] = s;
				out++;
			}
		}
		else
		{
			if (inside(p, bound0, bound1))
			{
				Vertex i = intersect(p, s, bound0, bound1);
				outV[out] = i;
				out++;
			}
		}
		p = s;
	}
}

///
// clipPolygon
//
// Clip the polygon with vertex count in and vertices inV against the
// rectangular clipping region specified by lower-left corner ll and
// upper-right corner ur. The resulting vertices are placed in outV.
//
// The routine should return the with the vertex count of polygon
// resulting from the clipping.
//
// @param in    the number of vertices in the polygon to be clipped
// @param inV   the incoming vertex list
// @param outV  the outgoing vertex list
// @param ll    the lower-left corner of the clipping rectangle
// @param ur    the upper-right corner of the clipping rectangle
//
// @return number of vertices in the polygon resulting after clipping
//
///
int Clipper::clipPolygon( int in, const Vertex inV[], Vertex outV[],
                          Vertex ll, Vertex ur )
{
	int out = 0;
	Vertex *out1, *out2, *out3; // vertex arrays to hold intermediate vertices
	Vertex bound;				// edge of clipping box
	bound.x = ur.x;
	bound.y = ll.y;
	SHCP(inV, outV, in, out, ll, bound);//call shcp for each edge
	in = out;
	out1 = new Vertex[out*out];
	out = 0;
	SHCP(outV, out1, in, out, bound, ur);
	in = out;
	out2 = new Vertex[out*out];
	out = 0;
	bound.x = ll.x;
	bound.y = ur.y;
	SHCP(out1, out2, in, out, ur, bound);
	in = out;
	out3 = new Vertex[out*out];
	out = 0;
	SHCP(out2, out3, in, out, bound, ll);
	for (int i = 0; i < out; i++)	//populate outV with correct vertices
	{
		outV[i] = out3[i];
	}
	delete[] out1, out2, out3;
	return( out );  // remember to return the outgoing vertex count!
}
