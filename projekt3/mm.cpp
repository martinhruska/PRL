/**
 * Project: Carry look-ahead adder
 * Author: Martin Hruska
 * E-mail: xhrusk16@stud.fit.vutbr.cz
 */

#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>

const int TAG = 0;
const int UNDEFINED = -1;

typedef enum dtype {UNDEF=-1, P, S, G} DTYPE;

void sendMasterToEveryoneInt(int *what, int procs)
{
	for (int i=1; i < procs; i++)
    {
		MPI_Send(what, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
    }
}

int nand(int o1, int o2)
{
    return (o1-o2)*(o1-o2);
}

DTYPE initD(int X, int Y)
{
    if (X == 1 && Y == 1)
        return G;
    else if (X == 0 && Y == 0)
        return S;
    else
        return P;
}

DTYPE opD(DTYPE o1, DTYPE o2)
{
    if (o1 == S)
        return S;
    else if (o1 == P)
        return o2;
    else if (o1 == G)
        return G;
}

char trans(DTYPE d)
{
    if (d == S)
        return 'S';
    else if (d == P)
        return 'P';
    else if (d == G)
        return 'G';
}

int main(int argc, char *argv[])
{
    typedef std::vector<int> Num;
    int numProcs;
    int procId;
    MPI_Status stat;
    Num num1 = Num();
    Num num2 = Num();

    int lenNum1 = num1.size();
    int lenNum2 = num2.size();
    int realSize = 0;
    
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
        Num* actNum = &num1;

    	while(fInputFile.good())
    	{
    		int number = fInputFile.get();
    		if (!fInputFile.good())
    		{ // ignores EOF
    			break;
    		}

            if (number == '\n')
            { // end of line
                actNum = &num2;
                std::cout  <<  std::endl;
                continue;
            }

            /*
            if (number == '0')
            {
    		    actNum->push_back(0);
                std::cout  << 0  <<   " ";
            }
            else
            {
    		    actNum->push_back(1);
                std::cout  << 1  <<   " ";
            }
            */
    		actNum->push_back(number);
            std::cout  << number  <<   " ";
    	}
    	std::cout << std::endl;

        realSize = num1.size();
        while (num1.size() < numProcs-1)
        {
            num1.insert(num1.begin(),0);
            num2.insert(num2.begin(),0);
        }
        lenNum1 = num1.size();
        lenNum2 = num2.size();
        sendMasterToEveryoneInt(&realSize, numProcs);
        sendMasterToEveryoneInt(&lenNum1, numProcs);
        sendMasterToEveryoneInt(&lenNum2, numProcs);
    }

    if (procId != 0)
    { // slaves get information about size of real input
    	MPI_Recv(&realSize, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    	MPI_Recv(&lenNum1, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    	MPI_Recv(&lenNum2, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
    }

    if (lenNum1 != lenNum2 || lenNum1+1 != numProcs)
    { // check if input is correct
        if (procId == 0)
        {
            std::cerr  << "Wrong number of CPU or length of numbers is not same"  <<  std::endl;
        }
        MPI_Finalize();
        return 0;
    }

    // start computing result
    int X = UNDEFINED;
    int Y = UNDEFINED;
    DTYPE D = UNDEF;

    if (procId == 0)
    {
        for (int i=1, j=lenNum1; i < numProcs; ++i,--j)
        {
	        MPI_Send(&num1[j-1], 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
	        MPI_Send(&num2[j-1], 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }
    }
    else
    {
	    MPI_Recv(&X, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
	    MPI_Recv(&Y, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
        D = initD(X,Y);
    }
    //double startTime = MPI::Wtime();
    
    if (procId != 0)
    { // compute scan
        if (procId == 1)
        {
            D = opD(D,S);
        }
        for (int j = 0; j <= log2(lenNum1)-1; j++)
        { // up-sweep
            int stepRec = pow(2,j+1);
            int stepSend = pow(2,j);

            if (procId%stepRec != 0 && procId%stepSend == 0 && procId+stepSend < numProcs)
            {
	            MPI_Send(&D, 1, MPI_INT, procId+stepSend, TAG, MPI_COMM_WORLD);
            }

            if (procId%stepRec == 0 && procId-stepSend > 0)
            {
                DTYPE temp = UNDEF;
	            MPI_Recv(&temp, 1, MPI_INT, procId-stepSend, TAG, MPI_COMM_WORLD, &stat);
                D = opD(D,temp);
            }
        }

        DTYPE reduce = UNDEF;
        if (procId == numProcs -1)
        {
            reduce = D;
            D = S;
        }

        for (int j = log2(lenNum1)-1; j >= 0; j--)
        { // down-sweep
            int stepParent = pow(2,j+1);
            int stepLeft = pow(2,j);

            if (procId%stepParent != 0 && procId%stepLeft == 0 && procId+stepLeft < numProcs)
            { // left sends its value and receives value of parent
	            MPI_Send(&D, 1, MPI_INT, procId+stepLeft, TAG, MPI_COMM_WORLD);
	            MPI_Recv(&D, 1, MPI_INT, procId+stepLeft, TAG, MPI_COMM_WORLD, &stat);
            }

            if (procId%stepParent == 0 && procId-stepLeft > 0)
            { // parent receives from left and sends its number back
                DTYPE temp = UNDEF;
	            MPI_Recv(&temp, 1, MPI_INT, procId-stepLeft, TAG, MPI_COMM_WORLD, &stat);
	            MPI_Send(&D, 1, MPI_INT, procId-stepLeft, TAG, MPI_COMM_WORLD);
                D = opD(temp,D);
            }
        }

        if (procId > 1)
        {
	        MPI_Send(&D, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD);
        }
        if (procId < numProcs-1)
        {
	        MPI_Recv(&D, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD, &stat);
        }
        else if (procId == numProcs-1)
        {
            D = reduce;
        }

        int C = UNDEFINED;

        if (procId < numProcs-1)
        {
            MPI_Send(&D, 1, MPI_INT, procId+1, TAG, MPI_COMM_WORLD);
        }
        if (procId > 1)
        {
            DTYPE temp = UNDEF;
            MPI_Recv(&temp, 1, MPI_INT, procId-1, TAG, MPI_COMM_WORLD, &stat);
            if (temp == G)
            {
                C = 1;
            }
            else if (temp == S)
            {
                C = 0;
            }
        }
        else if (procId == 1)
        {
            C = 0;
        }

        int Z = nand(C,nand(X,Y));

        if (procId == realSize && D == P)
        { // check overflow on original maximal size bit
            std::cout  <<  "overflow"  <<  std::endl;
        }

        if (procId <= realSize && procId > 0)
        {
            std::cout  <<  procId  <<  ":"  <<  Z  <<   std::endl;
        }
    }

    
    
    MPI_Finalize(); 
	return 0;
}
