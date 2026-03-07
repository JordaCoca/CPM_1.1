#!/bin/bash

# comprobar argumento
if [ $# -ne 1 ]; then
    echo "Uso: $0 archivo.c"
    exit 1
fi

SRC=$1
BIN=$(basename "$SRC" .c)

# compilar
echo "Compilando $SRC ..."
cc -O "$SRC" -o "$BIN" -lpthread -lm

if [ $? -ne 0 ]; then
    echo "Error compilando."
    exit 1
fi

# crear carpetas de resultados
mkdir -p resultados_teen
mkdir -p resultados_orca

echo "===== Ejecutando en TEEN ====="

for t in $(seq 2 32); do
    echo "Threads: $t"

    {
        echo "Threads: $t"
        /usr/bin/time -f "Tiempo_total: %e segundos" \
        srun -p teen -c $t ./$BIN
    } &> resultados_teen/teen_$t
done


echo "===== Ejecutando en ORCA ====="

for t in $(seq 2 128); do
    echo "Threads: $t"

    {
        echo "Threads: $t"
        /usr/bin/time -f "Tiempo_total: %e segundos" \
        srun -p orca -c $t ./$BIN
    } &> resultados_orca/orca_$t
done


echo "Ejecuciones completadas."