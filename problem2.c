#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mpi.h"

char Encrypt(char c) {
    if(islower(c))return (char)((((c - 'a') + 3) % 26) + 'a');
    return (char)((((c - 'A') + 3) % 26) + 'A');    
}

char Decrypt(char c) {
    if(islower(c))return ((((c - 'a') - 3 + 26) % 26) + 'a');
    return ((((c - 'A') - 3 + 26) % 26) + 'A');
}

int main(int argc, char *argv[]) {
    int my_rank;
    int p;
    int range;
    FILE *file;
    char str[200];
    char fileName[200];
    int start, end;
    char temp[200]; // Use an appropriate size for temporary substring
    char result[400] = "";
    int option;
    int inputOption;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        printf("If you want to read from file enter 1 otherwise enter 2 \n");
        scanf("%d", &inputOption);
        if(inputOption == 2)
        {
        printf("Enter the String: \n");
        scanf("%s", str);
        getchar();
        }
        else 
        {
        printf("Enter the file name: \n");
        scanf("%s", fileName);
        file = fopen(fileName, "r");

        if (file == NULL) {
            perror("Error opening the file");
            return EXIT_FAILURE;
        }
         if (fgets(str, 200, file) != NULL) {
        // Remove newline character if present
        if (str[strlen(str) - 1] == '\n')
            str[strlen(str) - 1] = '\0';
        
        printf("String read from file: %s\n", str);
        }
         else {
            printf("Error reading from file.\n");
            return EXIT_FAILURE;
        }

        // Close the file
        fclose(file);

        }
        printf("If you want to encrypt enter 1 otherwise enter 2 \n");
        scanf("%d", &option);

        int str_length = strlen(str);
        range = (str_length + (p - 2)) / (p - 1);
        int i;
        for (i = 1; i < p; i++) {
            MPI_Send(&range, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&option, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(str + (i - 1) * range, range, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        for ( i = 1; i < p; i++) {
            MPI_Recv(temp, range, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            strcat(result, temp);
        }

        printf("Total processed string is: %s\n", result);

    } else {
        MPI_Recv(&range, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&option, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive substring into temporary buffer
        MPI_Recv(temp, range, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Process the substring
        int i;
        for ( i = 0; i < range && temp[i] != '\0'; i++) {
            if (option == 1) {
                temp[i] = Encrypt(temp[i]);
            } else {
                temp[i] = Decrypt(temp[i]);
            }
        }

        // Send processed substring back to master process
        MPI_Send(temp, range, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
