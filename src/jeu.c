#include "jeu.h"

void initJeu(Jeu* restrict jeu, int cagnotte) 
{
    jeu->tour = 1;
    jeu->cagnotte = cagnotte;
    jeu->zombies = NULL;
    jeu->plants = NULL;

    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_LIGNES; j++)
        {
            jeu->grille[i][j].ptr = NULL;
            jeu->grille[i][j].type = VIDE;
        }
    }
}

void printTour (const Jeu* restrict jeu)
{
    for (int ligne = 1; ligne <= 7; ligne++) {
        printf("%d", ligne);
        printf("|   ");
        for (int pos = 1; pos <= 15; pos++) {
        
            // Check if a cell is occupied by a zombie
            if(jeu->grille[pos-1][ligne-1].type == ZOMBIE)
            {
                Zombie* zombie = (Zombie*)jeu->grille[pos-1][ligne-1].ptr;
                if (zombie->type == NORMAL)
                    printf("%dZ  ", zombie->pointsDeVie);
                continue;
            }
            
            // Check if a cell is occupied by a plant
            else if (jeu->grille[pos-1][ligne-1].type == PLANT)
            {
                Plant* plant = (Plant*)jeu->grille[pos-1][ligne-1].ptr;
                if (plant->type == BASIC)
                    printf("%dP  ", plant->pointsDeVie);
                else if (plant->type == NUKE)
                    printf("%dN  ", plant->pointsDeVie);
                continue;
            }
            else
                printf(".   ");
        }
        printf("\n");
    }
    printf("\n");
}

char* readFile(const char* filePath) 
{
    // Open the file in binary mode
    FILE* file = fopen(filePath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Move the cursor of the file to the end
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(file);
        return NULL;
    }

    // ftell in binary mode returns the number of bytes from start to the cursor
    long fileSize = ftell(file);
    if (fileSize < 0) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }
    // reset the position indicator to start of file
    rewind(file);

    // Allocate memory for the file contents
    char* buffer = (char*)malloc(fileSize + 1); // +1 for null terminator
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file contents
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead < fileSize && ferror(file)) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    // Null-terminate the string
    buffer[bytesRead] = '\0';

    // Close the file
    fclose(file);

    return buffer;
}

void connectLinesPtrs (Jeu* jeu)
{
    for (int i=1; i<=NUM_LIGNES; i++)
    {
        Zombie* curZombie = jeu->zombies;
        Zombie* prevZombieLigne = NULL;

        while(curZombie)
        {
            if (curZombie->ligne == i)
            {
                if (prevZombieLigne)
                {
                    prevZombieLigne->next_line = curZombie;
                    curZombie->prev_line = prevZombieLigne;
                }
                else
                {
                    curZombie->prev_line = NULL;
                }
                prevZombieLigne = curZombie;
            }
            curZombie = curZombie->next;
        }
    }
}

int parseFileContent(const char* fileContents, Jeu *jeu)
{
    // Parse the file contents
    int lineCount = 0; // Tracks the number of parsed lines
    char buffer[MAX_LINE_LENGTH];
    const char* current = fileContents;
    Zombie* zombie = NULL;
    Zombie* lastZombie = NULL;

    // Loop while there is a line to read
    // sscanf will store whatever is right after % in the later arguments
    while (sscanf(current, "%[^\n]\n", buffer) == 1 && isdigit(buffer[0])) {
        //read the cagnotte on first iteration
        if (lineCount==0)
        {
            sscanf(buffer, "%d", &jeu->cagnotte);
        }
        else
        {
            char type;

            // Dynamically allocate memory for an instance of zombie
            zombie = (Zombie*)malloc(sizeof(Zombie));
            if (zombie == NULL) {
                perror("Error allocating memory for Zombie");
                return -1; // Return an error code
            }

            initZombie(zombie);

            sscanf(buffer, "%d %d %c", &zombie->tour, &zombie->ligne, &type);
            zombie->position = NUM_COLS+zombie->tour; // Start 

            if (type == 'Z') 
            {
                zombie->type = NORMAL;
                zombie->pointsDeVie = 5;
            }

            if (zombie->position <= NUM_COLS)
            {
                jeu->grille[zombie->position-1][zombie->ligne-1].ptr = zombie;
                jeu->grille[zombie->position-1][zombie->ligne-1].type = ZOMBIE;
            }

            // if not first zombie
            if (lastZombie) 
            {
                zombie->prev = lastZombie;
                lastZombie->next = zombie;
            }
            else
            {
                zombie->prev = NULL;
                jeu->zombies = zombie;
            }

            lastZombie = zombie;
        }
        // Move to the next line
        current += strlen(buffer) + 1;
        lineCount++;

    }
    lastZombie->next = NULL;

    connectLinesPtrs(jeu);

    return lineCount;
}

