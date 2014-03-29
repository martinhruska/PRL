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
    		//std::cerr << number  <<  " ";
    		numbers.push_back(number);
    	}
        //std::cerr  <<  std::endl;

    	std::vector<int> res(numbers);
    	//print right order
    	std::sort(res.begin(), res.end());
    	for (std::vector<int>::iterator num = res.begin(); num != res.end(); num++)
    	{
    		std::cout << *num << " ";
    	}
    	std::cout << std::endl;

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

    // Not enought processors
    if (numProcs <= n && procId == 0)
    { // print error
        std::cerr  <<  numProcs <<   " processors are not enough for "  <<  n  <<  " numbers"  <<  std::endl;
    }
    if (numProcs <= n)
    { // end because of processor lack
        MPI_Finalize(); 
        return 0;
    }

    double startTime = MPI::Wtime();

    // main sorting loop
    for (int k=0; k < 2*n; k++)
    {
    	int h = getH(k, n);
        int currentProc = k;

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

    	if (procId >= h && procId < numProcs-1 && procId >= 1)
    	{ // y_i sends its value to its neighbour
	        MPI_Send(&y, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
    	}

    	if (procId >= h+1 && procId < numProcs)
    	{ // y_i+1 receive message
			MPI_Recv(&y, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);
    	}

    	if (k < n && procId == 0)
    	{ // master reads a new number to y and sends it to x of the k-th cpu
    		y = numbers[0];
    		numbers.erase(numbers.begin());
	        MPI_Send(&y, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
    	}
        /*
        if (k < n && procId == 1)
        { // y_1  receives new input
			MPI_Recv(&y, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
        }
        */

    	if (k < n && currentProc == procId)
    	{ // x_k = nextinput
			MPI_Recv(&x, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    	}
    	if (k >= n)
    	{
    		if (procId == currentProc -n)
    		{ // send C_{k-n}
    			sendToEveryoneInt(&c, numProcs);
	        	MPI_Send(&x, 1, MPI_INT, c, TAG, MPI_COMM_WORLD);
    		}

    		// get ID of x_{k-n} receiver 
			int recId;
			MPI_Recv(&recId, 1, MPI_INT, currentProc-n, TAG, MPI_COMM_WORLD, &stat);
			if (recId == procId)
			{ // receive x_{k-n}
				MPI_Recv(&z, 1, MPI_INT, currentProc-n, TAG, MPI_COMM_WORLD, &stat);
			}
    	}
    }
    //std::cout << "My number is " << procId << " and value: " << z << std::endl;

    std::vector<int> output;
    for(int k=1; k < n+1; k++)
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

    double endTime = MPI::Wtime();

    if (procId == 0)
    {
        //std::cout  <<  numProcs  <<  " "  <<  endTime - startTime  <<  std::endl;
    }
    if (procId == numProcs-1)
    {
    	for (std::vector<int>::iterator num = output.begin(); num != output.end(); num++)
    	{
    		std::cout << *num << " ";
    	}
    	std::cout << std::endl;
    }
    MPI_Finalize(); 
	return 0;
}
