#include <stdio.h>
#include <string.h>
#include "mpi.h"

char Encrypt(char c) {
    return (char)((((c - 'a') + 3) % 26) + 'a');
}

char Decrypt(char c) {
    return ((((c - 'a') - 3 + 26) % 26) + 'a');
}

int main(int argc, char *argv[]) {
    int my_rank;
    int p;
    int range;
    char str[200];
    int start, end;
    char temp[200]; // Use an appropriate size for temporary substring
    char result[400] = "";
    int option;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        printf("Enter the String: \n");
        scanf("%s", str);
        getchar();
        printf("If you want to encrypt enter 1 otherwise enter 2 \n");
        scanf("%d", &option);

        int str_length = strlen(str);
        range = (str_length + (p - 2)) / (p - 1);

        for (int i = 1; i < p; i++) {
            MPI_Send(&range, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&option, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(str + (i - 1) * range, range, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < p; i++) {
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
        for (int i = 0; i < range && temp[i] != '\0'; i++) {
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
