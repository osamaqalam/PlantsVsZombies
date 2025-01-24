#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <stddef.h> // for NULL

enum ZombieType {
    NORMAL = 0,
    RAPIDE = 1,
    COSTAUD = 2
};

static const int STEP_SIZE[] = {
    [NORMAL] = 1,
    [RAPIDE] = 2,
    [COSTAUD] = 1
};

typedef struct zombie{
enum ZombieType type;
int pointsDeVie;
int ligne;
int position;
int vitesse;
int tour;
struct zombie* prev;
struct zombie* next;
struct zombie* next_line;
struct zombie* prev_line;
} Zombie;

void initZombie(Zombie* zombie);

#endif