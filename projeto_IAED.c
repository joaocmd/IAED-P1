/* Joao Carlos Morgado David - 89471 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#define MAXFILENAME 80
#define MAXELEMENTS 10000
#define MAXMLENGTH ULONG_MAX
#define MAXLINEDIFFERENCE 20000

typedef struct {
    unsigned long line, column;
    double value;
} MatrixElement;

typedef struct {
    MatrixElement elements[MAXELEMENTS];
    unsigned long minL, maxL;
    unsigned long minC, maxC;
    unsigned long size;
    /* Dirty flags if the limits are possibly wrong.
    e.g. after deleting an element that is
    part of the limits.*/
    int length, dirty;
    double zero;
} SparseMatrix;

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
int lessLine(MatrixElement a, MatrixElement b);
int lessColumn(MatrixElement a, MatrixElement b);
void insertionSort(int (*cmpParameter) (MatrixElement, MatrixElement));
void sortByLineAndDensity(MatrixElement elements[], int nEls[]);
void printCompressedMatrix(double values[], unsigned long indexes[],
                           int offsets[], int maxOffset);
void deleteElement(int start);
void checkLimits();
void printElement(MatrixElement element);

SparseMatrix matrix;
char fileToWrite[MAXFILENAME+1];

#define mElements matrix.elements
#define cleanLimits() {matrix.minL = matrix.minC = MAXMLENGTH;\
                       matrix.maxL = matrix.maxC = 0;}
#define isLimit(A) (A.line == matrix.minL || A.line == matrix.maxL ||\
                   A.column == matrix.minC ||  A.column == matrix.maxC)

int main(int argc, char *argv[])
{
    char command, parameters[MAXFILENAME+3];
    FILE *file;

    /* Initialize the limits. */
    cleanLimits();
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
    for (i = 0; i < matrix.length; i++) {
        if (mElements[i].line == inLine && mElements[i].column == inColumn) {
            if (inValue != matrix.zero) {
                mElements[i].line = inLine;
                mElements[i].column = inColumn;
                mElements[i].value = inValue;
            } else {
                deleteElement(i);
            }
            return;
        }
    }
    
    if (matrix.length == MAXELEMENTS || inValue == matrix.zero) {
        return;
    } else {
        mElements[matrix.length].line = inLine; 
        mElements[matrix.length].column = inColumn; 
        mElements[matrix.length].value = inValue;

        /* Check as we add so that we don't iterate 
        over the whole array everytime.*/
        if (inLine < matrix.minL)
            matrix.minL = inLine;
        if (inLine > matrix.maxL)
            matrix.maxL = inLine;
        if (inColumn < matrix.minC)
            matrix.minC = inColumn;
        if (inColumn > matrix.maxC)
            matrix.maxC = inColumn;

        matrix.length++;
    }
} 

void listRelevant()
{
    /* Lists every element of the matrix that isn't a zero. */

    int i;

    if (matrix.length == 0) {
        printf("empty matrix\n");
        return;
    }

    for (i = 0; i < matrix.length; i++) {
        printElement(mElements[i]);
    }
}

void printMatrixInfo()
{
    /* Prints the "boundaries" of the matrix and
    its density inside that range.*/ 

    float dens;

    if (matrix.length == 0) {
        printf("empty matrix\n");
        return;
    }

    checkLimits();

    dens = (float) matrix.length/matrix.size;

    printf("[%lu %lu] [%lu %lu] %d / %ld = %.3f%%\n", matrix.minL, matrix.minC,
                                                      matrix.maxL, matrix.maxC,
                                                      matrix.length, matrix.size,
                                                      dens * 100);
}

void printLine(char parameters[])
{
    /* Prints a given line in format: value1 value2 ... valuen */

    unsigned long inLine, nColumns;
    int i, empty = 1;
    double values[MAXELEMENTS];

    for (i = 0; i < MAXELEMENTS; i++) {
        values[i] = matrix.zero;
    }
    sscanf(parameters, "%lu", &inLine); 
    checkLimits();

    /* Gather the lines of the elements on that line */
    for (i = 0; i < matrix.length; i++) {
        if (mElements[i].line == inLine) {
            values[mElements[i].column - matrix.minC] = mElements[i].value;
            empty = 0;
        }
    }

    if (empty) {
        printf("empty line\n");
    } else {
        nColumns = matrix.maxC-matrix.minC + 1;
        for (i = 0; i < nColumns; i++) {
            printf(" %.3f", values[i]);
        }
        printf("\n");
    }
}

