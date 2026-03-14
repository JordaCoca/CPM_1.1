#!/bin/bash

SRC="./paralelo1.c"
BIN="./pal1"


echo "Compilando programa..."
cc -O3 -fopenmp $SRC -o $BIN

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi


echo "===== EJECUCIONES EN ORCA ====="

for t in 2 4 8 16 32 64 128
do
    echo "Threads: $t"

    {
        echo "Programa: paralelo1"
        echo "Maquina: orca"
        echo "Threads: $t"
        echo "----------------------"

        srun -p orca -c $t time $BIN

    } &> orca_$t
done

echo "Ejecuciones completadas"