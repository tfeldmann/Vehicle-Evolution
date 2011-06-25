/*
 *  main.c
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 01.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GLUT/glut.h>
#include "SOIL.h"
#include "chipmunk.h"
#include "chipmunk_unsafe.h"

#include "vehicle.h"
#include "genetics.h"
#include "terrain.h"
#include "parents.h"
#include "statistics.h"
#include "camera.h"
#include "functions.h"
#include "keys.h"
#include "definitions.h"

//	Objects
Vehicle		vehicle[TEST_GROUP_SIZE];
Terrain		terrain[TERRAIN_SEGMENTS];
cpSpace*	_space;

int			individual			= 0;
int			generation			= 0;
int			steps				= 0;

//	Counter
char		slowMotionSteps		= 0;

//	Terrain editing mode
char		mouseIsDragging		=  FALSE;
int			selectedTerrainNode = -1;

//	Flags and toggles
char		nextVehicleFlag		= FALSE;
char		genTextIsShown		= TRUE;
char		parentsAreShown		= TRUE;
char		statisticsAreShown	= TRUE;

char		editMode			= FALSE;
char		lineMode			= FALSE;
char		slowMotion			= FALSE;
char		fullscreenMode		= FALSE;

//	GUI Text
char generationMessage[] = "Random vehicles";
char individualMessage[20];

/*  FUNCTIONS	*/

void mouseClick(int button, int state, int x, int y);
void mouseMove(int x, int y);
void mouseDrag(int x, int y);
void keyPressDown(unsigned char key, int x, int y);
void keyPressUp(unsigned char key, int x, int y);
void specialKeysDown(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void resizeWindow(int width, int height);
void update(int value);
void testVehicle(int value);
void drawScene();
void drawGenText();

//	Register Chipmunk collision callback functions
cpBool massAndGroundTouch(cpArbiter* _arb, cpSpace* _space, void* _unused);
cpBool wheelAndGroundTouch(cpArbiter* _arb, cpSpace* _space, void* _unused);
void wheelAndGroundSeparate(cpArbiter* _arb, cpSpace* _space, void* _unused);


/*  IMPLEMENTAION	*/

int main(int argc, char** argv)
{
	//srand(20000);
	srand(time(NULL));
	
	//	Init GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	
	glutInitWindowPosition(INIT_SCREEN_LEFT, INIT_SCREEN_RIGHT);
	glutInitWindowSize(INIT_SCREEN_WIDTH, INIT_SCREEN_HEIGHT);
	
	glutCreateWindow("Vehicle Evolution");
	gluOrtho2D(0, INIT_SCREEN_WIDTH, 0, INIT_SCREEN_HEIGHT);
	
	//	Register GLUT callback functions
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMove);
	glutMotionFunc(mouseDrag);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyPressDown);
	glutKeyboardUpFunc(keyPressUp);
	glutSpecialFunc(specialKeysDown);
	glutSpecialUpFunc(specialKeysUp);
	glutReshapeFunc(resizeWindow);

	//	Some OpenGl settings
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	//	Init Chipmunk
	cpInitChipmunk();
	_space = cpSpaceNew();
	_space->gravity = cpv(0, -9.81);
		
	//	Init Camera
	cameraReset();
	
	//	Init Statistics
	initStatistics();
	
	//	Init Terrain
	randomlyInitTerrain(_space, terrain);

	//	Init Vehicles
	for (int i = 0; i < TEST_GROUP_SIZE; i++)
	{
		randomlyInitVehicle(&vehicle[i]);
		moveVehicleToStartpoint(&vehicle[i]);
		initVehiclePhysics(_space, &vehicle[i]);
		
		// put all bodies into sleep
		// so they have no physical representation
		if (i != 0)
		{
			for (int j = 0; j < 2; j++)
			{
				cpBodySleep(vehicle[i].freight[j]._body);
				cpBodySleep(vehicle[i].wheel[j]._body);
			}
		}
	}
	
	//	write "Individual: ..."
	sprintf(individualMessage, "Individual: %d / %d", 1, TEST_GROUP_SIZE);
	
	//	Chipmunk collision Handlers are given pointers to functions that
	//	are called when the defined collision types touch
	cpSpaceAddCollisionHandler(_space,
							   GROUND_COLLISION_TYPE,
							   MASS_COLLISION_TYPE,
							   massAndGroundTouch,
							   NULL,
							   NULL,
							   NULL,
							   NULL);
	cpSpaceAddCollisionHandler(_space,
							   WHEEL_COLLISION_TYPE,
							   GROUND_COLLISION_TYPE,
							   wheelAndGroundTouch,
							   NULL,
							   NULL,
							   wheelAndGroundSeparate,
							   NULL);
	
	//	Loads the textures (with a little help of Objective-C)
	//	by accessing the application bundle in Mac OS X
	NSString *_wheelTextureLocation = [[NSBundle mainBundle] pathForResource:@"wheel" ofType:@"png"];
	NSString *_massTextureLocation  = [[NSBundle mainBundle] pathForResource:@"freight" ofType:@"png"];
	
	SOIL_load_OGL_texture([_wheelTextureLocation UTF8String], 0, WHEEL_TEXTURE_ID, SOIL_FLAG_POWER_OF_TWO);
	SOIL_load_OGL_texture([_massTextureLocation UTF8String], 0, MASS_TEXTURE_ID, SOIL_FLAG_POWER_OF_TWO);
	
	[_wheelTextureLocation release];
	[_massTextureLocation  release];
	
	//	Start the GLUT Timer
	glutTimerFunc(500, update, 0);
	glutMainLoop();
		
	return 0;
}


