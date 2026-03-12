#!/bin/bash

CODIGO_SEQ="secuencial/codigo/sequential.c"
RESULT_DIR="secuencial/resultados"

mkdir -p "$RESULT_DIR"

echo "Usando solo orca..."

NAME=$(basename "$SRC" .c)
BIN="$CODIGO_DIR/$NAME"

echo "Compilando $SRC"
cc -O "$SRC" -o "$BIN" -lm -lpthread

if [ $? -ne 0 ]; then
    echo "Error compilando $SRC"
    continue
fi

echo "Ejecutando $NAME en ORCA"
{
    echo "Programa: $NAME"
    echo "Máquina: orca"
    echo "----------------"
    /usr/bin/time -f "Tiempo_total: %e segundos" \
    srun -p orca -c 1 ./$BIN
} &> "$RESULT_DIR/orca_${NAME}"

done
echo "Ejecución secuencial completada."