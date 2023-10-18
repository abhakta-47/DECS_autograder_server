#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

// Maximum Dimension of Matrix
#define MAXLEN 5000

// Mutex for ensuring access to rowReady is safe
pthread_mutex_t lock[10];

// Condition variable to signal if rows are ready
pthread_cond_t rowrdy[10];

// Variable to confirm if the rows are ready for each thread
int rowReady[10];

// Array to store the computed results by the threads
// elementValues[i][k] stores the value of the kth element calculated by the ith thread
int elementValues[10][MAXLEN/10+1];

// Array to store the vector a
int a[MAXLEN];

// Array to store the vector b
int b[MAXLEN];

// Array to store the rows given to a thread,
// row[i][j] represents the jth row given to the ith thread
int row[10][MAXLEN/10+1][MAXLEN];

// Final answer vector 
int ans[MAXLEN];

// Variables to pass the thread IDs as function arguments
int threadIDs[10];

//Mutex for safe access of ctr
pthread_mutex_t ctrlock;

// Condition variable to signal if all threads are done
pthread_cond_t allDone;

// Counter of completed threads
int ctr=0;

// Number of active threads
int nThreads=0;

// Variable to keep track of threads with extra number of rows
// Eg if there are 153 rows, we will have the first (153%10)=3 threads assigned (153/10)+1 = 16 rows
// and the next 7 threads will have (153/10) = 15 rows assigned
int extra=0;

// Row Number and Column Number of matrix
int rowNumber=0,colNumber=0;

// Actual function for calculating vector elements
void *vectorDotProduct(void *thrID) {
	
  // Thread ID
  int id = *((int*) (thrID));
		
	printf("\n Thread %d ready for work\n", id);	
	
	while (1) {

    // If row is not ready, waiting on rowrdy[id]
    //***This is a busy wait, change this to waiting on condition variable rowrdy[id]
   		pthread_mutex_lock(&lock[id]);
		while (!rowReady[id])
		{
			pthread_cond_wait(&rowrdy[id], &lock[id]);
		}
		pthread_mutex_unlock(&lock[id]);
		
      // If thread is among first "extra" threads i.e. the threads with one extra row to multiply,
      // calculate elements for (rowNumber)/10 +1 rows
      // If thread is not among first "extra" threads, calculate elements for (rowNumber)/10 rows
			
    if(id<extra){
      for(int i=0;i<rowNumber/10+1;i++){
        int elementValue=0;
        for(int k=0;k<colNumber;k++){

            elementValue+=row[id][i][k]*a[k];
        }
        elementValues[id][i]=elementValue;
        printf("Element values calculated by thread %d at row %d = %d\n", id, i,elementValue);
      }
    }else{
      for(int i=0;i<rowNumber/10;i++){
        int elementValue=0;
        for(int k=0;k<colNumber;k++){

            elementValue+=row[id][i][k]*a[k];
        }
        elementValues[id][i]=elementValue;
        printf("Element values calculated by thread %d at row %d = %d\n", id, i,elementValue);
      }
    }
    
    // After calculation set rowReady as 0 and increment ctr
		pthread_mutex_lock(&lock[id]);
		rowReady[id] = 0;
		pthread_mutex_unlock(&lock[id]);
    //*** This needs to be under mutex
    		pthread_mutex_lock(&ctrlock);
		ctr++;
		
	   
    //*** If ctr is same as number of active threads, signal that all threads are done
		//*** Add allDone condition variable signal 
	if(ctr == nThreads){pthread_cond_signal(&allDone);}
	pthread_mutex_unlock(&ctrlock);
	}	
  return NULL;
    
}



