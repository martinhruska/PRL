/**
 * Project: Carry look-ahead adder
 * Author: Martin Hruska
 * E-mail: xhrusk16@stud.fit.vutbr.cz
 */

#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>

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
    std::vector<int> numbers;
 
    // Initialize MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	       // MPI_Send(&y, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
			//MPI_Recv(&y, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);

    if (procId == 0)
    { // master reads input
    	char fileName[] = "numbers";
    	std::fstream fInputFile;
    	fInputFile.open(fileName, std::ios::in);

    	while(fInputFile.good())
    	{
    		int number = fInputFile.get();
    		if (!fInputFile.good())
    		{ // ignores EOF
    			break;
    		}
    		numbers.push_back(number);
            std::cout  << number  <<   " ";
    	}
    	std::cout << std::endl;
 
    }

    double startTime = MPI::Wtime();

    
    
    MPI_Finalize(); 
	return 0;
}
