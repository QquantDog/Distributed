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

double getSoloValRes(int val){
    return getPartialRes(0,val,val);
}


int main(int argc, char **argv)
{
    double FinalRes = 0;
    double _add_p = 0;

    double CAPPED_CONST;
    double FIRST_100_err;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int overall = BATCH_SIZE * n_all;
    
    
    if (!id)
        printf("overall = %d\n", overall);

    if(id == 0){
        //err_prec in procents
        double err_prec_pow;
        std::cin >> err_prec_pow;
        double err_prec = pow(10, err_prec_pow);
        std::cout << "err_prec: " << err_prec << "\n";

        double _v1000 = getSoloValRes(1000);
        double _v2000 = getSoloValRes(2718);
        //1 - ln from 271/100 = 2.71 as exp = 2.71828...
        CAPPED_CONST = fabs(_v2000 - _v1000) * 1;

        double err_100 = fabs(M_PI - _v1000);
        double end_err = M_PI * (1 - err_prec);

        double NX = 1000 * pow(M_E, fabs(err_100 - end_err)/CAPPED_CONST);
        std::cout << "NX = " << NX << "\n"; 
        // int N_checkErr = 170;
        
        FinalRes = getSoloValRes((int)NX+1) * (4 / (NX+1));
        std::cout << "Error: " << M_PI - FinalRes << " %: " << 100*fabs(M_PI - FinalRes) / M_PI; 

        //now calculate summands for 1 process
        
    }

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

    MPI_Finalize();

    return 0;
}