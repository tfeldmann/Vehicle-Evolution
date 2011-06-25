/*
 *  camera.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 02.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include "camera.h"

void cameraReset()
{
	zoomFactor = 0.8f;
	offset.x = CAMERA_OFFSET_X;
	offset.y = CAMERA_OFFSET_Y;
}

void cameraUpdate(Vehicle* _vehicle)
{
	pos = cpv(offset.x + _vehicle->pos.x, offset.y);
	
	glScalef(zoomFactor, zoomFactor, 0.0f);
	glTranslatef(-pos.x, -pos.y, 0.0f);
}


cpFloat getCameraZoomFactor()
{
	return zoomFactor;
}


cpVect  getCameraPosition()
{
	return pos;
}


void cameraMove()
{
	offset.x += (keys.right - keys.left) * 5 * getCameraZoomFactor();
	offset.y += (keys.up    - keys.down) * 5 * getCameraZoomFactor();
	
	zoomFactor += (keys.plus - keys.minus) / 1000.0;
	
	if (zoomFactor < 0.1f)
	{
		zoomFactor = 0.1f;
	}
}

//	Coordinate Transformations
cpVect windowToWorld(cpVect windowPoint)
{
	return cpv(getCameraPosition().x + windowPoint.x / getCameraZoomFactor(),
			   getCameraPosition().y + (glutGet(GLUT_WINDOW_HEIGHT) - windowPoint.y) / getCameraZoomFactor());
}
