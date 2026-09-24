#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ordenamientos.h"

void intercambiar(int *a, int *b, Metricas *m) {
    int t = *a;
    *a = *b;
    *b = t;
    m->movimientos += 1;
}

void burbuja(int arr[], int n, Metricas *m) {
    for (int i = 0; i < n - 1; i++) {
        m->iteraciones++;
        for (int j = 0; j < n - 1; j++) {
            m->consultas++;
            if (arr[j] > arr[j + 1]) {
                intercambiar(&arr[j], &arr[j + 1], m);
            }
        }
    }
}

void burbuja_mejorado(int arr[], int n, Metricas *m) {
    int intercambiado;
    for (int i = 0; i < n - 1; i++) {
        intercambiado = 0;
        m->iteraciones++;
        for (int j = 0; j < n - i - 1; j++) {
            m->consultas++;
            if (arr[j] > arr[j + 1]) {
                intercambiar(&arr[j], &arr[j + 1], m);
                intercambiado = 1;
            }
        }
        if (!intercambiado) break;
    }
}

void selection_sort(int arr[], int n, Metricas *m) {
    for (int i = 0; i < n - 1; i++) {
        m->iteraciones++;
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            m->consultas++;
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            intercambiar(&arr[i], &arr[min_idx], m);
        }
    }
}

void insertion_sort(int arr[], int n, Metricas *m) {
    for (int i = 1; i < n; i++) {
        m->iteraciones++;
        int key = arr[i];
        int j = i - 1;
        m->movimientos++;
        while (j >= 0) {
            m->consultas++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                m->movimientos++;
                j = j - 1;
            } else break;
        }
        arr[j + 1] = key;
        m->movimientos++;
    }
}

void shell_sort(int arr[], int n, Metricas *m) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        m->iteraciones++;
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            m->movimientos++;
            int j = i;
            while (j >= gap) {
                m->consultas++;
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    m->movimientos++;
                    j = j - gap;
                } else {
                    break;
                }
            }
            arr[j] = temp;
            m->movimientos++;
        }
    }
}
void qs(int arr[], int bajo, int alto, Metricas *m) {
    if (bajo < alto) {
        m->iteraciones++;
        int pivote = arr[alto];
        int i = (bajo - 1);
        for (int j = bajo; j <= alto - 1; j++) {
            m->consultas++;
            if (arr[j] < pivote) {
                i++;
                intercambiar(&arr[i], &arr[j], m);
            }
        }
        intercambiar(&arr[i + 1], &arr[alto], m);
        int pi = i + 1;
        qs(arr, bajo, pi - 1, m);
        qs(arr, pi + 1, alto, m);
    }
}
void quick_sort(int arr[], int n, Metricas *m) { qs(arr, 0, n - 1, m); }

void merge(int arr[], int l, int med, int r, Metricas *m) {
    int n1 = med - l + 1, n2 = r - med;
    int L[15], R[15];
    for (int i = 0; i < n1; i++) { L[i] = arr[l + i]; m->movimientos++; }
    for (int j = 0; j < n2; j++) { R[j] = arr[med + 1 + j]; m->movimientos++; }

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        m->consultas++;
        if (L[i] <= R[j]) arr[k] = L[i++];
        else arr[k] = R[j++];
        m->movimientos++; k++;
    }
    while (i < n1) { arr[k++] = L[i++]; m->movimientos++; }
    while (j < n2) { arr[k++] = R[j++]; m->movimientos++; }
}
void ms(int arr[], int l, int r, Metricas *m) {
    if (l < r) {
        m->iteraciones++;
        int med = l + (r - l) / 2;
        ms(arr, l, med, m);
        ms(arr, med + 1, r, m);
        merge(arr, l, med, r, m);
    }
}
void merge_sort(int arr[], int n, Metricas *m) { ms(arr, 0, n - 1, m); }

void heapify(int arr[], int n, int i, Metricas *m) {
    int mayor = i, izq = 2 * i + 1, der = 2 * i + 2;
    if (izq < n) { m->consultas++; if(arr[izq] > arr[mayor]) mayor = izq; }
    if (der < n) { m->consultas++; if(arr[der] > arr[mayor]) mayor = der; }
    if (mayor != i) {
        intercambiar(&arr[i], &arr[mayor], m);
        heapify(arr, n, mayor, m);
    }
}
void heap_sort(int arr[], int n, Metricas *m) {
    for (int i = n / 2 - 1; i >= 0; i--) { m->iteraciones++; heapify(arr, n, i, m); }
    for (int i = n - 1; i > 0; i--) {
        m->iteraciones++;
        intercambiar(&arr[0], &arr[i], m);
        heapify(arr, i, 0, m);
    }
}

void counting_sort(int arr[], int n, Metricas *m) {
    int max = arr[0];
    int min = arr[0];
    for (int i = 1; i < n; i++) {
        m->consultas++;
        if (arr[i] > max) max = arr[i];
        m->consultas++;
        if (arr[i] < min) min = arr[i];
    }

    int rango = max - min + 1;
    int *count = (int *)calloc(rango, sizeof(int));
    int out[15];

    m->iteraciones++;
    for (int i = 0; i < n; i++) {
        count[arr[i] - min]++;
        m->movimientos++;
    }

    m->iteraciones++;
    for (int i = 1; i < rango; i++) {
        count[i] += count[i - 1];
    }

    m->iteraciones++;
    for (int i = n - 1; i >= 0; i--) {
        out[count[arr[i] - min] - 1] = arr[i];
        count[arr[i] - min]--;
        m->movimientos++;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = out[i];
        m->movimientos++;
    }
    free(count);
}

