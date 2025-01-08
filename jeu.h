#ifndef JEU_H
#define JEU_H

#include <stdio.h>
#include <stdlib.h>
#include "tourelle.h"
#include "etudiant.h"

typedef struct {
Tourelle* tourelles;
Etudiant* etudiants;
int cagnotte;
int tour;
} Jeu;

void printGame (const Jeu *j);
char* readFile(const char* filePath);

#endif