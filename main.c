#include "jeu.h"

int main() {

    const char* filePath = "vague.txt";

    Jeu* jeu;
    jeu->tour = 1;

    char* contents = readFile(filePath);
    if (contents != NULL) {
        parseFileContent(contents, jeu);
        printTour(jeu);
        free(contents); // Free the allocated memory
    }

}