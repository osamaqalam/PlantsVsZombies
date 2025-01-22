#include "jeu.h"


void initializeTourelle(Jeu* jeu, enum TourelleType type) {

    if (!canPurchaseTourelle(jeu->cagnotte, type))
    {
        printf("You do not have enough cagnotte to purchase the tourelle.\n");
        return;
    }
    
    int x, y;
    printf("Enter the position to place the tourelle\n");
    printf("x:");
    scanf("%d", &x);
    printf("y:");
    scanf("%d", &y);

    if (placeTourelle(jeu, type, x, y)) {
        printf("Tourelle placed successfully.\n");
        jeu->cagnotte -= TOURELLE_PRICES[type];
    } else {
        printf("Failed to place tourelle.\n");
    }
}

void displayPurchaseMenu(Jeu* jeu) {
    int choice;
    printf("Purchase Menu: (Current Budget is %d)\n", jeu->cagnotte);
    printf("1. Basic Tower (Attacks first in Ligne)\n");
    printf("2. Nuke Tower (Kills all etudiants in the arena at a random turn between 1-5)\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("You chose to buy a Basic Tower.\n");
            initializeTourelle(jeu, BASIC);
            break;
        case 2:
            printf("You chose to buy a Nuke Tower.\n");
            initializeTourelle(jeu, NUKE);
            break;
        case 3:
            printf("Exiting purchase menu.\n");
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
    while (continuePurchasing) {
        displayPurchaseMenu(jeu);
        printf("Do you want to add another tower? (1 for Yes, 0 for No): ");
        scanf("%d", &continuePurchasing);
    }

    // The file for details of the enemies waves
    char* contents = readFile(filePath);
    if (contents != NULL) {
        parseFileContent(contents, jeu);

        while (!checkGameOver(jeu))
        {
            towersAttack(jeu);
            moveEtudiants(jeu);
            printTour(jeu);
            jeu->tour = jeu->tour + 1;
            //sleep(1);
        }
        free(contents); 
    }
    freeJeu(jeu);
}


