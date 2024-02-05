#include <mpi.h>
#include <math.h>
// #include <iomanip>
#include "mult.h"

//  (int **, CB_class *, int (CB_class::*)(int x, int y))

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

    void setAB(int a, int b)
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

int main(int argc, char **argv)
{

    int **A;
    int **B;

    int **subM;
    int **passedsubM;

    MPI_Init(&argc, &argv);

    int n_all;
    MPI_Comm_size(MPI_COMM_WORLD, &n_all);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    //

    int R;
    if (id == 0)
    {
        int input;
        std::cin >> input;
        R = 2 * input;
    }
    MPI_Bcast(&R, 1, MPI_INT, 0, MPI_COMM_WORLD);

    A = initMatr(R);
    B = initMatr(R);

    subM = initMatr(R / 2);
    passedsubM = initMatr(R / 2);

    if (id == 0)
    {


        CB_class* i3j10 = new CB_class(1,2);
        CB_class* i5j2 = new CB_class(2,1);
        // CB_class* i3j10 = new CB_class(3,10);
        // CB_class* i5j2 = new CB_class(5,2);
        // i3j10.setAB(3, 10);

        A = initMatrCB(R, i3j10, &CB_class::gen);
        B = initMatrCB(R, i5j2,  &CB_class::gen);

        // B = genMatrDefaultB(R);

        showMatr("A: ", A, R);
        showMatr("B: ", B, R);
    }
    // Bcast SHOULD BE NEVER IN CONDITIONAL
    MPI_Bcast(&A[0][0], R * R, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], R * R, MPI_INT, 0, MPI_COMM_WORLD);

    switch (id)
    {
    // C11 C12
    // C21 C22
    case 0:
        // A11 B11
        subM = SquareMatrMult(A, B, 0, 0, 0, 0, R / 2);
        // showMatr("subm output", subM, R / 2);
        // MPI_Ssend(&subM[0][0], R/2*R/2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // MPI_Barrier(MPI_COMM_WORLD);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        break;
    case 1:
        // MPI_Barrier(MPI_COMM_WORLD);
        // showMatr("B matrix in 1 id: ", B, R);
        // case0 + A12 + B21
        // y_offest ->
        // x_offset down
        subM = SquareMatrMult(A, B, 0, R / 2, R / 2, 0, R / 2);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2);
        // int** C11 = subM;
        // showMatr("C11 final: ", subM, R / 2);
        // showMatr("FROM id1 A: ", A, R);
        // showMatr("passed output", passedsubM, R / 2);
        // subM = SquareMatrAdd(SquareMatrMult(A, B, 0, R/2, R/2, 0, R/2), passedsubM, R/2);
        // std::cout <<  "\nsddfagf" <<**passedsubM << "  " << *(*(passedsubM+1)+1);
        // should output send to overall gatherer
        // res
        break;
    case 2:
        subM = SquareMatrMult(A, B, 0, 0, 0, R / 2, R / 2);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 3, 0, MPI_COMM_WORLD);
        break;
    case 3:
        subM = SquareMatrMult(A, B, 0, R / 2, R / 2, R / 2, R / 2);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2);
        // showMatr("C12 final: ", subM, R / 2);
        break;
    case 4:
        subM = SquareMatrMult(A, B, R / 2, 0, 0, 0, R / 2);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 5, 0, MPI_COMM_WORLD);
        break;
    case 5:
        subM = SquareMatrMult(A, B, R / 2, R / 2, R / 2, 0, R / 2);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 4, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2);
        // showMatr("C21 final: ", subM, R / 2);
        break;
    case 6:
        subM = SquareMatrMult(A, B, R / 2, 0, 0, R / 2, R / 2);
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 7, 0, MPI_COMM_WORLD);
        break;
    case 7:
        subM = SquareMatrMult(A, B, R / 2, R / 2, R / 2, R / 2, R / 2);
        MPI_Recv(&passedsubM[0][0], R / 2 * R / 2, MPI_INT, 6, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SquareMatrAddTo1(subM, passedsubM, R / 2);
        // showMatr("C22 final: ", subM, R / 2);
        break;
    default:
        break;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    switch (id)
    {
    case 0:
    {
        // heheheheheheheheheh
        //  int*** arr_submatricies
        int **mC11 = initMatr(R / 2);
        int **mC12 = initMatr(R / 2);
        int **mC21 = initMatr(R / 2);
        int **mC22 = initMatr(R / 2);

        // could be irecv
        // MPI_Irecv();

        MPI_Recv(&mC11[0][0], R / 2 * R / 2, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC12[0][0], R / 2 * R / 2, MPI_INT, 3, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC21[0][0], R / 2 * R / 2, MPI_INT, 5, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mC22[0][0], R / 2 * R / 2, MPI_INT, 7, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int **final = initMatr(R);

        rewriteTargetMerged(final, mC11, 0, 0, R / 2);
        rewriteTargetMerged(final, mC12, 0, R / 2, R / 2);
        rewriteTargetMerged(final, mC21, R / 2, 0, R / 2);
        rewriteTargetMerged(final, mC22, R / 2, R / 2, R / 2);

        showMatr("final parallel matrix merged: ", final, R);
        int **seqM = SquareMatrMult(A, B, 0, 0, 0, 0, R);
        showMatr("sequence matrix: ", seqM, R);
        // merged[0][0]++;
        std::cout << "Are matricies same? " << isSame(final, seqM, R) << "\n";

        break;
    }
    case 1:
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        break;

    case 3:
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        break;

    case 5:
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        break;

    case 7:
        MPI_Ssend(&subM[0][0], R / 2 * R / 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // MPI_Isend();
        break;

    default:
        break;
    }

    MPI_Finalize();

    return 0;
}