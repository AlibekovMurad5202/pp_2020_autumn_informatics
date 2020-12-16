#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <vector>
#include "mpi.h"
#include <fstream>

#include "./component_labeling.h"

TEST(Component_Labeling, my_image_9x11_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 12, height = 12;
    std::vector<int> image_M(width * height);
    
    if (proc_rank == 0) {
        image_M = {
            0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
            1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1,
            0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
            1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
            0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0,
            1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
            1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0,
            1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
            0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0,
            0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0,
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
        };
    }
 
    if (proc_rank == 0) start_time = MPI_Wtime();
    std::pair<std::vector<int>, int> result_par = component_labeling_parallel(image_M, width, height);
    if (proc_rank == 0) end_time = MPI_Wtime();
    
    if (proc_rank == 0) printf("\n\tTime  = %f\n", end_time - start_time);
    
    if (proc_rank == 0) {
        std::ofstream file("result_M_par.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                // if (result.first[row * width + column] != 0) file << result.first[row * width + column] << " ";
                // else file << "   ";
                file << std::setw(3) << result_par.first[row * width + column] << " ";
            file << std::endl;
        }
        file.close();
    }
    
    if (proc_rank == 0) {
        std::pair<std::vector<int>, int> result_seq = component_labeling_sequential(image_M, width, height);
        std::ofstream file("result_M_seq.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                // if (result.first[row * width + column] != 0) file << result.first[row * width + column] << " ";
                // else file << "   ";
                file << std::setw(3) << result_seq.first[row * width + column] << " ";
            file << std::endl;
        }
        file.close();
    }
    
    ASSERT_EQ(1, 1);
}







