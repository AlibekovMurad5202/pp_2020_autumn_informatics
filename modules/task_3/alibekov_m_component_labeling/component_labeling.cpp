#include <mpi.h>
#include <time.h>
#include <random>
#include <vector>
#include <algorithm>
#include "iostream"
#include <utility>

#include "../../../modules/task_3/alibekov_m_component_labeling/component_labeling.h"

std::pair<std::vector<int>, int> remarking(const std::vector<int>& image, size_t width, size_t height) {
    int size = width * height;
    std::vector<int> result(size);
    std::vector<int> last_labels(size / 2 + 1);
    std::vector<int> new_labels(size / 2 + 1);
    int max_label = 0;
    
    for (int i = 0; i < size; i++) {
        int pixel = image[i];
        if (pixel != 0) {
            int idx = -1;
            for (int k = 1; last_labels[k] != 0; k++) if (last_labels[k] == pixel) { idx = k; break; }
            if (idx == -1) {
                last_labels[++max_label] = pixel;
                new_labels[max_label] = max_label;
                result[i] = new_labels[max_label];
            } else result[i] = new_labels[idx];
        }
    }
    
    return std::make_pair(result, max_label);
}

std::vector<int> generate_random_image(size_t width, size_t height) {
    std::mt19937 gen;
    gen.seed(time(0) + random_offset++ * 17);

    std::vector<int> image(height * width);
    for (int i = 0; i < height * width; ++i)
        image[i] = gen() % 2;

    return image;
}

std::pair<std::vector<int>, std::vector<int> > first_pass(
        const std::vector<int>& image, 
        size_t width, 
        size_t height,
        size_t begin_label
    ) {
        
        /*
    // Потом удалт!!!!!
    int proc_count, proc_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
         ////////
         */
    int label = 0;
    int size = width * height;
    std::vector<int> disjoint_sets(size);  // disjoint sets of labels
    std::vector<int> tmp_image(size);
    for (int x = 0; x < size; x++) { disjoint_sets[x] = x + begin_label; tmp_image[x] = (int)image[x]; }
    
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int idx = row * width + column;
            if (tmp_image[idx] != 0) {
                // [0]      [A]
                // [B] [tmp_image[idx]]
                int A = idx < width ? 0 : tmp_image[idx - width];
                int B = ((idx < 1) || ((idx - 1) / width != row)) ? 0 : tmp_image[idx - 1];
                
                if ((A == 0) && (B == 0)) tmp_image[idx] = idx + begin_label;// ++label + begin_label;
                if ((A == 0) && (B != 0)) tmp_image[idx] = B;
                if ((A != 0) && (B == 0)) tmp_image[idx] = A;
                if ((A != 0) && (B != 0)) {
                    if (A == B) tmp_image[idx] = A;
                    else {
                        int root_max_AB = std::max(A, B);
                        while (disjoint_sets[root_max_AB - begin_label] != root_max_AB) 
                            root_max_AB = disjoint_sets[root_max_AB - begin_label];
                            
                        int root_min_AB = std::min(A, B);
                        while (disjoint_sets[root_min_AB - begin_label] != root_min_AB) 
                            root_min_AB = disjoint_sets[root_min_AB - begin_label];

                        if (root_max_AB != root_min_AB) {
                            disjoint_sets[root_max_AB - begin_label] = root_min_AB;
                        }
                        tmp_image[idx] = root_min_AB;
                        
                    }
                    
                    // В прошлом:
                    /*
                    int min = std::min(A, B);
                    tmp_image[idx] = min;
                    disjoint_sets[std::distance(disjoint_sets.begin(), 
                                                std::find(disjoint_sets.begin(), 
                                                          disjoint_sets.end(), 
                                                          std::max(A, B)))] = min;
                    // disjoint_sets[std::max(A, B)] = min;
                    */
                }
                
            }
        }
    }
    
    return std::make_pair(tmp_image, disjoint_sets);
    // return std::make_pair(tmp_image, std::vector<int>(disjoint_sets.cbegin(), disjoint_sets.cbegin() + label + 1));
}


