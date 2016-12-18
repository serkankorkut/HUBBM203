#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{

    int size;

    int *values;


} stackInt;

typedef struct
{

    int size;

    char *values;


} stackOperotor;

typedef struct
{
    int size;

    char **elements;

    int frontIndex;
} queue;

queue initQueue(void);

void enqueue(queue *aQueue, char *string);

char *dequeue(queue *aQueue);

void printQueue(queue *aQueue, char *filePath);

int isHexDigit(char aChar);

char *hexToInt(char *expression);

int hexHasError(char *expression, queue *q);


char ***linesToToken(char **lines, char *delims, int tokenNumber, int linesCount, int *lineChars);

char ***readAsTokens(char *filePath, int *maxCharCount, int *lineCount, int tokenNumber, char *delims);

stackInt initStackInt(void);

int isOperator(char aChar);

int hasError(char *expression, queue *q);

char popStackOperator(stackOperotor *a);

char *hexCalculator(char *expression, queue *q);


char *intCalculator(char *expression, queue *q);

void pushStackInt(stackInt *a, int value);

int popStackInt(stackInt *a);

int topStackInt(stackInt a);

void pushStackOperator(stackOperotor *a, char value);

stackOperotor initStackOperator();

char topStackOperator(stackOperotor a);


int main(int argc, char const *argv[])
{


    char ***linesTokens;

    int expectedMaxToken = 3;

    int maxCharCount = 0;

    int lineCount = 0;


    linesTokens = readAsTokens((char *) argv[1], &maxCharCount,
                               &lineCount, expectedMaxToken, " \"\n\r");

    int i;

    queue q;

    q = initQueue();


    for (i = 0; i < lineCount; i++)
    {
        if (strcmp(linesTokens[i][0], "calculate") == 0)
        {
            if (strcmp(linesTokens[i][1], "integer") == 0)
            {
                char *dummy = intCalculator(linesTokens[i][2], &q);
                if (strcmp(dummy, "error") == 0)
                {

                }
                else
                {

                }
            }
            else if (strcmp(linesTokens[i][1], "hex") == 0)
            {
              /* CHANGED  */
              /* DELETED if(strcmp(dummy,"error")==0) */
                char *dummy = hexCalculator(linesTokens[i][2], &q);

                /* DELETED  */
                /*
                if (strcmp(dummy, "error") == 0)
                {

                }
                else
                {

                }

                */

                /* END OF DELETION */
            }
            else
            {
            }
        }
        else if (strcmp(linesTokens[i][0], "print") == 0)
        {
            printQueue(&q, (char *) argv[2]);
        }
        else
        {
            printf("Wrong command\n");
        }

    }


    return 0;
}

int operatorPrecedence(char operator)
{
    if (operator == '(')
    {
        return -7;
    }
    else if (operator == ')')
    {
        return -7;
    }
    else if (operator == '*')
    {
        return 3;
    }
    else if (operator == '/')
    {
        return 3;
    }
    else if (operator == '+')
    {
        return 2;
    }
    else if (operator == '-')
    {
        return 2;
    }
    else
    {
        /*   printf("Invalid operator %c in function operatorPresedence\n", operator); */
        return -1;
    }
}


char popStackOperator(stackOperotor *a)
{
    if (a->size > 0)
    {
        char *tmp = (char *) calloc(a->size, sizeof(char));
        int i;

        char retVal = a->values[a->size - 1];


        for (i = 0; i < a->size; i++)
        {
            tmp[i] = a->values[i];
        }

        free(a->values);

        a->size--;

        a->values = (char *) calloc(a->size, sizeof(char));

        for (i = 0; i < a->size; i++)
        {
            a->values[i] = tmp[i];
        }

        free(tmp);

        return retVal;

    }

    return 'z';


}

/*Function that reads file as tokens of lines for given parameters, the first tier of return value is for the line number,
 * the second is for token number and the third is for char's index in string */
