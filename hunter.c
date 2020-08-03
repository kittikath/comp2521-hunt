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

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
   if (HvGetRound(hv) == 0) {
      randStartLocation();
   } else {
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
   const char *abbrev = placeIdToAbbrev(start);
   char *location = strdup(abbrev);
   registerBestPlay(location, "Let's crash Dracula's wedding");
}

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   
   int move = randGen(numReturnedLocs);
   const char *abbrev = placeIdToAbbrev(validLocs[move]);
   char *location = strdup(abbrev);
   free(validLocs);
   registerBestPlay(location, "We're onto your scent");   
}

int randGen(int max) {
    srand(time(0));
    return rand() % max;
}
