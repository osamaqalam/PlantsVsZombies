#include "jeu.h"

int main() {

    const char* filePath = "vague.txt";

    Jeu* jeu = (Jeu*)malloc(sizeof(Jeu));
    jeu->tour = 1;

    char* contents = readFile(filePath);
    if (contents != NULL) {
        parseFileContent(contents, jeu);

        while (!checkGameOver(jeu))
        {
            printTour(jeu);
            moveEtudiants(jeu);
            jeu->tour++;
        }
        free(contents); // Free the allocated memory
    }
    free (jeu);
}