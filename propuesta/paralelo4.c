#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define N 600000
#define G 200

long V[N];
long R[G];
int A[G];

void kmean(int fN, int fK, long fV[], long fR[], int fA[])
{
    int i, j, iter = 0;
    long dif;
    long fS[G];
    int fD[N];

    double t_b1 = 0.0;
    double t_b2 = 0.0;
    double t_b3 = 0.0;
    double t_b4 = 0.0;

    do
    {
        double t0;
        t0 = omp_get_wtime();

        #pragma omp parallel for private(j) schedule(static)
        for (i = 0; i < fN; i++)
        {
            int min = 0;
            long best_dif = labs(fV[i] - fR[0]);

            for (j = 1; j < fK; j++)
            {
                long d = labs(fV[i] - fR[j]);

                if (d < best_dif)
                {
                    min = j;
                    best_dif = d;
                }
            }

            fD[i] = min;
        }

        t_b1 += omp_get_wtime() - t0;

        t0 = omp_get_wtime();

        for (i = 0; i < fK; i++)
        {
            fS[i] = 0;
            fA[i] = 0;
        }

        t_b2 += omp_get_wtime() - t0;

        t0 = omp_get_wtime();

        #pragma omp parallel for schedule(static) reduction (+:fS[:fK], fA[:fK])
        for (i = 0; i < fN; i++)
        {
            int k = fD[i];

            fS[k] += fV[i];
            fA[k]++;
        }

        t_b3 += omp_get_wtime() - t0;

        t0 = omp_get_wtime();

        dif = 0;

        for (i = 0; i < fK; i++)
        {
            long old = fR[i];

            if (fA[i])
            {
                fR[i] = fS[i] / fA[i];
            }

            dif += labs(old - fR[i]);
        }

        t_b4 += omp_get_wtime() - t0;

        iter++;

    } while (dif);

    printf("iter %d\n", iter);
    printf("Tiempo bucle1: %f\n", t_b1);
    printf("Tiempo bucle2: %f\n", t_b2);
    printf("Tiempo bucle3: %f\n", t_b3);
    printf("Tiempo bucle4: %f\n", t_b4);
}


void qs(int ii, int fi, long fV[], int fA[])
{
    int i, f;
    long pi, pa, vtmp, vta, vfi, vfa;

    pi = fV[ii];
    pa = fA[ii];

    i = ii + 1;
    f = fi;

    vtmp = fV[i];
    vta = fA[i];

    /*
     * No se paraleliza porque solo ordena G = 200 elementos.
     * El coste es muy pequeño comparado con kmean.
     */
    while (i <= f)
    {
        if (vtmp < pi)
        {
            fV[i - 1] = vtmp;
            fA[i - 1] = vta;

            i++;

            vtmp = fV[i];
            vta = fA[i];
        }
        else
        {
            vfi = fV[f];
            vfa = fA[f];

            fV[f] = vtmp;
            fA[f] = vta;

            f--;

            vtmp = vfi;
            vta = vfa;
        }
    }

    fV[i - 1] = pi;
    fA[i - 1] = pa;

    if (ii < f)
    {
        qs(ii, f, fV, fA);
    }

    if (i < fi)
    {
        qs(i, fi, fV, fA);
    }
}


int main()
{
    int i;
    double tiempo_qs = 0.0;

    printf("Threads: %d\n", omp_get_max_threads());

    /*
     * Inicialización del vector de datos.
     * Se mantiene secuencial para respetar el comportamiento original.
     */
    for (i = 0; i < N; i++)
    {
        V[i] = (rand() % rand()) / N;
    }

    /*
     * Primeros candidatos a representantes.
     */
    for (i = 0; i < G; i++)
    {
        R[i] = V[i];
    }

    /*
     * Cálculo de los G representantes mediante k-means.
     */
    kmean(N, G, V, R, A);

    /*
     * Ordenación final de los centroides.
     */
    double t0 = omp_get_wtime();

    qs(0, G - 1, R, A);

    tiempo_qs = omp_get_wtime() - t0;

    printf("Tiempo del quickSearch: %f\n", tiempo_qs);

    for (i = 0; i < G; i++)
    {
        printf("R[%d] : %ld te %d agrupats\n", i, R[i], A[i]);
    }

    return 0;
}