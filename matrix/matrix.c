#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

#define MAX 4;
int MAX;

struct v {
  size_t i;
  size_t j;
};

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) { int core = *(int*)args;
  for (int i = core * MAX / 4; i < (core + 1) * MAX / 4; i++) {
    for (int j = 0; j < MAX; j++) {
      matSumResult[i][j] = matA[i][j] + matB[i][j];
     }
  }
  
  return NULL;
}
// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { int core = *(int*) args;
  for (int i = core * MAX / 4; i < (core + 1) * MAX / 4; i++) {
    for (int j = 0; j < MAX; j++) {
      matDiffResult[i][j] = matA[i][j] - matB[i][j];
    }
  }
  
  return NULL;
}



// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { struct v *data = (struct v *)args;
  size_t l,d;
  for(l=0; l < MAX; l++) {
    size_t i=(data[l]).i;
    size_t j=(data[l]).j;
    double sum = 0;
    for (d = 0; d < MAX; d++) {
      sum = sum + matA[i][d]*matB[d][j];
    }
    
    matProductResult[i][j] = sum;
    sum = 0;
  }
    
  return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 0. Get the matrix size from the command line and assign it to MAX
    
    // 1. Fill the matrices (matA and matB) with random values.
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t threads[MAX];
    
    // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
    size_t i, k;
     struct v **values;
     values = (struct v **)malloc(MAX * sizeof(struct v*));

    for(i = 0; i < MAX; i++)
    {
      values[i] = (struct v *)malloc(MAX * sizeof(struct v));
      for(k = 0; k < MAX; k++) {
        values[i][k].i = i;
        values[i][k].j = k;
      }
      pthread_create(&threads[i], NULL, computeProduct, values[i]);
      pthread_create(&threads[i], NULL, computeSum, values[i]);
      pthread_create(&threads[i + MAX], NULL, computeDiff, values[i]);
    }
    free(values);
    
    // 5. Wait for all threads to finish.
    for(i = 0; i < MAX; i++) {
      pthread_join(threads[i], NULL);
    }
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
  
}



