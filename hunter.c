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
void randStartLocation(void);
void randMove(HunterView hv);
PlaceId *hunterBfs(HunterView hv, Player player, PlaceId src, Round r);
void researchMove(HunterView hv);
static void registerPlayWithPlaceId(PlaceId move);

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   if (HvGetRound(hv) == 0) {
      randStartLocation();
   } 
   researchMove(hv);
   restMove(hv);
   randMove(hv);
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

// if dracula trail is all unknown, do research
// return the hunters current location
void researchMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));

   // if no real location is in dracula's trail, research
   if (trailContains(NUM_REAL_PLACES) == false) {
      registerPlayWithPlaceId(hunterLocation);
   }
   return;
}

// if the hunter has >= 3, rest
// returns the hunters current location
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(100);

   // if player is less than 4 health, rngesus will decide if you want to 
   // play it safe or yolo
   if (HvGetHealth(hv, HvGetPlayer(hv)) >= 3 && random > 50) {
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
