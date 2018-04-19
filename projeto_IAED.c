#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define MAXFILENAME 80
#define MAXELEMENTS 10000
#define MATRIXLENGHT ULONG_MAX

typedef struct {
    unsigned long line, column;
    double value;
} MatrixElement;

typedef struct {
    unsigned long minC, maxC;
    unsigned long minL, maxL;
    /* Dirty flags if the limits are possibly wrong.
    i.e. after deleting an element that is
    one of the limits.*/
    int lenght, dirty;
    double zero;
} MatrixInfo;

void addElement(MatrixElement matrix[], MatrixInfo *info);
void deleteEmptySpace(MatrixElement matrix[], int start, int nEls);
void listRelevant(MatrixElement matrix[], int nEls);
void printMatrixInfo(MatrixInfo info);
void printElement(MatrixElement e);

int main()
{
    char fileName[MAXFILENAME+1];
    MatrixInfo info = {MATRIXLENGHT, 0, MATRIXLENGHT, 0, 0, 0, 0.0};
    MatrixElement elements[MAXELEMENTS];

    char command;

    do {
        command = getchar();
        /* printf("%d ", command); */
        switch(command) {
            case 'a':
                addElement(elements, &info);
                break;
            case 'p':
                listRelevant(elements, info.lenght);
                break;
            case 'i':
                printMatrixInfo(info);
                break;
            case 'l':
                break;
            case 'c':
                break;
            case 'o':
                break;
            case 'z':
                break;
            case 's':
                break;
            case 'w':
                break;
            case 'q':
                break;
            default:
                printf("Invalid Command.\n");
                break;
        }
        getchar(); /* cleans the \n that was left on the buffer */
    } while (command != 'q');
    
    return 0;
}

void addElement(MatrixElement matrix[], MatrixInfo *info)
{
    unsigned long inLine, inColumn;
    double inValue;
    int i;

    scanf("%lu %lu %lf", &inLine, &inColumn, &inValue); 

    /* Check if the position is already occupied. 
    If it is, just change the value) */
    for (i = 0; i < info->lenght; i++) {
        if (matrix[i].line == inLine && matrix[i].column == inColumn) {
            if (inValue != info->zero) {
                matrix[i].value = inValue;
            } else {
                deleteEmptySpace(matrix, i, info->lenght);
                /* Check for new max/min line/column. */
                /* Check for new max/min line/column. */
                /* Check for new max/min line/column. */
                /* Check for new max/min line/column. */
                info->lenght--;
            }
            return;
        } 
    } 

    if (info->lenght == MAXELEMENTS) {
        printf("Max Elementes Reached.");
    } else {
        matrix[info->lenght].line = inLine; 
        matrix[info->lenght].column = inColumn; 
        matrix[info->lenght].value = inValue;

        /* Check as we add so that we don't iterate 
        over the whole array everytime.*/
        if (inLine < info->minL)
            info->minL = inLine;
        if (inLine > info->maxL)
            info->maxL = inLine;
        if (inColumn < info->minC)
            info->minC = inColumn;
        if (inColumn > info->maxC)
            info->maxC = inColumn;

        info->lenght++;
    }
} 

void deleteEmptySpace(MatrixElement matrix[], int start, int nEls)
{
    int i;

    for (i = start+1; i < nEls; i++) {
        matrix[i-1] = matrix[i];
    }
}

void listRelevant(MatrixElement matrix[], int nEls)
{
    /* Since the zeros are never added or deleted there's
    no need to check if the element is a zero.*/

    int i;

    if (nEls == 0) {
        printf("empty matrix\n");
        return;
    }

    for (i = 0; i < nEls; i++) {
        printElement(matrix[i]);    
    }
}

void printMatrixInfo(MatrixInfo info)
{
    int size;
    double dens;

    if (info.lenght == 0) {
        printf("empty matrix\n");
        return;
    }

    size = (info.maxL - info.minL + 1) * (info.maxC - info.minC + 1);
    dens = ((double) info.lenght/size) * 100;

    printf("[%lu %lu] [%lu %lu] %d/%d = %.3f%%\n", info.minL, info.maxL,
                                                   info.minC, info.maxC,
                                                   info.lenght, size, dens);    
}

void printElement(MatrixElement e) 
{
    printf("[%lu;%lu]=%.3f\n", e.line, e.column, e.value);
}
