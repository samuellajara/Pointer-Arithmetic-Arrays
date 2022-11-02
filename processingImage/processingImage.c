#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* Structure for the control of execution of times */
typedef struct times{ 
    int create;
    clock_t run_start;
    clock_t run_end;
}*Times;

/* Function to validate the number of arguments */
void argumentsOK(int argc){ 
     if(argc != 5){
        printf("./processingImage filename (average / median / sobel) row col\n");
        exit(-1);
    }
}

/* Function to create the two matrix dynamically */
unsigned char** createMatrix(int *ROWS, int *COLUMNS){ 
    
    unsigned char **matrixOriginal;
    unsigned char **matrixFiltered;
    
    matrixOriginal = (unsigned char**)malloc(*ROWS * sizeof(unsigned char*)); // Reserve memory for rows
    matrixFiltered = (unsigned char**)malloc(*ROWS * sizeof(unsigned char*));

    for (int i = 0; i < *ROWS; i++) {
        matrixOriginal[i] = (unsigned char*)malloc(*COLUMNS * sizeof(unsigned char)); // Reserve memory for columns
        matrixFiltered[i] = (unsigned char*)malloc(*COLUMNS * sizeof(unsigned char));
    }
    
    return matrixOriginal, matrixFiltered;
}

/*  In this function we open the file that we indicate by argument.
    Read the file by lines
    Store it in an array */
void readFile (unsigned char **matrix, char *file_entry, int *ROWS, int *COLUMNS){ // READ MATRIX BY ROWS
    FILE *f = fopen(file_entry, "rb");
    if(f) { 
        for (int i = 0; i < *ROWS; i++) {
            int bytes_read = fread(matrix[i], sizeof(unsigned char), *COLUMNS, f);  
        }
        fclose(f);
    }
}

/* In this function we open a new file and write inside by rows the values of a matrix */
void writeFile(unsigned char **matrix, int *ROWS, int *COLUMNS){
    FILE *f = fopen("filteredImage.raw", "wb");
    for (int i = 0; i < *ROWS; i++) {
        size_t r1 = fwrite(matrix[i], sizeof(unsigned char), *COLUMNS, f);
    }
    fclose(f);
}

/* This function is used to show us a few data from the matrix of the image that we read */
void showMatrixOrigianl(unsigned char **matrixOriginal){
    
    /*SHOW ORIGINAL MATRIX 10 ELEMENTS*/ 

    printf("\n\nORIGINAL MATRIX\n\n"); 
    for(int i = 0; i < 20; i++){  
        for(int j = 0; j < 20;j++){   
            printf("%i   ",matrixOriginal[i][j]);  
        }   
        printf("\n"); 
    } 
        
}

/* This function is used to show us a few data of the matrix that we are going to write */
void showMatrixFiltered(unsigned char **matrixFiltered){
    
   /*SHOW FILTERED MATRIX 10 ELEMENTS*/
       
    printf("\n\nFILTERED MATRIX \n\n"); 
    for(int i = 0; i < 10; i++){  
        for(int j = 0; j < 10;j++){   
            printf("%i   ",matrixFiltered[i][j]);  
        } 
        printf("\n");   
    }   
    
}

/*  This function calculates the average of 9 elements.
    First we copy the edges of the original to the filtered one.
    Then from position [1][1] we store in variables the values of the positions around them.
    We add those values and divide them by the number of elements, we write the result in the filtered matrix. */
void averageMatrix(unsigned char **matrixOriginal, unsigned char **matrixFiltered, int *ROWS, int *COLUMNS, Times times){

        times -> run_start = clock();
       
       //showMatrixOrigianl(matrixOriginal);
        
        int topLeft, topCenter, topRight, centerLeft, centerCenter, centerRight, downLeft, downCenter, downRight, average; 

        for(int i = 0; i < *ROWS; i++){ // COPY BORDERS
            memcpy(matrixFiltered[i], matrixOriginal[i], *ROWS * sizeof(char) );}
            
        for(int j = 0; j < *COLUMNS; j++){ // COPY BORDERS
             memcpy(matrixFiltered[j], matrixOriginal[j], *COLUMNS * sizeof(char) );}

        for(int i = 1; i < *ROWS; i++){ // BEGIN THE BLUCLE TO DO THE CALCULATIONS
            for (int j = 1; j < *COLUMNS; j++){
            
                if (  (i) > 0 && (j) > 0 && (i+1) < *ROWS && (j+1) < *COLUMNS  ) {
                    // COPY THE VALUE THAT IS IN THE POSITIONS
                    topLeft = (int) matrixOriginal[i-1][j-1];
                    topCenter = (int) matrixOriginal[i-1][j];
                    topRight = (int) matrixOriginal[i-1][j+1];
                    centerLeft = (int) matrixOriginal[i][j-1];
                    centerCenter = (int) matrixOriginal[i][j];
                    centerRight = (int) matrixOriginal[i][j+1];
                    downLeft  = (int) matrixOriginal[i+1][j-1];
                    downCenter  = (int) matrixOriginal[i+1][j];
                    downRight  = (int) matrixOriginal[i+1][j+1];
            
                    average = (topLeft + topCenter + topRight + centerLeft + centerCenter + centerRight + downLeft + downCenter + downRight) / 9;         
                    matrixFiltered[i][j] = average;  // IN THE POSITION I COPY THE VALUE OF THE CALCULATED AVERAGE
                }       
            }
        }
        
        //showMatrixFiltered(matrixFiltered);
        
        writeFile(matrixFiltered, ROWS, COLUMNS);

        times -> run_end = clock();

        double run_times = (double)(times -> run_end - times -> run_start) / CLOCKS_PER_SEC;
        printf(" RUN TIME: %fs.\n ", run_times);

}

