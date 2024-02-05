#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct Passed
{
    int *arr;
    int arr_length;

    int t_id;
    int nthreads;

    struct RecursiveResponse *resArr;
};

void decompose(int **arr, int *arr_length, int *t_id, int *nthreads, struct RecursiveResponse **resArr, struct Passed *_str)
{
    *arr = _str->arr;
    *arr_length = _str->arr_length;
    *t_id = _str->t_id;
    *nthreads = _str->nthreads;
    *resArr = _str->resArr;
}

struct Passed *compose(int *arr, int arr_length, int t_id, int nthreads, struct RecursiveResponse *resArr)
{
    struct Passed *_str = (struct Passed *)(malloc(sizeof(struct Passed)));
    _str->arr = arr;
    _str->arr_length = arr_length;
    _str->t_id = t_id;
    _str->nthreads = nthreads;
    _str->resArr = resArr;

    return _str;
}

int *initArr(int N)
{
    int *arr = (int *)malloc(sizeof(int) * N);
    for (size_t i = 0; i < N; i++)
    {
        // fabs(rand() % (2 << 16) * rand() % (2 << 16))
        arr[i] = 7+i;
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

// s_ind and f_ind are included

struct RecursiveResponse
{
    int index;
    int el;
};
void show(struct RecursiveResponse *_resp)
{
    printf("Recursive min_el: %d index: %d\n", _resp->el, _resp->index);
}

struct RecursiveResponse *recursiveArrMin(int *arr, int s_ind, int f_ind)
{
    if (s_ind > f_ind)
        printf("start is larger than final");

    if (s_ind == f_ind)
    {
        struct RecursiveResponse *_ret = (struct RecursiveResponse *)malloc(sizeof(struct RecursiveResponse));
        _ret->el = arr[s_ind];
        _ret->index = s_ind;
        return _ret;
    }

    struct RecursiveResponse *l_wing = recursiveArrMin(arr, s_ind, (s_ind + f_ind) / 2);
    struct RecursiveResponse *r_wing = recursiveArrMin(arr, (s_ind + f_ind) / 2 + 1, f_ind);

    // gow the fuck this return works
    return l_wing-> el - r_wing->el < 0 ? l_wing : r_wing;
}

void *resArrMerge(void *passed_args)
{

    // int *arr, int s_ind, int f_ind, int t_id, struct RecursiveResponse* resArr

    struct Passed *_parr = (struct Passed *)passed_args;

    int arr_length, nthreads, t_id;
    int *arr;
    struct RecursiveResponse *resArr;

    decompose(&arr, &arr_length, &t_id, &nthreads, &resArr, _parr);

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
        resArr[t_id] = *recursiveArrMin(arr, delta * (t_id), delta * (t_id + 1) - 1);
    }
    else
    {
        resArr[t_id] = *recursiveArrMin(arr, forward_mod * delta + (delta - 1) * (t_id - forward_mod), forward_mod * delta + (delta - 1) * (t_id - forward_mod + 1) - 1);
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

void showMin(struct RecursiveResponse *ResponseArray, int N)
{
    struct RecursiveResponse _r = ResponseArray[0];
    for (size_t i = 1; i < N; i++)
    {
        if (_r.el > ResponseArray[i].el)
        {
            _r = ResponseArray[i];
        }
    }
    printf("After Merge Parallel recursive minimal elem: %d, index: %d\n", _r.el, _r.index);
}

int main()
{
    // 10.5 secs vs 0.58 secs for recursive returning struct
    srand(time(NULL));

    int N = 61;
    int n_threads = 12;

    int *arr = initArr(N);
    // showArr(arr, N);

    clock_t seq_s, seq_f;
    seq_s = clock();
    seqArrMin(arr, 0, N - 1);
    seq_f = clock();
    printf("time for seq: %f\n", ((double)(seq_f-seq_s)) / CLOCKS_PER_SEC);
    printf("Recursive minimal: %d\n", recursiveArrMin(arr, 0, N-1)->el);

    struct RecursiveResponse *ResponseArray = (struct RecursiveResponse *)malloc(sizeof(struct RecursiveResponse) * n_threads);

    // recursive solo
    clock_t rec_s, rec_f;
    rec_s = clock();
    show(recursiveArrMin(arr,0,N-1));
    rec_f = clock();
    printf("time for rec: %f\n", ((double)(rec_f-rec_s)) / CLOCKS_PER_SEC);


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
    showMin(ResponseArray, n_threads);
    f_par = clock();
    printf("Parallel clock with sync and printf_unfort: %f\n", ((double)(f_par-s_par)) / CLOCKS_PER_SEC);
    // showSqMatr(_m, N);

    return 0;
}