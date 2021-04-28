/* SOR program */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define N 10
#define ITMAX 20

int main(int an, char **as) {
  int i, j, it;
  float MAXEPS = 0.5E-5f;
  float w = 0.5f;
  float A[N][N];
#pragma omp parallel default(shared)
  {
#pragma omp for private(j)
    for (i = 0; i < N; i++)
      for (j = 0; j < N; j++)
        if (i == j)
          A[i][j] = N + 2;
        else
          A[i][j] = -1.0f;
  }

  for (it = 1; it <= ITMAX; it++) {
    float eps = 0.f;
#pragma omp parallel default(shared)
    {
#pragma omp for reduction(max : eps) collapse(2) ordered(2) schedule(static, 1)
      for (i = 1; i < N - 1; i++)
        for (j = 1; j < N - 1; j++) {
#pragma omp ordered depend(sink : i - 1, j) depend(sink : i, j - 1)

          float s;
          s = A[i][j];
          A[i][j] = (w / 4) * (A[i - 1][j] + A[i + 1][j] + A[i][j - 1] +
                               A[i][j + 1]) +
                    (1 - w) * A[i][j];
          eps = Max(fabs(s - A[i][j]), eps);
#pragma omp ordered depend(source)
        }
    }

    printf("it=%4i   eps=%e\n", it, eps);
    if (eps < MAXEPS)
      break;
  }
  return 0;
}
