/*
 *  functions.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 01.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "functions.h"

float radianToDegree(float radianAngle)
{
	return 180.0f / PI * radianAngle;
}

unsigned int power_i(int b, int e)
{	
	if (e == 0)	return 1;
	
	unsigned int result = 1;
	while (e-- > 0)
	{
		result *= b;
	}
	return result;
}


char valueToGrayCode(int value, char bit)
{
	value++;
	return (
			(value - power_i(2, bit)) % power_i(2, bit + 2) <= power_i(2, bit + 1)
		&&	(value - power_i(2, bit)) % power_i(2, bit + 2) != 0
		&&	(value - power_i(2, bit)) > 0);
}

unsigned int grayCodeToValue(char* _gray, char bitCount)
{
	unsigned int	value  = 0;
	char			faktor = 1;
	
	for (int i = 0; i < bitCount; i++)
	{
		if (_gray[bitCount - 1 - i] == 1)
		{
			value  += (power_i(2, bitCount - i) - 1) * faktor;
			faktor *= -1;
		}
	}
	return value;
}


void drawCircle(float r, int segments)
{
	/*
	 *	Only writes the bare vertices. Translation, color and
	 *	line thickness have to be set up before
	 */
	
	double mx = r * cos(359 * PI/180.0f);
	double my = r * sin(359 * PI/180.0f);
	
	for (int i = 0; i < 358; i++)
	{
		glVertex2f(mx, my);
		mx = r * cos(i * PI/180.0f);
		my = r * sin(i * PI/180.0f);
		glVertex2f(mx, my);
	}
}