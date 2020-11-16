// Copyright 2020 Alibekov Murad
#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_2/alibekov_m_seidel_method/seidel_method.h"

double parallel_dot_product(std::vector<double> x, std::vector<double> y) {
    int proc_count, proc_rank, n = x.size();
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n = n / proc_count + n % proc_count > proc_rank ? 1 : 0;
    
    std::vector<double> a(n);
    std::vector<double> b(n);
    
    MPI_Scatter(&x[0], n, MPI_DOUBLE, a, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(&y[0], n, MPI_DOUBLE, b, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    int sum = 0, sum_all;
    for (int i = 0; i < n; i++)
        sum += a[i] * b[i];
    
    MPI_Reduce(&sum, &sum_all, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    return sum_all;
}

double d(std::vector<double> x, std::vector<double> y) {
    double max_dist = 0;
    int size = x.size();
    for (int i = 0; i < size; i++)
        if (std::abs(x[i] - y[i]) > max_dist) max_dist = std::abs(x[i] - y[i]);
    return max_dist;
}

std::vector<double> solving_SLAE_sequential(std::vector<std::vector<double> > A,
                                            std::vector<double> b) {
    int max_count = 10;
    std::vector<double> x_pred(A.size());
    std::vector<double> x(A.size());
    int iter_number = 0;
    do {
        x_pred = x;
        for (int i = 0; i < A.size(); i++) {
            double sum = 0;
            for (int j = 0; j < i; j++)
                sum += A[i * x.size() + j] * x[j];
            for (int j = i + 1; j < x.size(); j++)
                sum += A[i * x.size() + j] * x[j];
            x[i] = (b[i] - sum) / A[i * A.size() + i];
        }
        iter_number++;
    } while ((d(x - x_pred) >= epsilon) && (iter_number < max_count));
    return x;
}

std::vector<double> solving_SLAE_parallel(std::vector<std::vector<double> > A,
                                          std::vector<double> b) {
    int max_count = 10;
    std::vector<double> x_pred(A.size());
    std::vector<double> x(A.size());
    int iter_number = 0;
    do {
        x_pred = x;
        for (int i = 0; i < A.size(); i++)
            x[i] = (b[i] - parallel_dot_product(A[i], x) + x[i] * A[i][i]) / A[i][i];
        iter_number++;
    } while ((d(x - x_pred) >= epsilon) && (iter_number < max_count));
    return x;
}
