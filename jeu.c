#include "jeu.h"

void printGame (const Jeu *j)
{
    printf("Tour %d\n", j->tour);

    for (int i = 0; i < 7; i++) {
        printf("%d", i+1);
        printf("|   ");
        for (int j = 0; j < 15; j++) {
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


