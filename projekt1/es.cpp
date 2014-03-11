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


int getH(int k, int n)
{
	if (k < n)
	{
		return 0;
	}
	else
	{
		return k - n;
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

    if (procId == 0)
    {
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
    		std::cout << "Loaded number " << number << std::endl;
    		numbers.push_back(number);
    	}
    	int inNumbersSize = numbers.size();
    	sendToEveryoneInt(&inNumbersSize, numProcs);
    	fInputFile.close();
    }

    // Initialize registers
   	int x = UNDEFINED;
   	int y = UNDEFINED;
   	int z = UNDEFINED;
   	int c = 1;

   	int n = 0;

   	// get numbers of elements in read array
    MPI_Recv(&n, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

    for (int k=0; k < 2*n; k++)
    {
    	int h = getH(k, n);

    	if (x != UNDEFINED && y != UNDEFINED)
    	{
    	 	if (x > y)
    	 	{
    			++c;
    		}
    		// this solves problem with equal numbers
    		else if (x == y && procId+1+procId < k)
    		{
   				++c;
    			//std::cout << "Proc " << procId << " in iter " << k << " " << comparingSame << ": " << x <<" is greater or equal then " <<y<<std::endl;
    		}
    	}

    	if (procId >= h && procId < numProcs-1)
    	{ // y_i sends its value to its neighbour
	        MPI_Send(&y, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
    	}

    	if (procId >= h+1 && procId < numProcs)
    	{ // y_i+1 receive message
			MPI_Recv(&y, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);
    	}

    	if (k < n && procId == 0)
    	{ // master reads a new number to y and sends it to x
    		y = numbers[0];
    		numbers.erase(numbers.begin());
	        MPI_Send(&y, 1, MPI_INT, k, TAG, MPI_COMM_WORLD);
    	}
    	if (k < n && k == procId)
    	{ // x_k = nextinput
			MPI_Recv(&x, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    	}
    	if (k >= n)
    	{
    		if (procId == k - n)
    		{ // send C_{k-n}
    			sendToEveryoneInt(&c, numProcs);
	        	MPI_Send(&x, 1, MPI_INT, c, TAG, MPI_COMM_WORLD);
    		}

    		// get ID of x_{k-n} receiver 
			int recId;
			MPI_Recv(&recId, 1, MPI_INT, k-n, TAG, MPI_COMM_WORLD, &stat);
			if (recId == procId)
			{ // receive x_{k-n}
				MPI_Recv(&z, 1, MPI_INT, k-n, TAG, MPI_COMM_WORLD, &stat);
			}
    	}
    }

    std::cout << "My number is " << procId << " and value: " << z << std::endl;
    MPI_Finalize(); 
	return 0;
}