void countSortRadix(int arr[], int n, long long exp, Metricas *m) {
    int out[15];
    int count[10] = {0};

    m->iteraciones++;
    for (int i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
        m->movimientos++;
    }

    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; i--) {
        out[count[(arr[i] / exp) % 10] - 1] = arr[i];
        count[(arr[i] / exp) % 10]--;
        m->movimientos++;
    }

    for (int i = 0; i < n; i++) {
        arr[i] = out[i];
        m->movimientos++;
    }
}

void radix_sort(int arr[], int n, Metricas *m) {
    int min = arr[0];
    for (int i = 1; i < n; i++) {
        m->consultas++;
        if (arr[i] < min) min = arr[i];
    }

    if (min < 0) {
        for (int i = 0; i < n; i++) {
            arr[i] -= min;
            m->movimientos++;
        }
    }

    int mx = arr[0];
    for (int i = 1; i < n; i++) {
        m->consultas++;
        if (arr[i] > mx) mx = arr[i];
    }

    for (long long exp = 1; mx / exp > 0; exp *= 10) {
        countSortRadix(arr, n, exp, m);
    }

    if (min < 0) {
        for (int i = 0; i < n; i++) {
            arr[i] += min;
            m->movimientos++;
        }
    }
}

void tim_sort(int arr[], int n, Metricas *m) {
    int RUN = 4;
    for (int i = 0; i < n; i += RUN) {
        m->iteraciones++;
        int fin = (i + RUN - 1 < n - 1) ? (i + RUN - 1) : (n - 1);
        for (int x = i + 1; x <= fin; x++) {
            int key = arr[x], y = x - 1;
            while (y >= i) {
                m->consultas++;
                if (arr[y] > key) {
                    arr[y + 1] = arr[y];
                    m->movimientos++;
                    y--;
                } else break;
            }
            arr[y + 1] = key;
            m->movimientos++;
        }
    }
    for (int size = RUN; size < n; size = 2 * size) {
        for (int izq = 0; izq < n; izq += 2 * size) {
            m->iteraciones++;
            int med = izq + size - 1;
            int der = ((izq + 2 * size - 1) < (n - 1)) ? (izq + 2 * size - 1) : (n - 1);
            if (med < der) merge(arr, izq, med, der, m);
        }
    }
}

void barajar_y_ordenar_cartas() {
    Carta cartas[52];
    int num_cartas = 0;
    int opcion;

    printf("\n--- BARAJA DE CARTAS ---\n");
    printf("1. Ingresar cartas a mi antojo\n");
    printf("2. Generar baraja aleatoria completa (52 cartas)\n");
    printf("Opcion: ");
    scanf("%d", &opcion);

    const char *nombresValor[] = {"As", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King"};
    const char *nombresPalo[] = {"Picas", "Treboles", "Corazones", "Diamantes"};

    if (opcion == 1) {
        printf("\nCuantas cartas deseas agregar al arreglo? (Max 52): ");
        scanf("%d", &num_cartas);
        if (num_cartas > 52) num_cartas = 52;

        for (int i = 0; i < num_cartas; i++) {
            int v, p;
            printf("\n--- Carta %d ---\n", i + 1);
            do {
                printf("Valor (1=As, 2 al 10, 11=J, 12=Q, 13=K): ");
                scanf("%d", &v);
            } while(v < 1 || v > 13);
            do {
                printf("Palo (1=Picas, 2=Treboles, 3=Corazones, 4=Diamantes): ");
                scanf("%d", &p);
            } while(p < 1 || p > 4);

            cartas[i].valor = v;
            cartas[i].palo = p;
        }
    } else {
        num_cartas = 52;
        int indice = 0;
        for(int palo = 1; palo <= 4; palo++) {
            for(int valor = 1; valor <= 13; valor++) {
                cartas[indice].valor = valor;
                cartas[indice].palo = palo;
                indice++;
            }
        }
        srand(time(NULL));
        for(int i = 0; i < 52; i++) {
            int r = rand() % 52;
            Carta temp = cartas[i];
            cartas[i] = cartas[r];
            cartas[r] = temp;
        }
    }

    printf("\n¿¿¿¿¿????? BARAJA DESORDENADA ¿¿¿¿¿????\n");
    int limite = (num_cartas > 15) ? 15 : num_cartas;
    for(int i = 0; i < limite; i++) {
        printf("Carta %d: %s de %s\n", i + 1, nombresValor[cartas[i].valor - 1], nombresPalo[cartas[i].palo - 1]);
    }
    if (num_cartas > 15) printf("... (%d cartas en el arreglo)\n", num_cartas);

    for(int i = 0; i < num_cartas - 1; i++) {
        for(int j = 0; j < num_cartas - 1 - i; j++) {
            if(cartas[j].valor > cartas[j + 1].valor ||
              (cartas[j].valor == cartas[j + 1].valor && cartas[j].palo > cartas[j + 1].palo)) {
                Carta temp = cartas[j];
                cartas[j] = cartas[j + 1];
                cartas[j + 1] = temp;
            }
        }
    }

    printf("\n!!!!!! BARAJA ORDENADA !!!!!!\n");
    for(int i = 0; i < limite; i++) {
        printf("Carta %d: %s de %s\n", i + 1, nombresValor[cartas[i].valor - 1], nombresPalo[cartas[i].palo - 1]);
    }
    if (num_cartas > 15) printf("... (%d cartas en el arreglo)\n", num_cartas);
}
