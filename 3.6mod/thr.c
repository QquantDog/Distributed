#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

struct Passed
{
    int *arr;
    int arr_length;

    int t_id;
    int nthreads;

    // arr
    int *resIntAnsw;
};

void decompose(int **arr, int *arr_length, int *t_id, int *nthreads, int **resIntAnsw, struct Passed *_str)
{
    *arr = _str->arr;
    *arr_length = _str->arr_length;
    *t_id = _str->t_id;
    *nthreads = _str->nthreads;
    *resIntAnsw = _str->resIntAnsw;
}

struct Passed *compose(int *arr, int arr_length, int t_id, int nthreads, int *resIntAnsw)
{
    struct Passed *_str = (struct Passed *)(malloc(sizeof(struct Passed)));
    _str->arr = arr;
    _str->arr_length = arr_length;
    _str->t_id = t_id;
    _str->nthreads = nthreads;
    _str->resIntAnsw = resIntAnsw;

    return _str;
}

int *initArr(int N)
{
    int *arr = (int *)malloc(sizeof(int) * N);
    for (size_t i = 0; i < N; i++)
    {
        // fabs(rand() % (2 << 16) * rand() % (2 << 16))
        arr[i] = rand() % (2 << 16);
    }
    return arr;
}

void showArr(int *arr, int N)
{
    for (size_t i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
}
// эвристики пятнашек
// 1  2
// 3 

//  1 2
//    3

//  

// рекурсия - контролировать глубину рекурсии

int recursiveArrMin(int *arr, int s_ind, int f_ind)
{
    if (s_ind > f_ind)
        printf("start is larger than final");

    if (s_ind == f_ind)
    {
        return arr[s_ind];
    }

    int l_wing = recursiveArrMin(arr, s_ind, (s_ind + f_ind) / 2);
    int r_wing = recursiveArrMin(arr, (s_ind + f_ind) / 2 + 1, f_ind);

    return l_wing - r_wing < 0 ? l_wing : r_wing;
}

void *resArrMerge(void *passed_args)
{

    // int *arr, int s_ind, int f_ind, int t_id, struct RecursiveResponse* resArr

    struct Passed *_parr = (struct Passed *)passed_args;

    int arr_length, nthreads, t_id;
    int *arr;
    int *resArr;

    decompose(&arr, &arr_length, &t_id, &nthreads, &resArr, _parr);
    // printf("id: %lu -- arrL: %d -- T_ID: %d\n", (unsigned long)pthread_self(), arr_length, t_id);
    sleep(1);
    int delta;
    int complemented_mod;
    if (arr_length % nthreads == 0)
    {
        complemented_mod = 0;
        delta = arr_length / nthreads;
    }
    else
    {
        delta = arr_length / nthreads + 1;
        complemented_mod = nthreads * delta - arr_length;
    }
    int forward_mod = nthreads - complemented_mod;
    if (t_id < forward_mod)
    {
        resArr[t_id] = recursiveArrMin(arr, delta * (t_id), delta * (t_id + 1) - 1);
    }
    else
    {
        resArr[t_id] = recursiveArrMin(arr, forward_mod * delta + (delta - 1) * (t_id - forward_mod), forward_mod * delta + (delta - 1) * (t_id - forward_mod + 1) - 1);
    }
}

void seqArrMin(int *arr, int s_ind, int f_ind)
{
    if (s_ind > f_ind)
        printf("start is larger than final");

    int _r = arr[s_ind];
    int _r_ind = s_ind;
    for (int i = s_ind; i <= f_ind; i++)
    {
        if (_r > arr[i])
        {
            _r = arr[i];
            _r_ind = i;
        }
    }
    printf("SEQ min_el: %d index: %d\n", _r, _r_ind);
    // return _r;
}

void showMin(int *ResponseArray, int N)
{
    int _r = ResponseArray[0];
    for (size_t i = 1; i < N; i++)
    {
        if (_r > ResponseArray[i])
        {
            _r = ResponseArray[i];
        }
    }
    printf("After Merge Parallel recursive minimal elem: %d", _r);
}

int main()
{
    // 10.5 secs vs 0.58 secs for recursive returning struct
    srand(time(NULL));

    int N = 6200;
    int n_threads = 6;

    int *arr = initArr(N);
    // showArr(arr, N);

    clock_t seq_s, seq_f;
    seq_s = clock();
    seqArrMin(arr, 0, N - 1);
    seq_f = clock();
    printf("time for seq: %f\n", ((double)(seq_f - seq_s)) / CLOCKS_PER_SEC);
    printf("Recursive minimal: %d\n", recursiveArrMin(arr, 0, N - 1));

    int *ResponseArray = (int *)malloc(sizeof(int) * n_threads);

    // recursive solo
    clock_t rec_s, rec_f;
    rec_s = clock();
    printf("recursive solo: %d\n", recursiveArrMin(arr, 0, N - 1));
    rec_f = clock();
    printf("time for rec: %f\n", ((double)(rec_f - rec_s)) / CLOCKS_PER_SEC);

    // lets suppose N > n_threads
    // for(int i=0;i<n_threads;i++){

    // }

    pthread_t *pthr_arr = (pthread_t *)(malloc(sizeof(pthread_t) * n_threads));

    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_t _t;
        pthr_arr[i] = _t;
    }

    clock_t s_par, f_par;
    for (size_t i = 0; i < n_threads; i++)
    {
        struct Passed *_pargs = compose(arr, N, i, n_threads, ResponseArray);
        s_par = clock();
        pthread_create(&pthr_arr[i], NULL, resArrMerge, (void *)_pargs);
    }

    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_join(pthr_arr[i], NULL);
    }
    f_par = clock();
    showMin(ResponseArray, n_threads);
    printf("Parallel clock with sync and printf_unfort: %f\n", ((double)(f_par - s_par)) / CLOCKS_PER_SEC);
    // showSqMatr(_m, N);

    return 0;
}