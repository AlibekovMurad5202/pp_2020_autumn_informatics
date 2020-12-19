// Copyright 2020 Alibekov Murad
#include <gtest-mpi-listener.hpp>
#include <gtest/gtest.h>
#include <mpi.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <utility>
#include "./component_labeling.h"

using namespace cv;

TEST(Component_Labeling, OpenCV_my_image_sequential) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    if (proc_rank == 0) {
        Mat image = 
            imread("C:\\Users\\murad\\Desktop\\pp\\pp_2020_autumn_informatics\\modules\\task_3\\alibekov_m_component_labeling\\test.bmp", 
                   IMREAD_GRAYSCALE);
        namedWindow("original_image", WINDOW_AUTOSIZE);
        imshow("original_image", image);
        waitKey(0);
        
        int width = image.cols;
        int height = image.rows;
        int size = image.total();
        std::vector<int> processing_input(size);
        double start_time, end_time;
        
        Mat flat = image.reshape(1, image.total() * image.channels());
        std::vector<int> vec = image.isContinuous() ? flat : flat.clone();
        
        for (int i = 0; i < size; i++) processing_input[i] = 1 - (vec[i] == 255);

        start_time = MPI_Wtime();
        std::pair<std::vector<int>, int> result_seq = component_labeling_sequential(processing_input, width, height);
        end_time = MPI_Wtime();
        std::vector<int> res = remarking(result_seq.first, width, height);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (res[i * width + j] != 0) printf("%2i ", res[i * width + j]);
                else printf("   ");
            }
            std::cout << std::endl;
        }
        
        printf("\n\tTime  = %f\n", end_time - start_time);
        printf("\tCount of components: %i\n", result_seq.second);
        
        
        ASSERT_EQ(result_seq.second, 4);
        
        /*
        std::vector<int> res_color(image.total() * 3);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                res_color[0 + i * width + j] = 0;
                res_color[1 + i * width + j] = 0;
                res_color[2 + i * width + j] = 50 * res[i * width + j];
            }
            std::cout << std::endl;
        }
    
        cv::Mat result_image(width * 3, height, cv::CV_8UC1, res_color.data(), res_color.size());
        result_image.reshape(height, width, 3);
    
        cv::namedWindow("result_image", cv::WINDOW_AUTOSIZE);
        cv::imshow("image_after", result_image);
        cv::waitKey();
        */
    }
        
        
        
        /*
    double start_time, end_time;
    MPI_Barrier(MPI_COMM_WORLD);
    height = 9, width = 11;
    std::vector<int> image(width * height);
    std::vector<int> right_result(width * height);

    if (proc_rank == 0) start_time = MPI_Wtime();
    std::pair<std::vector<int>, int> result = component_labeling_parallel(image, width, height);
    if (proc_rank == 0) end_time = MPI_Wtime();

    if (proc_rank == 0) {
        printf("\tTime  = %f\n", end_time - start_time);
        printf("\tCount of components: %i\n\n", result.second);

        std::vector<int> new_result = remarking(result.first, width, height);
        for (int i = 0; i < height * width; i++)
            ASSERT_EQ(right_result[i], new_result[i]);
    }*/
}


TEST(Component_Labeling, OpenCV_my_image_parallel) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    cv::Mat image = 
        cv::imread("C:\\Users\\murad\\Desktop\\pp\\pp_2020_autumn_informatics\\modules\\task_3\\alibekov_m_component_labeling\\test.bmp", 
                   IMREAD_GRAYSCALE);
        
    int width = image.cols;
    int height = image.rows;
    int size = image.total();
    std::vector<int> processing_input(size);
    double start_time, end_time;
        
    if (proc_rank == 0) {
        cv::namedWindow("original_image", WINDOW_AUTOSIZE);
        cv::imshow("original_image", image);
        cv::waitKey(0);
        cv::Mat flat = image.reshape(1, image.total() * image.channels());
        std::vector<int> vec = image.isContinuous() ? flat : flat.clone();
        
        for (int i = 0; i < size; i++) processing_input[i] = 1 - (vec[i] == 255);

        start_time = MPI_Wtime();
    }
    std::pair<std::vector<int>, int> result_seq = component_labeling_parallel(processing_input, width, height);
    if (proc_rank == 0) {
        end_time = MPI_Wtime();
        std::vector<int> res = remarking(result_seq.first, width, height);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (res[i * width + j] != 0) printf("%2i ", res[i * width + j]);
                else printf("   ");
            }
            std::cout << std::endl;
        }
        
        printf("\n\tTime  = %f\n", end_time - start_time);
        printf("\tCount of components: %i\n", result_seq.second);
        
        
        ASSERT_EQ(result_seq.second, 4);
        
        /*
        std::vector<int> res_color(image.total() * 3);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                res_color[0 + i * width + j] = 0;
                res_color[1 + i * width + j] = 0;
                res_color[2 + i * width + j] = 50 * res[i * width + j];
            }
            std::cout << std::endl;
        }
    
        cv::Mat result_image(width * 3, height, cv::CV_8UC1, res_color.data(), res_color.size());
        result_image.reshape(height, width, 3);
    
        cv::namedWindow("result_image", cv::WINDOW_AUTOSIZE);
        cv::imshow("image_after", result_image);
        cv::waitKey();
        */
    }
}
















