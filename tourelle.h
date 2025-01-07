#ifndef TOURELLE_H
#define TOURELLE_H

typedef struct tourelle {
int type;
int pointsDeVie;
int ligne;
int position;
int prix;
struct tourelle* next;
} Tourelle;

#endif