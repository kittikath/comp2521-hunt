#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"
#include "Places.h"

int randGen(int max) {
    srand(time(0));
    return rand() % max;
}

int main(void) {
    int random = randGen(100);
    printf("random chance is: %d\n", random);

    return 0;
}