TEST(Component_Labeling, OpenCV_my_image_robot_face) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time_par, end_time_par, start_time_seq, end_time_seq;
    int component_counts_seq, component_counts_par;

    Mat image = 
        imread("C:\\Users\\murad\\Desktop\\pp\\pp_2020_autumn_informatics\\modules\\task_3\\alibekov_m_component_labeling\\test_2.bmp", 
               IMREAD_GRAYSCALE);

    int width = image.cols;
    int height = image.rows;
    int size = image.total();
    std::vector<int> processing_input(size);
    std::pair<std::vector<int>, int> result_seq;

    if (proc_rank == 0) {
        namedWindow("original_image", WINDOW_AUTOSIZE);
        imshow("original_image", image);
        waitKey(0);
        
        Mat flat = image.reshape(1, image.total() * image.channels());
        std::vector<int> vec = image.isContinuous() ? flat : flat.clone();
        
        for (int i = 0; i < size; i++) processing_input[i] = 1 - (vec[i] == 255);
    }

    if (proc_rank == 0) {
        start_time_seq = MPI_Wtime();
        std::pair<std::vector<int>, int> result_seq = component_labeling_sequential(processing_input, width, height);
        end_time_seq = MPI_Wtime();
        component_counts_seq = result_seq.second;
        std::vector<int> res_seq = remarking(result_seq.first, width, height);
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (res_seq[i * width + j] != 0) printf("%2i ", res_seq[i * width + j]);
                else printf("   ");
            }
            std::cout << std::endl;
        }
    }

    if (proc_rank == 0) start_time_par = MPI_Wtime();
    std::pair<std::vector<int>, int> result_par = component_labeling_parallel(processing_input, width, height);
    if (proc_rank == 0) end_time_par = MPI_Wtime();
    
    if (proc_rank == 0) {
        std::vector<int> res_par = remarking(result_par.first, width, height);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (res_par[i * width + j] != 0) printf("%2i ", res_par[i * width + j]);
                else printf("   ");
            }
            std::cout << std::endl;
        }
    }

    if (proc_rank == 0) {
        printf("\tTime (sequential) = %f\n", end_time_seq - start_time_seq);
        printf("\tCount of components (sequential): %i\n\n", component_counts_seq);

        component_counts_par = result_par.second;
        printf("\tTime (parallel) = %f\n", end_time_par - start_time_par);
        printf("\tCount of components (parallel): %i\n", component_counts_par);

        ASSERT_EQ(component_counts_seq, component_counts_par);
    }
}















TEST(Component_Labeling, random_image_1080x1920) {
    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    double start_time_par, end_time_par, start_time_seq, end_time_seq;

    int height = 1080, width = 1920;
    std::vector<int> generated_image(width * height);
    std::pair<std::vector<int>, int> result_seq;

    if (proc_rank == 0) {
        generated_image = generate_random_image(width, height);
    }

    if (proc_rank == 0) {
        start_time_seq = MPI_Wtime();
        result_seq = component_labeling_sequential(generated_image, width, height);
        end_time_seq = MPI_Wtime();
    }

    if (proc_rank == 0) start_time_par = MPI_Wtime();
    std::pair<std::vector<int>, int> result_par = component_labeling_parallel(generated_image, width, height);
    if (proc_rank == 0) end_time_par = MPI_Wtime();

    if (proc_rank == 0) {
        int component_counts_seq = result_seq.second;
        printf("\tTime (sequential) = %f\n", end_time_seq - start_time_seq);
        printf("\tCount of components (sequential): %i\n\n", component_counts_seq);

        int component_counts_par = result_par.second;
        printf("\tTime (parallel) = %f\n", end_time_par - start_time_par);
        printf("\tCount of components (parallel): %i\n", component_counts_par);

        ASSERT_EQ(component_counts_seq, component_counts_par);
    }
}


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