void printColumn(char parameters[])
{
    /* Prints a given column vertically in format [line;column]=value */

    unsigned long inColumn, nLines;
    int i, empty = 1;
    double values[MAXELEMENTS];

    for (i = 0; i < MAXELEMENTS; i++) {
        values[i] = matrix.zero;
    }
    sscanf(parameters, "%lu", &inColumn); 
    checkLimits();

    /* Gather the columns of the elements on that line */
    for (i = 0; i < matrix.length; i++) {
        if (mElements[i].column == inColumn) {
            values[mElements[i].line - matrix.minL] = mElements[i].value;
            empty = 0;
        }
    }

    if (empty) {
        printf("empty column\n");
    } else { 
        nLines = matrix.maxL-matrix.minL + 1;
        for (i = 0; i < nLines; i++) {
            printf("[%lu;%lu]=%.3f\n", i+matrix.minL, inColumn, values[i]);
        }
    }
}

void sortElements(char parameters[])
{
    /* Sorts the elements by lines then columns or
    columns then lines if it receives the c argument.*/

    char c;
    sscanf(parameters, "%c", &c);
 
    if (c != '\n')
        insertionSort(lessColumn);
    else
        insertionSort(lessLine);
}

void changeZero(char parameters[])
{
    /* Changes the current zero and removes it from the matrix.
    Uses a different algorithm than deleteElement to 
    optimize when there are multiple values with the new "zero".*/

    int i, j;
    double newZero;

    sscanf(parameters, "%lf", &newZero);
    matrix.zero = newZero;

    for (i = 0, j = 0; i < matrix.length; i++) {
        if (mElements[i].value != matrix.zero) {
            mElements[j] = mElements[i];
            j++;
        }
    }
    matrix.dirty = 1;
    matrix.length = j;
}



void compressMatrix()
{
    /* Calculates the compression of the global matrix 
    with the given algorithm. I'm probably declaring way
    too big */

    int i, j, largestIndexInLine;
    double values[MAXELEMENTS*2];
    unsigned long indexes[MAXELEMENTS*2] = {0};
    int offset, maxOffset = 0, offsets[MAXELEMENTS] = {0};
    int nElsLine, nElsLines[MAXELEMENTS] = {0};
    MatrixElement elements[MAXELEMENTS];
 
    checkLimits(); 
    if ((float) matrix.length/matrix.size > 0.5) {
        printf("dense matrix\n");
        return;
    }

    for (i = 0; i < matrix.length; i++) {
        elements[i] = mElements[i]; 
        nElsLines[mElements[i].line - matrix.minL]++;
        values[i] = matrix.zero;
    }
    sortByLineAndDensity(elements, nElsLines);

    /* The main for goes from line to line, lines are all
    grouped on the array. */
    for (i = 0; i < matrix.length; i += nElsLine) {
        nElsLine = nElsLines[elements[i].line - matrix.minL];
        offset = 0;
        /* Iterates over each element of the line.
        Restarts if it encounters an occupied slot. */
        largestIndexInLine = i+nElsLine;
        for (j = i; j < largestIndexInLine; j++) {
            if (values[elements[j].column+offset - matrix.minC] != matrix.zero) {
                j = i-1;
                offset++;
            }
        }
        /* Add the correct values to the arrays. */
        for (j = i; j < largestIndexInLine; j++) {
            values[elements[j].column+offset - matrix.minC] = elements[j].value;
            indexes[elements[j].column+offset - matrix.minC] = elements[i].line;
        }
        offsets[elements[i].line - matrix.minL] = offset;
        if (offset > maxOffset) {
            maxOffset = offset;
        }
    }

    printCompressedMatrix(values, indexes, offsets, maxOffset);
}

