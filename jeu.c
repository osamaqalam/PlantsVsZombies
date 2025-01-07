#include "jeu.h"

void printGame (const Jeu *j)
{
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 15; j++) {
            printf(".   ");
        }
        printf("\n");
    }
}
