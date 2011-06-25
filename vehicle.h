/*
 *  vehicle.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 01.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>
#include "chipmunk.h"
#include "functions.h"
#include "definitions.h"

extern char lineMode;

typedef struct Freight
{
	cpVect		pos;
	cpFloat		radius;
	cpBody*		_body;
	cpShape*	_shape;
}	Freight;

typedef struct Wheel
{
	cpVect		pos;
	cpFloat		radius;
	cpBody*		_body;
	cpShape*	_shape;
}	Wheel;

typedef struct Vehicle
{
	char			hasPhysics;
	struct Freight	freight[2];
	struct Wheel	wheel[2];
	cpConstraint*	_connection[6];
	
	char			DNA[DNA_LENGTH];
	char			parentDNA[2][DNA_LENGTH];
	
	cpFloat			fitness;
	cpVect			pos;
}	Vehicle;

void moveVehicleToStartpoint(Vehicle* _vehicle);
void randomlyInitVehicle(Vehicle* _vehicle);
void initVehicleFromDNA(Vehicle* _vehicle, char* _newDNA);
void initVehiclePhysics(cpSpace* space, Vehicle* _vehicle);
void updateVehicle(Vehicle* _vehicle);
void drawVehicle(Vehicle* _vehicle, cpFloat lineWidth);

void drawWheel(cpFloat x, cpFloat y, cpFloat radius, cpFloat angle, cpFloat lineWidth);
void drawMass(cpFloat x, cpFloat y, cpFloat radius, cpFloat angle, cpFloat lineWidth);