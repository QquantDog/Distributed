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
        arr[i] = 1 + rand() % 100;
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

void showArr(int *arr, int N)
{
    for (int i = 0; i < N; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int* merge2SortedArrays(int *f, int *s, int f_length, int s_length)
{
    int *res = (int *)malloc((f_length + s_length) * sizeof(int));
    int f_ind = 0;
    int s_ind = 0;

    while (f_ind < f_length && s_ind < s_length)
    {
        if (f[f_ind] < s[s_ind])
        {
            res[f_ind + s_ind] = f[f_ind];
            f_ind++;
        }
        else
        {
            res[f_ind + s_ind] = s[s_ind];
            s_ind++;
        }
    }
    while (f_ind < f_length)
    {
        res[f_ind + s_ind] = f[f_ind];
        f_ind++;
    }
    while (s_ind < s_length)
    {
        res[f_ind + s_ind] = s[s_ind];
        s_ind++;
    }
    printf("res show(l=%d):\n",f_length+s_length);
    showArr(res, f_length+s_length);
    printf("end\n");

    return res;
}

int* recursiveTraverseSort(int* arr, int N){

    int _first = N/2;
    int _last = N - _first;

    // 1 - condition of tree leaves return 
    if(N == 1){
        return &arr[0];
    }
    
    // 2 - block of recursive invocation with correct shifting
    //array shift : every array comes shifted - so pointer points to correct initial address
    int* l_wing_arr = recursiveTraverseSort(&arr[0], _first);
    int* r_wing_arr = recursiveTraverseSort(&arr[_first], _last);
    // int* l_wing_arr = merge2SortedArrays(); 

    // 3 - state passing through configuration tree - "saving invariant abstract state"
    //for exmaple in merge sort - always returning 2 sorted arrays which needs to be sorted
    // 4 - possible return
    
    return merge2SortedArrays(l_wing_arr, r_wing_arr, _first, _last);

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

    int N = 9;
    int* arr = initArr(N);

    showArr(arr, N);
    int* new_arr = recursiveTraverseSort(arr, N);
    showArr(new_arr, N);

    return 0;
}