char ***readAsTokens(char *filePath, int *maxCharCount, int *lineCount, int tokenNumber, char *delims)
{

    FILE *filePointer;
    /* We assume that every line's character count is smaller than 100  */
    int MAX_LINE_SIZE = 100;
    int i = 0;
    int *lineChars;
    char *line = (char *) calloc(MAX_LINE_SIZE, sizeof(char));
    char **lines;
    char ***buffer;

    filePointer = fopen(filePath, "r");
    if (filePointer != NULL)
    {

        /*Count lines for know how much to allocate*/
        while (fgets(line, MAX_LINE_SIZE, filePointer) != NULL)
        {
            if (*maxCharCount < (int) strlen(line) + 1)
            {
                *maxCharCount = (int) strlen(line) + 1;
            }

            (*lineCount)++;

        }


        lineChars = (int *) calloc((*lineCount), sizeof(int));

        filePointer = fopen(filePath, "r");
        i = 0;
        while (fgets(line, MAX_LINE_SIZE, filePointer) != NULL && i < *lineCount)
        {
            lineChars[i] = (int) strlen(line) + 1;
            i++;

        }



        /*Actually allocate*/

        lines = (char **) calloc(((*lineCount) + 1), sizeof(char *));

        for (i = 0; i < (*lineCount) + 1; i++)
        {
            lines[i] = (char *) calloc(lineChars[i], sizeof(char));
        }

        /* Reinitalize file pointer */

        i = 0;
        filePointer = fopen(filePath, "r");

        /*Actually fill lines*/
        while (fgets(lines[i], *maxCharCount, filePointer) != NULL)
        {


            i++;


        }


        fclose(filePointer);


        buffer = linesToToken(lines, delims, tokenNumber, *lineCount, lineChars);

        free(line);

        for (i = 0; i < *lineCount; i++)
        {
            free(lines[i]);
        }

        free(lines);

        return buffer;


    }
    else
    {
        printf("Someone did something nasty and forget to set the file path \n");

        return 0;
    }


}

/*Function that convert lines to tokens of lines for given parameters*/
char ***linesToToken(char **lines, char *delims, int tokenNumber, int linesCount, int *lineChars)
{
    char ***tokenOfLine = (char ***) calloc(linesCount, sizeof(char **));
    int i;
    for (i = 0; i < linesCount; i++)
    {
        tokenOfLine[i] = (char **) calloc(tokenNumber, sizeof(char *));

    }

    int j;

    for (i = 0; i < linesCount; i++)
    {
        for (j = 0; j < tokenNumber; j++)
        {
            tokenOfLine[i][j] = (char *) calloc(lineChars[i], sizeof(char));
        }
    }


    char **tokens;

    tokens = (char **) calloc(tokenNumber, sizeof(char *));

    for (i = 0; i < tokenNumber; i++)
    {
        tokens[i] = (char *) calloc(lineChars[i] + 1, sizeof(char));
    }

    i = 0;
    j = 0;

    while (lines[i] != NULL && i < linesCount)
    {
        j = 0;
        tokens[j] = strtok(lines[i], delims);

        while (tokens[j])
        {
            strcpy(tokenOfLine[i][j], tokens[j]);
            j++;
            tokens[j] = strtok(NULL, delims);


        }

        i++;
    }


    return tokenOfLine;


}