cpBool massAndGroundTouch(cpArbiter* _arb, cpSpace* _space, void* _unused)
{
	nextVehicleFlag = TRUE;	
	return cpFalse;
}

cpBool wheelAndGroundTouch(cpArbiter* _arb, cpSpace* _space, void* _unused)
{
	//	enable wheel motor
	CP_ARBITER_GET_BODIES(_arb, _wheel, _ground);
	cpBodySetTorque(_wheel, - MOTOR_STRENGTH);
	cpBodySetAngVel(_wheel, - 5.0f);
	return cpTrue;
}

void wheelAndGroundSeparate(cpArbiter* _arb, cpSpace* _space, void* _unused)
{
	//	disable wheel motor
	CP_ARBITER_GET_BODIES(_arb, _wheel, _ground);
	cpBodySetTorque(_wheel, 0.0f);
	cpBodySetAngVel(_wheel, -2.0f);
}


void update(int value)
{		
	if (!editMode)
	{
		updateVehicle(&vehicle[individual]);
		
		//	Vehicle reaches flag
		if (vehicle[individual].pos.x >= terrain[FLAG_POSITION].p[1].x)
		{
			vehicle[individual].fitness += 2.0;	//	Increase fitness when vehicles reaches the red flag
			nextVehicleFlag = TRUE;
		}
		
		//	Test the next vehicle when nextVehicle flag is risen
		if (nextVehicleFlag)
		{			
			if (individual < TEST_GROUP_SIZE - 1)
			{			
				/*
				 *	The Fitness-Function
				 *	3 * Speed + Distance
				 *
				 */
				vehicle[individual].fitness +=  3 * (vehicle[individual].pos.x - STARTING_OFFSET_X) / steps
												  + (vehicle[individual].pos.x - STARTING_OFFSET_X) / 500.0;
							
				testVehicle(individual + 1);
			}
			else
			{
				newGeneticGeneration(_space, vehicle);
				generation ++;
				testVehicle(0);

				sprintf(generationMessage, "Generation: %d", generation);
			}
			
			//	Buffer the currently tested vehicle's parents
			//	so that we can show them in the draw function
			if (generation > 0)
			{
				bufferParentsOfVehicleForDisplay(&vehicle[individual]);
			}
			
			nextVehicleFlag = FALSE;
		}

		//	Every nth slow-motion step (n = SLOW_DOWN_FACTOR) the regular step
		//	is increased so that slow motion doesn't affect the vehicle's fitness
		if (slowMotion)
		{
			cpSpaceStep(_space, FRAMERATE / SLOW_DOWN_FACTOR);
			
			slowMotionSteps++;
			if (slowMotionSteps >= SLOW_DOWN_FACTOR)
			{
				slowMotionSteps = 0;
				steps++;
			}
			
			//cpSpaceHashEach(_space->activeShapes, &eachShape, nil);
			//cpSpaceHashEach(_space->staticShapes, &eachShape, nil);
		}
		else
		{
			steps++;
			cpSpaceStep(_space, FRAMERATE);
		}
	}
	
	cameraMove();	//	Gets its direction from ArrowKeys.h
	
	glutPostRedisplay();
	glutTimerFunc(FRAMERATE * 100, update, 0);
}