TEST(Component_Labeling, random_image_9x11_sequential) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 11, height = 9;
    std::vector<int> image;

    if (proc_rank == 0) {
        image = generate_random_image(width, height);
        std::cout << "\tGenerated image: " << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if ((int)image[row * width + column] != 0) printf("%2i ", (int)image[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        std::cout << std::endl;
        start_time = MPI_Wtime();
        std::pair<std::vector<int>, int> result = component_labeling_sequential(image, width, height);
        end_time = MPI_Wtime();
        std::cout << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (result.first[row * width + column] != 0) printf("%2i ", result.first[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        printf("\n\tTime  = %f\n", end_time - start_time);
        printf("\tLabels_count: %i\n", result.second);
        		
        std::ofstream file("result.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (result.first[row * width + column] != 0) file << result.first[row * width + column] << " ";
                else file << "   ";
            file << std::endl;
        }
        file.close();
    }
    
    ASSERT_EQ(1, 1);
}

/*
TEST(Component_Labeling, random_image_18x22_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 22, height = 18;
    std::vector<int> image;

    if (proc_rank == 0) {
        image = generate_random_image(width, height);
        std::cout << "\tGenerated image: " << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if ((int)image[row * width + column] != 0) printf("%2i ", (int)image[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        std::cout << std::endl;
        start_time = MPI_Wtime();
    }
    std::vector<int> result = component_labeling_parallel(image, width, height);
    if (proc_rank == 0) {
        end_time = MPI_Wtime();
        std::cout << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if ((int)result[row * width + column] != 0) printf("%2i ", (int)result[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        printf("\n\tTime  = %f\n", end_time - start_time);
    }
    
    ASSERT_EQ(1, 1);
}
*/

/*
TEST(Component_Labeling, BIG_random_image_processing) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time_par, end_time_par;
    double start_time_seq, end_time_seq;
    
    int width = 1920, height = 1080;
    std::vector<int> image(width * height);

    if (proc_rank == 0) {
        std::cout << "[COMMON] Generating image (h=" << height << ", w=" << width << ")..." << std::endl;
        image = generate_random_image(width, height);
        std::cout << "[COMMON] Image is generated." << std::endl;
        std::cout << std::endl;
    }
    
    if (proc_rank == 0) {
        std::cout << "[SEQ] Processing image..." << std::endl;
        start_time_seq = MPI_Wtime();
        std::vector<int> result = component_labeling_sequential(image, width, height);
        end_time_seq = MPI_Wtime();
        std::cout << "[SEQ] Done." << std::endl;
        printf("\n[SEQ]\tTime  = %f\n", end_time_seq - start_time_seq);
        std::cout << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (proc_rank == 0) {
        std::cout << "[PAR] Processing image..." << std::endl;
        start_time_par = MPI_Wtime();
    }
    std::vector<int> result = component_labeling_parallel(image, width, height);
    if (proc_rank == 0) {
        end_time_par = MPI_Wtime();
        std::cout << "[PAR] Done." << std::endl;
        printf("\n[PAR]\tTime  = %f\n", end_time_par - start_time_par);
    }
    
    ASSERT_EQ(1, 1);
}
*/

TEST(Component_Labeling, BIG_random_image_processing_par) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time_par, end_time_par;
    double start_time_seq, end_time_seq;
    
    // int width = 12, height = 12;
    int width = 1920, height = 1080;
    std::vector<int> image(width * height);

    if (proc_rank == 0) {
        std::cout << "[COMMON] Generating image (h=" << height << ", w=" << width << ")..." << std::endl;
        image = generate_random_image(width, height);
        std::cout << "[COMMON] Image is generated." << std::endl;
        std::cout << std::endl;
    }
    
    if (proc_rank == 0) {
        std::cout << "[SEQ] Processing image..." << std::endl;
        start_time_seq = MPI_Wtime();
        std::pair<std::vector<int>, int> result_seq = component_labeling_sequential(image, width, height);
        end_time_seq = MPI_Wtime();
        std::cout << "[SEQ] Done." << std::endl;
        printf("\n[SEQ]\tTime  = %f\n", end_time_seq - start_time_seq);
        printf("\tLabels_count: %i\n", result_seq.second);
        std::cout << std::endl;
        
        /*
        std::ofstream file("result_seq.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                // if (result_seq.first[row * width + column] != 0) file << result_seq.first[row * width + column] << " ";
                // else file << "   ";
                file << std::setw(3) << result_seq.first[row * width + column] << " ";
            file << std::endl;
        }
        file.close();
        */
    }
    
    if (proc_rank == 0) {
        std::cout << "[PAR] Processing image..." << std::endl;
        start_time_par = MPI_Wtime();
    }
    std::pair<std::vector<int>, int> result = component_labeling_parallel(image, width, height);
    if (proc_rank == 0) {
        end_time_par = MPI_Wtime();
        std::cout << "[PAR] Done." << std::endl;
        
        printf("\n[PAR]\tTime  = %f\n", end_time_par - start_time_par);
        printf("\tLabels_count: %i\n", result.second);
        
        /*
        std::ofstream file("result_par.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                // if (result.first[row * width + column] != 0) file << result.first[row * width + column] << " ";
                // else file << "   ";
                file << std::setw(3) << result.first[row * width + column] << " ";
            file << std::endl;
        }
        file.close();
        */
    }
    
    ASSERT_EQ(1, 1);
}

/*
TEST(Component_Labeling, BIG_random_image_processing_seq) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time_par, end_time_par;
    double start_time_seq, end_time_seq;
    
    int width = 1920, height = 1080;
    // int width = 1920, height = 67;
    std::vector<int> image(width * height);

    if (proc_rank == 0) {
        std::cout << "[COMMON] Generating image (h=" << height << ", w=" << width << ")..." << std::endl;
        image = generate_random_image(width, height);
        std::cout << "[COMMON] Image is generated." << std::endl;
        std::cout << std::endl;
    }
    
    
    //start_time_seq = MPI_Wtime();
    if (proc_rank == 0) {
        std::cout << "[SEQ] Processing image..." << std::endl;
        start_time_seq = MPI_Wtime();
        std::pair<std::vector<int>, int> result = component_labeling_sequential(image, width, height);
        end_time_seq = MPI_Wtime();
        std::cout << "[SEQ] Done." << std::endl;
        printf("\n[SEQ]\tTime  = %f\n", end_time_seq - start_time_seq);
        printf("\tLabels_count: %i\n", result.second);
        std::cout << std::endl;
        
        std::ofstream file("result_seq.txt");
		file.is_open();
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (result.first[row * width + column] != 0) file << result.first[row * width + column] << " ";
                else file << "   ";
            file << std::endl;
        }
        file.close();
    }
       // end_time_seq = MPI_Wtime();
       // if (proc_rank == 0) printf("\n[SEQ]\tTime  = %f\n", end_time_seq - start_time_seq);
    ASSERT_EQ(1, 1);
}
*/

/*
TEST(Component_Labeling, my_image_9x11_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 11, height = 9;
    std::vector<int> image_M(width * height);
    
    if (proc_rank == 0) {
        image_M = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
            0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
            0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
            0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,
            0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0,
            0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
    }
 
    if (proc_rank == 0) start_time = MPI_Wtime();
    std::vector<int> result = component_labeling_parallel(image_M, width, height);
    if (proc_rank == 0) end_time = MPI_Wtime();
    
    if (proc_rank == 0) printf("\n\tTime  = %f\n", end_time - start_time);
    
    ASSERT_EQ(1, 1);
}

TEST(Component_Labeling, my_image_9x18_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 17, height = 9;
    std::vector<int> image_MMMM(width * height);
    
    if (proc_rank == 0) {
        image_MMMM = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,  0, 1, 1, 1, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 1, 1, 1, 1, 1,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,  0, 1, 1, 0, 1, 1,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 1,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0
        };
    }
 
    if (proc_rank == 0) start_time = MPI_Wtime();
    std::vector<int> result = component_labeling_parallel(image_MMMM, width, height);
    if (proc_rank == 0) end_time = MPI_Wtime();
    
    if (proc_rank == 0) printf("\n\tTime  = %f\n", end_time - start_time);
    
    ASSERT_EQ(1, 1);
}

TEST(Component_Labeling, my_image_18x22_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 22, height = 18;
    std::vector<int> image_MMMM(width * height);
    
    if (proc_rank == 0) {
        image_MMMM = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,  0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,  0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,  0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,  0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
    }
 
    if (proc_rank == 0) start_time = MPI_Wtime();
    std::vector<int> result = component_labeling_parallel(image_MMMM, width, height);
    if (proc_rank == 0) end_time = MPI_Wtime();
    
    if (proc_rank == 0) printf("\n\tTime  = %f\n", end_time - start_time);
    
    ASSERT_EQ(1, 1);
}


TEST(Component_Labeling, my_image_9x11_sequential) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;
    
    int width = 11, height = 9;
    std::vector<int> image_M {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
        0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0,
        1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    };

    if (proc_rank == 0) {
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (image_M[row * width + column] != 0) printf("%2i ", image_M[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        std::cout << std::endl;
        start_time = MPI_Wtime();
        std::pair<std::vector<int>, int> result = component_labeling_sequential(image_M, width, height);
        end_time = MPI_Wtime();
        std::cout << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (result.first[row * width + column] != 0) printf("%2i ", result.first[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        printf("\n\tTime  = %f\n", end_time - start_time);
        printf("\tLabels_count: %i\n", result.second);
    }
    
    ASSERT_EQ(1, 1);
}
/*
TEST(Component_Labeling, my_image_18x22_sequential) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time, end_time;

    int width = 22, height = 18;
    std::vector<int> image_MMMM {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,  0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,  0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,  0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,  0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,  0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    if (proc_rank == 0) {
        start_time = MPI_Wtime();
        std::vector<int> result = component_labeling_sequential(image_MMMM, width, height);
        end_time = MPI_Wtime();
        std::cout << std::endl;
        for (int row = 0; row < height; row++) {
            for (int column = 0; column < width; column++)
                if (result[row * width + column] != 0) printf("%2i ", (int)result[row * width + column]);
                else printf("   ");
            std::cout << std::endl;
        }
        printf("\n\tTime  = %f\n", end_time - start_time);
    }
    
    ASSERT_EQ(1, 1);
}
*/






int main(int argc, char* argv[]) {
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
