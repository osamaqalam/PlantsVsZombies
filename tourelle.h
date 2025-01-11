#ifndef TOURELLE_H
#define TOURELLE_H

#include <stdbool.h>

static const int TOURELLE_PRICES[] = {100, 200, 300, 400};

typedef struct tourelle {
int type;
int pointsDeVie;
int ligne;
int position;
int prix;
struct tourelle* next;
} Tourelle;

bool canPurchaseTourelle(int cagnotte, int type);

#endif