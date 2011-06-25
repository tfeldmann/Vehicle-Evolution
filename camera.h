/*
 *  camera.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include <GLUT/GLUT.h>
#include "chipmunk.h"
#include "vehicle.h"
#include "keys.h"

cpFloat zoomFactor;
cpVect	offset;
cpVect	pos;

void cameraReset();
void cameraUpdate(Vehicle* _vehicle);
cpFloat getCameraZoomFactor();
cpVect getCameraPosition();
void cameraMove();

//	Coordinate transformations
cpVect windowToWorld(cpVect windowPoint);