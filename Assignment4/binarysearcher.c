#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

typedef struct file
{
    const char *filePath;
    int localFrequency;
} file;

enum bool
{
    true, false
};

typedef struct node
{
    int numberOfFiles;
    char *name;
    struct node *left;
    struct node *right;

    struct node *parent;
    int depth;
    int totalFrequency;


    file *files;


} node;


void decreaseDepth(node **root);


void extendTreeFromFile(node **root, char *filePath, char *delimeters);

void addBinarySearchTree(node **root, char *name, int *depth, char *filePath);

char *toLowerCase(char *string);

void printDescending(node *root,FILE * outFile);

void printTree(node *root,FILE * outFile);

char ***readAsTokens(char *filePath, int *maxCharCount, int *lineCount, int tokenNumber, char *delims);

node *findMinSubRight(node *root);

void buildBstFromDir(char *directory, node **root, char *delimeters);

char *readFileStream(char *filePath);

char ***linesToToken(char **lines, char *delims, int tokenNumber, int linesCount, int *lineChars);


enum bool checkIfText(char *fileName);


char *setDelimsExceptAlphaNumeric(void);


void printAscending(node *root,FILE * outFile);


node *findNode(node *root, char *name);

void deleteNode(node **root, char *name);


int main(int argc, char const *argv[])
{

    node *root;
    root = NULL;
    FILE* outFile;

    outFile=fopen("output.txt","w");
    char *delimeters = setDelimsExceptAlphaNumeric();
    /* TODO change it to args[1] */



    /* Scan directory and build the tree */
    buildBstFromDir((char * )argv[1], &root, delimeters);


    int i;

    int lineCount = 0;


    // Parse commands tokens
    int maxCharCount = 0;
    int maxTokenNumber = 3;
    char ***linesAsTokens = readAsTokens((char*)argv[2], &maxCharCount, &lineCount, maxTokenNumber, " \r\n");


    /* Execute parsed commands comm */

    i = 0;
    while (i < lineCount)
    {


        if (strcmp(linesAsTokens[i][0], "PRINT") == 0)
        {
            if (strcmp(linesAsTokens[i][1], "TREE") == 0)
            {
                if (strcmp(linesAsTokens[i][2], "") != 0)
                {
                    if (strcmp(linesAsTokens[i][2], "ASC") == 0)
                    {
                        fprintf(outFile,"PRINT TREE ASC\n");
                        printf("PRINT TREE ASC\n");
                        printAscending(root,outFile);

                    }
                    else if (strcmp(linesAsTokens[i][2], "DSC") == 0)
                    {
                        fprintf(outFile,"PRINT TREE DSC\n");

                        printf("PRINT TREE DSC\n");
                        printDescending(root,outFile);

                    }
                }
                else
                {
                    fprintf(outFile,"PRINT TREE\n");
                    printf("PRINT TREE\n");
                    printTree(root,outFile);
                }
            }


        }
        else if (strcmp(linesAsTokens[i][0], "SEARCH") == 0)
        {
            node *searchingNode = findNode(root, linesAsTokens[i][1]);

            fprintf(outFile,"SEARCH %s\r\n", toLowerCase(linesAsTokens[i][1]));
            printf("SEARCH %s\r\n", toLowerCase(linesAsTokens[i][1]));
            if (searchingNode != NULL)
            {
                fprintf(outFile,"%s\r\n\t%d\r\n\t%d\r\n\t", searchingNode->name, searchingNode->totalFrequency,
                       searchingNode->depth);

                printf("%s\r\n\t%d\r\n\t%d\r\n\t", searchingNode->name, searchingNode->totalFrequency,
                       searchingNode->depth);

                int j = 0;
                while (j < searchingNode->numberOfFiles)
                {

                    if (searchingNode->numberOfFiles - 1 != i)
                    {
                        fprintf(outFile,"%s %d\r\n\t", searchingNode->files[i].filePath, searchingNode->files[i].localFrequency);
                        printf("%s %d\r\n\t", searchingNode->files[i].filePath, searchingNode->files[i].localFrequency);
                    }
                    else
                    {
                        fprintf(outFile,"%s %d\r\n", searchingNode->files[i].filePath, searchingNode->files[i].localFrequency);

                        printf("%s %d\r\n", searchingNode->files[i].filePath, searchingNode->files[i].localFrequency);
                    }


                    j++;
                }
            }

        }
        else if (strcmp(linesAsTokens[i][0], "ADD") == 0)
        {

            extendTreeFromFile(&root, toLowerCase(linesAsTokens[i][1]), delimeters);
        }
        else if (strcmp(linesAsTokens[i][0], "REMOVE") == 0)
        {

            fprintf(outFile,"REMOVE %s\n", linesAsTokens[i][1]);
            printf("REMOVE %s\n", linesAsTokens[i][1]);
            deleteNode(&root, toLowerCase(linesAsTokens[i][1]));
        }


        i++;
    }





    return 0;

}

