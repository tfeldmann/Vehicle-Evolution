/*
 *  definitions.h
 *  vehicle evolution
 *
 *  Created by Thomas Feldmann on 07.12.10.
 *  Copyright 2010 FH Bocholt. All rights reserved.
 *
 */

//	General
#define TRUE						1
#define FALSE						0
#define PI							3.141592654

//	GLUT
#define FRAMERATE					(1.0f / 40.0f)
#define INIT_SCREEN_WIDTH			1000
#define INIT_SCREEN_HEIGHT			500
#define INIT_SCREEN_LEFT			100
#define INIT_SCREEN_RIGHT			100
#define SLOW_DOWN_FACTOR			6

//	Genetics
#define TEST_GROUP_SIZE				14
#define GRAY_CODE_BIT				8
#define GRAY_CODE_HIGHEST			255		//for automatic adjustment consider using: (int)(powf(2, GRAY_CODE_BIT) - 1)
#define DNA_PARTS					12
#define DNA_LENGTH					(DNA_PARTS * GRAY_CODE_BIT)

#define MUTATIONS_MIN				0
#define MUTATIONS_MAX				8

//	Camera
#define CAMERA_OFFSET_X				- 260.0f
#define CAMERA_OFFSET_Y				- 100.0f

//	Chipmunk
#define VEHICLE_COLLISION_GROUP		1
#define WHEEL_COLLISION_TYPE		1
#define MASS_COLLISION_TYPE			2
#define GROUND_COLLISION_TYPE		3

//	Terrain
#define TERRAIN_FLAT_LENGTH			1000.0
#define TERRAIN_SEGMENTS			55
#define FLAG_POSITION				50

//	Vehicle
#define STIFF_CONNECTIONS			FALSE

#define STARTING_OFFSET_Y			200.0f
#define STARTING_OFFSET_X			300.0f

#define SPRING_SITFFNESS			4.0f
#define SPRING_DAMPING				0.15f
#define MOTOR_STRENGTH				10.0f

#define WHEEL_SIZE_MIN				10
#define WHEEL_SIZE_MAX				30
#define MASS_SIZE_MIN				5
#define MASS_SIZE_MAX				30

#define MASS_TO_RADIUS_RELATION		(1.0f / 20.0f)
#define WHEEL_TO_RADIUS_RELATION	(1.0f / 30.0f)

#define	WHEEL_TEXTURE_ID			1
#define MASS_TEXTURE_ID				2

//	Parents
#define PARENTS_SCALE				0.25f
#define PARENTS_WIDTH				200.0f
#define PARENTS_HEIGHT				150.0f
#define PARENTS_PADDING				10
#define PARENTS_BORDER				10

//	Statistics
#define STAT_ENTRIES_MAX			20
#define STAT_POSITION_X				730.0f
#define STAT_POSITION_Y				330.0f
#define STAT_WIDTH					250.0f
#define STAT_HEIGHT					150.0f
#define STAT_PADDING				10
#define STAT_BORDER					10
