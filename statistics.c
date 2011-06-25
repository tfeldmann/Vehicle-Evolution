/*
 *  statistics.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 13.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "statistics.h"

void initStatistics()
{
	statCounter = 1;
	for (int i = 0; i < STAT_ENTRIES_MAX; i++)
	{
		statEntry[i].average = 0;
		statEntry[i].variance = 0;
	}
}


void addGenePoolToStatistics(Vehicle* _vehicle)
{
	statCounter++;
	
	if (statCounter > STAT_ENTRIES_MAX)
	{
		statCounter = STAT_ENTRIES_MAX;
		
		//	Shift array entries one to the left
		//	so that the last entry is free again
		for (int j = 0; j < STAT_ENTRIES_MAX - 1; j++)
		{
			statEntry[j] = statEntry[j+1];
		}
	}
	
	statEntry[statCounter - 1].average = 0;
	statEntry[statCounter - 1].variance = 0;
	
	//	Calculate average
	cpFloat average = 0;
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{
		average += _vehicle[i].fitness;
	}
	statEntry[statCounter - 1].average = average / TEST_GROUP_SIZE;
	
	//	Calculate Variance
	cpFloat variance = 0;
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{
		variance += pow(_vehicle[i].fitness - average, 2.0);
	}
	statEntry[statCounter - 1].variance = sqrt(variance / TEST_GROUP_SIZE) / 30;
}


void drawStatistics()
{
	//	The stats are located in the top right corner.
	glPushMatrix();
	glTranslatef(glutGet(GLUT_WINDOW_WIDTH) - STAT_WIDTH - STAT_PADDING - STAT_BORDER,
				 glutGet(GLUT_WINDOW_HEIGHT) - STAT_HEIGHT - STAT_PADDING - STAT_BORDER, 0.0f);
	
	//	Draw Background
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
	glBegin(GL_QUADS);
	glVertex2f(- STAT_PADDING,				- STAT_PADDING);
	glVertex2f(STAT_WIDTH + STAT_PADDING,	- STAT_PADDING);
	glVertex2f(STAT_WIDTH + STAT_PADDING,	STAT_HEIGHT + STAT_PADDING);
	glVertex2f(- STAT_PADDING,				STAT_HEIGHT + STAT_PADDING);
	glEnd();

	//	Get the biggest and lowest value for sizing the graph
	cpFloat biggestValue = 0;
	cpFloat biggestVariance = 0;
	cpFloat lowestValue = 100;
	for (int i = 0; i < statCounter; i++)
	{
		if (statEntry[i].average + statEntry[i].variance > biggestValue)
		{
			biggestValue = statEntry[i].average + statEntry[i].variance;
		}
		
		if (statEntry[i].variance > biggestVariance)
		{
			biggestVariance = statEntry[i].variance;
		}
		
		if (statEntry[i].average - statEntry[i].variance < lowestValue)
		{
			lowestValue = statEntry[i].average - statEntry[i].variance;
		}
	}
	
	//	Compute scaling factors for each axis
	cpFloat scaleX = (statCounter == 0) ? STAT_WIDTH : STAT_WIDTH / (statCounter - 1);
	cpFloat scaleY = STAT_HEIGHT / (biggestValue - lowestValue);
	
	//	Draw variance
	glColor4f(0.7f, 0.7f, 0.7f, 0.4f);
	glBegin(GL_QUADS);
	for (int i = 0; i < statCounter - 1; i++)
	{		
		glVertex2f(i * scaleX, (statEntry[i].average - statEntry[i].variance) * scaleY);
		glVertex2f(i * scaleX, (statEntry[i].average + statEntry[i].variance) * scaleY);
		
		glVertex2f((i + 1) * scaleX, (statEntry[i+1].average + statEntry[i+1].variance) * scaleY);
		glVertex2f((i + 1) * scaleX, (statEntry[i+1].average - statEntry[i+1].variance) * scaleY);
	}
	glEnd();
	
	//	Draw average
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < statCounter; i++)
	{
		glVertex2f(i * scaleX, statEntry[i].average * scaleY);
	}
	glEnd();
	
	glPopMatrix();
}


