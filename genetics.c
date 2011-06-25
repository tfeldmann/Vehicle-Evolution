/*
 *  genetics.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "genetics.h"


int randomWeightedVehicle(Vehicle* _vehicle, int indexToExclude)
{
	//	Multiply each fitness with a random float value between 0 and 10,
	//	biggest resulting number wins
	
	float biggestValue		= 0.0;
	int   resultingIndex	= 0;
	
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{		
		float current = _vehicle[i].fitness * (rand() % 1000) / 100.0f;
		if (current > biggestValue && i != indexToExclude)
		{
			biggestValue   = current;
			resultingIndex = i;
		}
	}
	
	return resultingIndex;
}


void getCombinedDNA(char* _DNA1, char* _DNA2, char* _result)
{
	//	Takes one block of the first DNA and inserts it
	//	into the second.
	
	unsigned char corner1 = rand() % DNA_LENGTH;
	unsigned char corner2 = rand() % DNA_LENGTH;
	
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		_result[i] = (i >= corner1 && i < corner2 || i >= corner2 && i < corner1) ? _DNA1[i] : _DNA2[i];
	}
}


void mutateDNA(char* _DNA)
{
	//	Take random digits in DNA and switch from 1 to 0 and vice-versa
	
	int mutationRate = rand() % (MUTATIONS_MAX - MUTATIONS_MIN + 1) + MUTATIONS_MIN;
	for (int i = 0; i < mutationRate; i++)
	{
		int position = rand() % DNA_LENGTH;
		_DNA[position] = !_DNA[position];
	}
}


void newGeneticGeneration(cpSpace* _space, Vehicle* _vehicle)
{	
	//	Let the statistics keep and analyse the old vehicle Genepool
	addGenePoolToStatistics(_vehicle);
	
	//	For all vehicles
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{
		//	Remove Constraints (springs or stiff connections)
		for (int j = 0; j < 6; j++)
		{
			cpSpaceRemoveConstraint(_space, _vehicle[i]._connection[j]);
		}
		
		//	Remove Wheels & Masses
		for (int j = 0; j < 2; j++)
		{
			cpSpaceRemoveBody(_space, _vehicle[i].wheel[j]._body);
			cpSpaceRemoveBody(_space, _vehicle[i].freight[j]._body);
			
			cpSpaceRemoveShape(_space, _vehicle[i].wheel[j]._shape);
			cpSpaceRemoveShape(_space, _vehicle[i].freight[j]._shape);
		}
		
		//	Generate two new DNAs and combine / mutate them
		//	Second argument is used to prohibit crossing with itself
		
		int parentIndex[2];
		parentIndex[0] = randomWeightedVehicle(_vehicle, -1);
		parentIndex[1] = randomWeightedVehicle(_vehicle, parentIndex[0]);
		
		getCombinedDNA(_vehicle[parentIndex[0]].DNA, _vehicle[parentIndex[1]].DNA, gene[i].newDNA);	
		mutateDNA(gene[i].newDNA);
		
		//	Save the parent's DNAs for showing later.
		//	Be careful with the old vehicle array since in the next step
		//	there is a new init and many values will be overwritten.
		
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < DNA_LENGTH; k++)
			{
				_vehicle[i].parentDNA[j][k] = _vehicle[parentIndex[j]].DNA[k];
			}
		}
	}
	
	//	Initialize new vehicle genepool from collected DNAs
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{
		//	Init vehicles from that new DNA
		initVehicleFromDNA(&_vehicle[i], gene[i].newDNA);
		moveVehicleToStartpoint(&_vehicle[i]);
		initVehiclePhysics(_space, &_vehicle[i]);
		
		//	Put non-active vehicles into sleep (physics)
		if (i != 0)
		{
			for (int j = 0; j < 2; j++)
			{
				cpBodySleep(_vehicle[i].freight[j]._body);
				cpBodySleep(_vehicle[i].wheel[j]._body);
			}
		}
	}
}