void printCompressedMatrix(double values[], unsigned long indexes[],
                           int offsets[], int maxOffset)
{
    /* Prints the representation of a compressed matrix with its
    given values, indexes and offsets.*/
    int i, nLines, compressedLength;

    compressedLength = (matrix.maxC-matrix.minC + 1) + maxOffset;
    nLines = matrix.maxL-matrix.minL + 1;
    printf("value =");
    for (i = 0; i < compressedLength; i++)
        printf(" %.3f", values[i]);
    printf("\nindex =");
    for (i = 0; i < compressedLength; i++)
        printf(" %lu", indexes[i]);
    printf("\noffset =");
    for (i = 0; i < nLines; i++)
        printf(" %d", offsets[i]);
    printf("\n");

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

    for (i = 0; i < matrix.length; i++) {
        fprintf(file, "[%lu;%lu]=%.3f\n", mElements[i].line, 
                                          mElements[i].column, 
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

int lessLine(MatrixElement a, MatrixElement b)
{
    /* Compares lines first then columns, returns if
    it's smaller*/
    return (a.line < b.line || (a.line == b.line && a.column < b.column));
}

int lessColumn(MatrixElement a, MatrixElement b)
{
    /* Compares columns first then lines, returns if
    it's smaller*/
    return (a.column < b.column || (a.column == b.column && a.line < b.line));
}

void insertionSort(int (*cmpParameter) (MatrixElement, MatrixElement))
{
    /* Typical inserton sort algorithm, takes a function pointer
    for the comparation. */
    
    int i, j;
    MatrixElement e;

    for (i = 1; i < matrix.length; i++) {
        e = mElements[i];
        j = i-1;
        while (j >= 0 && cmpParameter(e, mElements[j])) {
            mElements[j+1] = mElements[j];
            j--;
        }
        mElements[j+1] = e;
    }
}

void sortByLineAndDensity(MatrixElement elements[], int nEls[])
{
    /* Used to sort list by the density of its line. Uses insertion
    sort algorithm. nEls is number of elements for each line (nEls[0] is
    the number of elements in line 0 if minL is 0)*/
    
    int i, j;
    MatrixElement e;

    for (i = 1; i < matrix.length; i++) {
        e = elements[i];
        j = i-1;
        while (j >= 0 && 
              (nEls[elements[j].line - matrix.minL] < nEls[e.line - matrix.minL] ||\
              (nEls[elements[j].line - matrix.minL] == nEls[e.line - matrix.minL] &&\
              elements[j].line > e.line))) {
            
            elements[j+1] = elements[j];
            j--;
        }
        elements[j+1] = e;
    }
}

void deleteElement(int start)
{
    /* Shifts every element to the left starting from
    the index start.*/

    int i;

    if (isLimit(mElements[start])) {
        matrix.dirty = 1;
    }

    for (i = start+1; i < matrix.length; i++) {
        mElements[i-1] = mElements[i];
    }

    matrix.length--;
}

void checkLimits() 
{
    /* Checks if the limits of the matrix are right if
    there's a possibility they are wrong. Also updates the size. */

    int i;

    /* If we're sure the limits are corret there's no reason
    to reset them.*/
    if (matrix.dirty) {
        cleanLimits();
        for (i = 0; i < matrix.length; i++) {
            if (mElements[i].line < matrix.minL)
                matrix.minL = mElements[i].line;
            if (mElements[i].line > matrix.maxL)
                matrix.maxL = mElements[i].line;
            if (mElements[i].column < matrix.minC)
                matrix.minC = mElements[i].column;
            if (mElements[i].column > matrix.maxC)
                matrix.maxC = mElements[i].column;
        }
    }
    matrix.dirty = 0;
    matrix.size = (matrix.maxL-matrix.minL + 1) *  (matrix.maxC-matrix.minC + 1);
}

void printElement(MatrixElement e) 
{
    /* Prints the line, column and value of an element. */
    printf("[%lu;%lu]=%.3f\n", e.line, e.column, e.value);
}
