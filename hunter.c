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
void randMove(HunterView hv);
void registerPlayWithPlaceId(PlaceId move);
void commonLocs(PlaceId *arr1, PlaceId *arr2, int size1, int size2);
int randGen(HunterView hv, int max);

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
	// TODO: Replace this with something better!
	
	Round round = HvGetRound(hv);
	Player hunter = HvGetPlayer(hv);
	
   if (round == 0) {
      randStartLocation(hv);
      return;
   }
   
   PlaceId revealedLocs[NUM_LAST_LOC];
   Round revealedRounds[NUM_LAST_LOC];
   
   int numRevealed = HvFillDraculaLocations(hv, revealedLocs, revealedRounds, NUM_LAST_LOC);
   for (int i = 0; i < numRevealed; i++) {
      printf("Revealed Loc: %s\n", placeIdToAbbrev(revealedLocs[i]));
      printf("Rounds Ago: %d\n", revealedRounds[i]);
   }
   if (numRevealed != 0) {
      int numPossible = 0;
      PlaceId *possibleLocs = HvGetDraculaLocations(hv, revealedLocs[0], revealedRounds[0],
                                                          &numPossible);
                                                          
      for (int i = 0; i < numPossible; i++) {
         printf("Possible Loc: %s\n", placeIdToAbbrev(possibleLocs[i]));      
      }
      
      int randLoc = randGen(hv, numPossible);
      int pathLength = 0;
      PlaceId *path = HvGetShortestPathTo(hv, hunter, possibleLocs[randLoc],
                                                                   &pathLength);
      registerPlayWithPlaceId(path[0]);
      
      free(path);
      free(possibleLocs);
   } else {   
      randMove(hv);
   }
}


////////////////////////////////////////////////////////////////////////
//

void randStartLocation(HunterView hv)
{
   PlaceId start = NOWHERE;
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(hv, NUM_REAL_PLACES);
   }
   registerPlayWithPlaceId(start);
}

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   int move = randGen(hv, numReturnedLocs);
   registerPlayWithPlaceId(validLocs[move]);   
}


/*
// find repeated values between 2 arrays
void commonLocs(PlaceId *arr1, PlaceId *arr2, int size1, int size2)
{
	int j = 0;
	PlaceId commArr[NUM_REAL_PLACES];
	
	for (int i = 0; i < size1; i++) {
		bool inArray = false;
		for (int k = 0; k < size2; k++) {
			if (arr1[i] == arr2[k]) {
				inArray = true;
			}
		}
		if (inArray) {
			commArr[j] = arr1[i];
			j++;
		}
	}
	
	int numCommonLocs = j;
	
	PlaceId *commonLocs = malloc(numCommonLocs * sizeof(*commonLocs));
	
	for (int i = 0; i < numCommonLocs; i++) {
		commonLocs[i] = commArr[i];
	}
	
}
*/

// calls registerBestPlay but uses a PlaceId
void registerPlayWithPlaceId(PlaceId move)
{
   char *play = placeIdToAbbrev(move);
   registerBestPlay(play, "");
}

int randGen(HunterView hv, int max) {
    srand(time(0) + HvGetPlayer(hv) + HvGetRound(hv));
    return rand() % max;
}
