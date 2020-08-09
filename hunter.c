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
void oceanMove(HunterView hv);
void randMove(HunterView hv);
void researchMove(HunterView hv);
void restMove(HunterView hv);
static void registerPlayWithPlaceId(PlaceId move, Message Message);
int randGen(HunterView hv, int max);
PlaceId prevLocation(HunterView hv);
void randMoveWithNoCongaLine(HunterView hv);

////////////////////////////////////////////////////////////////////////

void decideHunterMove(HunterView hv)
{
   int round = HvGetRound(hv);
   int r;
	// TODO: Replace this with something better!
   HvGetLastKnownDraculaLocation(hv, &r);
   if (round == 0) {
      randStartLocation(hv);
      return;
   } 
   if (round%6 == 0) {
      researchMove(hv);
      return;
   }
   // randMove(hv);
   // PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, &round);
   // if (placeIsReal(draculaLocation)) {
      
   // }
   randMoveWithNoCongaLine(hv);
   normalMove(hv);
   // oceanMove(hv);
   restMove(hv);
}

// void oceanMove(HunterView hv){
//    int round = HvGetRound(hv);
//    PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, &round);
   
//    int pathLength = 0;
//    int numReturnedLocs = -1;
//    // int draculaLocs = -1;
//    Round rounds[NUM_LAST_LOC];
//    PlaceId draculaHistory[NUM_LAST_LOC];
// }



/////////////////////////////////////////////////////////////////////////
void normalMove(HunterView hv) {
   int round = HvGetRound(hv);
   int r = HvGetRound(hv);
   int player = HvGetPlayer(hv);
   PlaceId draculaLocation = HvGetLastKnownDraculaLocation(hv, &round);
   PlaceId *shortestPath;
   PlaceId *validLocs;
   PlaceId *draculaPlaces;
   int pathLength = 0;
   int numReturnedLocs = -1;
   int draculaLocs = -1;
   Round rounds[NUM_LAST_LOC];
   PlaceId draculaHistory[NUM_LAST_LOC];

   //array to find record other player locations
   int otherHunters[3];
   int counter = 0;
   for(int i = 0; i < NUM_PLAYERS; i++){
      if(i != player && i != PLAYER_DRACULA){
         otherHunters[counter] = HvGetPlayerLocation(hv, player);
         counter++;
      }
   }
   
   if (placeIsReal(draculaLocation)) {
      int knownLocations = HvLastThreeKnownDraculaLocation(hv, draculaHistory, rounds);
      // //check surrounding places of last known dracula location
      int v;
      // printf("current round %d, found location round %d latest? %d\n", round, rounds[knownLocations-1], rounds[0]);
      if(r - rounds[0] == 2){
         PlaceId location = draculaHistory[0];
         draculaPlaces = HvWhereCanDraculaGoByType(hv, rounds[0], location, &draculaLocs);
         v = randGen(hv, draculaLocs);
        
         for(int j = 0; j < draculaLocs; j++){
            printf("placelocation: %s\n", placeIdToName(draculaPlaces[j]));
         }
         v = randGen(hv, draculaLocs);
         for(int k = 0; k < draculaLocs; k++){
            for(int h = 0; h < 3; h++){
               if(otherHunters[h] == draculaPlaces[v]){
                  v = randGen(hv, draculaLocs);
                  break;
               }
            }
         }
         shortestPath = HvGetShortestPathTo(hv, player, draculaPlaces[v], &pathLength);
         printf("get to %s by going through %s\n",  placeIdToName(draculaPlaces[v]), placeIdToName(shortestPath[0]));
         validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
         for (int i = 0; i < numReturnedLocs; i++){
            if(shortestPath[0] == validLocs[i]){
               registerPlayWithPlaceId(shortestPath[0], "power move");
               break;
            }
         }
      }
      else{
         int val = randGen(hv, knownLocations);
         for (int i = 0; i < knownLocations; i++){
            printf("trail locations: %s at round %d\n", placeIdToAbbrev(draculaHistory[i]), rounds[i]);
         }
         printf("current round is %d, found hints at round %d\n", r, rounds[0]);
         shortestPath = HvGetShortestPathTo(hv, player, draculaHistory[val], &pathLength);
         validLocs = HvWhereCanIGo(hv, &numReturnedLocs);
         for (int i = 0; i < numReturnedLocs; i++){
            if(shortestPath[0] == validLocs[i]){
               registerPlayWithPlaceId(shortestPath[0], "normal move");
               break;
            }
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
      registerPlayWithPlaceId(ROME, "start location");
      return;
   }
   while (!placeIsLand(start) && start != HOSPITAL_PLACE) {
      start = randGen(hv, NUM_REAL_PLACES);
   }   
   const char *location = placeIdToAbbrev(start);
   char *play = strdup(location);
   registerBestPlay(play, "start location");
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

   registerPlayWithPlaceId(hunterLocation, "research");
   return;
}

// if the hunter has >= 3, rest
void restMove(HunterView hv) {
   PlaceId hunterLocation = HvGetPlayerLocation(hv, HvGetPlayer(hv));
   int random = randGen(hv, 100);

   // if player is less than 4 health, rngesus will decide if you want to 
   // play it safe or yolo
   if (HvGetHealth(hv, HvGetPlayer(hv)) <= 3 && random > 20) {
      registerPlayWithPlaceId(hunterLocation, "rest move");
   }
   return;
}

// calls registerBestPlay but uses a PlaceId
static void registerPlayWithPlaceId(PlaceId move, Message message)
{
   const char *location = placeIdToAbbrev(move);
   char *play = strdup(location);
   registerBestPlay(play, message);
}

int randGen(HunterView hv, int max) {
    srand(time(0)*HvGetPlayer(hv) + 1);
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
   registerBestPlay(play, "");
}

