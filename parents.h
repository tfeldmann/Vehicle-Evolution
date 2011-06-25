/*
 *  parents.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 13.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include "vehicle.h"
#include "chipmunk.h"
#include <GLUT/GLUT.h>

Vehicle parent[2];

void bufferParentsOfVehicleForDisplay(Vehicle *_vehicle);
void drawParents(Vehicle *_vehicle);
cpFloat getParentWheelAngle();

void drawDNA(char* _DNA, cpFloat posX, cpFloat posY, cpFloat width, cpFloat height);