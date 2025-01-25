#include "jeu.h"

void clearConsole() {
    printf("\033[0;0H"); // on se place en haut à gauche
    printf("\033[2J"); // on efface tout
}

void initializePlant(Jeu* jeu, enum PlantType type) {

    if (!canPurchasePlant(jeu->cagnotte, type))
    {
        printf("You do not have enough budget to purchase the plant.\n");
        return;
    }
    
    int x, y;
    printf("Enter the position to place the plant\n");
    printf("x:");
    scanf("%d", &x);
    printf("y:");
    scanf("%d", &y);

    clearConsole();
    if (placePlant(jeu, type, x, y)) {
        printf("Plant placed successfully.\n");
        jeu->cagnotte -= PLANT_PRICES[type];
    } else {
        printf("Failed to place plant.\n");
    }
}

int displayPurchaseMenu(Jeu* jeu) {
    int choice;
    printf("Purchase Menu: (Current Budget is %d)\n", jeu->cagnotte);
    printf("1. Basic Plant (Attacks first in line)\n");
    printf("2. Nuke Plant (Kills all zombies in the arena at a random turn between 1-5)\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("You chose to buy a Basic Plant.\n");
            initializePlant(jeu, BASIC);
            return 1;
            break;
        case 2:
            printf("You chose to buy a Nuke Plant.\n");
            initializePlant(jeu, NUKE);
            return 2;
            break;
        case 3:
            printf("Exiting purchase menu.\n");
            return 3;
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            displayPurchaseMenu(jeu);
            break;
    }
}


int main() {

    const char* filePath = "vague.txt";

    Jeu* jeu = (Jeu*)malloc(sizeof(Jeu));
    initJeu(jeu, 1000);

    int continuePurchasing = 1;
    while (displayPurchaseMenu(jeu) != 3) {
        printTour(jeu);
    }

    // The file for details of the enemies waves
    char* contents = readFile(filePath);
    if (contents != NULL) {
        parseFileContent(contents, jeu);

        while (!checkGameOver(jeu))
        {
            clearConsole();
            towersAttack(jeu);
            moveZombies(jeu);

            printf("Tour %d\n", jeu->tour);
            printTour(jeu);

            jeu->tour = jeu->tour + 1;
            sleep(1);
        }
        printf("Press Enter to exit...");
        while (getchar() != '\n'); // Clear the input buffer
        getchar(); // Wait for Enter key

        free(contents); 
    }
    freeJeu(jeu);
}


