#!/usr/bin/env bash
#
# Este script se encarga de invocar los tres programas que permiten la 
# conversion de un PNG a secuencia de pixeles, se procesan esos pixeles y
# posteriormente se convierte esa secuencia de pixeles a un archivo en formato
# PNG
#
# Autor: John Sanabria - john.sanabria@correounivalle.edu.co
# Fecha: 2024-08-22
#
#!/usr/bin/env bash
#
# Este script procesa todos los archivos PNG en el directorio actual, aplicando
# el filtro y generando archivos de salida con el sufijo .bin.PNG
#

# Iterar sobre todos los archivos PNG en el directorio actual

for INPUT_PNG in *.png; do
    echo "Procesando $INPUT_PNG..."
    TEMP_FILE="${INPUT_PNG%.png}.bin"
    
    # Convertir de PNG a binario
    python3 fromPNG2Bin.py ${INPUT_PNG}
    
    # Aplicar el filtro con el programa en C
    ./main ${TEMP_FILE}
    
    # Convertir de binario a PNG
    python3 fromBin2PNG.py ${TEMP_FILE}.new
done

echo "Procesamiento completado para todas las imágenes."
