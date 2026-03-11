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
int i,j,min,iter=0;
long dif,t;
long fS[G];
int fD[N];

do{

    /* ===== BUCLE 1 : asignación de clusters ===== */

    #pragma omp parallel for private(j,min,dif)
    for (i=0;i<fN;i++)
    {
        min = 0;
        dif = labs(fV[i]-fR[0]);

        for (j=1;j<fK;j++)
        {
            long d = labs(fV[i]-fR[j]);
            if (d < dif)
            {
                min = j;
                dif = d;
            }
        }

        fD[i] = min;
    }


    /* ===== BUCLE 2 : inicializar acumuladores ===== */

    #pragma omp parallel for
    for(i=0;i<fK;i++)
    {
        fS[i] = 0;
        fA[i] = 0;
    }


    /* ===== BUCLE 3 : sumar valores por cluster ===== */

    #pragma omp parallel
    {
        long fS_local[G];
        int  fA_local[G];

        for(int k=0;k<fK;k++)
        {
            fS_local[k] = 0;
            fA_local[k] = 0;
        }

        #pragma omp for
        for(i=0;i<fN;i++)
        {
            int c = fD[i];
            fS_local[c] += fV[i];
            fA_local[c] ++;
        }

        #pragma omp critical
        {
            for(int k=0;k<fK;k++)
            {
                fS[k] += fS_local[k];
                fA[k] += fA_local[k];
            }
        }
    }

    /* ===== BUCLE 4 : recalcular centroides ===== */
    dif = 0;

    for(i=0;i<fK;i++)
    {
        t = fR[i];

        if (fA[i])
            fR[i] = fS[i] / fA[i];

        dif += labs(t - fR[i]);
    }

    iter++;

}while(dif);

printf("iter %d\n",iter);
}


void qs(int ii, int fi, long fV[], int fA[])
{
int i,f;
long pi,pa,vtmp,vta,vfi,vfa;

pi = fV[ii];
pa = fA[ii];

i = ii+1;
f = fi;

vtmp = fV[i];
vta  = fA[i];


while(i <= f)
{
    if(vtmp < pi)
    {
        fV[i-1] = vtmp;
        fA[i-1] = vta;

        i++;

        vtmp = fV[i];
        vta  = fA[i];
    }
    else
    {
        vfi = fV[f];
        vfa = fA[f];

        fV[f] = vtmp;
        fA[f] = vta;

        f--;

        vtmp = vfi;
        vta  = vfa;
    }
}

fV[i-1] = pi;
fA[i-1] = pa;

if(ii < f) qs(ii,f,fV,fA);
if(i < fi) qs(i,fi,fV,fA);
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

return 0;
}