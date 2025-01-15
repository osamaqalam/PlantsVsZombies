#include "jeu.h"

void initJeu(Jeu *jeu, int cagnotte) 
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

void printTour (const Jeu *jeu)
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

            sscanf(buffer, "%d %d %c", &etudiant->tour, &etudiant->ligne, &type);
            etudiant->position = NUM_COLS+etudiant->tour-1; // Start 

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
                lastEtudiant->next = etudiant;
            else
            { 
                //printf("etudiants was initialized with %d %d %d", etudiant->tour, etudiant->ligne, etudiant->type);
                jeu->etudiants = etudiant;
            }

            lastEtudiant = etudiant;
        }
        // Move to the next line
        current += strlen(buffer) + 1;
        lineCount++;

    }
    lastEtudiant->next = NULL;
    return lineCount;
}


// If there is an object in the path of the etudiant, it will be stored in firstObj
void encounteredObject(Jeu* jeu, Etudiant* movingEtudiant, CellPointer* firstObj)
{
    Etudiant* curEtudiant = jeu->etudiants;
    Tourelle* curTourelle = jeu->tourelles;

    while (curEtudiant != NULL)
    {
        if (curEtudiant != movingEtudiant && curEtudiant->ligne == movingEtudiant->ligne &&
            curEtudiant->position <= movingEtudiant->position && curEtudiant->position >= movingEtudiant->position-STEP_SIZE[movingEtudiant->type])
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
         && curTourelle->position >= movingEtudiant->position-STEP_SIZE[movingEtudiant->type])
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
}

bool checkGameOver(Jeu *jeu)
{
    Etudiant* curEtudiant = jeu->etudiants;

    while (curEtudiant != NULL)
    {
        if (curEtudiant->position <= 0)
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
bool placeTourelle(Jeu* jeu, int type, int x, int y) 
{

    Tourelle* tourelle = (Tourelle*)malloc(sizeof(Tourelle));
    tourelle->type = type;
    if (type == 0)
        tourelle->pointsDeVie = 3;
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
        if (curTourelle->type == 0)
        {
            basicTowerAttack(jeu, curTourelle);
        }
        curTourelle = curTourelle->next;
    }
}

void basicTowerAttack(Jeu* jeu, Tourelle* tourelle)
{
    Etudiant* prevEtudiant = NULL;
    Etudiant* curEtudiant = jeu->etudiants;

    while (curEtudiant != NULL)
    {
        // iterate till we find the etudiant in the same ligne as the tourelle
        if (tourelle->ligne == curEtudiant->ligne)
        {
            curEtudiant->pointsDeVie--;
            if (curEtudiant->pointsDeVie <= 0)
            {
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].ptr = NULL;
                jeu->grille[curEtudiant->position-1][curEtudiant->ligne-1].type = VIDE;

                if (prevEtudiant == NULL)
                {
                    jeu->etudiants = curEtudiant->next;
                    free(curEtudiant);
                }
                else
                { 
                    prevEtudiant->next = curEtudiant->next;
                    free(curEtudiant);
                }
            }
            // We can return here since basic tower attacks the first eutudiant in the ligne
            return;
        }
        prevEtudiant = curEtudiant;
        curEtudiant = curEtudiant->next;
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
