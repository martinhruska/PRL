/**
 * Project: Carry look-ahead adder
 * Author: Martin Hruska
 * E-mail: xhrusk16@stud.fit.vutbr.cz
 */

#include <mpi.h>

const int TAG = 0;
const int UNDEFINED = -1;

void sendToEveryoneInt(int *what, int procs)
{
	for (int i=0; i < procs; i++)
    {
		MPI_Send(what, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
    }
}


int main(int argc, char *argv[])
{
    int numProcs;
    int procId;
    MPI_Status stat;
 
    // Initialize MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	       // MPI_Send(&y, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
			//MPI_Recv(&y, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);

    if (procId == 0)
    { // master reads input
    }

    
    
    MPI_Finalize(); 
	return 0;
}
