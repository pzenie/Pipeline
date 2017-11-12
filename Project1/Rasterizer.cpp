//
//  Rasterizer.cpp
//
//  Created by Joe Geigel on 11/30/11.
//  Modifications by Warren R. Carithers.
//  Copyright 2011 Rochester Institute of Technology. All rights reserved.
//
//  Contributor:  Paul Zenie, pxz5572
//

#include <list>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "Rasterizer.h"
#include "Canvas.h"

using namespace std;

///
// Simple class that performs rasterization algorithms
///

///
// Constructor
//
// @param n number of scancurrentLines
// @param C The Canvas to use
///
Rasterizer::Rasterizer(int n, Canvas &canvas) : n_scanlines(n), C(canvas)
{
}

class EdgeBucket
{
public:
	int yMin;
	int	yMax;
	double x; //x value associated with yMin coordinate
	double m; //inverse of slope
};
struct SortByY
{
	inline bool operator() (const EdgeBucket struct1, const EdgeBucket struct2)
	{
		return (struct1.yMin < struct2.yMin);
	}
};
struct SortByX
{
	inline bool operator() (const EdgeBucket struct1, const EdgeBucket struct2)
	{
		return (struct1.x < struct2.x);
	}
};
///
//Initializes the edge table with the appropriate values
// from x[] and y[]
///
vector<EdgeBucket> CreateEdgeTable(int n, const int x[], const int y[])
{
	vector<EdgeBucket> edgeTable;
	EdgeBucket bucket;
	for (int i = 0; i < n; i++) // loop through x and y and create buckets 
	{
		if (y[i % n] < y[(i + 1) % n])
		{
			bucket.yMin = y[i % n];
			bucket.yMax = y[(i + 1) % n];
			bucket.x = x[i % n];
		}
		else
		{
			bucket.yMin = y[(i + 1) % n];
			bucket.yMax = y[i % n];
			bucket.x = x[(i + 1) % n];
		}
		if (bucket.yMax - bucket.yMin != 0)
		{
			bucket.m = (double)(x[i % n] - x[(i + 1) % n]) / (double)(y[i % n] - y[(i + 1) % n]);
			edgeTable.push_back(bucket); // add bucket to table
		}
	}
	return edgeTable;
}
double RoundDouble(double d)
{
	return floor(d + 0.5);
}
///
// Draw a filled polygon.
//
// Implementation should use the scan-currentLine polygon fill algorithm
// discussed in class.
//
// The polygon has n distinct vertices.  The coordinates of the vertices
// making up the polygon are stored in the x and y arrays.  The ith
// vertex will have coordinate (x[i],y[i]).
//
// You are to add the implementation here using only calls to the
// setPixel() function.
//
// @param n - number of vertices
// @param x - array of x coordinates
// @param y - array of y coordinates
///
void Rasterizer::drawPolygon(int n, const int x[], const int y[])
{
	std::vector<EdgeBucket> edgeTable = CreateEdgeTable(n, x, y); // main edge table
	EdgeBucket bucket;
	// sort by yMin
	sort(edgeTable.begin(), edgeTable.end(), SortByY());

	bool inside;
	vector<EdgeBucket> activeList;
	int endScanLine = edgeTable[edgeTable.size() - 1].yMax;

	for (int currentLine = edgeTable[0].yMin; currentLine <= endScanLine; currentLine++)
	{
		inside = false;
		int i = 0;
		while (i < edgeTable.size())
		{
			bucket = edgeTable[i];
			if (bucket.yMin == currentLine) //if currentline is at ymin of the 
			{								//bucket add it to the activelist
				activeList.push_back(bucket);
				edgeTable.erase(edgeTable.begin() + i);//and remove from table
			}
			else
			{
				i++;
			}
		}
		if (currentLine < endScanLine)
		{
			i = 0;
			while (i < activeList.size())
			{
				bucket = activeList[i];
				if (bucket.yMax == currentLine) //if the buckets y.max is at
				{								//current scan line remove it
					activeList.erase(activeList.begin() + i);//from the active list
				}
				else
				{
					i++;
				}
			}
		}
		//sort the active list by x
		sort(activeList.begin(), activeList.end(), SortByX());

		int edgeIndex = 0;
		int endX = *max_element(x, x + n);
		for (int currentX = 0; currentX <= endX && edgeIndex < activeList.size(); currentX++)
		{
			if (currentX == RoundDouble(activeList[edgeIndex].x))
			{
				if (edgeIndex + 1 < activeList.size() && currentX != RoundDouble(activeList[edgeIndex + 1].x))
				{
					inside = !inside; //crossed line, change whether inside or out
				}
				while (edgeIndex < activeList.size() && currentX == RoundDouble(activeList[edgeIndex].x))
				{
					//update x values
					activeList[edgeIndex].x += activeList[edgeIndex].m;
					C.setPixel(currentX, currentLine); //set the buckets last pixel
					edgeIndex++;
				}
			}
			if (inside)//set pixel if inside polygon 
			{
				C.setPixel(currentX, currentLine);
			}
		}
	}
}