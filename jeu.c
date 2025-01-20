#include "jeu.h"

void initJeu(Jeu* restrict jeu, int cagnotte) 
{
    jeu->tour = 1;
    jeu->cagnotte = cagnotte;
    jeu->etudiants = NULL;
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
    printf("Tour %d\n", jeu->tour);

    for (int ligne = 1; ligne <= 7; ligne++) {
        printf("%d", ligne);
        printf("|   ");
        for (int pos = 1; pos <= 15; pos++) {
        
            // Check if a cell is occupied by an etudiant
            if(jeu->grille[pos-1][ligne-1].type == ETUDIANT)
            {
                Etudiant* etudiant = (Etudiant*)jeu->grille[pos-1][ligne-1].ptr;
                if (etudiant->type == NORMAL)
                    printf("%dZ  ", etudiant->pointsDeVie);
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
        Etudiant* curEtudiant = jeu->etudiants;
        Etudiant* prevEtudiantLigne = NULL;

        while(curEtudiant)
        {
            if (curEtudiant->ligne == i)
            {
                if (prevEtudiantLigne)
                {
                    prevEtudiantLigne->next_line = curEtudiant;
                    curEtudiant->prev_line = prevEtudiantLigne;
                }
                else
                {
                    curEtudiant->prev_line = NULL;
                }
                prevEtudiantLigne = curEtudiant;
            }
            curEtudiant = curEtudiant->next;
        }
    }
}

int parseFileContent(const char* fileContents, Jeu *jeu)
{
    // Parse the file contents
    int lineCount = 0; // Tracks the number of parsed lines
    char buffer[MAX_LINE_LENGTH];
    const char* current = fileContents;
    Etudiant* etudiant = NULL;
    Etudiant* lastEtudiant = NULL;

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

            // Dynamically allocate memory for an instance of Etudiant
            etudiant = (Etudiant*)malloc(sizeof(Etudiant));
            if (etudiant == NULL) {
                perror("Error allocating memory for Etudiant");
                return -1; // Return an error code
            }

            initEtudiant(etudiant);

            sscanf(buffer, "%d %d %c", &etudiant->tour, &etudiant->ligne, &type);
            etudiant->position = NUM_COLS+etudiant->tour; // Start 

            if (type == 'Z') 
            {
                etudiant->type = NORMAL;
                etudiant->pointsDeVie = 5;
            }

            if (etudiant->position <= NUM_COLS)
            {
                jeu->grille[etudiant->position-1][etudiant->ligne-1].ptr = etudiant;
                jeu->grille[etudiant->position-1][etudiant->ligne-1].type = ETUDIANT;
            }

            // if not first etudiant
            if (lastEtudiant) 
            {
                etudiant->prev = lastEtudiant;
                lastEtudiant->next = etudiant;
            }
            else
            {
                etudiant->prev = NULL;
                jeu->etudiants = etudiant;
            }

            lastEtudiant = etudiant;
        }
        // Move to the next line
        current += strlen(buffer) + 1;
        lineCount++;

    }
    lastEtudiant->next = NULL;

    connectLinesPtrs(jeu);

    return lineCount;
}

// If there is an object in the path of the etudiant, it will be stored in firstObj
void encounteredObject(Jeu* jeu, Etudiant* movingEtudiant, CellPointer* firstObj)
{
    int FinalPosition = movingEtudiant->position - STEP_SIZE[movingEtudiant->type];
    Etudiant* curEtudiant = jeu->etudiants;
    Tourelle* curTourelle = jeu->tourelles;

    while (curEtudiant != NULL)
    {
        if (curEtudiant != movingEtudiant && curEtudiant->ligne == movingEtudiant->ligne &&
            curEtudiant->position <= movingEtudiant->position && curEtudiant->position >= FinalPosition)
        {
            firstObj->ptr = curEtudiant;
            firstObj->type = ETUDIANT;
            return;
        }
        curEtudiant = curEtudiant->next;
    }

    while (curTourelle != NULL)
    {
        if (curTourelle->ligne == movingEtudiant->ligne && curTourelle->position <= movingEtudiant->position
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

void moveEtudiants(Jeu *jeu)
{
    Etudiant* curEtudiant = jeu->etudiants;
    CellPointer* cell = malloc(sizeof(CellPointer));

    while (curEtudiant != NULL)
    {   
        encounteredObject(jeu, curEtudiant, cell);

        // Only move etudiant if the path is clear
        if(cell->type == VIDE)
        {    
            // Clear the current cell if the etudiant is in the range of visible arena
            if (curEtudiant->position - 1 >= 0 && curEtudiant->position - 1 < NUM_COLS)
            {
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].ptr = NULL;
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].type = VIDE;
            }

            curEtudiant->position = curEtudiant->position - STEP_SIZE[curEtudiant->type];

            // Set the new cell if the etudiant is in the range of visible arena
            if (curEtudiant->position - 1 >= 0 && curEtudiant->position - 1 < NUM_COLS)
            {
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].ptr = curEtudiant;
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].type = ETUDIANT;
            }
        }
        else if (cell->type == TOURELLE)
        {
            Tourelle* tourelle = (Tourelle*)cell->ptr;
            etudiantAttack(jeu, curEtudiant, tourelle);
        }
        curEtudiant = curEtudiant->next;   
    }
    free(cell);
}

