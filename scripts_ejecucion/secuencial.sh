#!/bin/bash

SRC="./kmeanCPM.c"
RESULTS_DIR="./res_secuenciales"
BIN="./sec"


echo "Compilando programa..."
cc -O3  $SRC -o $BIN

if [ $? -ne 0 ]; then
    echo "Error en compilacion"
    exit 1
fi
mkdir -p $RESULTS_DIR

echo "===== EJECUCION  SECUENCIAL EN ORCA ====="
srun -p orca -c 1 time $BIN &> $RESULTS_DIR/orca_sec
echo "===== EJECUCION  SECUENCIAL EN TEEN ====="
srun -p teen -c 1 time $BIN &> $RESULTS_DIR/teen_sec


echo "Ejecuciones completadas"