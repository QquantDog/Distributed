#include <mpi.h>
#include <math.h>
// #include <iomanip>
#include "mult.h"
#include <fstream>
#include <time.h>

//  (int **, CB_class *, int (CB_class::*)(int x, int y))

double g_tstart, g_tfinal;

class CB_class
{
    private:
    int a;
    int b;

    public:
    CB_class(int a, int b)
    {
        this->a = a;
        this->b = b;
    }
    int gen(int x, int y)
    {
        return a * x + b * y;
    }
    int genRand()
    {
        return 1 + rand() % 256;
    }
};

int **initMatrCB(int R, CB_class *cBstr_ptr, int (CB_class::*init_callback)(int, int))
{
    int **matrix = initMatr(R);

    for (size_t i = 0; i < R; i++)
    {
        for (size_t j = 0; j < R; j++)
        {
            // matrix[i][j] = init_callback(i, j);
            matrix[i][j] = (cBstr_ptr->*init_callback)(i, j);
        }
    }

    return matrix;
}

int **initMatrCB(int R, CB_class *cBstr_ptr, int (CB_class::*init_callback)(void))
{
    int **matrix = initMatr(R);

    for (size_t i = 0; i < R; i++)
    {
        for (size_t j = 0; j < R; j++)
        {
            // matrix[i][j] = init_callback(i, j);
            matrix[i][j] = (cBstr_ptr->*init_callback)();
        }
    }

    return matrix;
}