void testVehicle(int value)
{
	//	Put former vehicle to sleep
	for (int i = 0; i < 2; i++)
	{
		cpBodySleep(vehicle[individual].freight[i]._body);
		cpBodySleep(vehicle[individual].wheel[i]._body);
	}
	
	//	Activate currently tested vehicle
	individual = value;
	vehicle[individual].fitness = 0;
	steps = 0;
	for (int i = 0; i < 2; i++)
	{
		cpBodyActivate(vehicle[individual].freight[i]._body);
		cpBodyActivate(vehicle[individual].wheel[i]._body);
	}
	
	//	Write current individual number in "Vehicle: ..." Text
	sprintf(individualMessage, "Individual: %d / %d", individual + 1, TEST_GROUP_SIZE);
}


void drawScene()
{		
	if (!lineMode)
	{
		glBegin(GL_QUADS);
		glColor3f(0.9, 0.9, 1.0f);
		glVertex2f(0, 0);
		
		glColor3f(0.9, 0.9, 1.0);
		glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
		
		glColor3f(1, 1, 1);
		glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		
		glColor3f(0.8, 0.8, 1.0);
		glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));
		glEnd();
	}
	else
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	
	//	Translate to camera view
	glPushMatrix();
	
	cameraUpdate(&vehicle[individual]);
	drawTerrain(terrain);
	drawVehicle(&vehicle[individual], getCameraZoomFactor() * 1.7);
	
	if (editMode) drawTerrainEditNodes(terrain, selectedTerrainNode);
	
	glPopMatrix();
	
	//	draws the GUI
	if (genTextIsShown)							drawGenText();
	if (generation > 0 && statisticsAreShown)	drawStatistics();
	if (generation > 0 && parentsAreShown)		drawParents(&vehicle[individual]);
	
	//	Swapping buffers to show drawn stuff on screen
	glutSwapBuffers();
}


void drawGenText()
{
	//	Draw generation-counter
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2f(10, glutGet(GLUT_WINDOW_HEIGHT) - 20);

	for (int i = 0; i < strlen(generationMessage); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, generationMessage[i]);
	}
	
	//	Draw individual-counter
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2f(10, glutGet(GLUT_WINDOW_HEIGHT) - 40);

	for (int i = 0; i < strlen(individualMessage); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, individualMessage[i]);
	}
}


void mouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (!editMode) return;
		
		for (int i = 1; i < TERRAIN_SEGMENTS - 1; i++)
		{
			if (cpvdist(windowToWorld(cpv(x, y)), terrain[i].p[1]) < 10)
			{
				selectedTerrainNode = i;
				mouseIsDragging = TRUE;
				break;
			}
		}
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (!mouseIsDragging) return;
		mouseIsDragging = FALSE;
		/*
		 *
		 *	Unsafe but official way of doing it. I did not yet understand why this doesn't work.
		 *
		cpSegmentShapeSetEndpoints( terrain.part[selectedTerrainNode]._shape,
									terrain.part[selectedTerrainNode].p[0],
									terrain.part[selectedTerrainNode].p[1]);
		cpSegmentShapeSetEndpoints( terrain.part[selectedTerrainNode + 1]._shape,
									terrain.part[selectedTerrainNode + 1].p[0],
									terrain.part[selectedTerrainNode + 1].p[1]);
		 */

		//	Remove former physics of terrain-segments
		cpSpaceRemoveStaticShape(_space, terrain[selectedTerrainNode]._shape);
		cpShapeFree(terrain[selectedTerrainNode]._shape);
		cpSpaceRemoveStaticShape(_space, terrain[selectedTerrainNode + 1]._shape);
		cpShapeFree(terrain[selectedTerrainNode + 1]._shape);
		
		//	Make a new ones
		for (int i = 0; i < 2; i++)
		{
			terrain[selectedTerrainNode + i]._shape = cpSegmentShapeNew(
														 &_space->staticBody,
														 terrain[selectedTerrainNode + i].p[0],
														 terrain[selectedTerrainNode + i].p[1],
														 0.0f);
			terrain[selectedTerrainNode + i]._shape->e = 1.0f;
			terrain[selectedTerrainNode + i]._shape->u = 0.8f;
			terrain[selectedTerrainNode + i]._shape->collision_type = GROUND_COLLISION_TYPE;
			cpSpaceAddShape(_space, terrain[selectedTerrainNode + i]._shape);
		}
		
		/*	Hint:
		 *	Deselecting the terrain node is no longer needed, because it becomes deselected when
		 *	the mouse leaves the dragging circle.
		 */
	}
}


