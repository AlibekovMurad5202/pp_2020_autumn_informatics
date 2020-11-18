// Copyright 2020 Alibekov Murad
#ifndef MODULES_TASK_2_ALIBEKOV_M_SEIDEL_METHOD_SEIDEL_METHOD_H_
#define MODULES_TASK_2_ALIBEKOV_M_SEIDEL_METHOD_SEIDEL_METHOD_H_

std::vector<double> generate_A(int size);
std::vector<double> generate_b(int size);
double d(std::vector<double> x, std::vector<double> y);
double parallel_dot_product(std::vector<double>& x, std::vector<double>& y);

std::vector<double> solving_SLAE_sequential(std::vector<double> A, std::vector<double> b);
std::vector<double> solving_SLAE_parallel(std::vector<double> A, std::vector<double> b);

#endif  // MODULES_TASK_2_ALIBEKOV_M_SEIDEL_METHOD_SEIDEL_METHOD_H_
