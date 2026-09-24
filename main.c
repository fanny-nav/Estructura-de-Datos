#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "ordenamientos.h"

#define MAX_USERS 8

char usuarios[MAX_USERS][8];
char contrasenas[MAX_USERS][8];
int num_usuarios = 0;

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int leer_cadena(char *buffer, int max_size) {
    char temp[1024];
    if (fgets(temp, sizeof(temp), stdin) != NULL) {
        int len = strcspn(temp, "\r\n");
        temp[len] = '\0';
        if (len == 1023 && temp[1022] != '\n') {
            limpiar_buffer();
        }
        strcpy(buffer, temp);
        return len;
    }
    return 0;
}

void rellenar_arreglo(char *origen, char destino[8], int len) {
    for (int i = 0; i < 8; i++) {
        if (i < len) destino[i] = origen[i];
        else destino[i] = ' ';
    }
}

void guardar_archivos() {
    FILE *fu = fopen("usuarios.txt", "w");
    FILE *fc = fopen("contrasenas.txt", "w");
    if (!fu || !fc) return;

    for (int i = 0; i < num_usuarios; i++) {
        fprintf(fu, "[%c,%c,%c,%c,%c,%c,%c,%c]\n",
                usuarios[i][0], usuarios[i][1], usuarios[i][2], usuarios[i][3],
                usuarios[i][4], usuarios[i][5], usuarios[i][6], usuarios[i][7]);

        fprintf(fc, "[%c,%c,%c,%c,%c,%c,%c,%c]\n",
                contrasenas[i][0], contrasenas[i][1], contrasenas[i][2], contrasenas[i][3],
                contrasenas[i][4], contrasenas[i][5], contrasenas[i][6], contrasenas[i][7]);
    }
    fclose(fu);
    fclose(fc);
}

void cargar_archivos() {
    FILE *fu = fopen("usuarios.txt", "r");
    FILE *fc = fopen("contrasenas.txt", "r");

    if (fu != NULL && fc != NULL) {
        char lineaU[50], lineaC[50];
        num_usuarios = 0;
        while (fgets(lineaU, sizeof(lineaU), fu) && fgets(lineaC, sizeof(lineaC), fc)) {
            if (num_usuarios >= MAX_USERS) break;
            int colU = 0, colC = 0;
            for (int i = 0; lineaU[i] != '\0' && lineaU[i] != '\n' && lineaU[i] != '\r'; i++) {
                if (lineaU[i] != '[' && lineaU[i] != ']' && lineaU[i] != ',') {
                    if (colU < 8) usuarios[num_usuarios][colU++] = lineaU[i];
                }
            }
            for (int i = 0; lineaC[i] != '\0' && lineaC[i] != '\n' && lineaC[i] != '\r'; i++) {
                if (lineaC[i] != '[' && lineaC[i] != ']' && lineaC[i] != ',') {
                    if (colC < 8) contrasenas[num_usuarios][colC++] = lineaC[i];
                }
            }
            num_usuarios++;
        }
        fclose(fu);
        fclose(fc);
    } else {
        num_usuarios = 1;
        char def_u[8] = {'H','A','R','O',' ',' ',' ',' '};
        char def_c[8] = {'1','2','3','4','5','6','7','8'};
        for (int i = 0; i < 8; i++) {
            usuarios[0][i] = def_u[i];
            contrasenas[0][i] = def_c[i];
        }
        guardar_archivos();
    }
}

void agregar_usuario() {
    if (num_usuarios >= MAX_USERS) {
        printf("\nError: Limite de %d usuarios alcanzado.\n", MAX_USERS);
        return;
    }

    char inputUser[1024], inputPass[1024];

    printf("\n<==> NUEVO USUARIO <==>\n");
    printf("Nombre de usuario (1 a 8 caracteres): ");
    int lenU = leer_cadena(inputUser, sizeof(inputUser));
    if (lenU < 1 || lenU > 8) {
        printf("Error: El usuario debe tener entre 1 y 8 caracteres. OnO\n");
        return;
    }

    printf("Contrasena (EXACTAMENTE 8 caracteres): ");
    int lenC = leer_cadena(inputPass, sizeof(inputPass));
    if (lenC != 8) {
        printf("Error: La contrasena DEBE tener exactamente 8 caracteres. (-n-)\n");
        return;
    }

    rellenar_arreglo(inputUser, usuarios[num_usuarios], lenU);
    rellenar_arreglo(inputPass, contrasenas[num_usuarios], lenC);
    num_usuarios++;
    guardar_archivos();
    printf("Usuario registrado.(-U-)\n");
}

