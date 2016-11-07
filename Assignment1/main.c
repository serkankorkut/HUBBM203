#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {

    /* ' ' for free seats 's' for students 'f' for full fare */
    char **seats;


    char *movieName;

    int height, width;


} seatMap;


typedef struct Hall {
    char *name;

    seatMap map;

    char *movie;

    struct Hall *next;

    int studentCount, fullCount;


} hall;


char **fileToLines(char *filePath, int *maxCharCount,int *);

hall *createHall(char *name, char *movie, int width, int height);

void commandController(char **buffer, int maxCharCount, hall **head,int );


void hallLinkedList(hall **head, hall *dummy);

hall *findHall(hall *head, char *movieOrName, char mode);

int *convertLabelToIndex(char *label);

void printHall(hall *h, char *filePath);

void buyTicket(char *movie, char *seatLabel, char *ticketType, int numberOfSeats, hall *h, hall **, char *filePath);

void showStats(hall *head, char *filePath);

void cancelTicket(hall **head, char *movieName, char *seatLabel, char *filePath, hall *dummy);

char defaultValue = ' ';

int main(int argc, char const *argv[]) {


    int maxCharCount = 0;

    char **buffer;
int lineCount=0;
    hall *head = NULL;


    buffer = fileToLines((char*)argv[1], &maxCharCount,&lineCount);


    commandController(buffer, maxCharCount, &head,lineCount);



    free(buffer);

    return 0;
}


/* A function that takes file path as an argument and creates a 2 dimensional dynamic array as lines in the file */
char **fileToLines(char *filePath, int *maxCharCount,int * lineCount) {

    FILE *filePointer;

    int i = 0;

    char line[100];
    char **lines;

    filePointer = fopen(filePath, "r");
    if (filePointer != NULL) {

        /*Count lines for know how much to allocate*/
        while (fgets(line, sizeof(line), filePointer) != NULL) {
            /*Find max character count for a line */
            if (strlen(line) > *maxCharCount) {
                *maxCharCount = (int) strlen(line);
            }

            (*lineCount)++;

        }




        /*Actually allocate*/

        lines = (char **) calloc(((*lineCount) + 1), sizeof(char *));

        for (i = 0; i < (*lineCount) + 1; i++) {
            lines[i] = (char *) calloc((*maxCharCount + 1), sizeof(char));
        }

        /* Reinitalize file pointer */

        i = 0;
        filePointer = fopen(filePath, "r");

        /*Actually fill lines*/
        while (fgets(lines[i], *maxCharCount + 1, filePointer) != NULL) {


            i++;


        }


        fclose(filePointer);



        return lines;


    }
    else {
        printf("Someone did something nasty and forget to set the file path \n");

        return 0;
    }


}





hall *createHall(char *name, char *movie, int width, int height) {
    int i, j;
    hall *h = (hall *) calloc(1, sizeof(hall));

    h->movie = (char *) calloc((strlen(movie)) + 1, sizeof(char));

    strcpy(h->movie, movie);

    h->next = NULL;

    h->name = (char *) calloc((strlen(name) + 1), sizeof(char));

    strcpy(h->name, name);


    h->map.movieName = (char *) calloc(strlen(movie) + 1, sizeof(char));

    strcpy(h->map.movieName, movie);

    h->map.height = height;
    h->map.width = width;


    h->map.seats = (char **) calloc((height), sizeof(char *));


    for (i = 0; i < height; i++) {
        h->map.seats[i] = (char *) calloc((width), sizeof(char));


    }


    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            h->map.seats[i][j] = defaultValue;


    return h;
}


void hallLinkedList(hall **head, hall *dummy) {
    hall *temp;
    hall *curr;
    int i;

    temp = (hall *) calloc(1, sizeof(hall));
    temp->movie = (char *) calloc(strlen(dummy->movie) + 1, sizeof(char));
    temp->name = (char *) calloc(strlen(dummy->name) + 1, sizeof(char));


    curr = *head;

    if (*head == NULL) {
        (*head) = (hall *) calloc(1, sizeof(hall));


        (*head)->next = NULL;
        (*head)->movie = (char *) calloc(strlen(dummy->movie) + 1, sizeof(char));
        (*head)->name = (char *) calloc(strlen(dummy->name) + 1, sizeof(char));


        strcpy((*head)->movie, dummy->movie);
        strcpy((*head)->name, dummy->name);

        (*head)->map.seats = (char **) calloc((dummy->map.height), sizeof(char *));


        for (i = 0; i < dummy->map.height; i++) {
            (*head)->map.seats[i] = (char *) calloc((dummy->map.width), sizeof(char));


        }

        (*head)->map = dummy->map;


    }
    else {

        temp = dummy;

        while (curr->next != NULL) {
            curr = curr->next;
        }

        curr->next = temp;


    }


}

