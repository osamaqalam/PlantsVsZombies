#include "jeu.h"

void printTour (const Jeu *jeu)
{
    Etudiant* curEtudiant = jeu->etudiants;

    printf("Tour %d\n", jeu->tour);

    for (int ligne = 1; ligne <= 7; ligne++) {
        printf("%d", ligne);
        printf("|   ");
        for (int pos = 1; pos <= 15; pos++) {
            //printf("pos1+1:%d==curEtudiant->position=%d\n", pos+1,  curEtudiant->position);
            if (jeu->tour == curEtudiant->tour && ligne == curEtudiant->ligne
                && pos == curEtudiant->position)
            {
                printf("%dZ  ", curEtudiant->pointsDeVie);
                curEtudiant = curEtudiant->next;
            }
            else
                printf(".   ");
        }

        printf("\n");
    }
}

char* readFile(const char* filePath) {
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

    // Debugging: Print file size and bytes read
    // printf("Expected file size: %ld bytes\n", fileSize);
    // printf("Bytes read: %zu bytes\n", bytesRead);

    return buffer;
}

int parseFileContent(const char* fileContents, Jeu *jeu)
{
    // Parse the file contents
    int lineCount = 0; // Tracks the number of parsed lines
    char buffer[MAX_LINE_LENGTH];
    const char* current = fileContents;
    Etudiant* lastEtudiant = NULL;

    // Loop while there is a line to read
    // sscanf will store whatever is right after % in the later arguments
    while (sscanf(current, "%[^\n]\n", buffer) == 1) {
        //read the cagnotte on first iteration
        if (lineCount==0)
        {
            sscanf(buffer, "%d", &jeu->cagnotte);
        }
        else
        {
            char type;

            // Dynamically allocate memory for an instance of Etudiant
            Etudiant* etudiant = (Etudiant*)malloc(sizeof(Etudiant));
            if (etudiant == NULL) {
                perror("Error allocating memory for Etudiant");
                return -1; // Return an error code
            }

            sscanf(buffer, "%d %d %c", &etudiant->tour, &etudiant->ligne, &type);
            etudiant->position = NUM_COLS; // Start 

            if (type == 'Z') 
            {
                etudiant->type = 0;
                etudiant->pointsDeVie = 3;
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

    return lineCount;
}