char *intCalculator(char *expression, queue *q)
{

    if (1 == hasError(expression, q))
    {
        return "error";
    }


    stackInt intStack = initStackInt();
    stackOperotor opStack = initStackOperator();
    int shouldMultiply = 0;
    int res = 0;

    int expSize = strlen(expression);
    int i;
    for (i = 0; i < expSize; i++)
    {
        if (isOperator(expression[i]) == 1)
        {
            if (operatorPrecedence(expression[i]) > operatorPrecedence(topStackOperator(opStack)))
            {
                if (i > 0 && i < strlen(expression))
                {

                    if ((expression[i] == '-' && isOperator(expression[i - 1]) != -1 &&
                         isOperator(expression[i + 1]) == -1) || ((expression[i] == '-' &&
                                                                   isOperator(expression[i + 1]) == 1)) ||
                        ((expression[i] == '-' &&
                          isOperator(expression[i - 1]) != -1)) || ((expression[i] == '-' &&
                                                                     expression[i + 2] == '*')) ||
                        ((expression[i] == '-' &&
                          expression[i + 2] == '/')))
                    {
                        /*int ajk = expression[i+1] -'0';
                         ajk = -ajk;

                        expression[i+1] = -expression[i+1];

                         */

                        shouldMultiply++;
                        if (isOperator(expression[i - 1]) != 1)
                            pushStackOperator(&opStack, '+');

                    }
                    else
                    {
                        pushStackOperator(&opStack, expression[i]);
                    }
                }

                if (i == 0)
                {
                    if ((expression[i] == '-' && isOperator(expression[i - 1]) != -1 &&
                         isOperator(expression[i + 1]) == -1) || ((expression[i] == '-' &&
                                                                   isOperator(expression[i + 1]) == 1)) ||
                        ((expression[i] == '-' &&
                          isOperator(expression[i - 1]) != -1)) || ((expression[i] == '-' &&
                                                                     expression[i + 2] == '*')) ||
                        ((expression[i] == '-' &&
                          expression[i + 2] == '/')))
                    {
                        /*int ajk = expression[i+1] -'0';
                         ajk = -ajk;

                        expression[i+1] = -expression[i+1];

                         */

                        shouldMultiply++;
                        if (isOperator(expression[i - 1]) != 1)
                            pushStackOperator(&opStack, '+');

                    }
                    else
                    {
                        pushStackOperator(&opStack, expression[i]);

                    }
                }


            }
            else
            {
                if (i > 0 && i < strlen(expression))
                {
                    if ((expression[i] == '-' && isOperator(expression[i - 1]) != -1 &&
                         isOperator(expression[i + 1]) == -1) || ((expression[i] == '-' &&
                                                                   isOperator(expression[i + 1]) == 1)) ||
                        ((expression[i] == '-' &&
                          isOperator(expression[i - 1]) != -1)) || ((expression[i] == '-' &&
                                                                     expression[i + 2] == '*')) ||
                        ((expression[i] == '-' &&
                          expression[i + 2] == '/')))
                    {


                        shouldMultiply++;
                        if (isOperator(expression[i - 1]) != 1)
                        {
                            /*CHANGED  */
                            /* ADDED */
                            if (intStack.size > 0)
                            {
                                char op = popStackOperator(&opStack);

                                int n1, n2;
                                if (intStack.size >= 2)
                                {
                                    n1 = popStackInt(&intStack);
                                    n2 = popStackInt(&intStack);

                                    if (op == '*')
                                    {
                                        res = n1 * n2;
                                    }
                                    else if (op == '/')
                                    {
                                        res = n2 / n1;
                                    }
                                    else if (op == '+')
                                    {
                                        res = n1 + n2;
                                    }
                                    else if (op == '-')
                                    {
                                        res = n2 - n1;
                                    }
                                }
                                else
                                {
                                    n1 = popStackInt(&intStack);

                                    if (op == '-')
                                    {
                                        n1 = -n1;
                                    }

                                    res = n1;
                                }

                                /* END OF ADDING */





                                /*  printf("n1 %d , n2 %d \n", n1, n2);

                                  printf("There occured a more precedence so this inter result is %d\n", res); */


                                /* CHANGED */

                                /*DELETED  pushStackOperator(&opStack, expression[i]); */

                                /* ADDED */
                                pushStackOperator(&opStack, '+');

                                pushStackInt(&intStack, res);

                                /* END OF ADDING */




                            }


                        }

                    }
                    else
                    {
                        if (intStack.size > 0)
                        {
                            char op = popStackOperator(&opStack);

                            int n1, n2;
                            if (intStack.size >= 2)
                            {
                                n1 = popStackInt(&intStack);
                                n2 = popStackInt(&intStack);


                                if (op == '*')
                                {
                                    res = n1 * n2;
                                }
                                else if (op == '/')
                                {
                                    res = n2 / n1;
                                }
                                else if (op == '+')
                                {
                                    res = n1 + n2;
                                }
                                else if (op == '-')
                                {
                                    res = n2 - n1;
                                }

                            }
                            else
                            {
                                n1 = popStackInt(&intStack);

                                if (op == '-')
                                {
                                    n1 = -n1;
                                }

                                res = n1;
                            }






                            /*  printf("n1 %d , n2 %d \n", n1, n2);

                               printf("There occured a more precedence so this inter result is %d\n", res); */



                            pushStackOperator(&opStack, expression[i]);


                            pushStackInt(&intStack, res);


                        }


                        if (intStack.size > 1 && i == 0)
                        {
                            char op = popStackOperator(&opStack);

                            int n1, n2;

                            n1 = popStackInt(&intStack);
                            n2 = popStackInt(&intStack);


                            if (op == '*')
                            {
                                res = n1 * n2;
                            }
                            else if (op == '/')
                            {
                                res = n2 / n1;
                            }
                            else if (op == '+')
                            {
                                res = n1 + n2;
                            }
                            else if (op == '-')
                            {
                                res = n2 - n1;
                            }


                            /*   printf("n1 %d , n2 %d \n", n1, n2);

                               printf("There occured a more precedence so this inter result is %d\n", res); */

                            pushStackOperator(&opStack, expression[i]);

                            pushStackInt(&intStack, res);

                        }

                    }
                }
            }
        }
        else if (isOperator(expression[i]) == -1)
        {

            int startIndex = i;
            int k;
            while (isOperator(expression[i + 1]) == -1 && i < strlen(expression))
            {
                i++;
            }

            int num;
            if (i == startIndex)
            {
                num = expression[i] - '0';
                if (shouldMultiply % 2 == 1)
                {
                    num = -num;
                    shouldMultiply = 0;
                }

                pushStackInt(&intStack, num);
            }
            else
            {
                char *charNumber = (char *) calloc(100, sizeof(char));

                for (k = 0; startIndex <= i; startIndex++)
                {
                    charNumber[k] = expression[startIndex];
                    k++;
                }

                num = atoi(charNumber);

                if (shouldMultiply % 2 == 1)
                {
                    num = -num;
                    shouldMultiply = 0;
                }
                pushStackInt(&intStack, num);

                /* free(charNumber); */

            }


        }
        else if (isOperator(expression[i]) == 2 && i < strlen(expression))
        {

            pushStackOperator(&opStack, '(');


        }
        else if (isOperator(expression[i]) == 3)
        {
            int k = i;

            /*TODO */
            while (topStackOperator(opStack) != '(')
            {
                if (isOperator(expression[k] == 1))
                {

                    int res = 0;
                    char op = 'k';

                    if (opStack.size > 0)
                        op = popStackOperator(&opStack);


                    int n1, n2;

                    if (intStack.size > 0)
                        n1 = popStackInt(&intStack);

                    if (intStack.size > 0)
                        n2 = popStackInt(&intStack);


                    if (op == '*')
                    {
                        res = n1 * n2;
                    }
                    else if (op == '/')
                    {
                        res = n2 / n1;
                    }
                    else if (op == '+')
                    {
                        res = n1 + n2;
                    }
                    else if (op == '-')
                    {
                        res = n2 - n1;
                    }

                    /*   pushStackOperator(&opStack, expression[k]); */

                    pushStackInt(&intStack, res);


                }
                else
                {
                    if (opStack.size > 0)
                        popStackOperator(&opStack);
                }


                k--;


            }


        }


    }


    while (intStack.size >= 1 && opStack.size > 0)
    {

        char op = topStackOperator(opStack);

        if (isOperator(op) == 2 || isOperator(op) == 3)
        {
            if (opStack.size > 0)
                popStackOperator(&opStack);
        }
        else
        {
            if (opStack.size > 0)
                popStackOperator(&opStack);
            int n1, n2;


            n1 = popStackInt(&intStack);
            if (intStack.size == 0)
            {
                pushStackInt(&intStack, n1);
            }
            else
            {
                n2 = popStackInt(&intStack);


                if (op == '*')
                {
                    res = n1 * n2;
                }
                else if (op == '/')
                {
                    res = n2 / n1;
                }
                else if (op == '+')
                {
                    res = n1 + n2;
                }
                else if (op == '-')
                {
                    res = n2 - n1;
                }
                else
                {
                    /*  printf("Invalid operator in calculation, and the invalid thing is %c \n", op); */
                }

                /*   printf("Res : n1 %d , n2 %d \n", n1, n2);

                   printf("There occured a more precedence so this not inter result is %d\n", res);
       */
                /*  pushStackOperator(&opStack,op);*/

                pushStackInt(&intStack, res);


            }


        }


    }


    /*  printf("Result is %d , size of stack int is %d\n", topStackInt(intStack), intStack.size);

      for (i = 0; i < intStack.size; i++)
      {
          printf("Value %dth is %d\n", i, intStack.values[i]);
      }
  */

    int retSize = 0;
    int retVal = popStackInt(&intStack);
    int dummy = retVal;

    while (dummy > 0)
    {
        retSize++;
        dummy = dummy / 10;

    }

    char *retChars = (char *) calloc(retSize + 1, sizeof(char));

    sprintf(retChars, "%d", retVal);

    /*   printf("The things are %s \n", retChars);


       printf("ret size is %d\n", retSize); */

    char *dull = (char *) calloc(retSize + 10, sizeof(char));

    strcpy(dull, "integer ");

    strcat(dull, retChars);

    enqueue(q, dull);

    return retChars;


}

