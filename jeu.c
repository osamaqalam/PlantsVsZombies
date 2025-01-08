#include "jeu.h"

void printGame (const Jeu *j)
{
    printf("Tour %d\n", j->tour);

    for (int i = 0; i < 7; i++) {
        printf("%d", i+1);
        printf("|   ");
        for (int j = 0; j < 15; j++) {
            printf(".   ");
        }
        printf("\n");
    }
}
