#!/bin/bash

SRC="./paralelo3.c"
RESULTS_DIR="./resultados_3"
BIN="./paralelo3"

RUNS=10

echo "Compilando programa..."
cc -O3 -fopenmp "$SRC" -o "$BIN"

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi

mkdir -p "$RESULTS_DIR"

echo "===== EJECUCIONES EN ORCA ====="

for t in 1 2 4 8 16 32 64 128
do
        echo "Configuracion: $t threads, run $r"

        {
            echo "Programa: paralelo3"
            echo "Maquina: orca"
            echo "Threads: $t"
            echo "Run: $r"
            echo "----------------------"

            export OMP_NUM_THREADS=$t
            export OMP_DYNAMIC=FALSE
            export OMP_PLACES=cores
            export OMP_PROC_BIND=spread

            srun -p orca \
                --nodes=1 \
                --ntasks=1 \
                --cpus-per-task=$t \
                time "$BIN"

        } &> "$RESULTS_DIR/orca_${t}"
done

echo "===== EJECUCIONES EN TEEN ====="

# TEEN tiene 16 CPUs logicas, por eso no ejecuto 32
for t in 1 2 4 8 16
do
        echo "Configuracion: $t threads, run $r"

        {
            echo "Programa: paralelo3"
            echo "Maquina: teen"
            echo "Threads: $t"
            echo "Run: $r"
            echo "----------------------"

            export OMP_NUM_THREADS=$t
            export OMP_DYNAMIC=FALSE
            export OMP_PLACES=cores
            export OMP_PROC_BIND=spread

            srun -p teen \
                --nodes=1 \
                --ntasks=1 \
                --cpus-per-task=$t \
                time "$BIN"

        } &> "$RESULTS_DIR/teen_${t}"
done

echo "Ejecuciones completadas"