int iniciar_sesion() {
    char inputUser[1024], inputPass[1024];
    char formatoU[8], formatoC[8];

    printf("\n INICIAR SESION (owo)\n");
    printf("Usuario: ");
    int lenU = leer_cadena(inputUser, sizeof(inputUser));
    printf("Contrasena: ");
    int lenC = leer_cadena(inputPass, sizeof(inputPass));

    if (lenU < 1 || lenU > 8 || lenC != 8) {
        printf("Error:invalidas, try again. (*n*)\n");
        return 0;
    }

    rellenar_arreglo(inputUser, formatoU, lenU);
    rellenar_arreglo(inputPass, formatoC, lenC);

    for (int i = 0; i < num_usuarios; i++) {
        int matchU = 1, matchC = 1;
        for (int j = 0; j < 8; j++) {
            if (usuarios[i][j] != formatoU[j]) matchU = 0;
            if (contrasenas[i][j] != formatoC[j]) matchC = 0;
        }
        if (matchU && matchC) return 1;
    }
    printf("Error:invalido sorry\n");
    return 0;
}

void generador_calculadora() {
    int numero_usuario;
    int numero_generado;
    float arreglo_resultados[4];

    printf("\n^w^ GENERADOR Y CALCULADORA ^w^\n");
    do {
        printf("Ingresa un numero del 1 al 100: ");
        if (scanf("%d", &numero_usuario) != 1) {
            limpiar_buffer();
            numero_usuario = -1;
        }
    } while(numero_usuario < 1 || numero_usuario > 100);

    numero_generado = (rand() % 100) + 1;
    printf("El numero generado por el sistema es: %d\n", numero_generado);

    arreglo_resultados[0] = numero_usuario + numero_generado;
    arreglo_resultados[1] = numero_usuario - numero_generado;
    arreglo_resultados[2] = numero_usuario * numero_generado;
    arreglo_resultados[3] = (float)numero_usuario / numero_generado;

    printf("\nResultados guardados en el arreglo:\n");
    printf("[0] Suma: %.2f\n", arreglo_resultados[0]);
    printf("[1] Resta: %.2f\n", arreglo_resultados[1]);
    printf("[2] Multiplicacion: %.2f\n", arreglo_resultados[2]);
    printf("[3] Division: %.2f\n\n", arreglo_resultados[3]);
}

void investigacion_almacenamiento() {
    printf("\n||===|| INVESTIGACION: ALMACENAMIENTO SECUNDARIO ||===||\n");
    printf("¿Que es?\n");
    printf("Es el conjunto de dispositivos y medios de almacenamiento de una computadora que retienen\n");
    printf("los datos de manera no volatil (permanente), incluso cuando el equipo se apaga.\n");
    printf("A diferencia de la memoria RAM (almacenamiento primario), es mas lento pero tiene\n");
    printf("mucha mayor capacidad.\n\n");

    printf("Tipos y Ejemplos:\n");
    printf("1. Magnetico: Utiliza superficies magnetizadas para guardar datos.\n");
    printf("   - Ejemplos: Discos Duros (HDD), Cintas magneticas.\n");
    printf("2. Estado Solido (Flash): No tiene partes moviles, utiliza circuitos electronicos.\n");
    printf("   - Ejemplos: Unidades de Estado Solido (SSD), Memorias USB, Tarjetas SD.\n");
    printf("3. Optico: Utiliza lasers para leer y escribir datos en discos.\n");
    printf("   - Ejemplos: CD, DVD, Blu-ray.\n");
    printf("4. En la Nube: Almacenamiento remoto a traves de internet.\n");
    printf("   - Ejemplos: Google Drive, OneDrive, AWS.\n\n");
}

void investigacion_tipos() {
    printf("\n||===|| INVESTIGACION: TIPOS DE DATOS EN C ||===||\n");
    printf("Los tipos de datos determinan el tipo de valor que una variable puede almacenar,\n");
    printf("asi como la cantidad de memoria que ocupara.\n\n");

    printf("Tipos Basicos:\n");
    printf("1. int (Entero)\n");
    printf("   - Almacena: Numeros enteros sin decimales.\n");
    printf("   - Memoria: Generalmente 4 bytes.\n");
    printf("   - Rango (aprox): -2,147,483,648 a 2,147,483,647\n");
    printf("   - Ejemplo: int edad = 20;\n\n");

    printf("2. float\n");
    printf("   - Almacena: Numeros con decimales (precision simple).\n");
    printf("   - Memoria: 4 bytes.\n");
    printf("   - Precisión: Hasta 6-7 digitos decimales.\n");
    printf("   - Ejemplo: float peso = 65.5;\n\n");

    printf("3. double\n");
    printf("   - Almacena: Numeros con decimales pero con mayor precision que float.\n");
    printf("   - Memoria: 8 bytes.\n");
    printf("   - Precision: Hasta 15 digitos decimales.\n");
    printf("   - Ejemplo: double pi = 3.14159265359;\n\n");

    printf("4. char\n");
    printf("   - Almacena: Un solo caracter (letra, numero o simbolo) usando codigo ASCII.\n");
    printf("   - Memoria: 1 byte.\n");
    printf("   - Ejemplo: char calificacion = 'A';\n\n");

    printf("5. void\n");
    printf("   - Representa la ausencia de un tipo. Se usa principalmente en funciones\n");
    printf("     para indicar que no devuelven ningun valor.\n\n");
}

