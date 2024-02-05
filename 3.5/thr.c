#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Passed
{
    double **matr;
    int x_off;
    int y_off;
    int size;
};

double **initSquareMatr(int N)
{
    double *pool = (double *)(malloc(sizeof(double) * N * N));
    double **matrix = (double **)(malloc(sizeof(double *) * N));
    for (size_t i = 0; i < N; i++)
    {
        matrix[i] = &pool[i * N];
    }
    return matrix;
}

void fillSqMatr(double **a, int R)
{
    for (size_t i = 0; i < R; i++)
    {
        for (size_t j = 0; j < R; j++)
        {
            a[i][j] = 3 * i + 10 * j;
        }
    }
}

void showSqMatr(double **a, int R)
{
    for (size_t i = 0; i < R; i++)
    {
        for (size_t j = 0; j < R; j++)
        {
            printf("%4.1f ", a[i][j]);
        }
        printf("\n");
    }
}

void decompose(double* **a, int* x_off, int* y_off, int* l, struct Passed *_str)
{
    *a = _str->matr;
    *l = _str->size;
    *x_off = _str->x_off;
    *y_off = _str->y_off;
}

void* matrix_subroutine(void *passed_args)
{
    // l is R/2 for 4 threads
    printf("thread id: %lu\n", (unsigned long)pthread_self());
    struct Passed *_parr = (struct Passed *)passed_args;

    int x_off, y_off, l;
    double **a;
    decompose(&a, &x_off, &y_off, &l, _parr);

    for (size_t i = 0; i < l; i++)
    {
        for (size_t j = 0; j < l; j++)
        {
            // powf(2.71828, log2(i * j + 1))
            a[i + x_off][j + y_off] = 5;
        }
    }
}

struct Passed *compose(double **a, int x_off, int y_off, int l)
{
    struct Passed *_str = (struct Passed *)(malloc(sizeof(struct Passed)));
    _str->matr = a;
    _str->size = l;
    _str->x_off = x_off;
    _str->y_off = y_off;
    return _str;
}
//pass by reference isnt working in C - only in CPP
struct Passed* schedule(double** _m, int N, int i){
        switch (i)
        {
        case 0:
            return compose(_m, 0, 0, N / 2);
            break;
        case 1:
            return compose(_m, N / 2, 0, N / 2);
            break;
        case 2:
            return compose(_m, 0, N / 2, N / 2);
            break;
        case 3:
            return compose(_m, N / 2, N / 2, N / 2);
            break;
        default:
            break;
        }
}

int main()
{

    int N = 32;
    int n_threads = 4;

    double **_m = initSquareMatr(N);
    fillSqMatr(_m, N);
    showSqMatr(_m, N);

    pthread_t *pthr_arr = (pthread_t *)(malloc(sizeof(pthread_t) * n_threads));
    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_t _t;
        pthr_arr[i] = _t;
    }

    for (size_t i = 0; i < n_threads; i++)
    {
        struct Passed *_pargs = schedule(_m, N, i);
        pthread_create(&pthr_arr[i], NULL, matrix_subroutine, (void *)_pargs);
    }

    for(size_t i=0;i < n_threads;i++){
        pthread_join(pthr_arr[i], NULL);
    }
    showSqMatr(_m, N);

    return 0;
}