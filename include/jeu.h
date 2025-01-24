#ifndef JEU_H
#define JEU_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      // for srand(time(NULL))
#include <unistd.h>    // for sleep()
#include "tourelle.h"
#include "Zombie.h"

#define MAX_LINE_LENGTH 100
#define NUM_LIGNES 7
#define NUM_COLS 15

typedef enum{
    ZOMBIE,
    TOURELLE,
    VIDE
} ObjectType;

typedef struct {
    void* ptr;
    ObjectType type;
} CellPointer;

typedef struct {
Tourelle* tourelles;
Zombie* zombies;
CellPointer grille[NUM_COLS][NUM_LIGNES];
int cagnotte;
int tour;
} Jeu;

void initJeu (Jeu *j, int cagnotte);
void printTour (const Jeu *j);
char* readFile(const char* filePath);
int parseFileContent(const char* fileContents, Jeu *jeu);
void encounteredObject(Jeu* jeu, Zombie* movingZombie, CellPointer* firstObj);
void moveZombies(Jeu *jeu);
bool checkGameOver(Jeu *jeu);

bool placeTourelle(Jeu* jeu, enum TourelleType type, int x, int y);
void towersAttack(Jeu* jeu);
void basicTowerAttack(Jeu* jeu, Tourelle* tourelle);
void nukeTowerAttack (Jeu* jeu, Tourelle* tourelle);

void zombieAttack(Jeu* jeu, Zombie* zombie, Tourelle* tourelle);

void freeJeu(Jeu* jeu);

#endif