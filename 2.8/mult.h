#include <stdio.h>
#include <iostream>

struct OperCounter
{
    // without some usual as ptr jump to fully another array
    unsigned long long add = 0;
    unsigned long long mult = 0;
    unsigned long long access = 0;
    // unsigned long long shift = 0;
    unsigned long long overall_calc = 0;

    void show()
    {
        std::cout << "Add: " << this->add << " Mult: " << this->mult << " Access: " << this->access << " Calc: " << this->overall_calc << "\n";
    }
};

// c22= a21b12 + a22b22 + a23b32
// l - matrix rank
// overall ops: l^2 * ( [l] + [l-1] )
//                     mult    add
//              2*l^3 - l^2

int **initMatr(int R)
{
    int *pool = new int[R * R];
    int **matrix = new int *[R];

    for (size_t i = 0; i < R; i++)
    {
        matrix[i] = &pool[i * R];
    }
    return matrix;
}

int **SquareMatrMult(int **a, int **b, int _a_x_off, int _a_y_off, int _b_x_off, int _b_y_off, int l, OperCounter *_counter)
{
    // l = R/2 in all subtasks
    int **matrix = initMatr(l);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            int sum = 0;
            for (int iter = 0; iter < l; iter++)
            {
                sum += a[i + _a_x_off][iter + _a_y_off] * b[iter + _b_x_off][j + _b_y_off];
                //
                // _counter->add += 1;
                // _counter->mult += 1;
                // _counter->access += 2;
                // _counter->overall_calc += 2;
            }
            _counter->add += l - 1;
            _counter->mult += l;
            _counter->access += 2 * l;
            _counter->overall_calc += 2 * l - 1;

            matrix[i][j] = sum;
        }
    }
    return matrix;
}

int **SquareMatrMultejh(int **a, int **b, int _a_x_off, int _a_y_off, int _b_x_off, int _b_y_off, int l)
{
    // l = R/2 in all subtasks
    int **matrix = initMatr(l);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            int sum = 0;
            for (int iter = 0; iter < l; iter++)
            {
                sum += a[i + _a_x_off][iter + _a_y_off] * b[iter + _b_x_off][j + _b_y_off];
                //
            }
            matrix[i][j] = sum;
        }
    }
    return matrix;
}

bool isSame(int **a, int **b, int l)
{
    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            if (a[i][j] != b[i][j])
                return false;
        }
    }
    return true;
}

int **createTargetMerged(int **subM, int x_offset, int y_offest, int l)
{

    // matrix with R = l * 2
    int **matrix = initMatr(l * 2);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            matrix[i + x_offset][j + y_offest] = subM[i][j];
        }
    }
    return matrix;
}

void rewriteTargetMerged(int **a, int **subM, int x_offset, int y_offest, int l)
{
    // l should be R/2
    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            a[i + x_offset][j + y_offest] = subM[i][j];
        }
    }
}

//  2l^3 - l^2      - parallel
//  2l^3 - 2l^2 + l^2 - seq
void SquareMatrAddTo1(int **a, int **b, int l, OperCounter *_counter)
{
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < l; j++)
        {
            a[i][j] = a[i][j] + b[i][j];
            // _counter->add += 1;
            // _counter->overall_calc += 1;
            // _counter->access += 2;
        }
        _counter->add += l ;
        _counter->overall_calc += l;
        _counter->access += 2 * l;
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
            // std::cout << "  addr: " << &a[i][j] << " -- ";
        }
        std::cout << "\n";
    }
}