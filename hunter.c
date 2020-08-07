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

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"

#define NUM_LAST_LOC 3
////////////////////////////////////////////////////////////////////////

void randStartLocation(HunterView hv);
void normalMove(HunterView hv);
void randMove(HunterView hv);
void researchMove(HunterView hv);
void restMove(HunterView hv);
static void registerPlayWithPlaceId(PlaceId move);
int randGen(HunterView hv, int max);

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   if (HvGetRound(hv) == 0) {
      randStartLocation(hv);
      return;
   } 
   // if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
   //    registerPlayWithPlaceId(CASTLE_DRACULA);
   //    return;
   // }
   if (HvGetRound(hv)%6 == 0) {
      researchMove(hv);
      return;
   }
   randMove(hv);
   normalMove(hv);
   restMove(hv);
}

/*
void normalMove(HunterView hv) {
   int hunter = HvGetPlayer(hv);
   int round = HvGetRound(hv);
   int numReturnedLocs = 0;
   PlaceId *possibleLocs = intersectingLocations(hv, hunter, round, &numReturnedLocs);
   // if number of possible locations is greater than 0, then randomly move once towards location
   if (numReturnedLocs > 0) {
      // if returned loc is 1, random will return 0 which is 1st index
      int random = randGen(numReturnedLocs);
      int pathlength = 0;
      PlaceId *bfs = HvGetShortestPathTo(hv, hunter, possibleLocs[random], &pathlength);
      registerPlayWithPlaceId(bfs[0]);
   }
   else {
      return;
   }
}
////////////////////////////////////////////////////////////////////////

// KATHS VERSION
*/
void normalMove(HunterView hv) {
   int round = HvGetRound(hv);
   int player = HvGetPlayer(hv);
   PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, &round);
   PlaceId *shortestPath;
   PlaceId *validLocs;
   int pathLength = 0;
   int numReturnedLocs = -1;
   Round rounds[NUM_LAST_LOC];
   PlaceId draculaHistory[NUM_LAST_LOC];

   if (placeIsReal(draculaLocation)) {
      int knownLocations = HvLastThreeKnownDraculaLocation(hv, draculaHistory, rounds);
      int val = randGen(hv, knownLocations);
      shortestPath = HvGetShortestPathTo(hv, player, draculaHistory[val], &pathLength);
      validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
      for (int i = 0; i < numReturnedLocs; i++){
         if(shortestPath[0] == validLocs[i]){
            registerPlayWithPlaceId(shortestPath[0]);
            break;
         }
      }
   }
   else {
      return;
   }
}

void randStartLocation(HunterView hv)
{
   PlaceId start = NOWHERE;
   // CHEESE
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CONSTANTA);
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_DR_SEWARD) {
      registerPlayWithPlaceId(LISBON);
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_MINA_HARKER) {
      registerPlayWithPlaceId(LIVERPOOL);
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_VAN_HELSING) {
      registerPlayWithPlaceId(NAPLES);
      return;
   }
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(hv, NUM_REAL_PLACES);
   }   
   const char *location = placeIdToAbbrev(start);
   char *play = strdup(location);
   registerBestPlay(play, "");
}

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   
   int move = randGen(hv, numReturnedLocs);
   const char *location = placeIdToAbbrev(validLocs[move]);
   char *play = strdup(location);
   free(validLocs);
   registerBestPlay(play, "");   
}

// if dracula trail is all unknown, do research
void researchMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));

   registerPlayWithPlaceId(hunterLocation);
   return;
}

// if the hunter has >= 3, rest
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(hv, 100);

   // if player is less than 4 health, rngesus will decide if you want to 
   // play it safe or yolo
   if (HvGetHealth(hv, HvGetPlayer(hv)) <= 3 && random > 20) {
      registerPlayWithPlaceId(hunterLocation);
   }
   return;
}

// calls registerBestPlay but uses a PlaceId
static void registerPlayWithPlaceId(PlaceId move)
{
   const char *location = placeIdToAbbrev(move);
   char *play = strdup(location);
   registerBestPlay(play, "");
}

int randGen(HunterView hv, int max) {
    srand(time(0)*HvGetPlayer(hv) + 1);
    return rand() % max;
}
