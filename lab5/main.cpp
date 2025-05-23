#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>

const int THRESHOLD = 100000;

void partition(std::vector<int> &v, int &i, int &j, int low, int high)
{
    i = low;
    j = high;
    int pivot = v[(low + high) / 2];
    do
    {
        while (v[i] < pivot)
            i++;
        while (v[j] > pivot)
            j--;
        if (i <= j)
        {
            std::swap(v[i], v[j]);
            i++;
            j--;
        }
    } while (i <= j);
}

void sequential_quicksort(std::vector<int> &v, int low, int high)
{
    if (low < high)
    {
        int i, j;
        partition(v, i, j, low, high);
        if (low < j)
            sequential_quicksort(v, low, j);
        if (i < high)
            sequential_quicksort(v, i, high);
    }
}

void parallel_quicksort(std::vector<int> &v, int low, int high)
{
    if (high - low < THRESHOLD)
    {
        sequential_quicksort(v, low, high);
        return;
    }

    int i, j;
    partition(v, i, j, low, high);

#pragma omp task shared(v)
    {
        if (low < j)
            parallel_quicksort(v, low, j);
    }

#pragma omp task shared(v)
    {
        if (i < high)
            parallel_quicksort(v, i, high);
    }

#pragma omp taskwait
}

void run_parallel_quicksort(std::vector<int> &v)
{
#pragma omp parallel
    {
#pragma omp single nowait
        parallel_quicksort(v, 0, v.size() - 1);
    }
}

int main()
{
    const int N = 500000;
    std::vector<int> data(N);

    std::generate(data.begin(), data.end(), []()
                  { return rand() % 1000; });

    std::vector<int> seq_data = data;
    std::vector<int> par_data = data;

    auto start = std::chrono::high_resolution_clock::now();
    sequential_quicksort(seq_data, 0, seq_data.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seq_time = end - start;

    start = std::chrono::high_resolution_clock::now();
    run_parallel_quicksort(par_data);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> par_time = end - start;

    if (seq_data != par_data)
    {
        std::cerr << "Sorting failed!" << std::endl;
        return 1;
    }

    std::cout << "Sequential time: " << seq_time.count() << " s" << std::endl;
    std::cout << "Parallel time: " << par_time.count() << " s" << std::endl;
    std::cout << "Speedup: " << seq_time.count() / par_time.count() << std::endl;

    return 0;
}