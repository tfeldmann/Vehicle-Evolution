/*
 *  parents.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 13.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "parents.h"

void bufferParentsOfVehicleForDisplay(Vehicle *_vehicle)
{
	//	The parents of a vehicle are stored in the array parentDNA so
	//	we first have to save, decompose and buffer them
	
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < DNA_LENGTH; j++)
		{
			parent[i].DNA[j] = _vehicle->parentDNA[i][j];
		}
	}
	
	for (int i = 0; i < 2; i++)
	{
		initVehicleFromDNA(&parent[i], parent[i].DNA);
		updateVehicle(&parent[i]);
	}
}


void drawParents(Vehicle *_vehicle)
{
	//	The parents are located left to the stats
	glPushMatrix();
	glTranslatef(glutGet(GLUT_WINDOW_WIDTH) - STAT_WIDTH - 2 * (STAT_PADDING + STAT_BORDER) - PARENTS_PADDING - PARENTS_WIDTH,
				 glutGet(GLUT_WINDOW_HEIGHT) - PARENTS_HEIGHT - PARENTS_PADDING - PARENTS_BORDER, 0.0f);
	
	//	Draw background
	glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
	glBegin(GL_QUADS);
	glVertex2f(- PARENTS_PADDING,				- PARENTS_PADDING);
	glVertex2f(PARENTS_WIDTH + PARENTS_PADDING, - PARENTS_PADDING);
	glVertex2f(PARENTS_WIDTH + PARENTS_PADDING, PARENTS_HEIGHT + PARENTS_PADDING);
	glVertex2f(- PARENTS_PADDING,				PARENTS_HEIGHT + PARENTS_PADDING);
	glEnd();
	
	//	Draw seperating line
	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(0.8f);
	glBegin(GL_LINES);
	glVertex2f(PARENTS_WIDTH / 2, PARENTS_HEIGHT);
	glVertex2f(PARENTS_WIDTH / 2, PARENTS_HEIGHT * 0.4);
	glEnd();
	
	//	Draw vehicles
	for (int i = 0; i < 2; i++)
	{
		glPushMatrix();
		glTranslatef(PARENTS_WIDTH / 2 * i + PARENTS_WIDTH / 4 - parent[i].pos.x * PARENTS_SCALE,
					 3 * PARENTS_HEIGHT / 4 - PARENTS_SCALE * parent[i].pos.y,
					 0.0f);
		glScalef(PARENTS_SCALE, PARENTS_SCALE, 0.0f);
		drawVehicle(&parent[i], 0.2f);
		glPopMatrix();
	}
	
	//	Draw parent's DNA
	for (int i = 0; i < 2; i++)
	{
		drawDNA(parent[i].DNA, 0, PARENTS_HEIGHT*0.3 - i*15, PARENTS_WIDTH, 10);
	}
	
	//	Draw current individual's DNA
	drawDNA(_vehicle->DNA, 0, 0, PARENTS_WIDTH, 15);
	
	glPopMatrix();
}


void drawDNA(char* _DNA, cpFloat posX, cpFloat posY, cpFloat width, cpFloat height)
{
	for (int i = 0; i < DNA_PARTS; i++)
	{
		//	Find the color of this part
		cpFloat brightness = (float)grayCodeToValue(&_DNA[i * GRAY_CODE_BIT], GRAY_CODE_BIT) / (float)GRAY_CODE_HIGHEST;
		
		//	and draw it
		glColor3f(1 - brightness, brightness, brightness);
		glBegin(GL_QUADS);
		glVertex2f(posX + i * width / DNA_PARTS, posY);
		glVertex2f(posX + (i + 1) * width / DNA_PARTS, posY);
		glVertex2f(posX + (i + 1) * width / DNA_PARTS, posY + height);
		glVertex2f(posX + i * width / DNA_PARTS, posY + height);
		glEnd();
	}
}


