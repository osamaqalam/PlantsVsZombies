#ifndef ETUDIANT_H
#define ETUDIANT_H

enum EtudiantType {
    NORMAL = 0,
    RAPIDE = 1,
    COSTAUD = 2
};

static const int stepSize[] = {
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
struct etudiant* next;
struct etudiant* next_line;
struct etudiant* prev_line;
} Etudiant;

#endif