int isOperator(char aChar)
{
    if (aChar == ')')
    {
        return 3;
    }
    else if (aChar == '(')
    {
        return 2;
    }
    else if (aChar == '+' || aChar == '-' || aChar == '/' || aChar == '*')
        return 1;
    else if (aChar >= 48 && aChar <= 57)
    {
        return -1;
    }
    else
        return -2;

}

stackInt initStackInt(void)
{
    stackInt a;
    a.size = 0;
    a.values = NULL;
    return a;

}

void pushStackInt(stackInt *a, int value)
{


    a->values = (int *) realloc(a->values, (a->size + 1) * sizeof(int));

    a->values[a->size] = value;

    a->size++;


}

int popStackInt(stackInt *a)
{
    if (a->size > 0)
    {
        int *tmp = (int *) calloc(a->size, sizeof(int));
        int i;

        int retVal = a->values[a->size - 1];


        for (i = 0; i < a->size; i++)
        {
            tmp[i] = a->values[i];
        }

        free(a->values);

        a->size--;

        a->values = (int *) calloc(a->size, sizeof(int));

        for (i = 0; i < a->size; i++)
        {
            a->values[i] = tmp[i];
        }

        free(tmp);

        return retVal;
    }

    return -99999;

}

int topStackInt(stackInt a)
{
    if (a.size == 0)
    {
        return -99;
    }
    else
    {
        return a.values[a.size - 1];
    }

}

