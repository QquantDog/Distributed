#include <mpi.h>
#include <math.h>
#include <iomanip>
#include "mult.h"

#define N 8

int** initMatr(int R){
    int** _m = new int*[R];
    for(int i=0;i<R;i++){
        _m[i] = new int[R];
        for(int j=0;j<R;j++){
            _m[i][j] = 0;
        }
    }
    return _m;
}

int** genMatr(int R){
    int** _m = new int*[R];
    for(size_t i=0;i<R;i++){
        _m[i] = new int[R];
        for(int j=0;j<R;j++){
            _m[i][j] = rand()%1024;
        }
    }
    return _m;
}

int** genMatrDefaultA(int R){
    int** _m = new int*[R];
    for(size_t i=0;i<R;i++){
        _m[i] = new int[R];
        for(int j=0;j<R;j++){
            _m[i][j] = 10*i+j;
        }
    }
    return _m;
}

int** genMatrDefaultB(int R){
    int** _m = new int*[R];
    for(size_t i=0;i<R;i++){
        _m[i] = new int[R];
        for(int j=0;j<R;j++){
            _m[i][j] = i+3*j;
        }
    }
    return _m;
}

int main(int argc, char **argv)
{

    int** A;
    int** B;

    int** subM;
    int** passedsubM;

    int row_ind;
    int col_ind;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    // 

    int R;
    if(id == 0){
        int input;
        std::cin >> input;
        R = 2 * input;
    }
    MPI_Bcast(&R, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // MPI_Barrier(MPI_COMM_WORLD);

    std::cout << "id - " << id << " R: " << R << "\n";

    A = initMatr(R);
    B = initMatr(R);

    subM = initMatr(R/2);
    passedsubM = initMatr(R/2);

    if(id == 0){
        // A = genMatr(R);
        // B = genMatr(R);

        A = genMatrDefaultA(R);
        B = genMatrDefaultB(R);

        showMatr("A: ", A, R);
        showMatr("B: ", B, R);
    
    }
    //Bcast SHOULD BE NEVER IN CONDITIONAL
    MPI_Bcast(&A[0][0], R*R, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], R*R, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    // else{

    //     MPI_Recv(&A[0][0], R*R, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     MPI_Recv(&B[0][0], R*R, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // }
    switch (id)
    {
        // C11 C12
        // C21 C22
        case 0:
            //A11 B11
            subM = SquareMatrMult(A, B, 0, 0, 0, 0, R/2);
            showMatr("subm output", subM, R/2);
            // MPI_Ssend(&subM[0][0], R/2*R/2, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Ssend(&subM[0], R/2, MPI_INT, 1, 0, MPI_COMM_WORLD);
            break;
        case 1:
            MPI_Barrier(MPI_COMM_WORLD);
            showMatr("B matrix in 1 id: ", B, R);
            //case0 + A12 + B21
            MPI_Recv(&passedsubM[0], R/2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            showMatr("passed output", passedsubM, R/2);
            // subM = SquareMatrAdd(SquareMatrMult(A, B, 0, R/2, R/2, 0, R/2), passedsubM, R/2);
            // std::cout <<  "\nsddfagf" <<**passedsubM << "  " << *(*(passedsubM+1)+1);
            //should output send to overall gatherer
            // res
            break;
        
        default:
            break;
    }

    // 
    MPI_Finalize();

    return 0;
}