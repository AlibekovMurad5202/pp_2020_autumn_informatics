// Copyright 2020 Alibekov Murad
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include "./seidel_method.h"

TEST(Parallel_Seidel_Method, SLAE_2_variables) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    std::vector<std::vector<double> > A = { 
        { 10., 1., 2.},
        { 0., 10., 3.},
        { 0., 0., 5.} 
    };
    
    std::vector<double> b = { 3., 7., 5. };
    
    if (proc_rank == 0) {
        solving_SLAE_sequential(
        global_str = getRandomString(count_chars);
    }

    int global_count = getSentencesCountParallel(global_str, count_chars);

    if (proc_rank == 0) {
        int reference_count = getSentencesCountSequential(global_str);
        ASSERT_EQ(reference_count, global_count);
    }
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