hall *findHall(hall *head, char *movieOrName, char mode) {

    /* Mode is for you to choose find hall by name or movie */

    if (mode == 'm') {
        if (movieOrName == NULL) {
            printf("Movie name cannot be null\n");
        }
        else {
            if (strcmp(movieOrName, (head)->movie) == 0) {
                return head;
            }

            while ((head)->next != NULL) {
                head = (head)->next;
                if (strcmp(movieOrName, (head)->movie) == 0)
                    return head;
            }


        }


        return NULL;

    }
    if (mode == 'n') {
        if (movieOrName == NULL) {
            printf("Movie name cannot be null\n");
        }
        else {

            if (strcmp(movieOrName, (head)->name) == 0) {
                return head;
            }

            while ((head)->next != NULL) {
                head = (head)->next;
                if (strcmp(movieOrName, (head)->name) == 0)
                    return head;
            }

        }

        return NULL;
    }

    return  NULL;

}

void buyTicket(char *movie, char *seatLabel, char *ticketType, int numberOfSeats, hall *h, hall **head,
               char *filePath) {

    int *rowAndColumn;

    FILE *filePointer;

    filePointer = fopen(filePath, "a+");

    hall *curr;
    int counter = 0;
    int hasSold=0;


    curr = *head;


    h = findHall(h, movie, 'm');

    if (h == NULL) {
        return;
    }


    rowAndColumn = convertLabelToIndex(seatLabel);

/* First, check if it are exists  */
    if (h->map.height <= rowAndColumn[0] || h->map.width <= rowAndColumn[1]) {
        fprintf(filePointer, "ERROR:Seat %s is not defined at \"%s\"\n", seatLabel, h->name);
        printf("ERROR:Seat %s is not defined at \"%s\"\n", seatLabel, h->name);
    }
    else if (h->map.seats[rowAndColumn[0]][rowAndColumn[1]] == 's' ||
             h->map.seats[rowAndColumn[0]][rowAndColumn[1]] == 'f') {
        fprintf(filePointer, "ERROR:Specified seat(s) %s in %s are not available! They have been already taken.\n",
               seatLabel, h->name);

        printf("ERROR:Specified seat(s) %s in %s are not available! They have been already taken.\n",
                seatLabel, h->name);


    }
    else if (h->map.seats[rowAndColumn[0]][rowAndColumn[1] ] == defaultValue)
    {

        int i;
        int dull = numberOfSeats;

        if (numberOfSeats == 1)
        {
            if (strcmp(ticketType, "Student") == 0) {
                h->map.seats[rowAndColumn[0]][rowAndColumn[1]] = 's';

                h->studentCount++;
                hasSold++;

            }
            else if (strcmp(ticketType, "FullFare") == 0) {
                h->map.seats[rowAndColumn[0]][rowAndColumn[1]] = 'f';

                h->fullCount++;
                hasSold++;

            }
            fprintf(filePointer, "%s [%s] Seat(s) %s successfully sold.\n", h->name, h->movie, seatLabel);
            printf("%s [%s] Seat(s) %s successfully sold.\n", h->name, h->movie, seatLabel);


        }
        else if (h->map.seats[rowAndColumn[0]][rowAndColumn[1] + numberOfSeats -1  ] == defaultValue &&
            rowAndColumn[1] + numberOfSeats <= h->map.width)
        {
            /*Then check n seat from outside to inside  */
            while (dull != 0) {
                dull--;

                if (h->map.seats[rowAndColumn[0]][rowAndColumn[1] + dull] == defaultValue)
                {
                    counter++;
                }
                else {
                    fprintf(filePointer,
                            "ERROR:Specified seat(s) %s in %s are not available! They have been already taken.\n",
                            seatLabel,h->name);

                    printf("ERROR:Specified seat(s) %s in %s are not available! They have been already taken.\n",
                            seatLabel,h->name);

                }

            }

            /* If it all avaliable check if student or full fare and assign */

            if (strcmp(ticketType, "Student") == 0) {
                for (i = 0; i < numberOfSeats; i++) {
                    h->map.seats[rowAndColumn[0] ][rowAndColumn[1] +i ] = 's';
                    h->studentCount++;
                    hasSold=1;

                }
            }
            else if ( strcmp(ticketType, "FullFare") == 0) {
                for (i = 0; i < numberOfSeats; i++)
                {
                    h->map.seats[rowAndColumn[0]][rowAndColumn[1] + i] = 'f';
                    h->fullCount++;
                    hasSold=1;

                }

            }



                fprintf(filePointer, "%s [%s] Seat(s) ", h->name, h->movie);
                printf("%s [%s] Seat(s) ", h->name, h->movie);


            for (i = 0; i < counter ; i++)
                {

                    if (strlen(seatLabel) <3 ) {

                        fprintf(filePointer, "%c%c ", seatLabel[0] +i, seatLabel[1] );

                        printf("%c%c ", seatLabel[0] +i, seatLabel[1] );


                    }
                    else
                    {
                        fprintf(filePointer, "%c", seatLabel[0]+i);
                        fprintf(filePointer,"%c%c ",seatLabel[1],seatLabel[2]);


                        printf("%c", seatLabel[0]+i);
                        printf("%c%c ",seatLabel[1],seatLabel[2]);
                    }


                }

                fprintf(filePointer, "successfully sold.\n");

                printf("successfully sold.\n");


            /* Here was the left control */


            *head = curr;


        }

        if(hasSold==0)
        {
            fprintf(filePointer,"ERROR: Specified seat(s) in %s are not available! They have been already taken.\n",h->name);
            printf("ERROR: Specified seat(s) in %s are not available! They have been already taken.\n",h->name);

        }

    }

    fclose(filePointer);

}


