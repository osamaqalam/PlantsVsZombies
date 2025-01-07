#ifndef ETUDIANT_H
#define ETUDIANT_H

typedef struct etudiant {
int type;
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