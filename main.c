#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <omp.h>
#include <stdlib.h>
#include <math.h>

// Funciones placeholder para la carga y guardado de imágenes
void cargarImagen(int *imagen, int width, int height);
void guardarImagen(int *imagen, int width, int height);

// Función para aplicar un filtro simple
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height);

// Función para calcular la suma de los píxeles (como una estadística)
int calcularSumaPixeles(int *imagen, int width, int height);

char *filename;

int main(int argc, char* argv[]) {
    int width = 1024, height = 1024;
    int *imagen = (int *)malloc(width * height * sizeof(int));
    int *imagenProcesada = (int *)malloc(width * height * sizeof(int));

    if (argc != 2) {
      fprintf(stderr, "Dar un nombre de archivo de entrada");
      exit(1);
    }

    filename = argv[1];
    // Cargar la imagen (no paralelizable)
    cargarImagen(imagen, width, height);

    // Aplicar filtro (paralelizable)
    aplicarFiltro(imagen, imagenProcesada, width, height);

    // Calcular suma de píxeles (parte paralelizable)
    int sumaPixeles = calcularSumaPixeles(imagenProcesada, width, height);

    printf("Suma de píxeles: %d\n", sumaPixeles);

    // Guardar la imagen (no paralelizable)
    guardarImagen(imagenProcesada, width, height);

    free(imagen);
    free(imagenProcesada);
    return 0;
}

// Carga una imagen desde un archivo binario
void cargarImagen(int *imagen, int width, int height) {
    FILE *archivo = fopen(filename, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar la imagen");
        return;
    }

    size_t elementosLeidos = fread(imagen, sizeof(int), width * height, archivo);
    if (elementosLeidos != width * height) {
        perror("Error al leer la imagen desde el archivo");
    }

    fclose(archivo);
}

// Guarda una imagen en un archivo binario
void guardarImagen(int *imagen, int width, int height) {
    char *output_filename;

    output_filename = (char*)malloc(sizeof(char)*(strlen(filename) + 4));
    sprintf(output_filename,"%s.new",filename);
    FILE *archivo = fopen(output_filename, "wb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para guardar la imagen");
        return;
    }

    size_t elementosEscritos = fwrite(imagen, sizeof(int), width * height, archivo);
    if (elementosEscritos != width * height) {
        perror("Error al escribir la imagen en el archivo");
    }

    fclose(archivo);
}


// Función para aplicar el filtro de Sobel con OpenMP
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    // Definición de las máscaras de Sobel para la detección de bordes
    // Gx y Gy son matrices que se usan para calcular los gradientes en los ejes x e y.
    int Gx[3][3] = {{-1, 0, 1}, 
                    {-2, 0, 2}, 
                    {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, 
                    {0, 0, 0}, 
                    {1, 2, 1}};

    // La directiva #pragma omp parallel for indica a OpenMP que paralelice el bucle
    // Esto significa que cada iteración del bucle externo (por cada fila de la imagen) puede ejecutarse en paralelo.
    #pragma omp parallel for
    for (int y = 1; y < height - 1; y++) { // Recorre las filas de la imagen, excluyendo los bordes
        for (int x = 1; x < width - 1; x++) { // Recorre las columnas, excluyendo los bordes
            int sumX = 0;
            int sumY = 0;

            // Aplicación de las máscaras de Sobel en la vecindad de 3x3 alrededor del píxel (x, y)
            // Aquí se calculan dos sumas: sumX y sumY, que representan los gradientes en las direcciones X e Y.
            for (int ky = -1; ky <= 1; ky++) { // Recorre el vecindario verticalmente
                for (int kx = -1; kx <= 1; kx++) { // Recorre el vecindario horizontalmente
                    // Se multiplica el valor del píxel vecino por el valor correspondiente en las máscaras Gx y Gy
                    // Esto calcula el gradiente en la dirección X y en la dirección Y
                    sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                    sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                }
            }

            // Calcular la magnitud del gradiente combinando sumX y sumY
            // abs() se usa para tomar el valor absoluto de los gradientes
            int magnitude = abs(sumX) + abs(sumY);

            // Normalizar la magnitud para que esté en el rango de 0 a 255 (8 bits)
            // Si el valor calculado es mayor que 255, se ajusta a 255
            imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude;
        }
    }
}

int calcularSumaPixeles(int *imagen, int width, int height) {
    int suma = 0;
    for (int i = 0; i < width * height; i++) {
        suma += imagen[i];
    }
    return suma;
}