int *convertLabelToIndex(char *label) {
    int *rowAndColumn;

    rowAndColumn = (int *) calloc(2, (sizeof(int)));

    /* A is 65 in ASCII table so if it's A it will be 0th index */
    rowAndColumn[1] = ((int) label[0] - 65);

    /*Go one byte more to eliminate the letter */
    label++;

    rowAndColumn[0] = atoi(label) - 1;


    return rowAndColumn;

}


void printHall(hall *h, char *filePath) {
    int i, j;

    FILE *filePointer;


    filePointer = fopen(filePath, "a+");

    /* Printing to file*/

    fprintf(filePointer, "%s sitting plan\n", h->name);

    for (i = 0; i < h->map.height; i++) {
        fprintf(filePointer, "  ");

        for (j = 0; j < h->map.width; j++)
            fprintf(filePointer, "--");
        fprintf(filePointer, "-");
        fprintf(filePointer, "\n");
        if (i + 1 < 10)
            fprintf(filePointer, "%d ", i + 1);
        else
            fprintf(filePointer, "%d", i + 1);

        for (j = 0; j < h->map.width; j++) {
            fprintf(filePointer, "|%c", h->map.seats[i][j]);
        }
        fprintf(filePointer, "|\n");

    }


    fprintf(filePointer, "  ");
    for (j = 0; j < h->map.width; j++)
        fprintf(filePointer, "--");
    fprintf(filePointer, "-");

    fprintf(filePointer, "\n   ");
    for (i = 0; i < h->map.width; ++i) {
        fprintf(filePointer, "%c ", i + 65);

    }
    fprintf(filePointer, "\n");
    for (i = 0; i < h->map.width - 6; i++) {
        fprintf(filePointer, " ");
    }

    fprintf(filePointer, "C U R T A I N\n");

    /* Printing to console*/


    printf("%s sitting plan\n", h->name);

    for (i = 0; i < h->map.height; i++) {
        printf("  ");

        for (j = 0; j < h->map.width; j++)
            printf("--");
        printf("-");
        printf("\n");
        if (i + 1 < 10)
            printf("%d ", i + 1);
        else
            printf("%d", i + 1);

        for (j = 0; j < h->map.width; j++) {
            printf("|%c", h->map.seats[i][j]);
        }
        printf("|\n");

    }

    printf("  ");
    for (j = 0; j < h->map.width; j++)
        printf("--");
    printf("-");

    printf("\n   ");
    for (i = 0; i < h->map.width; ++i) {
        printf("%c ", i + 65);

    }
    printf("\n");
    for (i = 0; i < h->map.width - 6; i++) {
        printf(" ");
    }

    printf("C U R T A I N\n");

    fclose(filePointer);

}


void printStats(hall *head, char *filePath) {

    int studentFare = 7;
    int fullFare = 10;


    FILE *filePointer = fopen(filePath, "a");


    printf("Statistics\n");
    fprintf(filePointer, "Statistics\n");

    while (head != NULL) {


        printf("%s %d student(s), %d full fare(s), sum: %d TL\n", head->name, head->studentCount, head->fullCount,
               ((head->studentCount * studentFare) + (head->fullCount * fullFare)));

        fprintf(filePointer, "%s %d student(s), %d full fare(s), sum: %d TL\n", head->name, head->studentCount,
                head->fullCount, ((head->studentCount * studentFare) + (head->fullCount * fullFare)));


        head = head->next;
    }

    fclose(filePointer);


}

