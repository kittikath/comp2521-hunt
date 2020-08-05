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

////////////////////////////////////////////////////////////////////////

int randGen(int max);
void randStartLocation(HunterView hv);
void campAtCastleDracula(HunterView hv);
void randMove(HunterView hv);
PlaceId *hunterBfs(HunterView hv, Player player, PlaceId src, Round r);
void researchMove(HunterView hv);
static void registerPlayWithPlaceId(PlaceId move);
void researchMove(HunterView hv);
void restMove(HunterView hv);

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   if (HvGetRound(hv) == 0) {
      randStartLocation(hv);
   } 
   campAtCastleDracula(hv);
   //normalMove(hv);
   researchMove(hv);
   restMove(hv);
   randMove(hv);
}

////////////////////////////////////////////////////////////////////////

void randStartLocation(HunterView hv)
{  
   // lord godalming will be our unwelcome freeloader at castle dracula
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CASTLE_DRACULA);
      return;
   }
   PlaceId start = NOWHERE;
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(NUM_REAL_PLACES);
   }   
   const char *location = placeIdToAbbrev(start);
   registerBestPlay(location, "Let's crash Dracula's wedding!");
}

// cheese cheese cheese
void campAtCastleDracula(HunterView hv) {
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CASTLE_DRACULA);
   }
}

// randomly move to a valid location
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

/*
// move towards dracula via deducting where his current location may be
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
*/

// if we definitely know dracula is within reach, oof him
void oofDraculaMove(HunterView hv) {
   int returnedRound = 0;
   PlaceId lastLoc = HvGetLastKnownDraculaLocation(hv, &returnedRound);
   int round = HvGetRound(hv);

   if (returnedRound == round) {
      registerPlayWithPlaceId(lastLoc);
   }
   else {
      return;
   }
}



// if dracula trail is all unknown, do research
void researchMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));

   // if no real location is in dracula's trail, research
   if (trailContains(hv, NUM_REAL_PLACES) == false) {
      registerPlayWithPlaceId(hunterLocation);
   }
   return;
}

// if player is less than 4 health, rngesus will decide if you want to 
// play it safe or yolo
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(100);

   if (HvGetHealth(hv, HvGetPlayer(hv)) >= 3 && random > 20) {
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
