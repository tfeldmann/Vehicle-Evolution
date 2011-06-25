/*
 *  statistics.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 13.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include "chipmunk.h"
#include <GLUT/glut.h>
#include "vehicle.h"
#include "definitions.h"

struct StatisticEntry
{
	cpFloat average;
	cpFloat variance;
}	statEntry[STAT_ENTRIES_MAX];

int statCounter;

void initStatistics();
void addGenePoolToStatistics(Vehicle* _vehicle);
void drawStatistics();