void mouseMove(int x, int y)
{
	if (!editMode) return;
	
	//	Select the terrain node the mouse is currently over
	selectedTerrainNode = -1;
	for (int i = 1; i < TERRAIN_SEGMENTS - 1; i++)
	{
		if (cpvdist(windowToWorld(cpv(x, y)), terrain[i].p[1]) < 10)
		{
			selectedTerrainNode = i;
			break;
		}
	}
}


void mouseDrag(int x, int y)
{
	if (!mouseIsDragging) return;
	
	/*
	 *	Only the second point of each terrain segment is draggable,
	 *	make sure to update the first point of the following segment, too.
	 */
	
	//	Refine graphical representation of terrain
	terrain[selectedTerrainNode].p[1]		= windowToWorld(cpv(x, y));
	terrain[selectedTerrainNode + 1].p[0]	= windowToWorld(cpv(x, y));
	
	//	don't let the endpoint overlap the startpoint of the next terrainpart (drag left)
	if (terrain[selectedTerrainNode].p[1].x < terrain[selectedTerrainNode].p[0].x)
	{
		terrain[selectedTerrainNode].p[1].x = terrain[selectedTerrainNode].p[0].x;
		terrain[selectedTerrainNode + 1].p[0].x = terrain[selectedTerrainNode].p[0].x;
	}
	//	and the same for the right
	if (terrain[selectedTerrainNode].p[1].x > terrain[selectedTerrainNode + 1].p[1].x)
	{
		terrain[selectedTerrainNode].p[1].x = terrain[selectedTerrainNode + 1].p[1].x;
		terrain[selectedTerrainNode + 1].p[0].x = terrain[selectedTerrainNode].p[1].x;
	}

	//	Everytime the mouse moves the window has to be redrawn
	glutPostRedisplay();
}


void keyPressDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		/*
		 *	Camera handling
		 */
		case '+':
			//cameraZoom( 0.1f);
			keys.plus = TRUE;
			break;
			
		case '-':
			//cameraZoom(-0.1f);
			keys.minus = TRUE;
			break;
			
		case '0':
			cameraReset();
			break;
			
		/*
		 *	Application features
		 */
		case 'k':
			nextVehicleFlag = TRUE && !editMode;
			break;
			
		case 'e':
			if (!mouseIsDragging)
			{
				editMode = !editMode;
				if (!editMode)
				{
					cpSpaceRehashStatic(_space);
					selectedTerrainNode = -1;
				}
			}
			break;
			
		case 's':
			slowMotion = !slowMotion && !editMode;
			break;
			
		case 'l':
			lineMode = !lineMode;
			break;
			
		/*
		 *	GUI
		 */
		case '1':
			genTextIsShown = !genTextIsShown;
			break;
			
		case '2':
			parentsAreShown = !parentsAreShown;
			break;
			
		case '3':
			statisticsAreShown = !statisticsAreShown;
			break;
			
		case 'f':
			fullscreenMode = !fullscreenMode;
			
			if (fullscreenMode)
			{
				glutFullScreen();
			}
			else
			{
				glutReshapeWindow(INIT_SCREEN_WIDTH, INIT_SCREEN_HEIGHT);
				glutPositionWindow(INIT_SCREEN_LEFT, INIT_SCREEN_RIGHT);
			}
			
			break;
			
		//	Escape key
		case 27 : exit(0);
	}
}

void keyPressUp(unsigned char key, int x, int y)
{
	switch (key)
	{
		case '+':
			keys.plus = FALSE;
			break;
			
		case '-':
			keys.minus = FALSE;
			break;
	}
}


void specialKeysDown(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			keys.left = TRUE;
			break;
		case GLUT_KEY_RIGHT:
			keys.right = TRUE;
			break;
		case GLUT_KEY_UP:
			keys.up = TRUE;
			break;
		case GLUT_KEY_DOWN:
			keys.down = TRUE;
			break;
	}
}

void specialKeysUp(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			keys.left = FALSE;
			break;
		case GLUT_KEY_RIGHT:
			keys.right = FALSE;
			break;
		case GLUT_KEY_UP:
			keys.up = FALSE;
			break;
		case GLUT_KEY_DOWN:
			keys.down = FALSE;
			break;
	}
}


void resizeWindow(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}