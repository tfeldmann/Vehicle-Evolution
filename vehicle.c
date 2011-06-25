/*
 *  vehicle.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 01.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "vehicle.h"

void moveVehicleToStartpoint(Vehicle* _vehicle)
{
	updateVehicle(_vehicle);
	int dx = STARTING_OFFSET_X - _vehicle->pos.x;
	int dy = STARTING_OFFSET_Y - _vehicle->pos.y;
	
	_vehicle->wheel[0].pos.x	+= dx;
	_vehicle->wheel[0].pos.y	+= dy;
	_vehicle->wheel[1].pos.x	+= dx;
	_vehicle->wheel[1].pos.y	+= dy;
	_vehicle->freight[0].pos.x  += dx;
	_vehicle->freight[0].pos.y  += dy;
	_vehicle->freight[1].pos.x  += dx;
	_vehicle->freight[1].pos.y  += dy;
}


void randomlyInitVehicle(Vehicle* _vehicle)
{
	//	Inits the vehicle with a random DNA
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		_vehicle->DNA[i] = rand() % 2;
	}
	initVehicleFromDNA(_vehicle, _vehicle->DNA);
	
	/*
	 *	The following lines are much more complex than they need to be.
	 *	Might be useful later for saving and loading vehicles
	 *	- so I'll just leave them here.
	 */
	
	/*
	//	Generate Random Positions
	for (int i = 0; i < 2; i++)
	{
		//	for wheels
		_vehicle->wheel[i].pos.x = rand() % 255;
		_vehicle->wheel[i].pos.y = rand() % 255;
		_vehicle->wheel[i].radius = rand() % 255;
		
		//	for the masses
		_vehicle->mass[i].pos.x = rand() % 255;
		_vehicle->mass[i].pos.y = rand() % 255;
		_vehicle->mass[i].radius = rand() % 255;
	}
	
	unsigned char entry = 0;
	for (int i = 0; i < DNA_PARTS; i++)
	{
		if (i ==  0)	entry = _vehicle->mass[0].pos.x;
		if (i ==  1)	entry = _vehicle->mass[1].pos.x;
		if (i ==  2)	entry = _vehicle->mass[0].pos.y;
		if (i ==  3)	entry = _vehicle->mass[1].pos.y;
		if (i ==  4)	entry = _vehicle->mass[0].radius;
		if (i ==  5)	entry = _vehicle->mass[1].radius;
		
		if (i ==  6)	entry = _vehicle->wheel[0].pos.x;
		if (i ==  7)	entry = _vehicle->wheel[1].pos.x;
		if (i ==  8)	entry = _vehicle->wheel[0].pos.y;
		if (i ==  9)	entry = _vehicle->wheel[1].pos.y;
		if (i == 10)	entry = _vehicle->wheel[0].radius;
		if (i == 11)	entry = _vehicle->wheel[1].radius;
		
		//	convert to gray-code
		for (int k = 0; k < GRAY_CODE_BIT; k++)
		{
			_vehicle->DNA[i * GRAY_CODE_BIT + k] = valueToGrayCode(entry, k);
		}
	}
	 */
}


void initVehicleFromDNA(Vehicle* _vehicle, char* _newDNA)
{
	/*
	 *	Composition of the DNA:
	 *
	 *	MASS1.X  - MASS2.X  - MASS1.Y  - MASS2.Y  - MASS1.R  - MASS2.R - 
	 *	WHEEL1.X - WHEEL1.Y - WHEEL2.X - WHEEL2.Y - WHEEL1.R - WHEEL2.R
	 *
	 */
	
	
	//	copy the given DNA
	for (int i = 0; i < DNA_LENGTH; i++)
	{
		_vehicle->DNA[i] = _newDNA[i];
	}
	
	//	Decompose the DNA and save the parts in
	//	the corresponding variables
	cpFloat* _entry;
	for (int i = 0; i < DNA_PARTS; i++)
	{
		if (i ==  0)	_entry = &_vehicle->freight[0].pos.x;
		if (i ==  1)	_entry = &_vehicle->freight[1].pos.x;
		if (i ==  2)	_entry = &_vehicle->freight[0].pos.y;
		if (i ==  3)	_entry = &_vehicle->freight[1].pos.y;
		if (i ==  4)	_entry = &_vehicle->freight[0].radius;
		if (i ==  5)	_entry = &_vehicle->freight[1].radius;
		
		if (i ==  6)	_entry = &_vehicle->wheel[0].pos.x;
		if (i ==  7)	_entry = &_vehicle->wheel[1].pos.x;
		if (i ==  8)	_entry = &_vehicle->wheel[0].pos.y;
		if (i ==  9)	_entry = &_vehicle->wheel[1].pos.y;
		if (i == 10)	_entry = &_vehicle->wheel[0].radius;
		if (i == 11)	_entry = &_vehicle->wheel[1].radius;
		
		*_entry = grayCodeToValue(&_vehicle->DNA[i * GRAY_CODE_BIT], GRAY_CODE_BIT);
	}
}


