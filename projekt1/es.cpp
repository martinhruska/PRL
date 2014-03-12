#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

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
    std::vector<int> res;
 
    // Initialize MPI
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &procId);

   	int n = 0;
    int readNumber = 0;
    std::fstream fInputFile;

    if (procId == 0)
    {
    	char fileName[] = "numbers";
    	fInputFile.open(fileName, std::ios::in);

        readNumber = fInputFile.get();
    	if (fInputFile.good())
    	{ // one char read
            n=1;
            res.push_back(readNumber); // OPTIONAL
    	}

        sendToEveryoneInt(&n, numProcs);
    }

    // Initialize registers
   	int x = UNDEFINED;
   	int y = UNDEFINED;
   	int z = UNDEFINED;
   	int c = 1;

   	// get numbers of elements in read array
    MPI_Recv(&n, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);

    for (int k=0; k < 2*n; k++)
    {
        if (numProcs <= n)
        {
            break;
        }

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
    		y = readNumber;

            readNumber = fInputFile.get();
    	    if (fInputFile.good())
    	    { // another num read
                n+=1;
                res.push_back(readNumber); // OPTIONAL
    	    }
    	    sendToEveryoneInt(&n, numProcs);
	        MPI_Send(&y, 1, MPI_INT, k, TAG, MPI_COMM_WORLD);
    	}
        
        if (k < n)
        {  // update n
            MPI_Recv(&n, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
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

    std::vector<int> output;
    for(int k=0; k < n; k++)
    {
    	if (procId == numProcs - 1)
    	{ // output = z_n
    		output.insert(output.begin(), z);
    	}
    	else
    	{ // send z_i to next
        	MPI_Send(&z, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
        }

        if (procId > 0)
        { // z_i+1 = z_i
			MPI_Recv(&z, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);
    	}
    }

    if (procId == numProcs-1 && numProcs > n)
    {
    	for (std::vector<int>::iterator num = output.begin(); num != output.end(); num++)
    	{
    		std::cout << *num << " ";
    	}
    	std::cout << std::endl;
    }
    else if (procId == numProcs-1 && numProcs <= n)
    {
        std::cerr  <<  "Not enough processors"  <<  std::endl;
    }

    if (procId == 0)
    {
        fInputFile.close();
    }

    // OPTIONAL
    if (procId == 0)
    {
    	//print right order
    	std::sort(res.begin(), res.end());
    	for (std::vector<int>::iterator num = res.begin(); num != res.end(); num++)
    	{
    		std::cout << *num << " ";
    	}
    	std::cout << std::endl;
    }

    MPI_Finalize();
	return 0;
}
