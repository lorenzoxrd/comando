#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>      // Para PATH_MAX

#define BUFFER_SIZE 4096 // Tamaño del buffer para lectura de archivos

// Estructura para almacenar información de archivo
typedef struct {
    char filepath[PATH_MAX];
    long long size; // Tamaño del archivo para una primera comparación rápida
} FileInfo;

// Array dinámico para almacenar la información de los archivos
FileInfo *all_files = NULL;
int file_count = 0;
int file_capacity = 0;

// Función para comparar el contenido de dos archivos
// Retorna 0 si son idénticos, -1 si son diferentes, -2 si hay error
int compare_files_content(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");

    if (fp1 == NULL || fp2 == NULL) {
        perror("Error al abrir archivos para comparación de contenido");
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return -2;
    }

    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];
    size_t bytes_read1, bytes_read2;

    do {
        bytes_read1 = fread(buffer1, 1, BUFFER_SIZE, fp1);
        bytes_read2 = fread(buffer2, 1, BUFFER_SIZE, fp2);

        if (bytes_read1 != bytes_read2 || memcmp(buffer1, buffer2, bytes_read1) != 0) {
            fclose(fp1);
            fclose(fp2);
            return -1; // Contenido diferente
        }
    } while (bytes_read1 > 0);

    fclose(fp1);
    fclose(fp2);
    return 0; // Contenido idéntico
}

// Función recursiva para buscar archivos
// Parámetros: dirname (directorio actual)
void find_all_files(const char *dirname) {
    DIR *dir = opendir(dirname); // Abrir directorio
    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    struct dirent *entry;
    char full_path[PATH_MAX];
    struct stat file_stat;

    // Iterar sobre cada entrada en el directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar directorios "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa del archivo/directorio
        snprintf(full_path, sizeof(full_path), "%s/%s", dirname, entry->d_name);

        // Obtener información del archivo/directorio
        if (stat(full_path, &file_stat) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }

        // Si es un archivo regular
        if (S_ISREG(file_stat.st_mode)) {
            // Expandir el array si es necesario
            if (file_count == file_capacity) {
                file_capacity = (file_capacity == 0) ? 10 : file_capacity * 2;
                all_files = realloc(all_files, file_capacity * sizeof(FileInfo));
                if (all_files == NULL) {
                    perror("Error de realloc");
                    exit(EXIT_FAILURE);
                }
            }
            // Almacenar información del archivo
            strncpy(all_files[file_count].filepath, full_path, PATH_MAX - 1);
            all_files[file_count].filepath[PATH_MAX - 1] = '\0'; // Asegurar terminación nula
            all_files[file_count].size = file_stat.st_size;
            file_count++;
        }
        // Si es un directorio, llamar recursivamente
        else if (S_ISDIR(file_stat.st_mode)) {
            find_all_files(full_path);
        }
    }
    closedir(dir); // Cerrar directorio
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Iniciar la búsqueda de archivos
    find_all_files(argv[1]);

    // Comparar archivos para encontrar duplicados
    if (file_count > 0) {
        printf("Buscando archivos duplicados en '%s'...\n", argv[1]);
        int duplicates_found = 0;
        for (int i = 0; i < file_count; i++) {
            for (int j = i + 1; j < file_count; j++) {
                // Primera comparación rápida por tamaño
                if (all_files[i].size == all_files[j].size) {
                    // Si los tamaños coinciden, comparar el contenido
                    int cmp_result = compare_files_content(all_files[i].filepath, all_files[j].filepath);
                    if (cmp_result == 0) {
                        printf("Archivos duplicados encontrados:\n");
                        printf("  - %s\n", all_files[i].filepath);
                        printf("  - %s\n", all_files[j].filepath);
                        printf("\n");
                        duplicates_found = 1;
                    } else if (cmp_result == -2) {
                        fprintf(stderr, "Error al comparar %s y %s\n", all_files[i].filepath, all_files[j].filepath);
                    }
                }
            }
        }
        if (!duplicates_found) {
            printf("No se encontraron archivos duplicados.\n");
        }
    } else {
        printf("No se encontraron archivos en el directorio especificado.\n");
    }

    free(all_files); // Liberar memoria asignada dinámicamente
    return EXIT_SUCCESS;
}
