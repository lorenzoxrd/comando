#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h> // Para PATH_MAX

// Función recursiva para imprimir la estructura de directorios en forma de árbol
// Parámetros: dirname (directorio actual), indent (nivel de indentación)
void print_directory_tree(const char *dirname, int indent) {
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

        // Imprimir la indentación
        for (int i = 0; i < indent; i++) {
            printf("  "); // Dos espacios por nivel
        }
        printf("|-- %s\n", entry->d_name); // Prefijo de árbol y nombre de entrada

        // Construir la ruta completa del archivo/directorio
        snprintf(full_path, sizeof(full_path), "%s/%s", dirname, entry->d_name);

        // Obtener información del archivo/directorio
        if (stat(full_path, &file_stat) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }

        // Si es un directorio, llamar recursivamente con un nivel de indentación mayor
        if (S_ISDIR(file_stat.st_mode)) {
            print_directory_tree(full_path, indent + 1);
        }
    }
    closedir(dir); // Cerrar directorio
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("%s\n", argv[1]); // Imprimir el directorio raíz
    print_directory_tree(argv[1], 0); // Iniciar la impresión del árbol desde el nivel 0

    return EXIT_SUCCESS;
}
