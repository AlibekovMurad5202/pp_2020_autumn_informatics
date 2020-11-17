// Copyright 2020 Alibekov Murad
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "iostream"
#include "./seidel_method.h"

TEST(Parallel_Seidel_Method, SLAE_3_variables_sequential) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    std::vector<std::vector<double> > A = { 
        { 10., 1., 2.},
        { 0., 10., 3.},
        { 0., 0., 5.} 
    };
    
    std::vector<double> b = { 3., 7., 5. };
    
    if (proc_rank == 0) {
        std::vector<double> x = solving_SLAE_sequential(A, b);
        
        std::cout << "A: ";
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < A[i].size(); j++)
                std::cout << A[i][j] << "  ";
            std::cout << std::endl;
        };
        
        std::cout << std::endl << "b: ";
        for (int i = 0; i < b.size(); i++) {
            std::cout << b[i] << "  ";
        };
        
        std::cout << std::endl << "x: ";
        for (int i = 0; i < x.size(); i++) {
            std::cout << x[i] << "  ";
        };
        //global_str = getRandomString(count_chars);
    }
    /*
    int global_count = getSentencesCountParallel(global_str, count_chars);

    if (proc_rank == 0) {
        int reference_count = getSentencesCountSequential(global_str);
        ASSERT_EQ(reference_count, global_count);
    }*/
}

TEST(Parallel_Seidel_Method, SLAE_3_variables_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    std::vector<std::vector<double> > A = { 
        { 10., 1., 2.},
        { 0., 10., 3.},
        { 0., 0., 5.} 
    };
    
    std::vector<double> b = { 3., 7., 5. };
    
        std::vector<double> x = solving_SLAE_parallel(A, b);
    if (proc_rank == 0) {
        
        std::cout << "A: ";
        for (int i = 0; i < A.size(); i++) {
            for (int j = 0; j < A[i].size(); j++)
                std::cout << A[i][j] << "  ";
            std::cout << std::endl;
        };
        
        std::cout << std::endl << "b: ";
        for (int i = 0; i < b.size(); i++) {
            std::cout << b[i] << "  ";
        };
        
        std::cout << std::endl << "x: ";
        for (int i = 0; i < x.size(); i++) {
            std::cout << x[i] << "  ";
        };
        //global_str = getRandomString(count_chars);
    }
    /*
    int global_count = getSentencesCountParallel(global_str, count_chars);

    if (proc_rank == 0) {
        int reference_count = getSentencesCountSequential(global_str);
        ASSERT_EQ(reference_count, global_count);
    }*/
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