/* Function to swap values in a vector. */
void exchangeQuicksort(int *V, unsigned int l, unsigned int r){ 
    int aux;
    aux = V[l];
    V[l] = V[r];
    V[r] = aux;
}

/* Function to sort values in a vector recursively. */
void quicksort(int *V, unsigned int left, unsigned int right){

    unsigned int l, r, p;
    int pivot;

    p = (left + right) / 2;;

    if(p > 0){
        pivot = V[p];
        l = left;
        r = right;

        while(l <= r){
            while(V[l] < pivot)
                l++;

            while(V[r] > pivot)
                r--;

            if(l <= r){
                exchangeQuicksort(V, l, r);

                l++;
                r--;
            }
        }

        if(left < r)
            quicksort(V, left, r);
        if(l < right)
            quicksort(V, l, right);
    }
}

/*  This function calculates the median of 9 elements.
    First we copy the edges of the original to the filtered one.
    Then from position [1][1] we store in a vector the values of the positions around them.
    We sort that vector with Quicksort, pop out the one in the middle position [4], and write it to the filtered matrix. */
void medianMatrix(unsigned char **matrixOriginal, unsigned char **matrixFiltered, int *ROWS, int *COLUMNS, char *opt, Times times){

         times -> run_start = clock();

        //showMatrixOriginal(matrixOriginal);

        int topLeft, topCenter, topRight, centerLeft, centerCenter, centerRight, downLeft, downCenter, downRight; 
        unsigned int median;
        
        for(int i = 0; i < *ROWS; i++){ // COPY BORDERS
            memcpy(matrixFiltered[i], matrixOriginal[i], *ROWS * sizeof(char) );}
            
        for(int j = 0; j < *COLUMNS; j++){ // COPY BORDERS
             memcpy(matrixFiltered[j], matrixOriginal[j], *COLUMNS * sizeof(char) );}

        for(int i = 1; i < *ROWS; i++){ // BEGIN THE BLUCLE TO DO THE CALCULATIONS
            for (int j = 1; j < *COLUMNS; j++){
          
                if (  (i) > 0 && (j) > 0 && (i+1) < *ROWS && (j+1) < *COLUMNS  ) {
                    // COPY THE VALUE THAT IS IN THE POSITIONS
                    topLeft = (int) matrixOriginal[i-1][j-1];
                    topCenter = (int) matrixOriginal[i-1][j];
                    topRight = (int) matrixOriginal[i-1][j+1];
                    centerLeft = (int) matrixOriginal[i][j-1];
                    centerCenter = (int) matrixOriginal[i][j];
                    centerRight = (int) matrixOriginal[i][j+1];
                    downLeft  = (int) matrixOriginal[i+1][j-1];
                    downCenter  = (int) matrixOriginal[i+1][j];
                    downRight  = (int) matrixOriginal[i+1][j+1];

                    int V[9] = {topLeft, topCenter, topRight, centerLeft, centerCenter, centerRight, downLeft, downCenter, downRight};

                    quicksort(V,0,9); // ORDER ARRAY POSITION [0 - 8]
                                
                    median = V[4]; // TAKE CENTER VALUE

                    matrixFiltered[i][j] = median; // IN THE POSITION I COPY THE VALUE OF THE CALCULATED AVERAGE          
                }          
            }
        }
        
        //showMatrixFiltered(matrixFiltered);  
       
        writeFile(matrixFiltered, ROWS, COLUMNS);
        
        times -> run_end = clock();

        double run_times = (double)(times -> run_end - times -> run_start) / CLOCKS_PER_SEC;
        printf(" RUN TIME: %fs.\n ", run_times);

}

/*  This function calculates the sobel of 9 elements.
    Previously we have reserved memory for 2 additional rows and 2 additional columns.
    Copy row/column 1 of the original matrix to 0 of the filtered one, and copy the entire original into the filtered one.
    With the filtered values from position [1][1] we multiply by the fixed numbers and perform the formula.
    Storing these values in the Original again. */
