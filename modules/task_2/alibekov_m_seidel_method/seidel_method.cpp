// Copyright 2020 Alibekov Murad
#include <mpi.h>
#include "../../../modules/task_2/alibekov_m_seidel_method/seidel_method.h"

void sequential_seidel_method(double* A, double* x, double* b, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++)
            sum += A[i * size + j] * x[j];
        for (int j = i + 1; j < size; j++)
            sum += A[i * size + j] * x[j];
        x[i] = (b[i] - sum) / A[i * size + i];
    }
}
