#include <stdio.h>
#include <limits.h>
#include <string.h>
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

void addElement(char parameters[]);
void listRelevant();
void printMatrixInfo();
void printLine(char parameters[]);
void printColumn(char parameters[]);
void sortElements(char parameters[]);
void changeZero(char parameters[]);
void compressMatrix();
void writeToFile(char parameters[]);
void readFile(FILE *file);
int compareLine(MatrixElement a, MatrixElement b);
int compareColumn(MatrixElement a, MatrixElement b);
void insertionSort(int (*cmpParameter) (MatrixElement, MatrixElement));
void deleteElement(int start);
void checkLimits();
void printElement(MatrixElement element);

MatrixElement mElements[MAXELEMENTS];
MatrixInfo mInfo = {MAXMLENGHT, 0, MAXMLENGHT, 0, 0, 0, 0.0};
char fileToWrite[MAXFILENAME+1];

#define cleanLimits() {mInfo.minL = mInfo.minC = MAXMLENGHT;\
                       mInfo.maxL = mInfo.maxC = 0;}
#define isLimit(A) (A.line == mInfo.minL || A.line == mInfo.maxL ||\
                   A.column == mInfo.minC ||  A.column == mInfo.maxC)

int main(int argc, char *argv[])
{
    char command, parameters[MAXFILENAME+3];
    FILE *file;

    if (argc == 2) {
        file = fopen(argv[1], "r");
        readFile(file);
        fclose(file);
        strcpy(fileToWrite, argv[1]);
    }

    do {
        command = getchar();
        /* trailing space, \n, \0 */
        fgets(parameters, MAXFILENAME+3, stdin);
        switch(command) {
            case 'a':
                addElement(parameters);
                break;
            case 'p':
                listRelevant();
                break;
            case 'i':
                printMatrixInfo();
                break;
            case 'l':
                printLine(parameters);
                break;
            case 'c':
                printColumn(parameters);
                break;
            case 'o':
                sortElements(parameters);
                break;
            case 'z':
                changeZero(parameters);
                break;
            case 's':
                compressMatrix();
                break;
            case 'w':
                writeToFile(parameters);
                break;
        }
    } while (command != 'q');
    
    return 0;
}

void addElement(char parameters[])
{
    /* Adds a new value to the elements list. If the
    value is a "zero"zero, it will delete the element. */

    unsigned long inLine, inColumn;
    double inValue;
    int i;

    sscanf(parameters, "%lu %lu %lf", &inLine, &inColumn, &inValue); 

    /* Check if the position is already occupied. 
    If it is, just change the value */
    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].line == inLine && mElements[i].column == inColumn) {
            if (inValue != mInfo.zero) {
                mElements[i].line = inLine;
                mElements[i].column = inColumn;
                mElements[i].value = inValue;
            } else {
                deleteElement(i);
            }
            return;
        }
    }
    
    if (mInfo.lenght == MAXELEMENTS || inValue == mInfo.zero) {
        return;
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

    unsigned long size;
    double dens;

    if (mInfo.lenght == 0) {
        printf("empty matrix\n");
        return;
    }

    checkLimits();

    size = (mInfo.maxL-mInfo.minL + 1) * (mInfo.maxC-mInfo.minC + 1);
    dens = ((double) mInfo.lenght/size) * 100;

    printf("[%lu %lu] [%lu %lu] %d / %ld = %.3f%%\n", mInfo.minL, mInfo.minC,
                                                      mInfo.maxL, mInfo.maxC,
                                                      mInfo.lenght, size, dens);    
}

void printLine(char parameters[])
{
    unsigned long inLine;
    int i, empty = 1;

    double values[MAXELEMENTS];
    for (i = 0; i < MAXELEMENTS; i++) {
        values[i] = mInfo.zero;
    }
    
    sscanf(parameters, "%lu", &inLine); 
    checkLimits();

    /* Gather the lines of the elements on that line */
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

void printColumn(char parameters[])
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

    sscanf(parameters, "%lu", &inColumn); 
    element.column = inColumn;

    /* Gather the columns of the elements on that line */
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

void sortElements(char parameters[])
{
    /* Sorts the elements by lines then columns or
    columns then lines if it receives the c argument.*/

    char c;

    int (*lineCmp) (MatrixElement, MatrixElement);
    int (*columnCmp) (MatrixElement, MatrixElement);
    lineCmp = &compareLine;
    columnCmp = &compareColumn;

    sscanf(parameters, " %c", &c);
 
    if (c == 'c')
        insertionSort(columnCmp);
    else
        insertionSort(lineCmp);
}

void changeZero(char parameters[])
{
    /* Changes the current zero and checks if any element
    corresponds with the new zero. Deletes it if it does. */

    int i;
    double newZero;

    sscanf(parameters, "%lf", &newZero);
    mInfo.zero = newZero;

    for (i = 0; i < mInfo.lenght; i++) {
        if (mElements[i].value == mInfo.zero) {
            deleteElement(i);
            i--;
        }
    }
}

void compressMatrix()
{
    checkLimits(); 
}

void writeToFile(char parameters[])
{
    /* Writes the elements to a file in format
    [line;column]=value.*/

    char fileName[MAXFILENAME+1]; 
    int i;
    FILE *file;

    if (parameters[0] != '\n') {
        sscanf(parameters, " %s", fileName);
        strcpy(fileToWrite, fileName);       
    }
    file = fopen(fileToWrite, "w");

    for (i = 0; i < mInfo.lenght; i++) {
        fprintf(file, "[%lu;%lu]=%.3f\n", mElements[i].line, mElements[i].column, 
                                          mElements[i].value);        
    }

    fclose(file);
}

void readFile(FILE *file)
{
    /* Interprets a matrix file and adds its
    elements to the matrix.*/

    unsigned long line, column;
    double value;
    char parameters[MAXFILENAME+3];

    while (fscanf(file, "[%lu;%lu]=%lf\n", &line, &column, &value) != EOF) {
        sprintf(parameters, " %lu %lu %f\n", line, column, value);
        addElement(parameters);
    }
}

int compareLine(MatrixElement a, MatrixElement b)
{
    /* Compares a's line to b's line and returns
    their offset.*/
    return (a.line ==  b.line) ? a.column < b.column : a.line < b.line;
}

int compareColumn(MatrixElement a, MatrixElement b) 
{
    /* Compares a's column to b's column and returns
    their offset.*/
    return (a.column ==  b.column) ? a.line < b.line : a.column < b.column;
}

void insertionSort(int (*cmpParameter) (MatrixElement, MatrixElement))
{
    /* Typical inserton sort algorithm but uses 2 keys as
    comparison. If the first key ties, sorts by the second.*/
    
    int i, j;

    for (i = 1; i < mInfo.lenght; i++) {
        MatrixElement e = mElements[i];
        j = i-1;
        while (j >= 0 && cmpParameter(e, mElements[j])) {
            mElements[j+1] = mElements[j];
            j--;
        }
        mElements[j+1] = e;
    }
}

void deleteElement(int start)
{
    /* Shifts every element to the left starting from
    the index i.*/

    int i;

    if (isLimit(mElements[start])) {
        mInfo.dirty = 1;
    }

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
