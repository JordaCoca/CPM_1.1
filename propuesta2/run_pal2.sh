#!/bin/bash

SRC="codigo/paralelo2.c"
BIN="codigo/p"

DIR_TEEN="resultados_teen"
DIR_ORCA="resultados_orca"

mkdir -p $DIR_TEEN
mkdir -p $DIR_ORCA

echo "Compilando programa..."
cc -O3 -fopenmp $SRC -o $BIN

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi

echo "===== EJECUCIONES EN TEEN ====="

for t in 2 4 8 16 32
do
    echo "Threads: $t"

    {
        echo "Programa: paralelo1"
        echo "Maquina: teen"
        echo "Threads: $t"
        echo "----------------------"

        srun -p teen -c $t time $BIN

    } &> $DIR_TEEN/teen_$t
done


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

    } &> $DIR_ORCA/orca_v2_$t
done

echo "Ejecuciones completadas"