#include "jeu.h"

int main() {

    const char* filePath = "vague.txt";

    Jeu jeu;
    jeu.tour = 1;

    printGame(&jeu);
    char* contents = readFile(filePath);

    if (contents != NULL) {
        printf("File Contents:\n%s\n", contents);
        free(contents); // Free the allocated memory
    }

}

