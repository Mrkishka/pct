#include <stdio.h>
#include <math.h>
#include <omp.h>

double func(double x)
{
    return sqrt(x * (3 - x)) / (x + 1);
}

int main(int argc, char **argv)
{
    double t = omp_get_wtime();
    const double eps = 1E-5;
    const double a = 1.0;
    const double b = 1.2;
    const int n0 = 1000000;

    printf("Numerical integration: [%f, %f], n0 = %d, EPS = %f\n", a, b, n0, eps);

    double sq[2];
#pragma omp parallel
    {
        int n = n0, k;
        double delta = 1;

        for (k = 0; delta > eps; n *= 2, k ^= 1)
        {
            double h = (b - a) / n;
            double s = 0.0;
            sq[k] = 0;

#pragma omp barrier

#pragma omp for nowait
            for (int i = 0; i < n; i++)
            {
                s += func(a + h * (i + 0.5));
            }

#pragma omp atomic
            sq[k] += s * h;

#pragma omp barrier

            if (n > n0)
            {
                delta = fabs(sq[k] - sq[k ^ 1]) / 3.0; // Правило Рунге (p=2)
            }
        }

#pragma omp master
        {
            printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k], eps, n / 2);
            t = omp_get_wtime() - t;
            printf("Elapsed time (sec.): %.6f\n", t);
        }
    }
    return 0;
}