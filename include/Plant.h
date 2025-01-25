#ifndef PLANT_H
#define PLANT_H

#include <stdbool.h>

enum PlantType {
    BASIC = 0,
    NUKE = 1
};

static const int PLANT_PRICES[] = {
    [BASIC] = 100,
    [NUKE] = 300
};

typedef struct plant {
int type;
int pointsDeVie;
int ligne;
int position;
int prix;
int nukeTriggerTurn;
struct plant* prev;
struct plant* next;
} Plant;

bool canPurchasePlant(int budget, int type);

#endif