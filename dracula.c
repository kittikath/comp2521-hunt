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

// james' special numbers, don't touch
#define MAKE_SPECIAL_MOVE 1
#define MAKE_SEA_MOVE 1
#define MAKE_LAND_MOVE 2
#define MAKE_HIDE_MOVE 1
#define HUNTER_NEAR_PORT 1 // LEAVE THIS AS 1!
#define SPECIAL_SEA_MOVE 2


////////////////////////////////////////////////////////////////////////

static void registerStartLocation(DraculaView dv);
static bool forbiddenStartPlace(PlaceId place);
static bool forbiddenFromSea(PlaceId place);
static bool forbiddenPlaces(PlaceId place);

static void registerPlayWithPlaceId(PlaceId move);
static int randGen(int max);
static int indexMax(int *array, int size);
static int indexMaxLand(int *distances, PlaceId* locations, int size);
static int indexMaxSea(int *distances, PlaceId* locations, int size);
//static bool probability(int chance);
static PlaceId locationToMove(DraculaView dv, PlaceId *validMoves,
                              int numMoves, PlaceId location);


////////////////////////////////////////////////////////////////////////

void decideDraculaMove(DraculaView dv)
{
	// TODO: Replace this with something better!
	
	int currentRound = DvGetRound(dv);
   int currentBlood = DvGetHealth(dv, PLAYER_DRACULA);
	PlaceId currentLocation = DvGetPlayerLocation(dv, PLAYER_DRACULA);	
	
	// start round
   if (currentRound == 0) {
      registerStartLocation(dv);
      return;
   }
   
   int numReturnedMoves = -1;
   PlaceId *validMoves = DvGetValidMoves(dv, &numReturnedMoves);
   int numReturnedLocs = -1;
   PlaceId *locations = DvWhereCanIGo(dv, &numReturnedLocs);
   
   if (numReturnedMoves == 0) {
      registerPlayWithPlaceId(CASTLE_DRACULA);
      free(validMoves);
      free(locations);
      return;
   }
   
   // register a random move incase of time
   registerPlayWithPlaceId(validMoves[(currentBlood + currentRound) %
                                                             numReturnedMoves]);
                                                                                      
   int distancesNormal[numReturnedMoves];
   int distancesSpecial[numReturnedLocs];
   bool landAvailable = false;
   bool seaAvailable = false;
   int indexSea;                                                          
   
   // generate distances from hunters
   for (int i = 0; i < numReturnedMoves; i++) {
      if (placeIsLand(validMoves[i]) && !forbiddenPlaces(validMoves[i])) {         
         distancesNormal[i] = closestHunter(dv, validMoves[i]);
         landAvailable = true;
      } else if (placeIsSea(validMoves[i])) {
         distancesNormal[i] = -1 * closestHunter(dv, validMoves[i]);
         seaAvailable = true;
         indexSea = i;
      } else {
         distancesNormal[i] = 0;
      }
   }

   // index of the best normal land move
   int indexLand = indexMaxLand(distancesNormal, validMoves, numReturnedMoves);
   
   /*
   if (landAvailable) {
      printf("Land IS Available\n");
      printf("NUMBER: %d, LOCATION: %s\n", distancesNormal[indexLand], placeIdToAbbrev(validMoves[indexLand]));
   } */  
   
   // dracula at land
   if (placeIsLand(currentLocation)) {
      // making a normal move
      if (landAvailable) {
         registerPlayWithPlaceId(validMoves[indexLand]);
      }
      // extra consideration to make a special move if required
      if (!landAvailable || distancesNormal[indexLand] <= MAKE_SPECIAL_MOVE) {                    
         // generating distances from hunters
         for (int i = 0; i < numReturnedLocs; i++) {
            if (placeIsLand(locations[i]) && !forbiddenPlaces(locations[i])) {
               distancesSpecial[i] = closestHunter(dv, locations[i]);
               landAvailable = true;
            // sea is a very special place
            } else if (placeIsSea(locations[i])) {
               distancesSpecial[i] = 0;
               seaAvailable = true;
               indexSea = i;
            } else {
               distancesSpecial[i] = 0;
            }
         }
         // index of best special land move
         indexLand = indexMaxLand(distancesSpecial, locations, numReturnedLocs);
         
         // make sea move if available and needed
         if (seaAvailable && distancesSpecial[indexLand] <= MAKE_SEA_MOVE) {
            PlaceId moveSea = locationToMove(dv, validMoves, numReturnedMoves,
                                                           locations[indexSea]);
            registerPlayWithPlaceId(moveSea);
         }
         // making special land move
         if (landAvailable && distancesSpecial[indexLand] >= MAKE_LAND_MOVE) {      
            PlaceId moveLand = locationToMove(dv, validMoves, numReturnedMoves,
                                                          locations[indexLand]);
            registerPlayWithPlaceId(moveLand);
         }
      }   
   }
   
   // dracula at sea
   if (placeIsSea(currentLocation)) {
      // make move to land
      if (landAvailable && distancesNormal[indexLand] > HUNTER_NEAR_PORT) {
         registerPlayWithPlaceId(validMoves[indexLand]);
      // otherwise, make another normal sea move
      } else if (seaAvailable) {
         registerPlayWithPlaceId(validMoves[indexSea]);
      // otherwise, consider to make a special move in the sea
      } else {
         // generating distances from hunters
         for (int i = 0; i < numReturnedLocs; i++) {
            if (placeIsLand(locations[i]) && !forbiddenFromSea(locations[i])) {
               distancesSpecial[i] = closestHunter(dv, locations[i]);
               landAvailable = true;
            // sea is a very special place
            } else if (placeIsSea(locations[i])) {
               distancesSpecial[i] = -1 * closestHunter(dv, locations[i]);
               seaAvailable = true;
            } else {
               distancesSpecial[i] = 0;
            }
         }
         indexLand = indexMaxLand(distancesSpecial, locations, numReturnedLocs);
         indexSea = indexMaxSea(distancesSpecial, locations, numReturnedLocs);
         // make special move to land
         if (landAvailable && distancesSpecial[indexLand] > HUNTER_NEAR_PORT) {      
            PlaceId moveLand = locationToMove(dv, validMoves, numReturnedMoves,
                                                          locations[indexLand]);
            registerPlayWithPlaceId(moveLand);
         // if no good land moves available, stay at sea
         } else if (seaAvailable) {
            PlaceId moveSea = locationToMove(dv, validMoves, numReturnedMoves,
                                                           locations[indexSea]);
            registerPlayWithPlaceId(moveSea);                                                
         }
      }
   }
   free(validMoves);
   free(locations);
   return;
}

