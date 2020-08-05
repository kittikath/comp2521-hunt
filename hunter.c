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
PlaceId *possibleDraculalocations(HunterView hv, PlaceId draculaLocation, Round r);
PlaceId *hunterBfs(HunterView hv, Player player, PlaceId src, Round r)

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   int round = HvGetRound(hv);
   PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, round);

   if (round == 0) {
      randStartLocation();
   } else if(draculaLocation != NOWHERE){

   }
   } else{
      randMove(hv);
   }
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

PlaceId *possibleDraculalocations(HunterView hv, Round round, Round futureRound, PlaceId Known){
   // PlaceId *possiblePlaces = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
   PlaceId *possiblePlaces = hunterBfs(hv, PLAYER_DRACULA, draculaLocation, futureRound);
   assert(possiblePlaces != NULL);
	return possiblePlaces;

}

PlaceId *crosslocations(HunterView hv, Player hunter, Round round, Round futureRound){
   PlaceId *draculaLocs = possibleDraculalocations(hv, round, futureRound, Known)
   PlaceId *hunterLocs = hunterBfs(hv, hunter, src, round);
   PlaceId *overlap = 
}
