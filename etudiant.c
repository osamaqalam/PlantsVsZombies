#include "etudiant.h"

void initEtudiant(Etudiant* etudiant)
{
    etudiant->type = NORMAL;
    etudiant->pointsDeVie = 0;
    etudiant->ligne = 0;
    etudiant->position = 0;
    etudiant->vitesse = 0;
    etudiant->tour = 0;
    etudiant->prev = NULL;
    etudiant->next = NULL;
    etudiant->next_line = NULL;
    etudiant->prev_line = NULL;
}
