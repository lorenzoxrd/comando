#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // Para stat, S_ISREG, S_ISDIR, etc.
#include <time.h>     // Para localtime, strftime
#include <pwd.h>      // Para getpwuid
#include <grp.h>      // Para getgrgid

// Función para imprimir los permisos de archivo en formato legible (rwxrwxrwx)
// Parámetros: mode (modo de archivo de struct stat)
void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-"); // Tipo de archivo (directorio o regular)
    printf((mode & S_IRUSR) ? "r" : "-"); // Permiso de lectura para el propietario
    printf((mode & S_IWUSR) ? "w" : "-"); // Permiso de escritura para el propietario
    printf((mode & S_IXUSR) ? "x" : "-"); // Permiso de ejecución para el propietario
    printf((mode & S_IRGRP) ? "r" : "-"); // Permiso de lectura para el grupo
    printf((mode & S_IWGRP) ? "w" : "-"); // Permiso de escritura para el grupo
    printf((mode & S_IXGRP) ? "x" : "-"); // Permiso de ejecución para el grupo
    printf((mode & S_IROTH) ? "r" : "-"); // Permiso de lectura para otros
    printf((mode & S_IWOTH) ? "w" : "-"); // Permiso de escritura para otros
    printf((mode & S_IXOTH) ? "x" : "-"); // Permiso de ejecución para otros
}

// Función para mostrar información detallada de un archivo/directorio
// Parámetros: filename (ruta del archivo/directorio)
void print_file_info(const char *filename) {
    struct stat file_stat; // Estructura para almacenar la información del archivo

    // Obtener información del archivo/directorio usando stat()
    if (stat(filename, &file_stat) == -1) {
        perror("Error al obtener información del archivo");
        exit(EXIT_FAILURE);
    }

    printf("--- Información del Archivo: %s ---\n", filename);

    // Tipo de archivo
    printf("Tipo: ");
    if (S_ISREG(file_stat.st_mode)) {
        printf("Archivo Regular\n");
    } else if (S_ISDIR(file_stat.st_mode)) {
        printf("Directorio\n");
    } else if (S_ISLNK(file_stat.st_mode)) {
        printf("Enlace Simbólico\n");
    } else if (S_ISCHR(file_stat.st_mode)) {
        printf("Dispositivo de Caracteres\n");
    } else if (S_ISBLK(file_stat.st_mode)) {
        printf("Dispositivo de Bloques\n");
    } else if (S_ISFIFO(file_stat.st_mode)) {
        printf("FIFO (Pipe Nombrado)\n");
    } else if (S_ISSOCK(file_stat.st_mode)) {
        printf("Socket\n");
    } else {
        printf("Desconocido\n");
    }

    // Permisos
    printf("Permisos: ");
    print_permissions(file_stat.st_mode);
    printf(" (%o)\n", file_stat.st_mode & 0777); // Permisos en octal

    // Propietario y Grupo
    struct passwd *pw = getpwuid(file_stat.st_uid); // Obtener información del propietario por UID
    struct group *gr = getgrgid(file_stat.st_gid); // Obtener información del grupo por GID

    if (pw != NULL) {
        printf("Propietario: %s (UID: %d)\n", pw->pw_name, file_stat.st_uid);
    } else {
        printf("Propietario: UID %d\n", file_stat.st_uid);
    }

    if (gr != NULL) {
        printf("Grupo: %s (GID: %d)\n", gr->gr_name, file_stat.st_gid);
    } else {
        printf("Grupo: GID %d\n", file_stat.st_gid);
    }

    // Tamaño
    printf("Tamaño: %lld bytes\n", (long long)file_stat.st_size); // Usar long long para tamaños grandes

    // Número de Inodo
    printf("Inodo: %lu\n", (unsigned long)file_stat.st_ino);

    // Número de enlaces duros
    printf("Enlaces Duros: %lu\n", (unsigned long)file_stat.st_nlink);

    // Fechas de Acceso, Modificación y Cambio de Estado
    char time_buffer[80];

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_atime));
    printf("Último Acceso: %s\n", time_buffer);

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    printf("Última Modificación: %s\n", time_buffer);

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_ctime));
    printf("Último Cambio de Estado: %s\n", time_buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_o_directorio>\n", argv[0]);
        return EXIT_FAILURE;
    }

    print_file_info(argv[1]); // Llamar a la función para mostrar la información

    return EXIT_SUCCESS;
}