bool checkGameOver(Jeu *jeu)
{
    Etudiant* curEtudiant = jeu->etudiants;

    while (curEtudiant != NULL)
    {
        if (curEtudiant->position <= 1)
        {
            printf("GAME OVER - YOU LOSE!\n");
            return true;
        }
        curEtudiant = curEtudiant->next;
    }

    if (jeu->etudiants == NULL)
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
        // iterate till we find the etudiant in the same ligne as the tourelle and
        // etudiant is in the displayable arena
        if (jeu->grille[i-1][ligne-1].type == ETUDIANT)
        {
            Etudiant* damagedEtudiant = (Etudiant*)jeu->grille[i-1][ligne-1].ptr;
            damagedEtudiant->pointsDeVie--;

            if (damagedEtudiant->pointsDeVie <= 0)
            {
                jeu->grille[i-1][ligne-1].ptr = NULL;
                jeu->grille[i-1][ligne-1].type = VIDE;

                // Correct the pointers of the dead etudiant
                if (damagedEtudiant == jeu->etudiants)
                {
                    jeu->etudiants = damagedEtudiant->next;
                    damagedEtudiant->next->prev = NULL;
                }
                else
                { 
                    damagedEtudiant->prev->next = damagedEtudiant->next;
                    damagedEtudiant->next->prev = damagedEtudiant->prev;
                }

                if (damagedEtudiant->next_line != NULL)
                    damagedEtudiant->next_line->prev_line = damagedEtudiant->prev_line;

                if (damagedEtudiant->prev_line != NULL)
                     damagedEtudiant->prev_line->next_line = damagedEtudiant->next_line;

                free(damagedEtudiant);
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
            if (jeu->grille[i-1][j-1].type == ETUDIANT)
            {
                Etudiant* damagedEtudiant = (Etudiant*)jeu->grille[i-1][j-1].ptr;
                damagedEtudiant->pointsDeVie = 0;

                if (damagedEtudiant->pointsDeVie <= 0)
                {
                    jeu->grille[i-1][j-1].ptr = NULL;
                    jeu->grille[i-1][j-1].type = VIDE;

                    // Correct the pointers of the dead etudiant
                    if (damagedEtudiant == jeu->etudiants)
                    {
                        jeu->etudiants = damagedEtudiant->next;
                        damagedEtudiant->next->prev = NULL;
                    }
                    else
                    { 
                        damagedEtudiant->prev->next = damagedEtudiant->next;
                        damagedEtudiant->next->prev = damagedEtudiant->prev;
                    }

                    if (damagedEtudiant->next_line != NULL)
                        damagedEtudiant->next_line->prev_line = damagedEtudiant->prev_line;

                    if (damagedEtudiant->prev_line != NULL)
                        damagedEtudiant->prev_line->next_line = damagedEtudiant->next_line;

                    free(damagedEtudiant);
                }
            }
        }
    }
}

void etudiantAttack(Jeu* jeu, Etudiant* etudiant, Tourelle* tourelle)
{
    if(etudiant->type == NORMAL)
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

void freeJeu(Jeu* jeu)
{
    Etudiant* curEtudiant = jeu->etudiants;
    Etudiant* nextEtudiant = NULL;

    while (curEtudiant != NULL)
    {
        nextEtudiant = curEtudiant->next;
        free(curEtudiant);
        curEtudiant = nextEtudiant;
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