stackOperotor initStackOperator()
{
    stackOperotor a;

    a.size = 0;
    a.values = NULL;

    return a;


}

void pushStackOperator(stackOperotor *a, char value)
{


    a->values = (char *) realloc(a->values, (a->size + 1));

    a->values[a->size] = value;

    a->size++;


}


char topStackOperator(stackOperotor a)
{
    if (a.size == 0)
    {
        return 'z';
    }
    else
    {
        return a.values[a.size - 1];
    }


}

int hasError(char *expression, queue *q)
{
    if (strcmp("", expression) == 0)
        return 1;
    int i;
    int leftPar = 0;
    int rightPar = 0;

    int hasError = 0;

    for (i = 0; i < strlen(expression); i++)
    {
        /* int a = isOperator(expression[i]); */

        if (isOperator(expression[i]) == -2)
        {
            hasError = 1;
            enqueue(q, "error");
            return hasError;
        }

    }

    for (i = 0; i < strlen(expression); i++)
    {
        if (expression[i] == '(')
        {
            leftPar++;
        }
        else if (expression[i] == ')')
        {
            rightPar++;
        }

    }

    if (rightPar != leftPar)
    {
        hasError = 1;
        enqueue(q, "error");
        return hasError;
    }

    for (i = 0; i < strlen(expression) - 2; i++)
    {
        if (isOperator(expression[i]) == 1)
        {
            if ((isOperator(expression[i + 1]) == 1 && isOperator(expression[i + 2]) == 1))
            {
                enqueue(q, "error");
                hasError = 1;

                return hasError;
            }
            else if ((expression[i] == '-' || expression[i] == '+') &&
                     (expression[i + 1] == '*' || expression[i + 1] == '/'))
            {
                hasError = 1;
                enqueue(q, "error");
                return hasError;
            }
            else if ((expression[i] == '*' || expression[i] == '/') &&
                     (expression[i + 1] == '*' || expression[i + 1] == '/'))
            {
                hasError = 1;
                enqueue(q, "error");
                return hasError;
            }
        }

    }


    return hasError;

}

