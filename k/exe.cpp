#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>


int main(int argc, char **argv)
{
    double FinalRes = 0;
    double _add_p = 0;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int R = 0;

    if(id == 0){
        R = 1;
    }
    else{
        if      (id == 1) R = 345;
        else if (id == 2) R = 24677;
        else if (id == 3) R = 4568485;
        MPI_Bcast(&R,1,MPI_INT, 2, MPI_COMM_WORLD);
    }    
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "pr id: " << id << " R: " << R << "\n";

    MPI_Finalize();

    return 0;
}