std::vector<int> second_pass(std::vector<int> map, std::vector<int> disjoint_sets, size_t width, size_t height) {
    std::vector<int> result(height * width);
    for (int row = 0; row < height; row++)
        for (int column = 0; column < width; column++) {
            int idx = row * width + column;
            int pixel = map[idx];
            if (pixel != 0)
                if (disjoint_sets[pixel] == pixel) result[idx] = static_cast<int>(pixel);
                else {
                    while (disjoint_sets[pixel] != pixel) pixel = disjoint_sets[pixel];
                    result[idx] = static_cast<int>(pixel);
                }
        }
    return result;
}

std::pair<std::vector<int>, int> component_labeling_sequential(const std::vector<int>& image, 
                                                   size_t width, 
                                                   size_t height) {
    
    std::pair<std::vector<int>, std::vector<int> > first_pass_result = first_pass(image, width, height);
    std::vector<int> map = first_pass_result.first;
    std::vector<int> disjoint_sets = first_pass_result.second;
    std::vector<int> second_pass_result = second_pass(map, disjoint_sets, width, height);
    std::pair<std::vector<int>, int> result = remarking(second_pass_result, width, height);
    
    return result;
}

std::pair<std::vector<int>, int> component_labeling_parallel(const std::vector<int>& image, 
                                                 size_t width, 
                                                 size_t height) {
    
    int proc_count, proc_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int size = width * height;
    const int delta = height / proc_count * width;
    const int remain = (height % proc_count) * width;
    // std::vector<int> disjoint_sets(size);  // disjoint sets of labels
    // std::vector<int> tmp_image(size);
    std::vector<int> result(size);
    
    // for (int x = 0; x < size; x++) { disjoint_sets[x] = x; tmp_image[x] = image[x]; }
    
    if (proc_rank == 0) {
        /////////////////////// Вывод изображения до раздачи данных процессам /////////////////////////////
        /*
        std::cout << std::endl;
        for (int i = 0; i < image.size(); i++) {
            std::cout << image[i] << " ";
            if (i % width == width - 1) std::cout << std::endl;
        }
        */
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        
        for (int proc = 1; proc < proc_count; proc++) {
            MPI_Send(image.data() + remain + proc * delta, delta, MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }
    
    std::vector<int> local_image(delta + remain);
    
    if (proc_rank == 0) {
        local_image = std::vector<int>(image.cbegin(), image.cbegin() + delta + remain);
    } else {
        MPI_Status status;
        MPI_Recv(local_image.data(), delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    /////////////////////// Вывод изображения после раздачи данных процессам /////////////////////////////
    /*
    if (proc_rank == 0) {
        std::cout << std::endl;
        for (int i = 0; i < (proc_rank == 0 ? remain + delta : delta); i++) {
            std::cout << local_image[i] << " ";
            if (i % width == width - 1) std::cout << std::endl;
        }
    }
    */
    //////////////////////////////////////////////////////////////////////////////////////////////////////
        
    
    std::pair<std::vector<int>, std::vector<int> > first_pass_result = 
        first_pass(local_image, 
                   width, 
                   (proc_rank == 0 ? remain + delta : delta) / width, 
                   (proc_rank != 0 ? remain : 0) + delta * proc_rank);
    std::vector<int> map = first_pass_result.first;
    std::vector<int> dis_set = first_pass_result.second;    
    
    
    
    
    
    
    
    
    /*
    if (proc_rank == 3) {
        std::cout << "\tOn process [1]:\n";
        for (int i = 0; i < map.size(); i++) {
            if (map[i] != 0) printf("%2i ", map[i]);
            else printf(" 0 ");
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        
        for (int i = 0; i < dis_set.size(); i++) {
            if (dis_set[i] != 0) printf("%2i ", dis_set[i]);
            else printf(" 0 ");
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << "\t-------||------\n";
    }
    */
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    std::vector<int> displs(proc_count);
    displs[1] = remain + delta;
    std::vector<int> recvcounts(proc_count);
    recvcounts[0] = remain + delta;
    recvcounts[1] = delta;
    for (int proc = 2; proc < proc_count; proc++) {
        displs[proc] = displs[proc - 1] + delta;
        recvcounts[proc] = delta;
    }
    
    std::vector<int> global_disjoint_sets(size);
    MPI_Gatherv(dis_set.data(),
                proc_rank == 0 ? remain + delta : delta,
                MPI_INT,
                global_disjoint_sets.data(),
                recvcounts.data(),
                displs.data(),
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    
    
    std::vector<int> global_map(size);
    MPI_Gatherv(map.data(),
                proc_rank == 0 ? remain + delta : delta,
                MPI_INT,
                global_map.data(),
                recvcounts.data(),
                displs.data(),
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    if (proc_rank == 0) {
        /////////////////////////// Вывод СРМ и карты до связи результатоов процесса ////////////////////////
        /*
        std::cout << "\t[Before]: "<< std::endl;
        for (int i = 0; i < global_disjoint_sets.size(); i++) {
            std::cout << global_disjoint_sets[i] << " ";
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        for (int i = 0; i < global_map.size(); i++) {
            printf("%3i ", global_map[i]);
            // if (global_map[i] != 0) printf("%2i ", global_map[i]);
            // else printf("   ");
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        */
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        
        for (int x = 1; x < proc_count; x++) {
            int second_line_begin = (x != 0 ? remain : 0) + delta * x;
            int first_line_begin = second_line_begin - width;
            for (int offset = 0; offset < width; offset++) {
                int first_line_idx = first_line_begin + offset;
                int second_line_idx = second_line_begin + offset;
                // ... [A] ...
                // ... [B] ...
                int A = global_map[first_line_idx];
                int B = global_map[second_line_idx];
                // std::cout << "\t[A]: " << A << "\t[B]: " << B << std::endl;
                if ((A != 0) && (B != 0)) {
                    int dis_set_A = global_disjoint_sets[A];
                    int dis_set_B = global_disjoint_sets[B];
                    if (dis_set_A != dis_set_B) {
                        
                        int root_max_AB = std::max(A, B);
                        while (global_disjoint_sets[root_max_AB] != root_max_AB) 
                            root_max_AB = global_disjoint_sets[root_max_AB];
                            
                        int root_min_AB = std::min(A, B);
                        while (global_disjoint_sets[root_min_AB] != root_min_AB) 
                            root_min_AB = global_disjoint_sets[root_min_AB];

                        global_disjoint_sets[root_max_AB] = root_min_AB;
                        /*
                        // int min = std::min(A, B);
                        // int max = std::max(A, B);
                        int min = std::min(dis_set_A, dis_set_B);
                        int max = std::max(dis_set_A, dis_set_B);
                        global_disjoint_sets[std::distance(global_disjoint_sets.begin(), 
                                                       std::find(global_disjoint_sets.begin(), 
                                                                 global_disjoint_sets.end(), 
                                                                 max))] = min;
                                                                 */
                    }
                }
            }
        }
        
        
        
        ////////////////////////// Вывод СРМ после сбора со всех процессов //////////////////////////////////
        /*
        std::cout << "\t[After]: " << std::endl;
        for (int i = 0; i < global_disjoint_sets.size(); i++) {
            std::cout << global_disjoint_sets[i] << " ";
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        */
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        
        result = second_pass(global_map, global_disjoint_sets, width, height);
        


        ////////////////////////////// Вывод финального результата //////////////////////////////////
        /*
        for (int i = 0; i < global_map.size(); i++) {
            if (result[i] != 0) printf("%2i ", result[i]);
            else printf(" 0 ");
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        */
        /*
        for (int i = 0; i < global_map.size(); i++) {
            if (global_map[i] != 0) printf("%2i ", global_map[i]);
            else printf(" 0 ");
            if (i % width == width - 1) std::cout << std::endl;
        }
        std::cout << std::endl;
        */
        ////////////////////////////////////////////////////////////////////////////////////////////
    }
    
    return remarking(result, width, height);
}