int isDigit(char aChar)
{
    if (aChar >= 48 || aChar <= 57)
    {
        return 1;
    }
    return 0;
}

queue initQueue(void)
{
    queue aQueue;

    aQueue.size = 0;

    aQueue.elements = NULL;

    aQueue.frontIndex = 0;


    return aQueue;


}

void enqueue(queue *aQueue, char *string)
{

    aQueue->elements = (char **) realloc((aQueue->elements), (aQueue->size + 1) * sizeof(char *));

    /* int i=0; */


    aQueue->elements[aQueue->size] = (char *) calloc(((int) strlen(string) + 1), sizeof(char));


    strcpy(aQueue->elements[aQueue->size], string);

    /* aQueue->elements[(int)strlen(string)+1] = '\0'; */

    aQueue->size++;

}

char *dequeue(queue *aQueue)
{
    if (aQueue->frontIndex == aQueue->size || aQueue->size == 0)
    {
        /* CHANGED */
        /* DELETED return "er"; */

        /* ADDED */

        return NULL;

        /* END OF ADDING */

    }
    else
    {

        /* DELETED   aQueue->size--; */
        aQueue->frontIndex++;
        return aQueue->elements[aQueue->frontIndex - 1];
    }


}

void printQueue(queue *aQueue, char *filePath)
{
    FILE *fp;
    fp = fopen(filePath, "a+");
    char *dummy;
    while ((dummy = dequeue(aQueue)) != NULL)
    {
        /* CHANGED */
        /*DELETED */
        /*
        if(strcmp(dummy,"er"))
        {
            fprintf(fp,"error\n");

        }
        else
        {
            fprintf(fp,"%s\n",dummy);

        }
         */

         /* END OF DELETION */

        /* ADDED */
        fprintf(fp, "%s\n", dummy);
        /* END OF ADDING */
    }

    *aQueue = initQueue();

    fclose(fp);

}

char *hexToInt(char *expression)
{
    queue q = initQueue();
    int oldSize = 0;
    int newSize = 0;
    int i;
    int digits = 0;
    int sign = 0;
    int decimal = 0;
    int counter = 0;

    int difference = 0;
    int j = 0;

    char *retChar = (char *) calloc(strlen(expression) + 100, sizeof(char));
    int k = 0;
    for (i = 0; i < strlen(expression); i++)
    {
        char *dullDecimal = (char *) calloc(strlen(expression) + 1, sizeof(char));


        if (isHexDigit(expression[i]) == -1)
        {
            while (isHexDigit(expression[i]) == -1)
            {
                retChar[k] = expression[i];
                k++;
                counter++;
                i++;
            }
            i--;
        }
        else
        {

            while (isHexDigit(expression[i]) == 1 && i < strlen(expression))
            {


                dullDecimal[digits] = expression[i];
                digits++;

                sign = 1;
                i++;

            }


            if (sign == 1)
            {
                oldSize = digits;

                for (j = 0; j < oldSize; j++)
                {
                    if (dullDecimal[j] == 'a' || dullDecimal[j] == 'A')
                    {
                        decimal = decimal + 10 * pow(16, digits - j - 1);
                    }
                    else if (dullDecimal[j] == 'b' || dullDecimal[j] == 'B')
                    {
                        decimal = decimal + 11 * pow(16, digits - j - 1);
                    }
                    else if (dullDecimal[j] == 'C' || dullDecimal[j] == 'c')
                    {
                        decimal = decimal + 12 * pow(16, digits - j - 1);
                    }
                    else if (dullDecimal[j] == 'd' || dullDecimal[j] == 'D')
                    {
                        decimal = decimal + 13 * pow(16, digits - j - 1);
                    }
                    else if (dullDecimal[j] == 'e' || dullDecimal[j] == 'E')
                    {
                        decimal = decimal + 14 * pow(16, digits - j - 1);
                    }
                    else if (dullDecimal[j] == 'f' || dullDecimal[j] == 'F')
                    {
                        decimal = decimal + 15 * pow(16, digits - j - 1);
                    }
                    else
                    {
                        int anInt = dullDecimal[j] - '0';
                        decimal = decimal + anInt * pow(16, digits - j - 1);
                    }


                }

                int dummy = decimal;

                while (dummy > 0)
                {
                    newSize++;
                    dummy = dummy / 10;

                }

                sign = 0;

                char *stringDec = (char *) calloc(newSize, sizeof(char));

                sprintf(stringDec, "%d", decimal);


                int m = 0;
                for (m = 0; m < newSize; m++)
                {
                    retChar[k] = stringDec[m];
                    k++;
                }


                newSize = 0;

                oldSize = 0;

                decimal = 0;
                dullDecimal = NULL;

                i = i - 1;

                digits = 0;

                counter = 0;


            }

        }


    }


    strcpy(retChar, intCalculator(retChar, &q));

    int result = atoi(retChar);

    if (result < 0)
    {
        result = -result;
        sprintf(retChar, "%x", result);

        char *minusSign = (char *) calloc(strlen(retChar + 2), sizeof(char));
        strcpy(minusSign, "-");
        strcat(minusSign, retChar);

        strcpy(retChar, minusSign);
    }
    else
    {
        sprintf(retChar, "%x", result);


    }

    for (i = 0; i < strlen(retChar); i++)
    {
        if (retChar[i] >= 97 && retChar[i] <= 102)
        {
            retChar[i] = retChar[i] - 32;
        }
    }

    return retChar;

}


