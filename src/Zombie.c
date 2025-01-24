#include "Zombie.h"

void initZombie(Zombie* zombie)
{
    zombie->type = NORMAL;
    zombie->pointsDeVie = 0;
    zombie->ligne = 0;
    zombie->position = 0;
    zombie->vitesse = 0;
    zombie->tour = 0;
    zombie->prev = NULL;
    zombie->next = NULL;
    zombie->next_line = NULL;
    zombie->prev_line = NULL;
}
