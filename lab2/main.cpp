#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

void dgemv_serial(double alpha, const std::vector<std::vector<double>> &A,
                  const std::vector<double> &x, double beta, std::vector<double> &y)
{
    int m = A.size();
    int n = A[0].size();

    for (int i = 0; i < m; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < n; ++j)
        {
            sum += A[i][j] * x[j];
        }
        y[i] = alpha * sum + beta * y[i];
    }
}

void dgemv_parallel(double alpha, const std::vector<std::vector<double>> &A,
                    const std::vector<double> &x, double beta, std::vector<double> &y,
                    int num_threads)
{
    int m = A.size();
    int n = A[0].size();

#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < m; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < n; ++j)
        {
            sum += A[i][j] * x[j];
        }
        y[i] = alpha * sum + beta * y[i];
    }
}

int main()
{
    std::vector<int> sizes = {15000, 20000, 25000};
    std::vector<int> threads = {2, 4, 6, 8};

    for (int n : sizes)
    {
        int m = n;
        std::vector<std::vector<double>> A(m, std::vector<double>(n, 1.0));
        std::vector<double> x(n, 2.0);
        std::vector<double> y(m, 0.0);
        double alpha = 1.0, beta = 0.0;

        // Последовательно
        auto start_serial = std::chrono::high_resolution_clock::now();
        dgemv_serial(alpha, A, x, beta, y);
        auto end_serial = std::chrono::high_resolution_clock::now();
        double serial_time = std::chrono::duration<double>(end_serial - start_serial).count();

        std::cout << "Размер матрицы: " << m << "x" << n << std::endl;
        std::cout << "Серийное время: " << serial_time << " секунды" << std::endl;

        // Параллельно
        for (int t : threads)
        {
            std::fill(y.begin(), y.end(), 0.0);

            auto start_parallel = std::chrono::high_resolution_clock::now();
            dgemv_parallel(alpha, A, x, beta, y, t);
            auto end_parallel = std::chrono::high_resolution_clock::now();
            double parallel_time = std::chrono::duration<double>(end_parallel - start_parallel).count();

            double speedup = serial_time / parallel_time;
            std::cout << "Потоки: " << t << ", Время: " << parallel_time
                      << " с, Ускорение: " << speedup << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}