int main (int argc, char* argv[]) {

  
  //create 10 worker threads for letter count
  pthread_t p[10];
  for (int i = 0; i < 10; i++) {
    // Initialization of thread IDs and initially no rows are ready
  	rowReady[i] = 0;
    threadIDs[i]=i;
	  int rc = pthread_create(&p[i], NULL, vectorDotProduct, (void *)(&threadIDs[i]));
  	assert(rc == 0);
    //***Initialize per thread condition variables and mutexes. Add this.
	pthread_cond_init(&rowrdy[i], NULL);
  	pthread_mutex_init(&lock[i], NULL);


  }
     
  //***Initialize other mutexes and condition variables. Add this. 
	 pthread_cond_init(&allDone, NULL);
 	 pthread_mutex_init(&ctrlock, NULL);

	
 	FILE* fd;
	char fname[20];

  while (1) {
		
			for (int i = 0; i < 10; i++) {
				// Set all values in elementValues to be 0
				memset(elementValues[i],0,sizeof(elementValues[i]));
			// Set all values in row to be 0
				for(int j=0;j< MAXLEN/10+1;j++){
				  for(int k=0;k< MAXLEN; k++)
						row[i][j][k] = 0;
				}  
				
				// Initialization of thread IDs and initially no rows are ready
				
				rowReady[i] = 0;
			}
			// Initialize final answer W a + b   to 0
			memset(ans, 0, sizeof(ans));
		  
		  //Initialize counter of how many threads are done, to 0
		  // Initialize number of threads that get work to 0
			ctr = 0; nThreads=0;
		  		  
			memset(fname, 0, 20);

			// Taking filename from which to read as input
			printf("Enter filename: ");
			scanf("%s", fname);

			fd = fopen(fname, "r");

			// Reading row and column numbers
			printf ("Enter Row and Column Numbers: \n");
			fscanf(fd,"%d %d",&rowNumber,&colNumber);

			if(rowNumber>MAXLEN || colNumber>MAXLEN){

				printf("Matrix Sizes too Large!\n");
				exit(-1);
			}

			// Reading vector a  
			printf ("Enter Vector A:\n");
			for(int i=0;i<colNumber;i++){

				fscanf(fd,"%d",&a[i]);
			}
			// Reading vector b  

			printf ("Enter Vector B:\n");
			for(int i=0;i<rowNumber;i++){

				fscanf(fd,"%d",&b[i]);
			}

			// Reading matrix

			printf ("Enter Matrix: \n");
			
			//-------------------------------------------------------------------			
			// Read the W(n) matrix and distribute to threads.
			// You need not understand this logic. Skip to the end of this part. 
			
			// The number of threads with extra rows = rowNumber%10
			extra = rowNumber%10;

			// Reading rows for the first extra threads
			for(int i=0;i<extra;i++){
				for(int j=0;j<(rowNumber/10)+1;j++){
				  for(int k=0;k<colNumber;k++){
				    fscanf(fd,"%d",&row[i][j][k]);
				  }
				}
			}
			// Reading rows for the rest ofthe threads
			for(int i=extra;i<(rowNumber<10? rowNumber:10);i++){
				for(int j=0;j<(rowNumber/10);j++){
				  for(int k=0;k<colNumber;k++){
				    fscanf(fd,"%d",&row[i][j][k]);
				  }
				}
			}

			// Number of threads is less than 10 only when number of rows is less than 10
			nThreads= rowNumber<10? rowNumber:10;
				  
			printf("nThreads = %d \n",nThreads);

			// End of reading matrix and row distribution logic. 
  		//-------------------------------------------------------------------
			
			// When matrix is read, set rowReady as 1 and signal condition variable
		 		for (int i = 0; i < nThreads; i++) {
					pthread_mutex_lock(&lock[i]);
					rowReady[i] = 1;
					//*** add signal on condition variable
					pthread_cond_signal(&rowrdy[i]);
					pthread_mutex_unlock(&lock[i]);
				}
			

			// Wait till threads are done 
			// ***Add wait on condition variable allDone
			pthread_mutex_lock(&ctrlock);
			while (ctr != nThreads)
			{
				pthread_cond_wait(&allDone, &ctrlock);
			} 
			
			pthread_mutex_unlock(&ctrlock);
		
			int currentRow=0;
			currentRow=0;

			// Print the final answer, after adding with vector b, taking care that the first "extra" threads have an extra row
			printf("Final Answer: [");

			for(int i=0;i<extra;i++){
				for(int j=0;j<(rowNumber/10+1);j++){
				  ans[currentRow] = elementValues[i][j] + b[currentRow];
				  printf("%d ",ans[currentRow]);
				  currentRow++;
				}
			}

			for(int i=extra;i<10;i++){
				for(int j=0;j<(rowNumber/10);j++){
				  ans[currentRow] = elementValues[i][j] + b[currentRow];
				  printf("%d ",ans[currentRow]);
				  currentRow++;
				}
			}
			printf("]\n");		
		}
}

