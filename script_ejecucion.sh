#!/bin/bash

# comprobar argumento
if [ $# -ne 1 ]; then
    echo "Uso: $0 archivo.c"
    exit 1
fi

SRC=$1
NAME=$(basename "$SRC" .c)
BIN="paralelo/codigo/$NAME"

# compilar
echo "Compilando $SRC ..."
cc -O "$SRC" -o "$BIN" -lpthread -lm

if [ $? -ne 0 ]; then
    echo "Error compilando."
    exit 1
fi

echo "===== Ejecutando en TEEN ====="

for t in 2 4 8 16 32; do
    echo "Threads: $t"

    {
        echo "Programa: $NAME"
        echo "Threads: $t"
        echo "Maquina: teen"
        echo "----------------------"

        /usr/bin/time -f "Tiempo_total: %e segundos" \
        srun -p teen -c $t $BIN

    } &> paralelo/resultados_teen/teen_${NAME}_$t
done


echo "===== Ejecutando en ORCA ====="

for t in 2 4 8 16 32 64 128; do
    echo "Threads: $t"

    {
        echo "Programa: $NAME"
        echo "Threads: $t"
        echo "Maquina: orca"
        echo "----------------------"

        /usr/bin/time -f "Tiempo_total: %e segundos" \
        srun -p orca -c $t $BIN

    } &> paralelo/resultados_orca/orca_${NAME}_$t
done


echo "Ejecuciones completadas."