// If there is an object in the path of the zombie, it will be stored in firstObj
void encounteredObject(Jeu* jeu, Zombie* movingZombie, CellPointer* firstObj)
{
    int FinalPosition = movingZombie->position - STEP_SIZE[movingZombie->type];
    Zombie* curZombie = jeu->zombies;
    Plant* curPlant = jeu->plants;

    while (curZombie != NULL)
    {
        if (curZombie != movingZombie && curZombie->ligne == movingZombie->ligne &&
            curZombie->position <= movingZombie->position && curZombie->position >= FinalPosition)
        {
            firstObj->ptr = curZombie;
            firstObj->type = ZOMBIE;
            return;
        }
        curZombie = curZombie->next;
    }

    while (curPlant != NULL)
    {
        if (curPlant->ligne == movingZombie->ligne && curPlant->position <= movingZombie->position
         && curPlant->position >= FinalPosition)
        {
            firstObj->ptr = curPlant;
            firstObj->type = PLANT;
            return;
        }
        curPlant = curPlant->next;
    }

    firstObj->ptr = NULL;
    firstObj->type = VIDE;
}

void moveZombies(Jeu *jeu)
{
    Zombie* curZombie = jeu->zombies;
    CellPointer* cell = malloc(sizeof(CellPointer));

    while (curZombie != NULL)
    {   
        encounteredObject(jeu, curZombie, cell);

        // Only move zombie if the path is clear
        if(cell->type == VIDE)
        {    
            // Clear the current cell if the zombie is in the range of visible arena
            if (curZombie->position - 1 >= 0 && curZombie->position - 1 < NUM_COLS)
            {
                jeu->grille[curZombie->position-1][curZombie->ligne-1].ptr = NULL;
                jeu->grille[curZombie->position-1][curZombie->ligne-1].type = VIDE;
            }

            curZombie->position = curZombie->position - STEP_SIZE[curZombie->type];

            // Set the new cell if the zombie is in the range of visible arena
            if (curZombie->position - 1 >= 0 && curZombie->position - 1 < NUM_COLS)
            {
                jeu->grille[curZombie->position-1][curZombie->ligne-1].ptr = curZombie;
                jeu->grille[curZombie->position-1][curZombie->ligne-1].type = ZOMBIE;
            }
        }
        else if (cell->type == PLANT)
        {
            Plant* plant = (Plant*)cell->ptr;
            zombieAttack(jeu, curZombie, plant);
        }
        curZombie = curZombie->next;   
    }
    free(cell);
}

bool checkGameOver(Jeu *jeu)
{
    Zombie* curZombie = jeu->zombies;

    while (curZombie != NULL)
    {
        if (curZombie->position <= 1)
        {
            printf("GAME OVER - YOU LOSE!\n");
            return true;
        }
        curZombie = curZombie->next;
    }

    if (jeu->zombies == NULL)
    {
        printf("GAME OVER - YOU WIN!\n");
        return true;
    }

    return false;
}

// x, y are 1-indexed
bool placePlant(Jeu* restrict jeu, enum PlantType type, int x, int y) 
{
    if(jeu->grille[x-1][y-1].type != VIDE ||
        x < 1 || x >= NUM_COLS || y < 1 || y > NUM_LIGNES)
        return false;

    Plant* plant = (Plant*)malloc(sizeof(Plant));
    plant->type = type;

    if (type == BASIC)
        plant->pointsDeVie = 3;
    else if (type == NUKE)
    {
        plant->pointsDeVie = 1;
        
        srand(time(NULL)); // use current time as seed for random generator
        plant->nukeTriggerTurn = (rand() % 5) + 1;
    }

    plant->ligne = y;
    plant->position = x;
    plant->prix = PLANT_PRICES[type];
    plant->prev = NULL;
    plant->next = NULL;

    jeu->grille[x-1][y-1].ptr = plant;
    jeu->grille[x-1][y-1].type = PLANT;

    Plant* prevPlant = NULL;
    Plant* curPlant = jeu->plants;
    if (curPlant == NULL) {
        jeu->plants = plant;
    } else {
        while (curPlant->next != NULL) {
            prevPlant = curPlant;
            curPlant = curPlant->next;
        }
        plant->prev = curPlant;
        curPlant->next = plant;
    }

    return true;
}

