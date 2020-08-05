////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "utils.h"

#define PROBABILITY_SEA_MOVE 50
#define PROBABILITY_LAND_MOVE 80
#define HIDE_DISTANCE 1

////////////////////////////////////////////////////////////////////////

static void randStartLocation(DraculaView dv);
static void makeMove(DraculaView dv);
static void makeMoveLand(DraculaView dv);

static int randGen(int max);
static void registerPlayWithPlaceId(PlaceId move);
static int indexMax(int *array, int size);
static int indexMaxLand(int *array, int size);
//static bool probability(int chance);
static PlaceId LocationToMove(DraculaView dv, PlaceId *validMoves,
                              int numMoves, PlaceId location);


////////////////////////////////////////////////////////////////////////

void decideDraculaMove(DraculaView dv)
{
	// TODO: Replace this with something better!
   if (DvGetRound(dv) == 0) {
      randStartLocation(dv);
   } else {
   	if (DvGetRound(dv) % 13 == 0) {
      	makeMoveLand(dv);
      } else {
      	makeMove(dv);
      }
   }
}

////////////////////////////////////////////////////////////////////////

// starts at a land location that is furthest away from any hunters
static void randStartLocation(DraculaView dv)
{
   int dist[NUM_REAL_PLACES];
   for (int i = 0; i < NUM_REAL_PLACES; i++) {
      if (placeIsLand(i) && i != HOSPITAL_PLACE) {
         dist[i] = closestHunter(dv, i);
      } else {
         dist[i] = 0;
      }
   }
   PlaceId start = indexMax(dist, NUM_REAL_PLACES);
   registerPlayWithPlaceId(start);
}


// generates a move to keep dracula away from the hunters
static void makeMove(DraculaView dv)
{
   int numReturnedMoves = -1;
   PlaceId *validMoves = DvGetValidMoves(dv, &numReturnedMoves);
   int numReturnedLocs = -1;
   PlaceId *locations = DvWhereCanIGo(dv, &numReturnedLocs);
   int dist[numReturnedLocs];
   
   if (numReturnedMoves > 0) {
      // generating distances from hunters
      for (int i = 0; i < numReturnedLocs; i++) {
         dist[i] = closestHunter(dv, locations[i]);
      }
      int index;
      index = indexMax(dist, numReturnedLocs);
      // match location with move
      PlaceId move = LocationToMove(dv, validMoves, numReturnedMoves, locations[index]);
      registerPlayWithPlaceId(move);
   } else {
      registerPlayWithPlaceId(CASTLE_DRACULA);
   }
   free(validMoves);
   free(locations);
}

static void makeMoveLand(DraculaView dv)
{
   int numReturnedMoves = -1;
   PlaceId *validMoves = DvGetValidMoves(dv, &numReturnedMoves);
   int numReturnedLocs = -1;
   PlaceId *locations = DvWhereCanIGo(dv, &numReturnedLocs);
   int dist[numReturnedLocs];
   
   if (numReturnedMoves > 0) {
      // generating distances from hunters
      for (int i = 0; i < numReturnedLocs; i++) {
         dist[i] = closestHunter(dv, locations[i]);
      }
      int index;
      index = indexMaxLand(dist, numReturnedLocs);
      // match location with move
      PlaceId move = LocationToMove(dv, validMoves, numReturnedMoves, locations[index]);
      registerPlayWithPlaceId(move);
   } else {
      registerPlayWithPlaceId(CASTLE_DRACULA);
   }
   free(validMoves);
   free(locations);
}




////////////////////////////////////////////////////////////////////////
// Utility Functions

// generates a random integer that's smaller than the input integer
static int randGen(int max)
{
    srand(time(0));
    return rand() % max;
}

// calls registerBestPlay but uses a PlaceId
static void registerPlayWithPlaceId(PlaceId move)
{
   const char *location = placeIdToAbbrev(move);
   char *play = strdup(location);
   registerBestPlay(play, "");
}

// returns the index of the max value in array
static int indexMax(int *array, int size)
{
   int index = 0;
   for (int i = 1; i < size; i++) {
      if (array[i] > array[index]) {
         index = i;
      }
   }
   return index;
}

static int indexMaxLand(int *array, int size)
{
   int index = 0;
   for (int i = 1; i < size; i++) {
      if (array[i] > array[index] && placeIsLand(array[i])) {
         index = i;
      }
   }
   return index;
}

/*
// calcualtes probability
static bool probability(int chance)
{
    srand(time(0));
    return ((rand() % 100) < chance ? true : false);
}
*/

// matches the location with the move dracula should make
static PlaceId LocationToMove(DraculaView dv, PlaceId *validMoves,
                              int numValidMoves, PlaceId location)
{
   // location is outside trail
   if(placesContains(validMoves, numValidMoves, location)) {
      return location;
   }
   PlaceId *trail = draculaTrail(dv);
   // determine double back move
   for (int i = 0; i < numValidMoves; i++) {
      switch(validMoves[i]) {
         case DOUBLE_BACK_1:
            if (trail[0] == location) {
               return DOUBLE_BACK_1;
            }
         case DOUBLE_BACK_2:
            if (trail[1] == location) {
               return DOUBLE_BACK_2;
            }
         case DOUBLE_BACK_3:
            if (trail[2] == location) {
               return DOUBLE_BACK_3;
            }         
         case DOUBLE_BACK_4:
            if (trail[3] == location) {
               return DOUBLE_BACK_4;
            }         
         case DOUBLE_BACK_5:
            if (trail[4] == location) {
               return DOUBLE_BACK_5;
            }         
         default:
            continue;
      }
   }
   return validMoves[randGen(numValidMoves)];
}
