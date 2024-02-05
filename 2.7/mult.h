#include <stdio.h>
#include <iostream>

int **initMatr(int R)
{
    // int *pool = new int[R * R];
    // int **matrix = new int *[R];

    // for (size_t i = 0; i < R; i++)
    // {
    //     matrix[i] = &pool[i * R];
    // }
    // return matrix;

    int** matrix = new int*[R];
    for(int i=0;i<R;i++){
        matrix[i] = new int[R];
    }

}

int **SquareMatrMult(int **a, int **b, int _a_x_off, int _a_y_off, int _b_x_off, int _b_y_off, int l)
{
    // l = R/2 in all subtasks
    int** matrix = initMatr(l);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            int sum = 0;
            for (int iter = 0; iter < l; iter++)
            {
                sum += a[i + _a_x_off][iter + _a_y_off] * b[iter + _b_x_off][j + _b_y_off];
            }
            matrix[i][j] = sum;
        }
    }

    return matrix;
}

bool isSame(int **a, int **b, int l){
    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            if(a[i][j] != b[i][j]) return false; 
        }
    }
    return true;
}

int** createTargetMerged(int** subM, int x_offset, int y_offest, int l){
    
    //matrix with R = l * 2
    int** matrix = initMatr(l*2);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            matrix[i+x_offset][j+y_offest] = subM[i][j];
        }
    }
    return matrix;
}

void rewriteTargetMerged(int** a, int** subM, int x_offset, int y_offest, int l){
    // l should be R/2
    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            a[i+x_offset][j+y_offest] = subM[i][j];
        }
    }
}

void SquareMatrAddTo1(int **a, int **b, int l)
{
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < l; j++)
        {
            a[i][j] = a[i][j] + b[i][j];
        }
    }
}

void showMatr(const char *mess, int **a, int l)
{
    std::cout << mess << "\n";
    for (int i = 0; i < l; i++)
    {

        for (int j = 0; j < l; j++)
        {
            std::cout << a[i][j] << " ";
            std::cout << "  addr: " << &a[i][j] << " -- ";
        }
        std::cout << "\n";
    }
}