////////////////////////////////////////////////////////////////////////

// starts at a land location that is furthest away from any hunters
static void registerStartLocation(DraculaView dv)
{
   int dist[NUM_REAL_PLACES];
   for (int i = 0; i < NUM_REAL_PLACES; i++) {
      if (placeIsLand(i) && !forbiddenStartPlace(i)) {
         dist[i] = closestHunter(dv, i);
      } else {
         dist[i] = -1;
      }
   }
   PlaceId start = indexMax(dist, NUM_REAL_PLACES);
   registerPlayWithPlaceId(start);
}

// list of forbidden start places
static bool forbiddenStartPlace(PlaceId place)
{
   if (place == HOSPITAL_PLACE || place == CAGLIARI || place == DUBLIN ||
       place == GALWAY) {
      return true;   
   }
   return false;
}

// list of forbidden places from the sea
static bool forbiddenFromSea(PlaceId place)
{
   if (place == CAGLIARI || place == DUBLIN || place == GALWAY) {
      return true;   
   }
   return false;
}

// list of locations Dracula shouldn't visit on land
static bool forbiddenPlaces(PlaceId place)
{
   if (place == CASTLE_DRACULA) {
      return true;
   }
   return false;
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
   char *play = placeIdToAbbrev(move);
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

// returns the land location furthest away from any hunter
static int indexMaxLand(int *distances, PlaceId* locations, int size)
{
   int index = 0;
   for (int i = 1; i < size; i++) {
      if (placeIsLand(locations[i]) && distances[i] > distances[index]) {
         index = i;
      }
   }
   return index;
}

// returns the sea location furthest away from any hunter
static int indexMaxSea(int *distances, PlaceId* locations, int size)
{
   int index = 0;
   for (int i = 1; i < size; i++) {
      if (placeIsSea(locations[i]) && distances[i] < distances[index]) {
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
static PlaceId locationToMove(DraculaView dv, PlaceId *validMoves,
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
         case HIDE:
            if (trail[0] == location) {
               return HIDE;
            }
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
