#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>

#define BATCH_SIZE 100
// 30000 ~ -5
//
// int getBatchSize(int pow)
// {
//     if (pow > -1)
//     {
//         return
//     }
// }

double getPartialRes(int id, int batch, int iteration_shift)
{
    double _r = 0;
    for (int i = iteration_shift + id * batch + 1; i < iteration_shift + (id + 1) * batch + 1; i++)
    {
        _r += 1 / (1 + (i / (double)) * (i / (double)overall));
    }
    printf("id - %d res - %f\n", id, _r);
    return _r;
}

// double getSoloValRes(int val)
// {
//     return getPartialRes(0, val, val);
// }

int main(int argc, char **argv)
{
    double FinalRes = 0;
    double _add_p = 0;
    double accuracy = 0.00001;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int overall = BATCH_SIZE * n_all;

    if (!id)
        printf("overall = %d\n", overall);

    double error = 1;
    int iter_count = 0;
    double sum = 0;
    // error > accuracy
    while (iter_count < 5)
    {

        _add_p = getPartialRes(id, overall, BATCH_SIZE, overall*iter_count);
        iter_count++;
        
        MPI_Barrier(MPI_COMM_WORLD);
        if(id == 0) std::cout << "iteration count: " << iter_count << "\n";
        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Reduce(&_add_p, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        if (id == 0)
        {
            FinalRes = sum * (4 / (double)overall*(iter_count+1));
            // std::cout << std::setprecision(15);
            // std::cout << "\n ----\nRes: " << FinalRes << " PI: " << M_PI << "\n";
            // std::cout << std::setprecision(4);
            // std::cout << "Error: " << M_PI - FinalRes << " %: " << fabs(M_PI - FinalRes) / M_PI;
            std::cout << "\n\nFinal res: " <<FinalRes << "\n";
            error = fabs(M_PI - FinalRes) / M_PI;
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    // _add_p = getPartialRes(id, overall, BATCH_SIZE);
    // MPI_Reduce(&_add_p, &FinalRes, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // if (id == 0)
    // {
    //     FinalRes = FinalRes * (4 / (double)overall);
    //     std::cout << std::setprecision(15);
    //     std::cout << "\n ----\nRes: " << FinalRes << " PI: "<< M_PI <<"\n";
    //     std::cout << std::setprecision(4);
    //     std::cout << "Error: " << M_PI - FinalRes << " %: " << 100*fabs(M_PI - FinalRes) / M_PI;
    // }

    return 0;
}