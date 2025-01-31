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

#define NUM_LAST_LOC 2
////////////////////////////////////////////////////////////////////////

void randStartLocation(HunterView hv);
void randMove(HunterView hv);
void researchMove(HunterView hv);
void restMove(HunterView hv);
void registerPlayWithPlaceId(PlaceId move, Message message);
PlaceId *commonLocs(PlaceId *array1, PlaceId *array2, int size1, int size2, 
                                                            int *numCommonLocs);
int randGen(HunterView hv, int max);

PlaceId prevLocation(HunterView hv);
void randMoveWithNoCongaLine(HunterView hv);

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
   
   // where dracula could be
   PlaceId revealedLocs[NUM_LAST_LOC];
   Round revealedRounds[NUM_LAST_LOC];
   
   int numRevealed = HvFillDraculaLocations(hv, revealedLocs, revealedRounds, NUM_LAST_LOC);
   
   // lists of possible dracula locations
   int numPossible1 = 0;
   PlaceId *possibleLocs1;
   int numPossible2 = 0;
   PlaceId *possibleLocs2;
   int numPossible3 = 0;
   PlaceId *possibleLocs3;
   
   // filling out arrays with dracula locations
   switch (numRevealed) {
      case 3:
         possibleLocs3 = HvGetDraculaLocations(hv, revealedLocs[2], 
                                              revealedRounds[2], &numPossible3);     
      case 2:
         possibleLocs2 = HvGetDraculaLocations(hv, revealedLocs[1], 
                                              revealedRounds[1], &numPossible2);     
      case 1:
         possibleLocs1 = HvGetDraculaLocations(hv, revealedLocs[0], 
                                              revealedRounds[0], &numPossible1);                                
      default:
         break;
   }
   
   PlaceId* draculaLocs = possibleLocs2;
   int numDraculaLocs = numPossible2;
   PlaceId *tmp;
   int numTmp;   
   
   // sorting out arrays
   switch (numRevealed) {
      case 3:
         draculaLocs = commonLocs(possibleLocs3, possibleLocs2, numPossible3, numPossible2, 
                                                         &numDraculaLocs);
         free(possibleLocs3);
         free(possibleLocs2);
      case 2:
         numTmp = numDraculaLocs;
         tmp = draculaLocs;
         draculaLocs = commonLocs(tmp, possibleLocs1, numTmp, numPossible1,
                                                         &numDraculaLocs);
         free(tmp);
         free(possibleLocs1);
         break;
      case 1:
         draculaLocs = possibleLocs1;
         numDraculaLocs = numPossible1;
         break;
      default:
         break;
   }
   // where dracula could be should be in DRACULA_LOCS with NUM_DRACULA_LOCS
   // now you just need to find a way to it!
   // also, have darwin implement a rest function where if numRevealed == 0; and round is >= 6, they do research!!
   // can do whatever from here on!


   // int numReturnedLocs = -1;
   if (numRevealed == 0) {
      if(((round/10)%2==0 && round%6 == 0) || ((round/10)%2==1 && round%7 == 0)){
         researchMove(hv);
      }
      else{
         randMoveWithNoCongaLine(hv);
      }
   } else if (HvGetHealth(hv, hunter) <= 4) {
   	restMove(hv); // rest if hunter is low on health
   } else {   
      // randMove(hv);
      // Random move on shortest path to a possible dracula location
      int randLoc = randGen(hv, numDraculaLocs);
      while (draculaLocs[randLoc] == HvGetPlayerLocation(hv, hunter)) {
      	randLoc = randGen(hv, numDraculaLocs);
      }
      int pathLength = 0;
      PlaceId *path = HvGetShortestPathTo(hv, hunter, draculaLocs[randLoc],
                                                                   &pathLength);
      registerPlayWithPlaceId(path[0], "bfs");
      // PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
      // for(int i = 0; i < numReturnedLocs; i++){
      //    if(path[0] == validLocs[i]){
      //       registerPlayWithPlaceId(path[0], "bfs");
      //       break;
      //    }
      // }                                                            
      free(path);
      free(draculaLocs);
   }
}


