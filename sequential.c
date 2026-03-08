#include <stdlib.h>
#include <stdio.h>

#define N 600000			// Numero de elementos a agrupar
#define G 200  				// Numero de grupos o clusters

long V[N];					// Arreglo de valores a agrupar
long R[G];					// Centroides (1 centroide es 1 valor representativo d cada grup)
int A[G];					// Num d elementos asignados a cada grupo

/*
	Parametros:
		-fn: Numero d elementos a mirar
		-fk: Num d clusters
		-fV[]: Array d valores a agrupar
		-fR[]: Array d centroides
		-fA[]: Num elementos por grupo
*/
void kmean(int fN, int fK, long fV[], long fR[], int fA[])
{
int i,j,min,iter=0;
long dif,t;
long fS[G];			
int fD[N];

	// Primera asignacion de cada elemento al cluster más cercano
	do
	 {
	  // Miramos todos los elementos
	  for (i=0;i<fN; i++)
	   {
		min = 0;						 // Indice del centroide (cluster) con el que se guarda una distancia menor (inicialmente 0)
		dif = abs(fV[i] -fR[0]);		 // Calculamos la distancia respecto el elemento actual y el centroide (un primer centroide, inicializamos basicamente el dif_menor)
		for (j=1;j<fK;j++)				 // Miramos los otros clusters
		  if (abs(fV[i] -fR[j]) < dif)   // SI la fistancia entre este elemento y el siguiente centroide (cluster), es menor que el anterior dif_min:
		   {	
			min = j;					// El siguiente cluster pasa a ser el cluster_minimo actual del elemento actual (de momento)
			dif = abs(fV[i] -fR[j]);	// La distancia minima es la nueva encontrada
		   }
		fD[i] = min;					// Asignamos finalmente el elemento actual el cluster con el que guarde menos distancia con su centroide
	   }
	  
	  
	  //-fK: Num d clusters
	  //-fS[]: Almacenar la suma d todos los elmentos asignados al cluster i
	  //-fA[]: Almacenar cuantos elementos hay en cada cluster
	  
	  // Inicializamos los acumuladres a 0
	  for(i=0;i<fK;i++)
		fS[i] = fA[i] = 0;

	  // Fd[]: Es un array que tiene tantas posiciones como elementos a asignar, cada casilla indica que elemento (i) pertenece a que cluster (el valor como tal (un indice))
	  for(i=0;i<fN;i++)
	   {
		fS[fD[i]] += fV[i];		// fF[indice_cluster_al_q_pertenece_este_elemento] = valor_numerico_de_este_elemento
		fA[fD[i]] ++;			// fA[indice_cluster_al_q_pertenece_este_elemento]++, indicamos que tiene 1 elemento más ese cluster
	   }
	  
	  // Recalculamos los centroides con la media de los elementos asignados
	  dif = 0;								// mide cuanto cambiaron los centroides en esta iteración, condicion d parada
	  for(i=0;i<fK;i++)						// miramos todos los clusters (num d clusters = fK)
	   {
		t = fR[i];							// pillamos el centroide actual y lo guardamos en 't'
		if (fA[i]) fR[i] = fS[i]/fA[i];     // si el cluster tiene algun miembro, calculamos entonces el nuevo centroide --> media d todos los valores numericos d todos los elementos que forman parte del cluster
		dif += abs(t - fR[i]);				// acumulamos diferencia absoluta entre el centroide anterior ('t') y el nuevo
	   }
	   iter ++;								// num d iteraciones totales (cada iteración implica asignar todos los elementos, y recalcular los centroides)
	 } while(dif);							// Hacemos esto siempre y cuando se mejoren los centroides, cuando se estabilicen llegamos al gin (cuando los centroides no hayan cambiado entre iteraciones, ese es el final)

	 printf("iter %d\n",iter);
}

void qs(int ii, int fi, long fV[], int fA[])
 {
  int i,f,j;
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

  for (i=0;i<N;i++) V[i] = (rand()%rand())/N;

  // primers candidats
  for (i=0;i<G;i++) R[i] = V[i];

  // calcular els G mes representatius
  kmean(N,G,V,R,A);

  qs(0,G-1,R,A);

  for (i=0;i<G;i++) 
   printf("R[%d] : %ld te %d agrupats\n",i,R[i],A[i]);

  return(0);
}
