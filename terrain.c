/*
 *  terrain.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "terrain.h"

void randomlyInitTerrain(cpSpace* _space, Terrain* _terrain)
{
	//	init first (flat) part
	_terrain[0].p[0] = cpv(0.0f, 0.0f);
	_terrain[0].p[1] = cpv(TERRAIN_FLAT_LENGTH, 0.0f);
	
	//	random Values for the rest of the terrain
	for (int i = 1; i < TERRAIN_SEGMENTS; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			_terrain[i].p[0] = _terrain[i - 1].p[1];
			_terrain[i].p[1] = cpv(_terrain[i].p[0].x + rand() % 300, rand() % 80);
		}
	}
	
	//	Add physics
	for (int i = 0; i < TERRAIN_SEGMENTS; i++)
	{
		_terrain[i]._shape = cpSegmentShapeNew(
													 &_space->staticBody,
													 _terrain[i].p[0],
													 _terrain[i].p[1],
													 0.0f);
		_terrain[i]._shape->e = 1.0f;
		_terrain[i]._shape->u = 0.8f;
		_terrain[i]._shape->collision_type = GROUND_COLLISION_TYPE;
		cpSpaceAddShape(_space, _terrain[i]._shape);
	}
}


void drawTerrain(Terrain* _terrain)
{
	//	Draw the terrain
	for (int i = 0; i < TERRAIN_SEGMENTS; i++)
	{
		if (!lineMode)
		{
			glBegin(GL_QUADS);
			
			glColor3f(0.62f, 0.35f, 0.0f);
			glVertex2f(_terrain[i].p[0].x, getCameraPosition().y);	//	y here is lowest visible point
			glColor3f(0.62f, 0.35f, 0.0f);
			glVertex2f(_terrain[i].p[1].x, getCameraPosition().y);
			
			glColor3f(0.14f, 0.30f, 0.0f);
			glVertex2f(_terrain[i].p[1].x, _terrain[i].p[1].y);
			glColor3f(0.14f, 0.30f, 0.0f);
			glVertex2f(_terrain[i].p[0].x, _terrain[i].p[0].y);
			
			glEnd();
		}
		else
		{
			glColor3f(0.0f, 0.0f, 0.0f);
			glLineWidth(getCameraZoomFactor());
			glBegin(GL_LINES);
			glVertex2f(_terrain[i].p[0].x, _terrain[i].p[0].y);
			glVertex2f(_terrain[i].p[1].x, _terrain[i].p[1].y);
			glEnd();
		}

	}
	
	//	Draw the little red flag
	glLineWidth(3.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(_terrain[FLAG_POSITION].p[1].x, _terrain[FLAG_POSITION].p[1].y);
	glVertex2f(_terrain[FLAG_POSITION].p[1].x, _terrain[FLAG_POSITION].p[1].y + 100.0f);
	glEnd();
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2f(_terrain[FLAG_POSITION].p[1].x, _terrain[FLAG_POSITION].p[1].y + 100.0f);
	glVertex2f(_terrain[FLAG_POSITION].p[1].x, _terrain[FLAG_POSITION].p[1].y + 60.0f);
	glVertex2f(_terrain[FLAG_POSITION].p[1].x - 40.0f,  _terrain[FLAG_POSITION].p[1].y + 80.0f);
	glEnd();
}

void drawTerrainEditNodes(Terrain* _terrain, int selectedTerrainNode)
{
	//	Draw a circle around all defining terrain-points
	//	The flat part at the beginning as well as the last parts
	//	cannot be edited.
	for (int i = 1; i < TERRAIN_SEGMENTS - 1; i++)
	{
		glPushMatrix();
		glTranslatef(_terrain[i].p[1].x, _terrain[i].p[1].y, 0.0f);
		
		glColor3f(1.0f, 0.0f, 0.0f);
		if (i == selectedTerrainNode)
		{
			glLineWidth(3.0f * getCameraZoomFactor());
		}
		else
		{
			glLineWidth(0.4f * getCameraZoomFactor());
		}
		
		glBegin(GL_LINES);
		drawCircle(10, 100);
		glEnd();
		glPopMatrix();
	}
}