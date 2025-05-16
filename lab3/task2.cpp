#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#define PI 3.14159265358979323846

double func(double x, double y)
{
    return exp((x + y) * (x + y));
}

double getrand(unsigned int *seed)
{
    return (double)rand_r(seed) / RAND_MAX;
}

int main(int argc, char **argv)
{
    const int n = 100000000; // Количество точек
    printf("Numerical integration by Monte Carlo method: n = %d\n", n);

    int in = 0;     // Количество точек в области
    double s = 0.0; // Сумма

    double t = omp_get_wtime();

#pragma omp parallel
    {
        double s_loc = 0.0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();

#pragma omp for nowait
        for (int i = 0; i < n; i++)
        {
            double x = getrand(&seed);
            double y = getrand(&seed) * (1 - x);
            in_loc++;
            s_loc += func(x, y);
        }

#pragma omp atomic
        s += s_loc;
#pragma omp atomic
        in += in_loc;
    }

    double v = 0.5;
    double res = v * s / in;

    printf("Result: %.12f, n %d\n", res, n);
    printf("Elapsed time (sec.): %.6f\n", omp_get_wtime() - t);

    return 0;
}