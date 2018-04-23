#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#define MAXFILENAME 80
#define MAXELEMENTS 10000
#define MAXMLENGHT ULONG_MAX

typedef struct {
    unsigned long line, column;
    double value;
} MatrixElement;

typedef struct {
    unsigned long minL, maxL;
    unsigned long minC, maxC;
    /* Dirty flags if the limits are possibly wrong.
    i.e. after deleting an element that is
    part of the limits.*/
    int lenght, dirty;
    double zero;
} MatrixInfo;

void addElement();
void listRelevant();
void printMatrixInfo();
void printLine();
void printColumn();
void deleteEmptySpace(int start);
void fixLimits();
void printElement();

MatrixElement mElements[MAXELEMENTS];
MatrixInfo mInfo = {MAXMLENGHT, 0, MAXMLENGHT, 0, 0, 0, 0.0};

#define cleanLimits() {mInfo.minL = mInfo.minC = MAXMLENGHT;\
                       mInfo.maxL = mInfo.maxC = 0;}
#define isLimit(A) A.line == mInfo.minL || A.line == mInfo.maxL ||\
                   A.column == mInfo.minC ||  A.column == mInfo.maxC

int main()
{
    char fileName[MAXFILENAME+1];

    char command;
    do {
        command = getchar();
        /* printf("%d ", command); */
        switch(command) {
            case 'a':
                addElement();
                break;
            case 'p':
                listRelevant();
                break;
            case 'i':
                printMatrixInfo();
                break;
            case 'l':
                printLine();
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

void addElement()
{
    /* Adds a new value to the elements list and also
    checks if it will redefine the matrix's boundaries.
    If the value is a zero, it will delete the current
    value of the position and flag the info as dirty
    if the element was part of the limits.*/

    unsigned long inLine, inColumn;
    double inValue;
    int i;

    scanf("%lu %lu %lf", &inLine, &inColumn, &inValue); 

    /* Check if the position is already occupied. 
    If it is, just change the value) */
    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].line == inLine && mElements[i].column == inColumn) {
            if (inValue != mInfo.zero) {
                mElements[i].value = inValue;
            } else {
                if (isLimit(mElements[i]))
                    mInfo.dirty = 1;
                
                deleteEmptySpace(i);
            }
            return;
        }
    }

    if (mInfo.lenght == MAXELEMENTS) {
        printf("Max Elementes Reached.");
    } else {
        mElements[mInfo.lenght].line = inLine; 
        mElements[mInfo.lenght].column = inColumn; 
        mElements[mInfo.lenght].value = inValue;

        /* Check as we add so that we don't iterate 
        over the whole array everytime.*/
        if (inLine < mInfo.minL)
            mInfo.minL = inLine;
        if (inLine > mInfo.maxL)
            mInfo.maxL = inLine;
        if (inColumn < mInfo.minC)
            mInfo.minC = inColumn;
        if (inColumn > mInfo.maxC)
            mInfo.maxC = inColumn;

        mInfo.lenght++;
    }
} 

void listRelevant()
{
    /* Lists every element of the matrix that isn't a zero. */

    /* Since the zeros are never added to the list or deleted 
    there's  no need to check if the element is a zero.*/

    int i;

    if (mInfo.lenght == 0) {
        printf("empty matrix\n");
        return;
    }

    for (i = 0; i < mInfo.lenght; i++) {
        printElement(mElements[i]);    
    }
}

void printMatrixInfo()
{
    /* Prints the "boundaries" of the matrix and
    its density inside that range.*/ 

    int size;
    double dens;

    if (mInfo.lenght == 0) {
        printf("empty matrix\n");
        return;
    }

    if (mInfo.dirty)
        fixLimits();

    size = (mInfo.maxL - mInfo.minL + 1) * (mInfo.maxC - mInfo.minC + 1);
    dens = ((double) mInfo.lenght/size) * 100;

    printf("[%lu %lu] [%lu %lu] %d/%d = %.3f%%\n", mInfo.minL, mInfo.maxL,
                                                   mInfo.minC, mInfo.maxC,
                                                   mInfo.lenght, size, dens);    
}

void printLine()
{
    unsigned long inLine;
    int i;

    scanf("%lu", &inLine); 

    for (i = 0; i < mInfo.lenght; i++) {
        
    }
}

void deleteEmptySpace(int start)
{
    /* Shifts every element to the left starting from
    an index.*/

    int i;

    for (i = start+1; i < mInfo.lenght; i++) {
        mElements[i-1] = mElements[i];
    }

    mInfo.lenght--;
}

void fixLimits() 
{
    /* Call everytime the limits might be wrong, i.e.
    the info is flagged as dirty and we need the matrix's
    boundaries (to print or to calculate a line/column).*/

    int i;

    cleanLimits();

    for (i = 0; i < mInfo.lenght; i++) {
        printElement(mElements[i]);
        if (mElements[i].line < mInfo.minL)
            mInfo.minL = mElements[i].line;
        if (mElements[i].line > mInfo.maxL)
            mInfo.maxL = mElements[i].line;
        if (mElements[i].column < mInfo.minC)
            mInfo.minC = mElements[i].column;
        if (mElements[i].column > mInfo.maxC)
            mInfo.maxC = mElements[i].column;
    }

    mInfo.dirty = 0;
}

void printElement(MatrixElement e) 
{
    /* Prints the line, column and value of an element. */

    printf("[%lu;%lu]=%.3f\n", e.line, e.column, e.value);
}
