#ifndef ORDENAMIENTOS_H
#define ORDENAMIENTOS_H

typedef struct {
    long long iteraciones;
    long long consultas;
    long long movimientos;
} Metricas;

typedef struct {
    int valor;
    int palo;
} Carta;

void burbuja(int arr[], int n, Metricas *m);
void burbuja_mejorado(int arr[], int n, Metricas *m);
void radix_sort(int arr[], int n, Metricas *m);
void heap_sort(int arr[], int n, Metricas *m);
void quick_sort(int arr[], int n, Metricas *m);
void merge_sort(int arr[], int n, Metricas *m);
void tim_sort(int arr[], int n, Metricas *m);
void shell_sort(int arr[], int n, Metricas *m);
void counting_sort(int arr[], int n, Metricas *m);
void selection_sort(int arr[], int n, Metricas *m);
void insertion_sort(int arr[], int n, Metricas *m);

void barajar_y_ordenar_cartas();

#endif
