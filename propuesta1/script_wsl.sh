#!/bin/bash

# crear carpeta de resultados
mkdir -p resultados_wsl

echo "Compilando..."
gcc -O3 -fopenmp ./paralelo1.c -o paralelo1

echo "Ejecuciones..."

for t in 1 2 4 8 16 32 64 128
do
    echo "Threads: $t"

    export OMP_NUM_THREADS=$t

    {
        echo "Programa: paralelo1"
        echo "Maquina: wsl"
        echo "Threads: $t"
        /usr/bin/time ./paralelo1
    } &> resultados_wsl/wsl_$t

done

echo "Todo terminado."