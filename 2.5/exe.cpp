#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>

#define BATCH_SIZE 10000

double getPartialRes(int id, int overall, int batch)
{
    double _r = 0;
    for (int i = id * batch + 1; i < (id + 1) * batch + 1; i++)
    {
        _r += 1 / (1 + (i / (double)overall) * (i / (double)overall));
    }
    printf("id - %d res - %f\n", id, _r);
    return _r;
}

int main(int argc, char **argv)
{
    double gstart = MPI_Wtime();
    double FinalRes = 0;
    double _add_p = 0;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int overall = BATCH_SIZE * n_all;
    if (!id)
        printf("overall = %d\n", overall);

    _add_p = getPartialRes(id, overall, BATCH_SIZE);
    MPI_Reduce(&_add_p, &FinalRes, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (id == 0)
    {
        FinalRes = FinalRes * (4 / (double)overall);
        std::cout << std::setprecision(15);
        std::cout << "\n ----\nRes: " << FinalRes << " PI: "<< M_PI <<"\n";
        std::cout << std::setprecision(4);
        std::cout << "Error: " << M_PI - FinalRes << " %: " << fabs(M_PI - FinalRes) / M_PI; 
    }

    MPI_Finalize();

    return 0;
}