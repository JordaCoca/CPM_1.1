#!/bin/bash

SRC="./propuesta/paralelo4.c"
RESULTS_DIR="./resultados_wsl_2"
BIN="./paralelo4_wsl"

echo "Compilando programa..."
gcc -O3 -fopenmp "$SRC" -o "$BIN"

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi

mkdir -p "$RESULTS_DIR"

echo "===== INFORMACION CPU WSL ====="
lscpu &> "$RESULTS_DIR/info_cpu_wsl.txt"

echo "===== EJECUCIONES EN WSL ====="

for t in 1 2 4 8 16 28
do
    echo "Configuracion: $t threads"

    {
        echo "Programa: paralelo4"
        echo "Maquina: WSL"
        echo "CPU: Intel i7-14700KF"
        echo "Threads: $t"
        echo "----------------------"

        export OMP_NUM_THREADS=$t
        export OMP_DYNAMIC=FALSE
        export OMP_PLACES=cores
        export OMP_PROC_BIND=close

        /usr/bin/time -p "$BIN"

    } &> "$RESULTS_DIR/wsl_${t}"
done

echo "Ejecuciones completadas"