#include <stdlib.h>
#include <stdio.h>
// #include <omp.h>   // Para la libreria Omp

#define N 600000
#define G 200  

long V[N];
long R[G];
int A[G];

void kmean(int fN, int fK, long fV[], long fR[], int fA[])
{
    int i,j,min,iter=0;
    long dif,t;
    long fS[G];
    int fD[N];

    do
    {
        /* Cada iteracion escribe en  fD[i] distinto */
        // #pragma omp parallel for private(j,min,dif)
        for (i=0;i<fN; i++)
        {
            min = 0;
            dif = labs(fV[i] -fR[0]);
            for (j=1;j<fK;j++)
                if (labs(fV[i] -fR[j]) < dif)
                {
                    min = j;
                    dif = labs(fV[i] -fR[j]);
                }
            fD[i] = min;
        }

        /*Sin dependencias, paralelizable facil */

        // #pragma omp parallel for
        for(i=0;i<fK;i++)
        {
            fS[i] = 0;
            fA[i] = 0;
        }

        /* 
           Aqui si hay dependencias --> múltiples hilos pueden escribir en fS[k] y fA[k]
           Para mantener exactamente el mismo resultado usamos acumuladores locales por hilo y luego
           reduccion manual en seccion critica
        */

        /*
        #pragma omp parallel
        {
            long localS[G] = {0};
            int  localA[G] = {0};

            #pragma omp for
            for(i=0;i<fN;i++)
            {
                localS[fD[i]] += fV[i];
                localA[fD[i]] ++;
            }

            #pragma omp critical
            {
                for(int k=0;k<fK;k++)
                {
                    fS[k] += localS[k];
                    fA[k] += localA[k];
                }
            }
        }
        */

        // Versión secuencial original (se mantiene)
        for(i=0;i<fN;i++)
        {
            fS[fD[i]] += fV[i];
            fA[fD[i]] ++;
        }

        /*  Podria reducirse dif? */

        dif = 0;

        /*
        #pragma omp parallel for private(t) reduction(+:dif)
        */
        for(i=0;i<fK;i++)
        {
            t = fR[i];
            if (fA[i]) 
                fR[i] = fS[i]/fA[i];

            dif += labs(t - fR[i]);
        }

        iter++;

    } while(dif);

    printf("iter %d\n",iter);
}

/* ===================================================
   No lo toco d momento porque:
   - Es recursivo
   - Tiene dependencias estructurales
   - Para paralelizar correctamente habría q usar tasks con umbral minimo d tamaño
   Lo dejo así 
=================================================== */

void qs(int ii, int fi, long fV[], int fA[])
{
    int i,f;
    long pi,pa,vtmp,vta,vfi,vfa;

    pi = fV[ii];
    pa = fA[ii];
    i = ii +1;
    f = fi;
    vtmp = fV[i];
    vta = fA[i];

    while (i <= f)
    {
        if (vtmp < pi) {
            fV[i-1] = vtmp;
            fA[i-1] = vta;
            i ++;
            vtmp = fV[i];
            vta = fA[i];
        }
        else {
            vfi = fV[f];
            vfa = fA[f];
            fV[f] = vtmp;
            fA[f] = vta;
            f --;
            vtmp = vfi;
            vta = vfa;
        }
    }
    fV[i-1] = pi;
    fA[i-1] = pa;

    if (ii < f) qs(ii,f,fV,fA);
    if (i < fi) qs(i,fi,fV,fA);
}

int main()
{
    int i;

    for (i=0;i<N;i++) 
        V[i] = (rand()%rand())/N;

    for (i=0;i<G;i++) 
        R[i] = V[i];

    kmean(N,G,V,R,A);

    qs(0,G-1,R,A);

    for (i=0;i<G;i++) 
        printf("R[%d] : %ld te %d agrupats\n",i,R[i],A[i]);

    return(0);
}