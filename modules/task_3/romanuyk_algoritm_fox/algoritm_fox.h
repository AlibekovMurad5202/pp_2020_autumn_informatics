// Copyright 2020 Romanuyk Sergey
#ifndef MODULES_TASK_3_ROMANUYK_ALGORITM_FOX_ALGORITM_FOX_H_
#define MODULES_TASK_3_ROMANUYK_ALGORITM_FOX_ALGORITM_FOX_H_

#include <vector>

struct Grid {
    int col;
    int row;
    int GridSize;
    MPI_Comm GridComm;
    MPI_Comm ColComm;
    MPI_Comm RowComm;
};

std::vector<double> genMatrix(int n);
std::vector<double> SequentinalMultiMatrix(std::vector<double> A, std::vector<double> B, int n);

bool assertMatrix(const std::vector<double> A, const std::vector<double> B);

void createGrid(Grid* grid, const int& procrank);
void MultiplyMatrixforParallel(const std::vector<double> A, const std::vector<double> matrixB,
    double* matrixC, int size);

std::vector<double> MultiplyMatrixParallel(const std::vector<double> A, const std::vector<double> B, int size);

#endif  // MODULES_TASK_3_ROMANUYK_ALGORITM_FOX_ALGORITM_FOX_H_