/* Add a node to binary search tree */
void addBinarySearchTree(node **root, char *name, int *depth, char *filePath)
{
    node *rootTmp = *root;
    (*depth)++;
    /* If tree is empty create first node */
    if (rootTmp == NULL)
    {
        rootTmp = (node *) calloc(1, sizeof(node));

        rootTmp->name = (char *) calloc(strlen(name) + 1, sizeof(char));

        strcpy(rootTmp->name, name);
        rootTmp->right = NULL;
        rootTmp->left = NULL;
        rootTmp->depth = *depth;
        rootTmp->totalFrequency = 1;
        rootTmp->files = (file *) calloc(1, sizeof(file));

        rootTmp->files[0].filePath = strdup(filePath);
        rootTmp->files->localFrequency = 1;
        rootTmp->numberOfFiles = 1;
        rootTmp->parent = NULL;

        *root = rootTmp;

    }
    else
    {
        /* If tree is not empty and adding node is less than root. If root's left is empty add there immediately*/
        if (strcmp(name, rootTmp->name) < 0)
        {
            if (rootTmp->left == NULL)
            {
                (*depth)++;
                rootTmp->left = (node *) calloc(1, sizeof(node));

                rootTmp->left->name = (char *) calloc(strlen(name) + 1, sizeof(char));

                strcpy(rootTmp->left->name, name);
                rootTmp->left->left = NULL;
                rootTmp->left->right = NULL;
                rootTmp->left->depth = *depth;
                rootTmp->left->totalFrequency = 1;
                rootTmp->left->parent = rootTmp;


                rootTmp->left->files = (file *) calloc(1, sizeof(file));
                rootTmp->left->files[0].filePath = strdup(filePath);
                rootTmp->left->files->localFrequency = 1;
                rootTmp->left->numberOfFiles = 1;

            }
            else
            {

                /* If it's left is not empty go one more left and call the function again recursively */
                rootTmp = rootTmp->left;
                addBinarySearchTree(&rootTmp, name, depth, filePath);


            }

        } /* Same process for the rigt */
        else if (strcmp(name, rootTmp->name) > 0)
        {
            if (rootTmp->right == NULL)
            {
                (*depth)++;

                rootTmp->right = (node *) calloc(1, sizeof(node));

                rootTmp->right->name = (char *) calloc(strlen(name) + 1, sizeof(char));

                strcpy(rootTmp->right->name, name);
                rootTmp->right->left = NULL;
                rootTmp->right->right = NULL;
                rootTmp->right->totalFrequency = 1;
                rootTmp->right->depth = *depth;
                rootTmp->right->parent = rootTmp;

                rootTmp->right->files = (file *) calloc(1, sizeof(file));

                rootTmp->right->files[0].filePath = strdup(filePath);
                rootTmp->right->files->localFrequency = 1;
                rootTmp->right->numberOfFiles = 1;
            }
            else
            {

                rootTmp = rootTmp->right;
                addBinarySearchTree(&rootTmp, name, depth, filePath);


            }

        }   /* If node's word is already in tree */
        else if (strcmp(name, rootTmp->name) == 0)
        {
            rootTmp->totalFrequency++;

            /* If it's from the same file that node has already has */
            if (strcmp(rootTmp->files[rootTmp->numberOfFiles - 1].filePath, filePath) == 0)
            {
                rootTmp->files[rootTmp->numberOfFiles - 1].localFrequency++;
            }
            else
            {
                /* If node does not have that text file that adding word is coming from, create it */
                rootTmp->numberOfFiles++;
                rootTmp->files = (file *) realloc(rootTmp->files, rootTmp->numberOfFiles * sizeof(file));


                rootTmp->files[rootTmp->numberOfFiles - 1].filePath = strdup(filePath);
                rootTmp->files[rootTmp->numberOfFiles - 1].localFrequency = 1;

            }


        }
    }

}

