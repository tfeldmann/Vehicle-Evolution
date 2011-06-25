/*
 *  genetics.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#pragma once
#include "vehicle.h"
#include "statistics.h"

struct Gene
{
	char newDNA[DNA_LENGTH];
}	gene[TEST_GROUP_SIZE];


int	 randomWeightedVehicle(struct Vehicle* _vehicle,  int indexToExclude);
void getCombinedDNA(char* _DNA1, char* _DNA2, char* _result);
void mutateDNA(char* _DNA);

void newGeneticGeneration(cpSpace* _space, struct Vehicle* _vehicle);