void sobelMatrix(unsigned char **matrixOriginal, unsigned char **matrixFiltered, int ROWS, int COLUMNS, Times times){

        times -> run_start = clock();

        //showMatrixOrigianl(matrixOriginal);
               
        for(int i = 1; i < ROWS - 1; i ++){ // row symmetry
            matrixFiltered[i][0] = matrixOriginal[i - 1][1];
            matrixFiltered[i][COLUMNS - 1] = matrixOriginal[i - 1][COLUMNS - 4];
        }

        for(int j = 1; j < COLUMNS - 1; j ++){ // columns symmetry
            matrixFiltered[0][j] = matrixOriginal[1][j - 1];
            matrixFiltered[ROWS - 1][j] = matrixOriginal[ROWS - 4][j - 1];
        }

        for(int i = 1; i < ROWS - 1; i ++){ // copy values to filtered matrix 
            for(int j = 1; j < COLUMNS - 1; j ++){
                matrixFiltered[i][j] = matrixOriginal[i - 1][j - 1];
            }
        }

        matrixFiltered[0][0] = matrixFiltered[2][2]; // give values to the corners
        matrixFiltered[0][COLUMNS - 1] = matrixFiltered[2][COLUMNS - 3];
        matrixFiltered[ROWS - 1][0] = matrixFiltered[ROWS - 3][2];
        matrixFiltered[ROWS - 1][COLUMNS - 1] = matrixFiltered[ROWS - 3][COLUMNS - 3];

        //showMatrixFiltered(matrixFiltered);

        for(int i = 1; i < ROWS -1; i ++){ // operations with the filtered matrix
            for(int j = 1; j < COLUMNS-1; j ++){ 

                // value of c
                int C = 
                (matrixFiltered[i-1][j-1] * -1) +
                (matrixFiltered[i-1][j]   *  0) +
                (matrixFiltered[i-1][j+1] *  1) +
                (matrixFiltered[i][j-1]   * -2) +
                (matrixFiltered[i][j]     *  0) +
                (matrixFiltered[i][j+1]   *  2) +
                (matrixFiltered[i+1][j-1] * -1) +
                (matrixFiltered[i+1][j]   *  0) +
                (matrixFiltered[i+1][j+1] *  1);

                // value of f
                int F =
                (matrixFiltered[i-1][j-1] * -1) +
                (matrixFiltered[i-1][j]   * -2) +
                (matrixFiltered[i-1][j+1] * -1) +
                (matrixFiltered[i][j-1]   *  0) +
                (matrixFiltered[i][j]     *  0) +
                (matrixFiltered[i][j+1]   *  0) +
                (matrixFiltered[i+1][j-1] *  1) +
                (matrixFiltered[i+1][j]   *  2) +
                (matrixFiltered[i+1][j+1] *  1);

                int sobel = sqrt(pow(C,2) + pow(F,2)); // formula
                matrixOriginal[i - 1][j - 1] = sobel; // rewrite the original
                 
            }
        }

        //showMatrixOrigianl(matrixOriginal);
    
        FILE *f = fopen("filteredImage.raw", "wb");
        for (int i = 0; i < ROWS -2; i++) {
            size_t r1 = fwrite(matrixOriginal[i], sizeof(unsigned char), COLUMNS - 2, f);
        }
        fclose(f);

        times -> run_end = clock();

        double run_times = (double)(times -> run_end - times -> run_start) / CLOCKS_PER_SEC;
        printf(" RUN TIME: %fs.\n ", run_times);
}

int main(int argc, char *argv[]){  // CONSOLE --> gcc processingImage.c -o processingImage -lm row col && ./processingImage lena512x512.bin average 512 512

    argumentsOK(argc); // ./processingImage lena512x512.raw average 512 512

    int row = atoi(argv[3]); // Fetch the value and cast it to int
    int col = atoi(argv[4]);

    unsigned char **matrixOriginal = createMatrix(&row, &col); // Create original matrix
    unsigned char **matrixFiltered;

    Times times = (struct times*)malloc(sizeof(struct times)); // Initialize the time structure

    FILE *f = fopen(argv[1], "rb"); // open file by argument
   
    if(f){ // if the file.raw exists ...
        
        times -> create = 0;

        readFile(matrixOriginal, argv[1], &row, &col);
       
        if(strcmp(argv[2], "average") == 0){ // compare two strings, if they are the same then = 0
            matrixFiltered = createMatrix(&row, &col);
            averageMatrix(matrixOriginal, matrixFiltered, &row, &col, times);

        }else if(strcmp(argv[2], "median") == 0){
            matrixFiltered = createMatrix(&row, &col);
            medianMatrix(matrixOriginal, matrixFiltered, &row, &col, argv[2], times);

        }else if(strcmp(argv[2], "sobel") == 0){
             
            int row2 = row + 2; // reserve additional memory
            int col2 = col + 2; 
            
            matrixFiltered = (unsigned char**)malloc(row2 * sizeof(unsigned char*));

            for (int i = 0; i < row2; i++) {
                matrixFiltered[i] = (unsigned char*)malloc(col2 * sizeof(unsigned char));
            }

            sobelMatrix(matrixOriginal, matrixFiltered, row2, col2, times);

        }else{
            printf("UNDEFINED FUNCTION");
        } 

    } else{
        printf("THE FILE.RAW ISN'T\n");
    }  
    
    free(times);
    free(matrixOriginal);
    free(matrixFiltered);

    return 0;
}