void ejecutar_ordenamiento(int opcion) {
    if (opcion == 12) { barajar_y_ordenar_cartas(); return; }

int arr[15];
    int n = 0;

    do {
        printf("\n¿Cuantos numeros deseas ingresar? (Minimo 2, Maximo 15): ");
        if (scanf("%d", &n) != 1) limpiar_buffer();
    } while (n < 2 || n > 15);

    printf("\nIngresa los numeros:\n");
    for (int i = 0; i < n; i++) {
        printf("Numero %d: ", i + 1);
        scanf("%d", &arr[i]);
    }

    int listo = 0;
    while (!listo) {
        printf("\n ARREGLO SIN ACOMODAR \n[");
        for (int i = 0; i < n; i++) printf("%d%s", arr[i], i == n - 1 ? "" : ", ");
        printf("]\n");

        printf("\n1. Iniciar el ordenamiento elegido\n2. Corregir algun numero de la lista\nOpcion: ");
        int resp;
        if (scanf("%d", &resp) != 1) { limpiar_buffer(); continue; }

        if (resp == 1) { listo = 1; }
        else if (resp == 2) {
            int idx, nuevo_num;
            printf("Posicion a corregir (Del 1 al %d): ", n);
            scanf("%d", &idx);
            if (idx >= 1 && idx <= n) {
                printf("Nuevo numero: ");
                scanf("%d", &nuevo_num);
                arr[idx - 1] = nuevo_num;
            }
        }
    }

    Metricas m = {0, 0, 0};

    switch(opcion) {
        case 1: burbuja(arr, n, &m); break;
        case 2: burbuja_mejorado(arr, n, &m); break;
        case 3: radix_sort(arr, n, &m); break;
        case 4: heap_sort(arr, n, &m); break;
        case 5: quick_sort(arr, n, &m); break;
        case 6: merge_sort(arr, n, &m); break;
        case 7: tim_sort(arr, n, &m); break;
        case 8: shell_sort(arr, n, &m); break;
        case 9: counting_sort(arr, n, &m); break;
        case 10: selection_sort(arr, n, &m); break;
        case 11: insertion_sort(arr, n, &m); break;
    }

    printf("\n|--| LISTA ORDENADA |--|\n[");
    for (int i = 0; i < n; i++) printf("%d%s", arr[i], i == n - 1 ? "" : ", ");
    printf("]\n");
    printf("\n- Total Iteraciones: %lld\n- Total Consultas: %lld\n- Total Movimientos: %lld\n", m.iteraciones, m.consultas, m.movimientos);
}

void menu_ordenamientos() {
    int opcion;
    do {
        printf("\n==||         MENU      ||==\n");
        printf("1. Burbuja           8. Shell Sort\n");
        printf("2. Burbuja Mejorado  9. Counting Sort\n");
        printf("3. Radix Sort        10. Selection Sort\n");
        printf("4. Heap Sort         11. Insertion Sort\n");
        printf("5. Quick Sort        12. Baraja de Cartas\n");
        printf("6. Merge Sort        13. Generador y Calculadora\n");
        printf("7. Tim Sort          14. Almacenamiento Secundario\n");
        printf("0. Cerrar Sesion     15. Tipos de Datos en C\n");
        printf("Elige: ");

        if (scanf("%d", &opcion) != 1) { limpiar_buffer(); opcion = -1; }

        if (opcion >= 1 && opcion <= 12) {
            ejecutar_ordenamiento(opcion);
        } else if (opcion == 13) {
            generador_calculadora();
        } else if (opcion == 14) {
            investigacion_almacenamiento();
        } else if (opcion == 15) {
            investigacion_tipos();
        }
    } while (opcion != 0);
    limpiar_buffer();
}

int main() {
    setlocale(LC_ALL, "spanish");
    srand(time(NULL));
#ifdef _WIN32
#endif

    cargar_archivos();

    int opcion;
    char entrada[10];

    do {
        printf("\n^^== INICIO ==^^\n");
        printf("1. Iniciar Sesion\n2. Agregar Usuario\n0. Salir\nOpcion: ");

        leer_cadena(entrada, sizeof(entrada));
        opcion = atoi(entrada);

        if (entrada[0] == '1') {
            if (iniciar_sesion()) { menu_ordenamientos(); }
        } else if (entrada[0] == '2') {
            agregar_usuario();
        }
    } while (entrada[0] != '0');

    return 0;
}