int isHexDigit(char aChar)
{
    /*ASCII table values for 0 to 9 A to F and  */
    if ((aChar >= 48 && aChar <= 57) || (aChar >= 65 && aChar <= 70) || (aChar >= 97 && aChar <= 102))
    {
        return 1;
    }
    else
        return -1;
}


int hexHasError(char *expression, queue *q)
{
    if (strcmp("", expression) == 0)
    {
        /* CHANGED */
        /* DELETED return "error"; */
        /* ADDED */

        enqueue(q, "error");

        /* END OF ADDING */

        return 1;
    }

    int i;
    int leftPar = 0;
    int rightPar = 0;

    int hasError = 0;

    for (i = 0; i < strlen(expression); i++)
    {
        /* int a = isOperator(expression[i]); */

        if (isHexDigit(expression[i]) != 1 && isOperator(expression[i]) != 3 && isOperator(expression[i]) != 2 &&
            isOperator(expression[i]) != 1)
        {
            hasError = 1;
            enqueue(q, "error");
            return hasError;
        }

    }

    for (i = 0; i < strlen(expression); i++)
    {
        if (expression[i] == '(')
        {
            leftPar++;
        }
        else if (expression[i] == ')')
        {
            rightPar++;
        }

    }

    if (rightPar != leftPar)
    {
        hasError = 1;
        enqueue(q, "error");
        return hasError;
    }

    for (i = 0; i < strlen(expression) - 2; i++)
    {
        if (isOperator(expression[i]) == 1)
        {
            if ((isOperator(expression[i + 1]) == 1 && isOperator(expression[i + 2]) == 1))
            {
                enqueue(q, "error");
                hasError = 1;

                return hasError;
            }
            else if ((expression[i] == '-' || expression[i] == '+') &&
                     (expression[i + 1] == '*' || expression[i + 1] == '/'))
            {
                hasError = 1;
                enqueue(q, "error");
                return hasError;
            }
            else if ((expression[i] == '*' || expression[i] == '/') &&
                     (expression[i + 1] == '*' || expression[i + 1] == '/'))
            {
                hasError = 1;
                enqueue(q, "error");
                return hasError;
            }
        }

    }


    return hasError;

}

char *hexCalculator(char *expression, queue *q)
{


    if (hexHasError(expression, q))
    {
        /* CHANGED */
        /* DELETED return "error"; */

        /* ADDED */
        return NULL;
        /* END OF ADDING */
    }

    char *retVal = (char *) calloc(strlen(expression) + 1, sizeof(char));

    char *dull = (char *) calloc(strlen(expression) + 10, sizeof(char));

    strcpy(dull, "hex ");

    strcat(dull, hexToInt(expression));

    enqueue(q, dull);


    return retVal;


}
