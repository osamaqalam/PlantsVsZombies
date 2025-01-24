#include "jeu.h"

void initJeu(Jeu* restrict jeu, int cagnotte) 
{
    jeu->tour = 1;
    jeu->cagnotte = cagnotte;
    jeu->zombies = NULL;
    jeu->tourelles = NULL;

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
            
            // Check if a cell is occupied by a tourelle
            else if (jeu->grille[pos-1][ligne-1].type == TOURELLE)
            {
                Tourelle* tourelle = (Tourelle*)jeu->grille[pos-1][ligne-1].ptr;
                if (tourelle->type == BASIC)
                    printf("%dT  ", tourelle->pointsDeVie);
                else if (tourelle->type == NUKE)
                    printf("%dN  ", tourelle->pointsDeVie);
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
    Tourelle* curTourelle = jeu->tourelles;

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

    while (curTourelle != NULL)
    {
        if (curTourelle->ligne == movingZombie->ligne && curTourelle->position <= movingZombie->position
         && curTourelle->position >= FinalPosition)
        {
            firstObj->ptr = curTourelle;
            firstObj->type = TOURELLE;
            return;
        }
        curTourelle = curTourelle->next;
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
        else if (cell->type == TOURELLE)
        {
            Tourelle* tourelle = (Tourelle*)cell->ptr;
            zombieAttack(jeu, curZombie, tourelle);
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
bool placeTourelle(Jeu* restrict jeu, enum TourelleType type, int x, int y) 
{
    if(jeu->grille[x-1][y-1].type != VIDE ||
        x < 1 || x >= NUM_COLS || y < 1 || y > NUM_LIGNES)
        return false;

    Tourelle* tourelle = (Tourelle*)malloc(sizeof(Tourelle));
    tourelle->type = type;

    if (type == BASIC)
        tourelle->pointsDeVie = 3;
    else if (type == NUKE)
    {
        tourelle->pointsDeVie = 1;
        
        srand(time(NULL)); // use current time as seed for random generator
        tourelle->nukeTriggerTurn = (rand() % 5) + 1;
    }

    tourelle->ligne = y;
    tourelle->position = x;
    tourelle->prix = TOURELLE_PRICES[type];
    tourelle->prev = NULL;
    tourelle->next = NULL;

    jeu->grille[x-1][y-1].ptr = tourelle;
    jeu->grille[x-1][y-1].type = TOURELLE;

    Tourelle* prevTourelle = NULL;
    Tourelle* curTourelle = jeu->tourelles;
    if (curTourelle == NULL) {
        jeu->tourelles = tourelle;
    } else {
        while (curTourelle->next != NULL) {
            prevTourelle = curTourelle;
            curTourelle = curTourelle->next;
        }
        tourelle->prev = curTourelle;
        curTourelle->next = tourelle;
    }

    return true;
}

void towersAttack(Jeu* jeu)
{
    Tourelle* curTourelle = jeu->tourelles;

    while (curTourelle != NULL)
    {
        if (curTourelle->type == BASIC)
        {
            basicTowerAttack(jeu, curTourelle);
        }
        else if (curTourelle->type == NUKE)
        {
            if (jeu->tour == curTourelle->nukeTriggerTurn)
            {
                nukeTowerAttack(jeu, curTourelle);
            }
        }
        curTourelle = curTourelle->next;
    }
}

void basicTowerAttack(Jeu* jeu, Tourelle* tourelle)
{
    const int ligne = tourelle->ligne;

    for(int i = tourelle->position+1; i <= NUM_COLS; i++)
    {
        // iterate till we find the zombie in the same ligne as the tourelle and
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

void nukeTowerAttack (Jeu* jeu, Tourelle* tourelle)
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

void zombieAttack(Jeu* jeu, Zombie* zombie, Tourelle* tourelle)
{
    if(zombie->type == NORMAL)
        tourelle->pointsDeVie -= 1;

    if (tourelle->pointsDeVie <= 0)
    {
        Tourelle* prevTourelle = tourelle->prev;

        jeu->grille[tourelle->position-1][tourelle->ligne-1].ptr = NULL;
        jeu->grille[tourelle->position-1][tourelle->ligne-1].type = VIDE;
        
        // Case of tourelle to be deleted is the first one added
        if (prevTourelle == NULL)
            jeu->tourelles = tourelle->next;

        // Case of tourelle to be deleted is a later added one
        else
            prevTourelle->next = tourelle->next;

        // Fix the prev pointer of the next tourelle
        if (tourelle->next != NULL)
            (tourelle->next)->prev = prevTourelle;

        free(tourelle);         
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

    Tourelle* curTourelle = jeu->tourelles;
    Tourelle* nextTourelle = NULL;

    while (curTourelle != NULL)
    {
        nextTourelle = curTourelle->next;
        free(curTourelle);
        curTourelle = nextTourelle;
    }

    free(jeu);
}