void randStartLocation(HunterView hv)
{
   PlaceId start = NOWHERE;
   if (HvGetPlayer(hv) == PLAYER_LORD_GODALMING) {
      registerPlayWithPlaceId(CONSTANTA, "start location");
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_DR_SEWARD) {
      registerPlayWithPlaceId(LISBON, "start location");
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_MINA_HARKER) {
      registerPlayWithPlaceId(LIVERPOOL, "start location");
      return;
   }
   else if(HvGetPlayer(hv) == PLAYER_VAN_HELSING) {
      registerPlayWithPlaceId(NAPLES, "start location");
      return;
   }
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(hv, NUM_REAL_PLACES);
   }
}

void randMove(HunterView hv)
{
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   int move = randGen(hv, numReturnedLocs);
   registerPlayWithPlaceId(validLocs[move], "random move");   
}

// if dracula trail is all unknown, do research
void researchMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));

   registerPlayWithPlaceId(hunterLocation, "research");
}

// if the hunter has >= 3, rest
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(hv, 100);

   // if player is less than 4 health, rngesus will decide if you want to 
   // play it safe or yolo
   if (HvGetHealth(hv, HvGetPlayer(hv)) <= 3 && random > 20) {
      registerPlayWithPlaceId(hunterLocation, "rest");
   }
   return;
}



// find repeated values between 2 arrays
PlaceId *commonLocs(PlaceId *array1, PlaceId *array2, int size1, int size2, 
                                                             int *numCommonLocs)
{
	int j = 0;
	PlaceId commonArray[NUM_REAL_PLACES];
	
	for (int i = 0; i < size1; i++) {
	   bool inArray = false;
		for (int k = 0; k < size2; k++) {
			if (array1[i] == array2[k]) {
				inArray = true;
				break;
			}
		}
		if (inArray) {
			commonArray[j] = array1[i];
			j++;
		}
	}
	
	// malloc and copy array over		
	PlaceId *commonLocs = malloc(j * sizeof(*commonLocs));
	for (int i = 0; i < j; i++) {
		commonLocs[i] = commonArray[i];
	}
	
	*numCommonLocs = j;
	return commonLocs;
}


// calls registerBestPlay but uses a PlaceId
void registerPlayWithPlaceId(PlaceId move, Message message)
{
   char *play = placeIdToAbbrev(move);
   registerBestPlay(play, message);
}

int randGen(HunterView hv, int max) {
    srand(time(0) + HvGetPlayer(hv) + HvGetRound(hv));
    return rand() % max;
}

// gets last location of player
PlaceId prevLocation(HunterView hv) {
   return lastLocation(hv, HvGetPlayer(hv));
}

// random play AND NO ONE GROUPS UPP
void randMoveWithNoCongaLine(HunterView hv)
{  
   int numReturnedLocs = -1;
   PlaceId *validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
   int move = randGen(hv, numReturnedLocs);
   const char *location = placeIdToAbbrev(validLocs[move]);
   char *play = strdup(location);
   for (int i = 0; i < 4; i++) {
      // if the most recent location of a hunter is this hunter's current play, change
      PlaceId lastLoc = lastLocation(hv, i);
      if (lastLoc == validLocs[move] && i != HvGetPlayer(hv)) {
         // loop through all possible plays
         for (int j = 0; j < numReturnedLocs; j++) {
            const char *locationcpy = placeIdToAbbrev(validLocs[j]);
            // if location is different from hunter, move there
            if (location != locationcpy) {
               play = strdup(locationcpy);
               free(validLocs);
               registerBestPlay(play, "random congo line");
               return;
            }
         }
      }
   }
   free(validLocs);
   registerBestPlay(play, "rand");
}
