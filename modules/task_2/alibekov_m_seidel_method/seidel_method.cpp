// Copyright 2020 Alibekov Murad
#include <mpi.h>
#include "iostream"
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "../../../modules/task_2/alibekov_m_seidel_method/seidel_method.h"

double epsilon = 0.01;
int max_count = 10;

std::vector<double> generate_A(int size) {
    std::cout << "Hello!";
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<double> A(size * size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            A[i * size + j] = (i == j) ? 
                std::abs(double(static_cast<int>(gen()) % 100)) + 100. * double(size - 1):
                double(static_cast<int>(gen()) % 100);
    return A;
}

std::vector<double> generate_b(int size) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)) + 7);
    std::vector<double> b(size);
    for (int i = 0; i < size; i++)
        b[i] = double(static_cast<int>(gen()) % 100);
    return b;
}

double parallel_dot_product(std::vector<double> x, std::vector<double> y) {
    int proc_count, proc_rank, n = x.size();
    
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n = n / proc_count + n % proc_count > proc_rank ? 1 : 0;
    
    double sum = 0.;
    
    if (n > 0) {
        std::vector<double> a(n);
        std::vector<double> b(n);
    
        MPI_Scatter(&x[0], n, MPI_DOUBLE, &a[0], n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(&y[0], n, MPI_DOUBLE, &b[0], n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
        for (int i = 0; i < n; i++)
            sum += a[i] * b[i];
    }
    
    double sum_all = 0.;
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

std::vector<double> solving_SLAE_sequential(std::vector<double> A, std::vector<double> b, int size) {
    int max_count = 10;
    std::vector<double> x_pred(size);
    std::vector<double> x(size);
    int iter_number = 0;
    double dist = epsilon;
    do {
        x_pred = x;
        for (int i = 0; i < size; i++) {
            double sum = 0;
            for (int j = 0; j < i; j++)
                sum += A[i * size + j] * x[j];
            for (int j = i + 1; j < size; j++)
                sum += A[i * size + j] * x[j];
            x[i] = (b[i] - sum) / A[i * size + i];
        }
        iter_number++;
        
        dist = d(x, x_pred);
    } while ((dist >= epsilon) && (iter_number < max_count));
    return x;
}

std::vector<double> solving_SLAE_parallel(std::vector<double> A, std::vector<double> b, int size) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    std::vector<double> x_pred(size);
    std::vector<double> x(size);
    int iter_number = 0;
    double dist = epsilon;
    do {
        x_pred = x;
        for (int i = 0; i < size; i++)
            x[i] = (b[i] - parallel_dot_product(std::vector<double>(A.begin() + (i * size), A.begin() + (i * size + size)), x)) / A[i * size + i] + x[i];
        iter_number++;
        
        dist = d(x, x_pred);
        MPI_Bcast(&dist, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } while ((dist >= epsilon) && (iter_number < max_count));
    return x;
}
