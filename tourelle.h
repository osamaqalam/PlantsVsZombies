#ifndef TOURELLE_H
#define TOURELLE_H

#include <stdbool.h>

enum TourelleType {
    BASIC = 0,
    ADVANCED = 1
};

static const int TOURELLE_PRICES[] = {
    [BASIC] = 100,
    [ADVANCED] = 200
};

typedef struct tourelle {
int type;
int pointsDeVie;
int ligne;
int position;
int prix;
struct tourelle* prev;
struct tourelle* next;
} Tourelle;

bool canPurchaseTourelle(int cagnotte, int type);

#endif