/* Print ascending recursively */
void printAscending(node *root,FILE *outFile)
{
    if (root == NULL)
    {
        return;
    }
    printAscending(root->left,outFile);


    fprintf(outFile," %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);
    printf(" %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);

    int i = 0;
    while (i < root->numberOfFiles)
    {

        if (root->numberOfFiles - 1 != i)
        {
            fprintf(outFile,"%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);
            printf("%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);
        }
        else
        {
            fprintf(outFile,"%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);

            printf("%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);
        }


        i++;
    }

    printAscending(root->right,outFile);


}

/* Print descending recursively */
void printDescending(node *root, FILE * outFile)
{
    if (root == NULL)
    {
        return;
    }


    printDescending(root->right,outFile);

    fprintf(outFile," %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);

    printf(" %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);

    int i = 0;


    while (i < root->numberOfFiles)
    {

        if (root->numberOfFiles - 1 != i)
        {
            fprintf(outFile,"%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);

            printf("%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);
        }
        else
        {
            fprintf(outFile,"%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);
            printf("%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);
        }


        i++;
    }

    printDescending(root->left,outFile);


}

/* Print tree recursively in pre order */
void printTree(node *root, FILE *outFile)
{
    if (root == NULL)
    {
        return;
    }
    fprintf(outFile," %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);
    printf(" %s\r\n\t%d\r\n\t%d\r\n\t", root->name, root->totalFrequency, root->depth);

    int i = 0;
    while (i < root->numberOfFiles)
    {

        if (root->numberOfFiles - 1 != i)
        {
            fprintf(outFile,"%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);

            printf("%s %d\r\n\t", root->files[i].filePath, root->files[i].localFrequency);
        }
        else
        {
            fprintf(outFile,"%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);
            printf("%s %d\r\n", root->files[i].filePath, root->files[i].localFrequency);
        }


        i++;
    }

    printTree(root->left,outFile);
    printTree(root->right,outFile);

}

/* Find a node in tree, if searching node is not in the tree, then return null */
node *findNode(node *root, char *name)
{
    node *rootTmp = root;

    if (rootTmp == NULL)
    {
        return NULL;
    }

    if (strcmp(rootTmp->name, name) == 0)
    {
        return root;
    }


    if (strcmp(name, rootTmp->name) < 0)
    {
        rootTmp = rootTmp->left;

        if (rootTmp == NULL)
        {
            return NULL;
        }
        else
        {
            if (strcmp(rootTmp->name, name) == 0)
            {
                return rootTmp;
            }
            else
            {
                node *dum;
                if ((dum = findNode(rootTmp, name)) != NULL)
                {
                    return dum;
                }
            }
        }


    }
    else
    {
        rootTmp = rootTmp->right;

        if (rootTmp == NULL || rootTmp->name == '\0' || strcmp(rootTmp->name, "") == 0)
        {
            return NULL;
        }
        else
        {
            if (strcmp(rootTmp->name, name) == 0)
            {
                return rootTmp;
            }
            else
            {
                node *dum;
                if ((dum = findNode(rootTmp, name)) != NULL)
                {
                    return dum;
                }


            }
        }


    }


    return NULL;


}

/* Delete a node from tree */
void deleteNode(node **root, char *name)
{
    if (*root)
    {
        node *deletingNode;


        deletingNode = findNode(*root, name);

        /* If deleting node not in the tree */
        if (deletingNode == NULL)
        {
            printf("\nThe node %s does not exist\n", name);
        }
        else
        {
            /* If deleting node's have no child at all */
            if (deletingNode->right == NULL && deletingNode->left == NULL && deletingNode->parent != NULL)
            {
                node *tmp = deletingNode;

                /* If deleting node linked to it's parent by left, delete it and null it's left link */
                if (deletingNode->parent->left == deletingNode)
                {

                    deletingNode->parent->left = NULL;

                    free(tmp);
                }/* If deleting node linked to it's parent by right, delete it and null it's right link */
                else
                {

                    deletingNode->parent->right = NULL;

                    free(tmp);

                }

            }/* If a non root node has just right sub tree  */
            else if (deletingNode->left == NULL && deletingNode->parent != NULL)
            {
                node *tmp = deletingNode;

                /* Find where deleting node linked to parent */
                if (deletingNode->parent->left == deletingNode)
                {

                    /* Link deleting node's right sub tree to deleting node's parent's left link since it's linked it's parent by left */
                    deletingNode->parent->left = deletingNode->right;
                    /* Fix that new linked sub tree's root's(that is formerly deleting node's subtree) parent */
                    deletingNode->parent->left->parent = tmp->parent;
                    /* Fix sub tree's depths starting by deleting node's subtree's root node and apply recursively all it's children */
                    decreaseDepth(&deletingNode->parent->left);

                    deletingNode->parent->left->depth  = tmp->depth;


                    free(tmp);
                }
                else
                {
                    /* Link deleting node's right sub tree to deleting node's parent's right link since it's linked it's parent by right */
                    deletingNode->parent->right = deletingNode->right;

                    /* Fix that new linked sub tree's root'ss(that is formerly deleting node's subtree) parent */
                    deletingNode->parent->right->parent = tmp->parent;

                    /* Fix sub tree's depths starting by deleting node's subtree's root node and apply recursively all it's children */
                    decreaseDepth(&deletingNode->parent->right);

                    deletingNode->parent->right->depth = tmp->depth;

                    free(tmp);

                }


            } /*If a non root node has just left sub tree */
            else if (deletingNode->right == NULL && deletingNode->parent != NULL)
            {
                node *tmp = deletingNode;

                /* Find where deleting node linked to parent */
                if (deletingNode->parent->left == deletingNode)
                {
                    /* Link deleting node's left sub tree to deleting node's parent's left link since it's linked it's parent by left */
                    deletingNode->parent->left = deletingNode->left;

                    /* Fix that new linked sub tree's root's(that is formerly deleting node's subtree) parent */
                    deletingNode->parent->left->parent = tmp->parent;

                    /* Fix sub tree's depths starting by deleting node's subtree's root node and apply recursively all it's children */
                    decreaseDepth(&deletingNode->parent->left);

                    deletingNode->parent->left->depth = tmp->depth;

                    free(tmp);
                }
                else
                {
                    /* Link deleting node's left sub tree to deleting node's parent's right link since it's linked it's parent by right */
                    deletingNode->parent->right = deletingNode->left;

                    /* Fix that new linked sub tree's root's(that is formerly deleting node's subtree) parent */
                    deletingNode->parent->right->parent = tmp->parent;

                    /* Fix sub tree's depths starting by deleting node's subtree's root node and apply recursively all it's children */
                    decreaseDepth(&deletingNode->parent->right);

                    deletingNode->parent->left->depth = tmp->depth;


                    free(tmp);

                }

            }/* If deleting node has left and right subtree */
            else
            {
                //If deleting node is root
                if (deletingNode->parent == NULL)
                {


                    node *tmpDel = deletingNode;

                    node *minSubRight;

                    /* Find minumum node of deleting node's right subtree */
                    minSubRight = findMinSubRight(deletingNode);

                    /* If it has right sub tree  */
                    if (minSubRight)
                    {
                        free(deletingNode);

                        node *newRoot = minSubRight;



                        /* As we can see, the minumum node of deleting node's sub right tree has to be linked it's parent from left(since it's
                         * minumum and smaller one's go to left in bst) and it cannot be left sub tree(since again it's minumum) but minumum node
                         * may have a right subtree so we assign it to it's parent's left where minumum node formerly linked to it's parent.*/
                        minSubRight->parent->left = minSubRight->right;

                        /* Fix the depth of new linked sub tree's depths  */
                        decreaseDepth(&minSubRight->parent->left);


                        /* Prepare the new root */
                        newRoot->left = tmpDel->left;

                        newRoot->right = tmpDel->right;

                        newRoot->depth = 1;

                        newRoot->parent = NULL;

                        /* If former root has a left subtree, fix that sub tree's parent */
                        if (newRoot->left)
                        {
                            newRoot->left->parent = newRoot;
                        }

                        /* If former root has a right subtree, fix that sub tree's parent */

                        if (newRoot->right)
                        {
                            newRoot->right->parent = newRoot;
                        }

                        /* Assign new root to the former one */

                        (*root) = newRoot;
                    }
                    else
                    {
                        /* If right deleting node's (or root's) right sub tree is empty, we should just erase the root
                         * and assign the left sub tree of root, as new root */


                        /* Of course, if it's has a one. Check the if root has a left subtree */
                        if ((*root)->left)
                        {
                            /* New root is root's left sub tree */
                            (*root) = (*root)->left;

                            /* Decrease root and all of children's depth by 1 */
                            decreaseDepth(root);

                            /* Null it's parent since it's the new root */
                            (*root)->parent = NULL;

                            free(deletingNode);
                        }/* If we enter that below else, that means root has no right or left sub tree, So we just delete it  */
                        else
                        {


                            printf("Last node %s has deleted\n", deletingNode->name);


                            free(*root);

                            *root = NULL;


                        }


                    }


                }/* If deleting node is not root but have 2 children */
                else
                {


                    node *tmp = deletingNode;

                    /* Find min node of right subtree of deletingNode */
                    node *minSubRight = findMinSubRight((deletingNode));

                    deletingNode = minSubRight;

                    /* Decrase all sub nodes of deleting node */
                    decreaseDepth(&deletingNode);


                    deletingNode->depth = tmp->depth;



                    deletingNode->name = strdup(minSubRight->name);

                    /* Assign tmp's left to deleting node's left(tmp is actually deleting node and deleting node is actually minSubRight) */
                    deletingNode->left = tmp->left;

                    /* If actual deleting node have just one child in it's right sub tree, than do nothing since we already assign it to deletingNode */
                    if (deletingNode == tmp->right)
                    {
                        //   DO nothing
                    }
                    else
                    {
                        deletingNode->right = tmp->right;
                        deletingNode->right->parent = deletingNode;


                    }
                    /* Fix deleting node's parent , all of it's subtree's depths and left subtree's parent,(since it has only have left subtree
                     * because it's the smallest node of the right sub tree */

                    deletingNode->parent->left = NULL;

                    deletingNode->parent = tmp->parent;


                    deletingNode->left->parent = deletingNode;



                    /* Fix the actual deleting node's (right now tmp) parent's left or right child, according to where it's linked to it's parent  */
                    if (tmp->parent->left == tmp)
                    {
                        tmp->parent->left = deletingNode;
                    }
                    else
                    {
                        tmp->parent->right = deletingNode;
                    }


                    free(tmp);


                }


            }


        }

    }
    else
    {
        printf("The binary search tree is empty\n");
    }

}

/* Finds minumum of root's right sub tree. Returns null if root has no right child */
node *findMinSubRight(node *root)
{
    if (root->right)
    {
        root = root->right;

        while (root->left)
        {
            root = root->left;
        }

        return root;
    }
    else
    {

        return NULL;
    }


}


void buildBstFromDir(char *directory, node **root, char *delimeters)
{

    DIR *directoryPointer;
    struct dirent *directoryEntry;
    struct stat statBuffer;
    if (!(directoryPointer = opendir(directory)))
    {

        printf("Couldn't open the directory\n");
        return;
    }
    chdir(directory);
    while ((directoryEntry = readdir(directoryPointer)) != NULL)
    {
        if (stat(directoryEntry->d_name, &statBuffer) == 0 && S_ISDIR(statBuffer.st_mode)
                )
        {
            if (strcmp(".", directoryEntry->d_name) == 0 ||
                strcmp("..", directoryEntry->d_name) == 0)
            {
                /* Do nothing in case of . and .. */
            }
            else
            {


                buildBstFromDir(directoryEntry->d_name, root, delimeters);

            }

        }
        else
        {

            extendTreeFromFile(root, directoryEntry->d_name, delimeters);

        }
    }
    chdir("..");
    closedir(directoryPointer);
}

char *readFileStream(char *filePath)
{
    FILE *filePointer;

    filePointer = fopen(filePath, "r");

    fseek(filePointer, 0, SEEK_END);

    int fileSize;

    fileSize = (int) ftell(filePointer);

    char *stream = (char *) calloc(fileSize + 1, sizeof(char));

    filePointer = fopen(filePath, "r");

    char *dummy = (char *) calloc(fileSize, sizeof(char));
    while (fgets(dummy, fileSize, filePointer))
    {
        strcat(stream, dummy);

    }

    //  free(dummy);

    fclose(filePointer);

    return stream;


}

/* Read first 100 bytes from file. If it's has an ASCII char break and decide it has a valid file to process.
 * Returns enum bool: true or false  */
enum bool checkIfText(char *fileName)
{
    FILE *filePointer;

    if (fileName[0] == '.')
    {
        return false;
    }

    enum bool result = false;

    filePointer = fopen(fileName, "r");

    int aCharWannaBeInt;

    int testLimit = 100;
    while (testLimit > 0)
    {
        aCharWannaBeInt = fgetc(filePointer);

        /* Check if it is in the ASCII limits */
        if ((char) aCharWannaBeInt >= 0 && (char) aCharWannaBeInt <= 127)
        {
            result = true;
            break;
        }

        testLimit--;
    }

    return result;
}

/* Add valid words from a file to tree */
void extendTreeFromFile(node **root, char *filePath, char *delimeters)
{
    if (checkIfText(filePath) != true)
    {
        return;

    }

    char *stream = readFileStream(filePath);


    char *bulky = (char *) calloc(strlen(stream) + 1, sizeof(char));

    char *dummy = strtok(stream, ".?!:;-—()[]\"/,");

    while (dummy)
    {
        strcat(bulky, dummy);

        dummy = strtok(NULL, ".;,");
    }


    char *token = (char *) calloc(100, sizeof(char));

    token = strtok(bulky, delimeters);

    while (token)
    {

        if (strcmp(token, "") != 0)
        {

            int depth = 0;
            addBinarySearchTree(root, toLowerCase(token), &depth, filePath);


        }

        token = strtok(NULL, delimeters);
    }

}

/* Converts a word to lower case */
char *toLowerCase(char *string)
{
    int i = 0;
    for (i = 0; i < strlen(string); i++)
    {
        if (string[i] >= 65 && string[i] <= 90)
        {
            string[i] = (char) (string[i] + 32);
        }

    }

    return string;

}

/* Set all non alpha numeric ASCII characters to use as delimeters except '-'  */
char *setDelimsExceptAlphaNumeric(void)
{

    char *delimeters = (char *) malloc(129 * sizeof(char));
    int i = 0;
    int j;

    for (i = 0; i < 128; i++)
    {
        delimeters[i] = ' ';
    }

    /* Add delimeters to all ASCII chars except numbers letters and - */
    for (i = 7; i < 45; i++)
    {
        delimeters[i - 7] = (char) i;
    }

    i = i - 7;

    for (j = 46; j < 48; j++)
    {
        delimeters[i] = (char) j;
        i++;

    }

    for (j = 58; j < 65; j++)
    {
        delimeters[i] = (char) j;
        i++;
    }

    for (j = 91; j < 97; j++)
    {
        delimeters[i] = (char) j;
        i++;
    }

    for (j = 123; j < 128; j++)
    {
        delimeters[i] = (char) j;
        i++;
    }

    return delimeters;


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

/* Decrease root's and all of it's children depth by one. Uses for when a node is going up */
void decreaseDepth(node **root)
{
    if (*root == NULL || root == NULL)
    {
        return;

    }

    decreaseDepth(&(*root)->left);
    (*root)->depth--;
    decreaseDepth(&(*root)->right);

}
