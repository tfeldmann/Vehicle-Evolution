/*
 *  functions.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 01.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include <math.h>
#include <glut/glut.h>
#include "definitions.h"

//	Costum Math
float radianToDegree(float radianAngle);
unsigned int power_i(int b, int e);

//	Graycode
char valueToGrayCode(int value, char bit);
unsigned int grayCodeToValue(char* _gray, char bitCount);

//	Drawing
void drawCircle(float r, int segments);