void cancelTicket(hall **head, char *movieName, char *seatLabel, char *filePath, hall *dummy) {

    FILE *filePointer = fopen(filePath, "a");


    int *rowAndColumn;

    rowAndColumn = convertLabelToIndex(seatLabel);


    dummy = findHall(*head, movieName, 'm');


    if (dummy == NULL) {
        printf("ERROR: Movie name cannot be empty\n");
        fprintf(filePointer, "ERROR: Movie name cannot be empty\n");
    }
    else {
        if ((dummy)->map.seats[rowAndColumn[0]][rowAndColumn[1]] == ' ') {
            printf("ERROR: Seat %s in %s was not sold.\n", seatLabel, movieName);
            fprintf(filePointer, "ERROR: Seat %s in %s was not sold.\n", seatLabel, (dummy)->name);


        }
        else if (dummy->map.seats[rowAndColumn[0]][rowAndColumn[1]] == 's' )
        {
            dummy->map.seats[rowAndColumn[0]][rowAndColumn[1]] = ' ';

            printf("%s [%s] purchase is cancelled. Seat %s is now free.\n", dummy->name, dummy->movie, seatLabel);
            fprintf(filePointer, "%s [%s] purchase is cancelled. Seat %s is now free.\n", dummy->name, dummy->movie,
                    seatLabel);
            dummy->studentCount--;

        }
        else if(dummy->map.seats[rowAndColumn[0]][rowAndColumn[1]] == 'f')
        {
            dummy->map.seats[rowAndColumn[0]][rowAndColumn[1]] = ' ';

            printf("%s [%s] purchase is cancelled. Seat %s is now free.\n", dummy->name, dummy->movie, seatLabel);
            fprintf(filePointer, "%s [%s] purchase is cancelled. Seat %s is now free.\n", dummy->name, dummy->movie,
                    seatLabel);
            dummy->fullCount--;
        }
    }


    fclose(filePointer);


}


void commandController(char **buffer, int maxCharCount, hall **head,int lineCount) {

    int i = 0;
    char *token[5];


    int j = 0;
    for (j = 0; j < 5; j++) {
        token[j] = (char *) calloc(maxCharCount + 10, sizeof(char));
    }

    int counterHall = 0;
    hall *dummy = *head;
    char *dummyLine;

    dummyLine = (char *) calloc(maxCharCount + 1, sizeof(char));

    if(i<=lineCount)
    {
        while (buffer[i] != NULL) {


            strcpy(dummyLine, buffer[i]);


            token[0] = strtok(dummyLine, " \n\r\"");

            if (token[0] != NULL) {
                if (!(strcmp(token[0], "CREATEHALL"))) {
                    int k = 1;


                    while (token[0] != NULL && k < 5) {
                        token[0] = strtok(NULL, " \n\r\"");

                        if(token[0]!=NULL)
                            strcpy(token[k], token[0]);

                        k++;
                    }

                    counterHall++;


                    dummy = createHall(token[1], token[2], atoi(token[3]), atoi(token[4]));

                    hallLinkedList(head, dummy);


                }
                else if (!(strcmp(token[0], "BUYTICKET"))) {
                    int k = 1;

                    while (token[0] != NULL && k < 5) {

                        token[k] = (char *) calloc(maxCharCount + 10, sizeof(char));

                        token[0] = strtok(NULL, " \n\r\"");

                        if(token[0]!=NULL)
                            strcpy(token[k], token[0]);

                        k++;
                    }


                    dummy = *head;

                    buyTicket(token[1], token[2], token[3], atoi(token[4]), dummy, head, "output.txt");


                }
                else if (!(strcmp(token[0], "CANCELTICKET"))) {

                    int k = 1;

                    while (token[0] != NULL && k < 3) {
                        token[0] = strtok(NULL, " \n\r");

                        if (strcmp(token[1], """") == 0) {
                            printf("Movie name cannot be empty\n");
                        }
                        else {
                            if(token[0]!=NULL)
                                strcpy(token[k], token[0]);

                        }

                        k++;
                    }
                    dummy = *head;

                    token[1] = strtok(token[1], "\"");

                         cancelTicket(head,token[1],token[2],"output.txt",dummy);



                }
                else if (!(strcmp(token[0], "SHOWHALL"))) {
                    int k = 1;

                    while (token[0] != NULL && k < 2) {
                        token[0] = strtok(NULL, " \n\r\"");

                        if(token[0]!=NULL)
                            strcpy(token[k], token[0]);

                        k++;
                    }

                    dummy = *head;
                    dummy = findHall(dummy, token[1], 'n');

                    printHall(dummy, "output.txt");

                }
                else if (!(strcmp(token[0], "STATISTICS"))) {


                    printStats(*head, "output.txt");

                }

            }


            i++;


        }
    }



}