void initVehiclePhysics(cpSpace* _space, Vehicle* _vehicle)
{
	for (int i = 0; i < 2; i++)
	{
		/*
		 *	Creates the chipmunk representations of the vehicle. The wheel size has to be interpolated to fit
		 *	in the range from 0 to 255 so that it can be stored in the DNA.
		 */
		
		//	create a physical representation of the wheels for chipmunk to use
		cpFloat wheelRadius = WHEEL_SIZE_MIN + _vehicle->wheel[i].radius * (WHEEL_SIZE_MAX - WHEEL_SIZE_MIN) / GRAY_CODE_HIGHEST;
		cpFloat wheelMass   = wheelRadius / 20.0f * 0.7f;
		
		_vehicle->wheel[i]._body = cpBodyNew(wheelMass, cpMomentForCircle(wheelMass, 0.0f, wheelRadius, cpvzero));
		_vehicle->wheel[i]._body->p = cpv(_vehicle->wheel[i].pos.x, _vehicle->wheel[i].pos.y);
		_vehicle->wheel[i]._body->v = cpv(0.0f, 0.0f);
		cpSpaceAddBody(_space, _vehicle->wheel[i]._body);
		
		_vehicle->wheel[i]._shape = cpSpaceAddShape(_space, cpCircleShapeNew(_vehicle->wheel[i]._body, wheelRadius, cpvzero));
		_vehicle->wheel[i]._shape->e = 0.4f;
		_vehicle->wheel[i]._shape->u = 0.8f;
		_vehicle->wheel[i]._shape->group = VEHICLE_COLLISION_GROUP;
		_vehicle->wheel[i]._shape->collision_type = WHEEL_COLLISION_TYPE;
		
		
		//	create a physical representation of the masses for chipmunk to use
		cpFloat freightRadius = MASS_SIZE_MIN + _vehicle->freight[i].radius * (MASS_SIZE_MAX - MASS_SIZE_MIN) / GRAY_CODE_HIGHEST;
		cpFloat freightMass   = freightRadius * MASS_TO_RADIUS_RELATION;
		
		_vehicle->freight[i]._body = cpBodyNew(freightMass, cpMomentForCircle(freightMass, 0.0f, freightRadius, cpvzero));
		_vehicle->freight[i]._body->p = cpv(_vehicle->freight[i].pos.x, _vehicle->freight[i].pos.y);
		_vehicle->freight[i]._body->v = cpvzero;
		cpSpaceAddBody(_space, _vehicle->freight[i]._body);
		
		_vehicle->freight[i]._shape = cpSpaceAddShape(_space, cpCircleShapeNew(_vehicle->freight[i]._body, freightRadius, cpvzero));
		_vehicle->freight[i]._shape->group = VEHICLE_COLLISION_GROUP;
		_vehicle->freight[i]._shape->collision_type = MASS_COLLISION_TYPE;
	}
	
	#if STIFF_CONNECTIONS
	//	create pin-point connections for vehicle
	_vehicle->_connection[0] = cpPinJointNew( _vehicle->mass[0]._body,   _vehicle->mass[1]._body, cpv(0,0), cpv(0,0));
	_vehicle->_connection[1] = cpPinJointNew( _vehicle->mass[0]._body,  _vehicle->wheel[0]._body, cpv(0,0), cpv(0,0));
	_vehicle->_connection[2] = cpPinJointNew( _vehicle->mass[0]._body,  _vehicle->wheel[1]._body, cpv(0,0), cpv(0,0));
	_vehicle->_connection[3] = cpPinJointNew( _vehicle->mass[1]._body   _vehicle->wheel[0]._body, cpv(0,0), cpv(0,0));
	_vehicle->_connection[4] = cpPinJointNew( _vehicle->mass[1]._body,  _vehicle->wheel[1]._body, cpv(0,0), cpv(0,0));
	_vehicle->_connection[5] = cpPinJointNew(_vehicle->wheel[0]._body,  _vehicle->wheel[1]._body, cpv(0,0), cpv(0,0));
	#else
	//	create spring connections for vehicle
	_vehicle->_connection[0] = cpDampedSpringNew(_vehicle->freight[0]._body, _vehicle->freight[1]._body, cpvzero, cpvzero, cpvdist( _vehicle->freight[0]._body->p,  _vehicle->freight[1]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	_vehicle->_connection[1] = cpDampedSpringNew(_vehicle->freight[0]._body,  _vehicle->wheel[0]._body, cpvzero, cpvzero, cpvdist( _vehicle->freight[0]._body->p, _vehicle->wheel[0]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	_vehicle->_connection[2] = cpDampedSpringNew(_vehicle->freight[0]._body,  _vehicle->wheel[1]._body, cpvzero, cpvzero, cpvdist( _vehicle->freight[0]._body->p, _vehicle->wheel[1]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	_vehicle->_connection[3] = cpDampedSpringNew(_vehicle->freight[1]._body,  _vehicle->wheel[0]._body, cpvzero, cpvzero, cpvdist( _vehicle->freight[1]._body->p, _vehicle->wheel[0]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	_vehicle->_connection[4] = cpDampedSpringNew(_vehicle->freight[1]._body,  _vehicle->wheel[1]._body, cpvzero, cpvzero, cpvdist( _vehicle->freight[1]._body->p, _vehicle->wheel[1]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	_vehicle->_connection[5] = cpDampedSpringNew(  _vehicle->wheel[1]._body,  _vehicle->wheel[0]._body, cpvzero, cpvzero, cpvdist(_vehicle->wheel[1]._body->p, _vehicle->wheel[0]._body->p), SPRING_SITFFNESS, SPRING_DAMPING);
	#endif
	
	//	Add constraints to Chipmunk Space
	for (int i = 0; i < 6; i++)
	{
		cpSpaceAddConstraint(_space, _vehicle->_connection[i]);
	}
	
	//	Set a flag that physics were set up,
	//	affects the way the vehicle is drawn
	_vehicle->hasPhysics = TRUE;
	
	//	Don't forget to reset the fitness
	_vehicle->fitness = 0;
}


void updateVehicle(Vehicle* _vehicle)
{
	//	Sets vehicle.pos as the center of the vehicle (average of wheel and mass positions)
	_vehicle->pos.x = (_vehicle->freight[0].pos.x + _vehicle->freight[1].pos.x + _vehicle->wheel[0].pos.x + _vehicle->wheel[1].pos.x) / 4;
	_vehicle->pos.y = (_vehicle->freight[0].pos.y + _vehicle->freight[1].pos.y + _vehicle->wheel[0].pos.y + _vehicle->wheel[1].pos.y) / 4;
}


void drawVehicle(Vehicle* _vehicle, cpFloat lineWidth)
{	
	//	Draw connections
	glLineWidth(lineWidth);
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINE_STRIP);
		glVertex2f(_vehicle->freight[0].pos.x, _vehicle->freight[0].pos.y);
		glVertex2f(_vehicle->freight[1].pos.x, _vehicle->freight[1].pos.y);
		
		glVertex2f(_vehicle->wheel[0].pos.x, _vehicle->wheel[0].pos.y);
		glVertex2f(_vehicle->wheel[1].pos.x, _vehicle->wheel[1].pos.y);
		
		glVertex2f(_vehicle->freight[0].pos.x, _vehicle->freight[0].pos.y);
		glVertex2f(_vehicle->wheel[0].pos.x, _vehicle->wheel[0].pos.y);
		
		glVertex2f(_vehicle->freight[0].pos.x, _vehicle->freight[0].pos.y);
		glVertex2f(_vehicle->wheel[1].pos.x, _vehicle->wheel[1].pos.y);
	
		glVertex2f(_vehicle->freight[1].pos.x, _vehicle->freight[1].pos.y);
		glVertex2f(_vehicle->wheel[0].pos.x, _vehicle->wheel[0].pos.y);
		
		glVertex2f(_vehicle->freight[1].pos.x, _vehicle->freight[1].pos.y);
		glVertex2f(_vehicle->wheel[1].pos.x, _vehicle->wheel[1].pos.y);
	glEnd();
	
	//	Draw wheels and masses
	for (int i = 0; i < 2; i++)
	{	
		//	Wheels
		if (_vehicle->hasPhysics)
		{
			_vehicle->wheel[i].pos.x = _vehicle->wheel[i]._body->p.x;
			_vehicle->wheel[i].pos.y = _vehicle->wheel[i]._body->p.y;
			
			drawWheel(
					  _vehicle->wheel[i].pos.x,
					  _vehicle->wheel[i].pos.y,
					  WHEEL_SIZE_MIN + _vehicle->wheel[i].radius * (WHEEL_SIZE_MAX - WHEEL_SIZE_MIN) / GRAY_CODE_HIGHEST,
					  radianToDegree(cpBodyGetAngle(_vehicle->wheel[i]._body)),
					  lineWidth
					  );
		}
		else
		{
			drawWheel(
					  _vehicle->wheel[i].pos.x,
					  _vehicle->wheel[i].pos.y,
					  WHEEL_SIZE_MIN + _vehicle->wheel[i].radius * (WHEEL_SIZE_MAX - WHEEL_SIZE_MIN) / GRAY_CODE_HIGHEST,
					  0.0f,
					  lineWidth
					  );
		}

		
		//	Masses
		if (_vehicle->hasPhysics)
		{
			_vehicle->freight[i].pos.x = _vehicle->freight[i]._body->p.x;
			_vehicle->freight[i].pos.y = _vehicle->freight[i]._body->p.y;
			
			drawMass(_vehicle->freight[i].pos.x,
					 _vehicle->freight[i].pos.y,
					 MASS_SIZE_MIN + _vehicle->freight[i].radius * (MASS_SIZE_MAX - MASS_SIZE_MIN) / GRAY_CODE_HIGHEST,
					 radianToDegree(cpBodyGetAngle(_vehicle->freight[i]._body)),
					 lineWidth
					 );	
		}
		else
		{
			drawMass(_vehicle->freight[i].pos.x,
					 _vehicle->freight[i].pos.y,
					 MASS_SIZE_MIN + _vehicle->freight[i].radius * (MASS_SIZE_MAX - MASS_SIZE_MIN) / GRAY_CODE_HIGHEST,
					 0,
					 lineWidth
					 );
		}

	}
}


void drawWheel(cpFloat x, cpFloat y, cpFloat radius, cpFloat angle, cpFloat lineWidth)
{		
	if (lineMode)
	{
		//	Draws Wheel as lines
		
		//	Translate to position
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		
		glLineWidth(lineWidth);
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);
		
		drawCircle(radius, 100);
		
		glEnd();
		glPopMatrix();
		
		//	Draw lines
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		
		for (int i = 0; i < 2; i++)
		{
			glRotatef(90.0, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex2f(radius * 0.6, 0.0f);
			glVertex2f(radius, 0.0f);
			
			glVertex2f(-radius * 0.6, 0.0f);
			glVertex2f(-radius, 0.0f);
			glEnd();
		}
		
		glPopMatrix();
	}
	else
	{		
		//	Translate to position
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		
		//	Shows Wheel Texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, WHEEL_TEXTURE_ID);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(- radius, -radius);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(radius, - radius);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(radius, radius);
		
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-radius, radius);
		glEnd();
		
		glDisable(GL_TEXTURE_2D);
		
		glPopMatrix();
	}
}


void drawMass(cpFloat x, cpFloat y, cpFloat radius, cpFloat angle, cpFloat lineWidth)
{
	if (lineMode)
	{
		glLineWidth(lineWidth);
		glColor3f(1.0f, 0.0f, 0.0f);
		
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_LINE_STRIP);
			drawCircle(radius, 360);
			glEnd();
			
			//	drawing will repeated with a
			//	new scale factor for inner circle
			glScalef(0.5f, 0.5f, 1.0f);
		}
		
		glPopMatrix();
	}
	else
	{
		//	Translate to position
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		
		//	Shows Wheel Texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, MASS_TEXTURE_ID);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(- radius, -radius);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(radius, - radius);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(radius, radius);
		
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-radius, radius);
		glEnd();
		
		glDisable(GL_TEXTURE_2D);
		
		glPopMatrix();
	}
}