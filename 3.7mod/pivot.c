#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "color.h"

struct Passed
{
    int *arr;
    int arr_length;

    int t_id;
    int nthreads;
};

// , int **resIntAnsw,
void decompose(int **arr, int *arr_length, int *t_id, int *nthreads, struct Passed *_str)
{
    *arr = _str->arr;
    *arr_length = _str->arr_length;
    *t_id = _str->t_id;
    *nthreads = _str->nthreads;
    // *resIntAnsw = _str->resIntAnsw;
}

struct Passed *compose(int *arr, int arr_length, int t_id, int nthreads)
{
    struct Passed *_str = (struct Passed *)(malloc(sizeof(struct Passed)));
    _str->arr = arr;
    _str->arr_length = arr_length;
    _str->t_id = t_id;
    _str->nthreads = nthreads;
    // _str->resIntAnsw = resIntAnsw;

    return _str;
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int *initArr(int N)
{
    int *arr = (int *)malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++)
    {
        // fabs(rand() % (2 << 16) * rand() % (2 << 16))
        arr[i] = 1 + rand() % 1000;
    }
    return arr;
}

// double sort should be 0..n-1 0..n-1
void usualSort(int *arr, int N)
{
    for (int i = 0; i < N - 1; i++)
    {
        for (int j = 0; j < N - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

void isAsc(int *arr, int N)
{
    int flag = 1;

    for (int i = 0; i < N - 1; i++)
    {
        if (arr[i] > arr[i + 1])
        {
            flag *= 0;
        }
    }
    printf("is GOOD ASC order?: %d\n", flag);
}

void showArrTarget(int *arr, int N, int Targ)
{
    for (int i = 0; i < N; i++)
    {
        if (arr[i] == Targ)
        {
            printf("\e[1;34m%d\e[0m ", arr[i]);
        }
        else
        {
            printf("%d ", arr[i]);
        }
    }
    printf("\n");
}

void showArr(int *arr, int N)
{
    for (int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void showSubArrHighlighted(int *arr, int N, int t_id)
{
    const char *color_start = getColorCode(t_id);
    const char *color_end = resetColorCode();

    for (int i = 0; i < N; i++)
    {
        printf("%s%d%s ", color_start, arr[i], color_end);
    }
}

void threadDelimeter(int *delta, int *complemented_mod, int *forward_mod, int arr_length, int nthreads)
{
    if (arr_length % nthreads == 0)
    {
        *complemented_mod = 0;
        *delta = arr_length / nthreads;
    }
    else
    {
        *delta = arr_length / nthreads + 1;
        *complemented_mod = nthreads * (*delta) - arr_length;
    }
    *forward_mod = nthreads - *complemented_mod;
}

void showFullArrHighlighted(int *arr, int arr_length, int nthreads)
{

    int delta;
    int complemented_mod;
    int forward_mod;
    //
    threadDelimeter(&delta, &complemented_mod, &forward_mod, arr_length, nthreads);
    //

    for (int thr_block = 0; thr_block < nthreads; thr_block++)
    {
        if (thr_block < forward_mod)
        {
            int _start = delta * (thr_block);
            int _length = delta * (thr_block + 1) - 1 - delta * (thr_block) + 1;

            showSubArrHighlighted(&arr[_start], _length, thr_block);
        }
        else
        {
            int _start = forward_mod * delta + (delta - 1) * (thr_block - forward_mod);
            int _length = (forward_mod * delta + (delta - 1) * (thr_block - forward_mod + 1) - 1) - (forward_mod * delta + (delta - 1) * (thr_block - forward_mod)) + 1;

            showSubArrHighlighted(&arr[_start], _length, thr_block);
        }
    }

    printf("\n");
}

void recursiveTraverseSort(int *arr, int N, int l_counter, int r_counter)
{

    int X_ind = N - 1;
    int X = arr[X_ind];

    int less_counter = 0;
    // 1 - condition of tree leaves return
    if (N <= 1)
    {
        return;
    }


    for (int i = 0; i < N; i++)
    {
        if (arr[i] < arr[X_ind])
        {
            less_counter++;
        }
    }

    int _t = arr[X_ind];
    arr[X_ind] = arr[less_counter];
    arr[less_counter] = _t;


    int incr_ind = less_counter + 1;
    for (int i = 0; i < N; i++)
    {
        if (arr[i] == X && ((i < less_counter || i > incr_ind)))
        {
            if (arr[i] == arr[incr_ind])
            {
                while (arr[incr_ind] == X && incr_ind < N)
                {
                    incr_ind++;
                }
                if (incr_ind == N)
                {
                    break;
                }
                swap(&arr[i], &arr[incr_ind]);
                incr_ind++;
                continue;
            }
            else
            {
                swap(&arr[i], &arr[incr_ind]);
                incr_ind++;
            }
        }
    }

    int iter = 0;
    int i = 0;
    int j = less_counter + 1;

    while (i < less_counter && j < N)
    {

        if (arr[i] > X)
        {
            if (arr[j] < X)
            {
                swap(&arr[i], &arr[j]);
                i++;
                j++;
                continue;
            }
            j++;
            continue;
        }
        i++;
    }

    recursiveTraverseSort(&arr[0], less_counter, l_counter + 1, r_counter);                        //+
    recursiveTraverseSort(&arr[less_counter + 1], N - less_counter - 1, l_counter, r_counter + 1); //
}

// dispatcher for sorting array in place
//
void *dispatcher(void *passed_args)
{

    struct Passed *_parr = (struct Passed *)passed_args;

    int arr_length, nthreads, t_id;
    int *arr;
    int *resArr;

    decompose(&arr, &arr_length, &t_id, &nthreads, _parr);

    if (t_id == 0)
    {
        showFullArrHighlighted(arr, arr_length, nthreads);
    }

    int delta;
    int complemented_mod;
    int forward_mod;

    threadDelimeter(&delta, &complemented_mod, &forward_mod, arr_length, nthreads);

    if (t_id < forward_mod)
    {
        // recursiveArrSort(arr, delta * (t_id), delta * (t_id + 1) - 1);
        // should be N
        int _start = delta * (t_id);
        int _length = delta * (t_id + 1) - 1 - delta * (t_id) + 1;
        printf("refular ++ t_id: %d -- s_ind: %d -- _l: %d\n", t_id, _start, _length);
        showSubArrHighlighted(&arr[_start], _length, t_id);
        printf("\n");
        recursiveTraverseSort(&arr[_start], _length, 0, 0);
    }
    else
    {
        // recursiveArrSort(arr, forward_mod * delta + (delta - 1) * (t_id - forward_mod), forward_mod * delta + (delta - 1) * (t_id - forward_mod + 1) - 1);
        int _start = forward_mod * delta + (delta - 1) * (t_id - forward_mod);
        int _length = (forward_mod * delta + (delta - 1) * (t_id - forward_mod + 1) - 1) - (forward_mod * delta + (delta - 1) * (t_id - forward_mod)) + 1;
        printf("border  ++ t_id: %d -- s_ind: %d -- _l: %d\n", t_id, _start, _length);
        showSubArrHighlighted(&arr[_start], _length, t_id);
        printf("\n");
        recursiveTraverseSort(&arr[_start], _length, 0, 0);
    }
    // showArr(arr, arr_length);
}

struct Containter
{
    int *arr;
    int L;
    int curr_iter;
    int isFinished;
};

void initContainerEl(struct Containter *el, int *arr, int L, int curr_iter, int isFinished)
{
    el->arr = arr;
    el->L = L;
    el->curr_iter = curr_iter;
    el->isFinished = isFinished;
}

void regularInit(int *arr, int arr_length,int delta, int nthreads, int forward_mod, struct Containter* _cont)
{

    for (int thr_block = 0; thr_block < forward_mod; thr_block++)
    {
        int _start = delta * (thr_block);
        int _length = delta * (thr_block + 1) - 1 - delta * (thr_block) + 1;

        initContainerEl(&_cont[thr_block], &arr[_start], _length, 0, 0);
        // showSubArrHighlighted(&arr[_start], _length, thr_block);
    }
}

void boundInit(int *arr, int arr_length,int delta, int nthreads, int forward_mod, struct Containter* _cont)
{
    for (int thr_block = forward_mod; thr_block < nthreads; thr_block++)
    {
        int _start = forward_mod * delta + (delta - 1) * (thr_block - forward_mod);
        int _length = (forward_mod * delta + (delta - 1) * (thr_block - forward_mod + 1) - 1) - (forward_mod * delta + (delta - 1) * (thr_block - forward_mod)) + 1;

        initContainerEl(&_cont[thr_block], &arr[_start], _length, 0, 0);
        // showSubArrHighlighted(&arr[_start], _length, thr_block);
    }
}

// N == nthreads
void MinIter(struct Containter* _cont, int N, int* result, int res_index){
    int min = __INT_MAX__;
    int min_index = -1;

    for(int i=0;i < N;i++){
        // better <= for explicit cases when value = INT_MAX
        if(_cont[i].arr[_cont[i].curr_iter] < min && (_cont[i].curr_iter < _cont[i].L)){
            min = _cont[i].arr[_cont[i].curr_iter];
            min_index = i;
        }
    }
    result[res_index] = min;
    _cont[min_index].curr_iter++;
    if(_cont[min_index].curr_iter == _cont[min_index].L){
        _cont[min_index].isFinished = 1;
    }

}

void K_Merge(int *arr, int arr_length, int nthreads)
{
    int delta;
    int complemented_mod;
    int forward_mod;

    threadDelimeter(&delta, &complemented_mod, &forward_mod, arr_length, nthreads);

    struct Containter *containter = (struct Containter *)malloc(nthreads * sizeof(struct Containter));

    regularInit(arr, arr_length, delta, nthreads, forward_mod, containter);
    boundInit(arr, arr_length, delta, nthreads, forward_mod, containter);

    int* RES = (int*) malloc(arr_length * sizeof(int));

    for(int i=0;i< arr_length;i++){
        MinIter(containter, nthreads, RES, i);
    }

    printf("\nIS IT IN ASCENDING ORDER:\n");
    showArr(RES, arr_length);
    isAsc(RES, arr_length);

}

int main()
{

    srandom(time(NULL));

    int N = 25;
    int n_threads = 6;

    int *arr = initArr(N);
    printf("init:\n");
    showArr(arr, N);
    printf("N_elements: %d\n", N);

    pthread_t *pthr_arr = (pthread_t *)(malloc(sizeof(pthread_t) * n_threads));

    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_t _t;
        pthr_arr[i] = _t;
    }

    for (size_t i = 0; i < n_threads; i++)
    {
        struct Passed *_pargs = compose(arr, N, i, n_threads);
        pthread_create(&pthr_arr[i], NULL, dispatcher, (void *)_pargs);
    }

    for (size_t i = 0; i < n_threads; i++)
    {
        pthread_join(pthr_arr[i], NULL);
    }
    printf("after parallel sort:\n");
    showArr(arr, N);

    //
    printf("after k-merge parallel sort:\n");

    isAsc(arr, N);

    K_Merge(arr, N, n_threads);

    return 0;
}