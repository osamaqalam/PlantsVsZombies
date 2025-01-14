#ifndef JEU_H
#define JEU_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tourelle.h"
#include "etudiant.h"

#define MAX_LINE_LENGTH 100
#define NUM_COLS 15

typedef struct {
Tourelle* tourelles;
Etudiant* etudiants;
int cagnotte;
int tour;
} Jeu;

void initJeu (Jeu *j, int cagnotte);
void printTour (const Jeu *j);
char* readFile(const char* filePath);
int parseFileContent(const char* fileContents, Jeu *jeu);
bool isPassingAllowed(Jeu* jeu, Etudiant* movingEtudiant);
void moveEtudiants(Jeu *jeu);
bool checkGameOver(Jeu *jeu);
bool placeTourelle(Jeu* jeu, int type, int x, int y);
void towersAttack(Jeu* jeu);
void basicTowerAttack(Jeu* jeu, Tourelle* tourelle);


#endif