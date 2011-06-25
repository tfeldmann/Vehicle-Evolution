/*
 *  terrain.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include <math.h>
#include "chipmunk.h"
#include "camera.h"
#include "definitions.h"

typedef struct Terrain
{
	cpVect		p[2];
	cpShape*	_shape;
}	Terrain;

void randomlyInitTerrain(cpSpace* _space, Terrain* _terrain);
void drawTerrain(Terrain* _terrain);
void drawTerrainEditNodes(Terrain* _terrain, int selectedTerrainNode);