int main(int argc, char **argv)
{

    int **A;
    int **B;

    int **subM;
    int **passedsubM;

    OperCounter _oc;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    //

    int R;
    // for(int GC = 5;GC<351;GC++){

    if (id == 0)
    {
        int input;
        std::cin >> input;
        R = 2 * input;
        // R = GC * 2;

    }
    MPI_Bcast(&R, 1, MPI_INT, 0, MPI_COMM_WORLD);

    A = initMatr(R);
    B = initMatr(R);

    subM = initMatr(R / 2);
    passedsubM = initMatr(R / 2);

    if (id == 0)
    {
        srand(time(NULL));
        CB_class* irjr = new CB_class(0,0);
        CB_class* i3j10 = new CB_class(1,2);
        CB_class* i5j2 = new CB_class(2,1);

        // A = initMatrCB(R, i3j10, &CB_class::gen);
        // B = initMatrCB(R, i5j2,  &CB_class::gen);
        A = initMatrCB(R, irjr, &CB_class::genRand);
        B = initMatrCB(R, irjr,  &CB_class::genRand);

    }
    // Bcast SHOULD BE NEVER IN CONDITIONAL
    g_tstart = MPI_Wtime();
    MPI_Bcast(&A[0][0], R * R, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], R * R, MPI_INT, 0, MPI_COMM_WORLD);

    switch (id)
    {
    // C11 C12
    // C21 C22
    case 0:
        // A11 B11
        subM = SquareMatrMult(A, B, 0, 0, 0, 0, R / 2, &_oc);

        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        break;
    case 1:

        subM = SquareMatrMult(A, B, 0, R / 2, R / 2, 0, R / 2, &_oc);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2, &_oc);

        break;
    case 2:
        subM = SquareMatrMult(A, B, 0, 0, 0, R / 2, R / 2, &_oc);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 3, 0, MPI_COMM_WORLD);
        break;
    case 3:
        subM = SquareMatrMult(A, B, 0, R / 2, R / 2, R / 2, R / 2, &_oc);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2, &_oc);
        // showMatr("C12 final: ", subM, R / 2);
        break;
    case 4:
        subM = SquareMatrMult(A, B, R / 2, 0, 0, 0, R / 2, &_oc);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 5, 0, MPI_COMM_WORLD);
        break;
    case 5:
        subM = SquareMatrMult(A, B, R / 2, R / 2, R / 2, 0, R / 2, &_oc);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 4, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2, &_oc);
        // showMatr("C21 final: ", subM, R / 2);
        break;
    case 6:
        subM = SquareMatrMult(A, B, R / 2, 0, 0, R / 2, R / 2, &_oc);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 7, 0, MPI_COMM_WORLD);
        break;
    case 7:
        subM = SquareMatrMult(A, B, R / 2, R / 2, R / 2, R / 2, R / 2, &_oc);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 6, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2, &_oc);
        // showMatr("C22 final: ", subM, R / 2);
        break;
    default:
        break;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    //4 matrices - 4 requests
    //unfortunately should be created for all
    // MPI_Request* req_arr = new MPI_Request[4];
    // MPI_Status* stat_arr = new MPI_Status[4];
    
    switch (id)
    {
    case 0:
    {
        // heheheheheheheheheh
        //  int*** arr_submatrices
        int **mC11 = initMatr(R / 2);
        int **mC12 = initMatr(R / 2);
        int **mC21 = initMatr(R / 2);
        int **mC22 = initMatr(R / 2);

        //11 - 0
        //12 - 1
        //21 - 2
        //22 - 3

        // MPI_Irecv(&mC11[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req_arr[0]);
        // MPI_Irecv(&mC12[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req_arr[1]);
        // MPI_Irecv(&mC21[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req_arr[2]);
        // MPI_Irecv(&mC22[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req_arr[3]);

        MPI_Recv(&mC11[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC12[0][0], R / 2 * R / 2, MPI_INT, 3, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC21[0][0], R / 2 * R / 2, MPI_INT, 5, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC22[0][0], R / 2 * R / 2, MPI_INT, 7, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int **final = initMatr(R);

        int ff_ind = -1;
        
        // for(int i=0;i<4;i++){
        //     MPI_Waitany(4, req_arr, &ff_ind, MPI_STATUS_IGNORE);
        //     std::cout << "ff_ind" << ff_ind;
        //     switch (ff_ind)
        //     //0 - 1
        //     //1 - 3
        //     //2 - 5
        //     //3 - 7
        //     {
        //     case 0:
        //         rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        //         break;
        //     case 1:
        //         rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        //         break;
        //     case 2:
        //         rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        //         break;
        //     case 3:
        //         rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        //         break;
            
        //     default:
        //         break;
        //     }
        // }

        rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        rewriteTargetMerged(final, mC12, 0, R / 2, R / 2);
        rewriteTargetMerged(final, mC21, R / 2, 0, R / 2);
        rewriteTargetMerged(final, mC22, R / 2, R / 2, R / 2);
        g_tfinal = MPI_Wtime();

        double d_parallel = g_tfinal - g_tstart;
        std::cout << "parallel time is " << d_parallel <<"s\n";
        // 
        // showMatr("final parallel matrix merged: ", final, R);
        // without adding operations

        double single_tstart, single_tfinal;
        
        single_tstart = MPI_Wtime();
        OperCounter _oc_single;
        int **seqM = SquareMatrMult(A, B, 0, 0, 0, 0, R, &_oc_single);
        single_tfinal = MPI_Wtime();

        double d_seq = single_tfinal - single_tstart;
        

        std::cout << "single time is " << d_seq <<"s\n";

        // showMatr("sequence matrix: ", seqM, R);

        // merged[0][0]++;
        std::cout << "Are matrices same? " << isSame(final, seqM, R) << "\n";

        // std::cout << "parallel\n";
        // _oc.show();
        std::cout << "single\n";
        _oc_single.show();

        // std::ofstream myfile;
        // std::ios_base::app - for append, default = rewrite
        // myfile.open ("stats.txt", std::ios_base::app);
 
        // myfile << R << ": " << "seq: " << d_seq << "parallel: " << d_parallel << "\n"; 
        // myfile.close();

        break;
    }
        //11 - 0
        //12 - 1
        //21 - 2
        //22 - 3
    case 1:
        //C11 - 0
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // MPI_Isend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_arr[0]);
        break;

    case 3:
        //C12
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // MPI_Isend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_arr[1]);
        break;

    case 5:
        //C21
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // MPI_Isend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_arr[2]);
        break;

    case 7:
        //C22
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // MPI_Isend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_arr[3]);
        break;

    default:
        break;
    }

    if(id == 0){
        //7 passed
        //hehe no additional datatypes would be presented
        unsigned long long* add = new unsigned long long[7];
        unsigned long long* mult = new unsigned long long[7];
        unsigned long long* access = new unsigned long long[7];
        unsigned long long* overall_calc = new unsigned long long[7];

        for(int i=0;i<7;i++){
            MPI_Recv(&add[i], 1, MPI_LONG, i+1, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&mult[i], 1, MPI_LONG, i+1, 200, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&access[i], 1, MPI_LONG, i+1, 300, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&overall_calc[i], 1, MPI_LONG, i+1, 400, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // MPI_Recv(&add[i], 1, MPI_LONG, i+1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for(int i=0;i<7;i++){
            _oc.add += add[i];
            _oc.mult += mult[i];
            _oc.access += access[i];
            _oc.overall_calc += overall_calc[i];
        }
        std::cout << "parallel count:\n";
        _oc.show();

    }
    else{
        MPI_Ssend(&(_oc.add),1,MPI_LONG,0,100,MPI_COMM_WORLD);
        MPI_Ssend(&(_oc.mult),1,MPI_LONG,0,200,MPI_COMM_WORLD);
        MPI_Ssend(&(_oc.access),1,MPI_LONG,0,300,MPI_COMM_WORLD);
        MPI_Ssend(&(_oc.overall_calc),1,MPI_LONG,0,400,MPI_COMM_WORLD);
    }


    // }
    MPI_Finalize();

    return 0;
}