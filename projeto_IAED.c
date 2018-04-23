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
void changeZero();
void deleteElement(int start);
void checkLimits();
void printElement(MatrixElement element);

MatrixElement mElements[MAXELEMENTS];
MatrixInfo mInfo = {MAXMLENGHT, 0, MAXMLENGHT, 0, 0, 0, 0.0};

#define cleanLimits() {mInfo.minL = mInfo.minC = MAXMLENGHT;\
                       mInfo.maxL = mInfo.maxC = 0;}
#define isLimit(A) (A.line == mInfo.minL || A.line == mInfo.maxL ||\
                   A.column == mInfo.minC ||  A.column == mInfo.maxC)

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
                printColumn();
                break;
            case 'o':
                break;
            case 'z':
                changeZero();
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
        /* clean the \n that was left on the input buffer */
        getchar(); 
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
            /* The element has to be deleted so that the new value goes to
            the end of the list*/
            deleteElement(i);
            
            if (inValue == mInfo.zero && isLimit(mElements[i])) {
                mInfo.dirty = 1;
                return;
            }
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

    checkLimits();

    size = (mInfo.maxL - mInfo.minL + 1) * (mInfo.maxC - mInfo.minC + 1);
    dens = ((double) mInfo.lenght/size) * 100;

    printf("[%lu %lu] [%lu %lu] %d/%d = %.3f%%\n", mInfo.minL, mInfo.maxL,
                                                   mInfo.minC, mInfo.maxC,
                                                   mInfo.lenght, size, dens);    
}

void printLine()
{
    unsigned long inLine;
    int i, empty = 1;

    /* c90 forbids variable lenght array, so I won't use maxC-minC + 1.
    Also not allowed to dinamically allocate memory, so I have to
    declare a 10000 lenght array even if only going occupy 1 space.*/
    double values[MAXELEMENTS];
    for (i = 0; i < MAXELEMENTS; i++) {
        values[i] = mInfo.zero;
    }
    
    scanf("%lu", &inLine); 
    checkLimits();

    /* Gather indexes of the elements on that line */
    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].line == inLine) {
            values[mElements[i].column] = mElements[i].value;
            empty = 0;
        } 
    }

    if (empty) {
        printf("empty line\n");
    } else {
        for (i = mInfo.minC; i <= mInfo.maxC; i++) {
            printf(" %.3f", values[i]);    
        }
        printf("\n");
    }
}

void printColumn()
{
    unsigned long inColumn;
    int i, empty = 1;

    double values[MAXELEMENTS];
    /* placeholder element to be able to print null elements */
    MatrixElement element;

    checkLimits();

    for (i = 0; i < MAXELEMENTS; i++) {
        values[i] = mInfo.zero;
    }

    scanf("%lu", &inColumn); 
    element.column = inColumn;

    /* Gather indexes of the elements on that line */
    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].column == inColumn) {
            values[mElements[i].line] = mElements[i].value;
            empty = 0;
        } 
    }

    if (empty) {
        printf("empty column\n");
    } else { 
        for (i = mInfo.minL; i <= mInfo.maxL; i++) {
            element.line = i;
            element.value = values[i];
            printElement(element);    
        }
    }
}

void changeZero()
{
    /* Changes the current zero and checks if any element
    corresponds with the new zero. Deletes it if it does. */

    int i;
    double newZero;

    scanf("%lf", &newZero);
    mInfo.zero = newZero;

    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].value == mInfo.zero) {
            deleteElement(i);
            i--;
        }
    }
}

void deleteElement(int start)
{
    /* Shifts every element to the left starting from
    the index i.*/

    int i;

    for (i = start+1; i < mInfo.lenght; i++) {
        mElements[i-1] = mElements[i];
    }

    mInfo.lenght--;
}

void checkLimits() 
{
    /* Checks if the limits of the matrix are right if
    there's a possibility they are wrong */

    int i;

    /* If we're sure the limits are corret there's no reason
    to continue.*/
    if (!mInfo.dirty)
        return;

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
