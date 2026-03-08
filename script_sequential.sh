#!/bin/bash

CODIGO_DIR="secuencial/codigo"
RESULT_DIR="secuencial/resultados"

mkdir -p "$RESULT_DIR"

echo "Compilando programas secuenciales..."

for SRC in $CODIGO_DIR/*.c; do

    NAME=$(basename "$SRC" .c)
    BIN="$CODIGO_DIR/$NAME"

    echo "Compilando $SRC"
    cc -O "$SRC" -o "$BIN" -lm -lpthread

    if [ $? -ne 0 ]; then
        echo "Error compilando $SRC"
        continue
    fi

    echo "Ejecutando $NAME en TEEN"

    {
        echo "Programa: $NAME"
        echo "Máquina: teen"
        echo "----------------"
        /usr/bin/time -f "Tiempo_total: %e segundos" \
        srun -p teen -c 1 ./$BIN
    } &> "$RESULT_DIR/teen_${NAME}"



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