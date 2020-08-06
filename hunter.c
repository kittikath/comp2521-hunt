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

void randStartLocation(HunterView hv);
void campAtCastleDracula(HunterView hv);
void randMove(HunterView hv);
PlaceId *hunterBfs(HunterView hv, Player player, PlaceId src, Round r);

PlaceId *possibleDraculalocations(HunterView hv, PlaceId draculaLocation, Round r);
// void researchMove(HunterView hv);
// void restMove(HunterView hv);
int randGen(int max);

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
   } else if(placeIsReal(draculaLocation)){
      printf("--------HELLO------------------\n");
      intersections = intersectingLocations(hv, player, round, &numberofPlaces);
      printf("--------------------------\n");
      for(int i = 0; i < numberofPlaces; i++){
         printf("%s >", placeIdToName(intersections[i]));
      }
      printf("\n");
      printf("--------------------------\n");
      int val = randGen(numberofPlaces);
      shortestPath = HvGetShortestPathTo(hv, player, intersections[val], &pathLength);
      for(int i = 0; i < pathLength; i++){
         printf("%s >", placeIdToName(shortestPath[i]));
      }
      printf("\n");
      if(pathLength == 0){
         randMove(hv);
      }
      else{
         // registerBestPlay(shortestPath[0], "Don't run, I'm not the sun.");   
         registerPlayWithPlaceId(shortestPath[0]);
      }
      free(intersections);
      free(shortestPath);
   } else{
      randMove(hv);
   }
   // if (HvGetRound(hv) == 0) {
   //    randStartLocation(hv);
   // } 
   campAtCastleDracula(hv);
   //normalMove(hv);
   researchMove(hv);
   restMove(hv);
   randMove(hv);
}

////////////////////////////////////////////////////////////////////////

void randStartLocation(HunterView hv)
{
///////////////// CHEESE /////////////////
   // lord godalming will be our unwelcome freeloader at castle dracula
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CASTLE_DRACULA);
      return;
   }
//////////////////////////////////////////
   PlaceId start = NOWHERE;
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(NUM_REAL_PLACES);
   }   
   const char *location = placeIdToAbbrev(start);
   char *play = strdup(location);
   registerBestPlay(play, "Let's crash Dracula's wedding!");
}

///////////////// CHEESE /////////////////
void campAtCastleDracula(HunterView hv) {
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CASTLE_DRACULA);
   }
}
//////////////////////////////////////////

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   
   int move = randGen(numReturnedLocs);
   const char *location = placeIdToAbbrev(validLocs[move]);
   char *play = strdup(location);
   free(validLocs);
   registerBestPlay(play, "Don't run, I'm not the sun.");   
}

// if dracula trail is all unknown, do research
// void researchMove(HunterView hv) {
//    PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));

//    // if no real location is in dracula's trail, research
//    if (trailContains(hv, NUM_REAL_PLACES) == false) {
//       registerPlayWithPlaceId(hunterLocation);
//    }
//    return;
// }

// if the hunter has >= 3, rest
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(100);

   // if player is less than 4 health, rngesus will decide if you want to 
   // play it safe or yolo
   if (HvGetHealth(hv, HvGetPlayer(hv)) >= 3 && random > 20) {
      registerPlayWithPlaceId(hunterLocation);
   }
   return;
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
   printf("last 2 locations!!!! %s %s\n", placeIdToName(location[0]), placeIdToName(location[1]));
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
   registerBestPlay(play, "please work :(");
}