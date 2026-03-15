#!/bin/bash

SRC="./paralelo1.c"
BIN="./pal1"

RUNS=10

echo "Compilando programa..."
cc -O3 -fopenmp $SRC -o $BIN

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi

echo "===== EJECUCIONES EN ORCA ====="

for t in 2 4 8 16 32 64 128
do
    echo "Configuracion: $t threads"

    mkdir -p $t

    for ((i=1;i<=RUNS;i++))
    do
        echo "  Tanda $i"

        {
            echo "Programa: paralelo1"
            echo "Maquina: orca"
            echo "Threads: $t"
            echo "Run: $i"
            echo "----------------------"

            srun -p orca -c $t time $BIN

        } &> $t/orca_${i}_${t}

    done
done

echo "Ejecuciones completadas"