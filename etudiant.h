#ifndef ETUDIANT_H
#define ETUDIANT_H

#include <stddef.h> // for NULL

enum EtudiantType {
    NORMAL = 0,
    RAPIDE = 1,
    COSTAUD = 2
};

static const int STEP_SIZE[] = {
    [NORMAL] = 1,
    [RAPIDE] = 2,
    [COSTAUD] = 1
};

typedef struct etudiant {
enum EtudiantType type;
int pointsDeVie;
int ligne;
int position;
int vitesse;
int tour;
struct etudiant* prev;
struct etudiant* next;
struct etudiant* next_line;
struct etudiant* prev_line;
} Etudiant;

void initEtudiant(Etudiant* etudiant);

#endif