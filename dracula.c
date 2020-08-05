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

////////////////////////////////////////////////////////////////////////

int randGen(int max);
void randStartLocation(void);
void randMove(DraculaView dv);

////////////////////////////////////////////////////////////////////////

void decideDraculaMove(DraculaView dv)
{
	// TODO: Replace this with something better!
   if (DvGetRound(dv) == 0) {
      randStartLocation();
   } else {
      randMove(dv);
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
   registerBestPlay(location, "You're all I've ever wanted, I've wait my whole life to bite the right one...");
}

void randMove(DraculaView dv)
{
   int numReturnedMoves = -1;
   PlaceId *validMoves = DvGetValidMoves(dv, &numReturnedMoves);
   
   if (numReturnedMoves > 0) {
      int move = randGen(numReturnedMoves);
      const char *location = placeIdToAbbrev(validMoves[move]);
      free(validMoves);
      registerBestPlay(location, "I never ran from no one, except from you.");
   } else {
      free(validMoves);
      registerBestPlay("CD", "My castle may be haunted, but I'm not frightened.");
   }
}

int randGen(int max) {
    srand(time(0));
    return rand() % max;
}
