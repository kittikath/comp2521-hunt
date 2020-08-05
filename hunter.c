////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
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
#include <assert.h>

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"
/////////////////// added headers
#include "Queue.h"
#include "utils.h"


#define NUM_LAST_LOC 2
////////////////////////////////////////////////////////////////////////

int randGen(int max);
void randStartLocation(void);
void randMove(HunterView hv);
PlaceId *hunterBfs(HunterView hv, Player player, PlaceId src, Round r);

PlaceId *possibleDraculalocations(HunterView hv, PlaceId draculaLocation, Round r);

PlaceId *intersectingLocations(HunterView hv, Player hunter, Round round, int *numReturnedLocs);
static int max(int num1, int num2);
static void registerPlayWithPlaceId(PlaceId move);
////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   int round = HvGetRound(hv);
   int player = HvGetPlayer(hv);
   PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, &round);
   PlaceId *intersections;
   PlaceId *shortestPath;
   int pathLength = 0;
   int numberofPlaces = 0;
   if (round == 0) {
      randStartLocation();
   } else if(draculaLocation != NOWHERE){
      intersections = intersectingLocations(hv, player, round, &numberofPlaces);
      int val = randGen(numberofPlaces-1);
      shortestPath = HvGetShortestPathTo(hv, player, intersections[val], &pathLength);
      if(pathLength == 0){
         randMove(hv);
      }
      else{
         registerPlayWithPlaceId(shortestPath[0]);
      }
      
   } else{
      randMove(hv);
   }
   free(intersections);
}

////////////////////////////////////////////////////////////////////////

void randStartLocation(void)
{
   PlaceId start = NOWHERE;
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(NUM_REAL_PLACES);
   }   
   const char *location = placeIdToAbbrev(start);
   registerBestPlay(location, "Let's crash Dracula's wedding!");
}

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   
   int move = randGen(numReturnedLocs);
   const char *location = placeIdToAbbrev(validLocs[move]);
   free(validLocs);
   registerBestPlay(location, "Don't run, I'm not the sun.");   
}

int randGen(int max) {
    srand(time(0));
    return rand() % max;
}

PlaceId *possibleDraculalocations(HunterView hv, PlaceId Known, Round round){
   // PlaceId *possiblePlaces = hunterBfs(hv, PLAYER_DRACULA, draculaLocation, futureRound);
   PlaceId *possiblePlaces = draculaBfs(hv, PLAYER_DRACULA, Known, round);
   assert(possiblePlaces != NULL);
	return possiblePlaces;

}


PlaceId *intersectingLocations(HunterView hv, Player hunter, Round round, int *numReturnedLocs){
   Round roundArr[NUM_LAST_LOC];
   PlaceId location[NUM_LAST_LOC];
   int knownLocations = HvLastTwoKnownDraculaLocation(hv, location, roundArr);
   PlaceId *bfs1;
   PlaceId *bfs2;
   if (knownLocations > 0) {
      bfs1 = possibleDraculalocations(hv, location[0], roundArr[0]);
   }
      
   if (knownLocations > 1) {
      bfs2 = possibleDraculalocations(hv, location[1], roundArr[1]);
   }      
 
   PlaceId found[NUM_REAL_PLACES];
   int counter = 0;
   PlaceId found2[NUM_REAL_PLACES];
   int counter2 = 0;

   //store known predecessor values into an array
   for(int i = 0; i < NUM_REAL_PLACES; i++){
      if(bfs1[i] != NOWHERE){
         found[counter] = bfs1[i];
         counter++;
      }
   }

   for(int j = 0; j < NUM_REAL_PLACES; j++){
      if(bfs2[j] != NOWHERE){
         found2[counter2] = bfs2[j];
         counter2++;
      }
   }
   free(bfs1);
   free(bfs2);

   int size = max(counter, counter2);
   PlaceId *intersections = malloc(size*sizeof(PlaceId));
   int overlaps = 0;
   for(int i = 0; i < counter; i++){
      for(int j = 0; j < counter2; j++){
         if(found[i] == found2[j]){
            intersections[overlaps] = found[i];
            overlaps++;
            break;
         }
      }
   }
   numReturnedLocs[0] = overlaps;

   return intersections;
}


static int max(int num1, int num2) {
	return (num1 > num2 ? num1 : num2);
}

// calls registerBestPlay but uses a PlaceId
static void registerPlayWithPlaceId(PlaceId move)
{
   const char *location = placeIdToAbbrev(move);
   char *play = strdup(location);
   registerBestPlay(play, "");
}