void towersAttack(Jeu* jeu)
{
    Plant* curPlant = jeu->plants;

    while (curPlant != NULL)
    {
        if (curPlant->type == BASIC)
        {
            basicTowerAttack(jeu, curPlant);
        }
        else if (curPlant->type == NUKE)
        {
            if (jeu->tour == curPlant->nukeTriggerTurn)
            {
                nukeTowerAttack(jeu, curPlant);
            }
        }
        curPlant = curPlant->next;
    }
}

void basicTowerAttack(Jeu* jeu, Plant* plant)
{
    const int ligne = plant->ligne;

    for(int i = plant->position+1; i <= NUM_COLS; i++)
    {
        // iterate till we find the zombie in the same ligne as the plant and
        // zombie is in the displayable arena
        if (jeu->grille[i-1][ligne-1].type == ZOMBIE)
        {
            Zombie* damagedZombie = (Zombie*)jeu->grille[i-1][ligne-1].ptr;
            damagedZombie->pointsDeVie--;

            if (damagedZombie->pointsDeVie <= 0)
            {
                jeu->grille[i-1][ligne-1].ptr = NULL;
                jeu->grille[i-1][ligne-1].type = VIDE;

                // Correct the pointers of the dead zombie
                if (damagedZombie == jeu->zombies)
                {
                    jeu->zombies = damagedZombie->next;
                    damagedZombie->next->prev = NULL;
                }
                else
                { 
                    damagedZombie->prev->next = damagedZombie->next;
                    damagedZombie->next->prev = damagedZombie->prev;
                }

                if (damagedZombie->next_line != NULL)
                    damagedZombie->next_line->prev_line = damagedZombie->prev_line;

                if (damagedZombie->prev_line != NULL)
                     damagedZombie->prev_line->next_line = damagedZombie->next_line;

                free(damagedZombie);
            }
            // We can return here since basic tower attacks the first eutudiant in the ligne
            return;
        }
    }
}

void nukeTowerAttack (Jeu* jeu, Plant* plant)
{
    for(int i = 1; i <= NUM_COLS; i++)
    {
        for (int j=1; j<=NUM_LIGNES; j++)
        {
            if (jeu->grille[i-1][j-1].type == ZOMBIE)
            {
                Zombie* damagedZombie = (Zombie*)jeu->grille[i-1][j-1].ptr;
                damagedZombie->pointsDeVie = 0;

                if (damagedZombie->pointsDeVie <= 0)
                {
                    jeu->grille[i-1][j-1].ptr = NULL;
                    jeu->grille[i-1][j-1].type = VIDE;

                    // Correct the pointers of the dead zombie
                    if (damagedZombie == jeu->zombies)
                    {
                        jeu->zombies = damagedZombie->next;
                        damagedZombie->next->prev = NULL;
                    }
                    else
                    { 
                        damagedZombie->prev->next = damagedZombie->next;
                        damagedZombie->next->prev = damagedZombie->prev;
                    }

                    if (damagedZombie->next_line != NULL)
                        damagedZombie->next_line->prev_line = damagedZombie->prev_line;

                    if (damagedZombie->prev_line != NULL)
                        damagedZombie->prev_line->next_line = damagedZombie->next_line;

                    free(damagedZombie);
                }
            }
        }
    }
}

void zombieAttack(Jeu* jeu, Zombie* zombie, Plant* plant)
{
    if(zombie->type == NORMAL)
        plant->pointsDeVie -= 1;

    if (plant->pointsDeVie <= 0)
    {
        Plant* prevPlant = plant->prev;

        jeu->grille[plant->position-1][plant->ligne-1].ptr = NULL;
        jeu->grille[plant->position-1][plant->ligne-1].type = VIDE;
        
        // Case of plant to be deleted is the first one added
        if (prevPlant == NULL)
            jeu->plants = plant->next;

        // Case of plant to be deleted is a later added one
        else
            prevPlant->next = plant->next;

        // Fix the prev pointer of the next plant
        if (plant->next != NULL)
            (plant->next)->prev = prevPlant;

        free(plant);         
    }
}

// hello
void freeJeu(Jeu* jeu)
{
    Zombie* curZombie = jeu->zombies;
    Zombie* nextZombie = NULL;

    while (curZombie != NULL)
    {
        nextZombie = curZombie->next;
        free(curZombie);
        curZombie = nextZombie;
    }

    Plant* curPlant = jeu->plants;
    Plant* nextPlant = NULL;

    while (curPlant != NULL)
    {
        nextPlant = curPlant->next;
        free(curPlant);
        curPlant = nextPlant;
    }

    free(jeu);
}