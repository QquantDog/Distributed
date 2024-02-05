#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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

void showArr(int *arr, int N, int Targ)
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
    // isAsc(arr, N);
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
    // 3 - state passing through configuration tree - "saving invariant abstract state"
    // for exmaple in merge sort - always returning 2 sorted arrays which needs to be sorted
    printf("l_tree: %d r_tree: %d\nbefore swap:\n", l_counter, r_counter);
    showArr(arr, N, X);

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
    // placed solo element
    // now adjusting others with same values
    // now element is on less_counter pos
    // ALL PROBLESMS HERE !!!!!!!!!!!!!!!
    printf("after swap:\n");
    showArr(arr, N, X);
    int incr_ind = less_counter + 1;
    for (int i = 0; i < N; i++)
    {
        if (arr[i] == X && ((i < less_counter || i > incr_ind)))
        {
            if (arr[i] == arr[incr_ind])
            {
                // could be overflow !!!!!!!!!!!!!
                printf("inside: ");
                printf("i = %d, incr_index = %d\n", i, incr_ind);
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
            else{
                swap(&arr[i],&arr[incr_ind]);
                incr_ind++;
            }
        }
    }

    // its MUST to throw low to left, high to right
    int l_ind = 0;
    int r_ind = 0;

    int iter = 0;
    int i = 0;
    int j = less_counter + 1;

    printf("after swap + ADJ:\n");
    showArr(arr, N, X);

    // ascending reshuffle
    while (i < less_counter && j < N)
    {

        if (arr[i] > X)
        {
            // if (arr[i] != X)
            // {
            if (arr[j] < X)
            {
                swap(&arr[i], &arr[j]);
                i++;
                j++;
                continue;
            }
            j++;
            continue;
            // }
        }
        i++;
    }

    // printf("PLUS reshuffle after swap:\n");
    // showArr(arr, N, X);
    // printf("X_ind: %d\n", X_ind);
    // printf("less_c: %d\n", less_counter);
    // printf("\n");

    // 2 - block of recursive invocation with correct shifting
    // array shift : every array comes shifted - so pointer points to correct initial address
    // recursiveTraverseSort(&arr[0], less_counter+1, l_counter+1, r_counter); //+
    // recursiveTraverseSort(&arr[(less_counter+1)], N-(less_counter+1), l_counter, r_counter+1); //

    recursiveTraverseSort(&arr[0], less_counter, l_counter + 1, r_counter);                        //+
    recursiveTraverseSort(&arr[less_counter + 1], N - less_counter - 1, l_counter, r_counter + 1); //

    // int* l_wing_arr = merge2SortedArrays();

    // 4 - possible return

    // return merge2SortedArrays(l_wing_arr, r_wing_arr, _first, _last);
}

int main()
{

    srandom(time(NULL));
    // int N_f = 10;
    // int N_s = 5;

    // int *f = initArr(N_f);
    // int *s = initArr(N_s);

    // usualSort(f, N_f);
    // usualSort(s, N_s);

    // showArr(f, N_f);
    // showArr(s, N_s);
    // f[0] = -123;
    // printf("after merge\n");
    // merge2SortedArrays(f, s, N_f, N_s);
    // showArr(f, N_f);
    // showArr(s, N_s);

    int N = 5000;
    int *arr = initArr(N);

    showArr(arr, N, -99999);
    printf("----\n");
    recursiveTraverseSort(arr, N, 0, 0);
    showArr(arr, N, -99999);
    isAsc(arr, N);

    return 0;
}