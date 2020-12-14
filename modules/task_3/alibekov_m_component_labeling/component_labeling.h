// Copyright 2020 Alibekov Murad
#ifndef MODULES_TASK_3_ALIBEKOV_M_COMPONENT_LABELING_COMPONENT_LABELING_H_
#define MODULES_TASK_3_ALIBEKOV_M_COMPONENT_LABELING_COMPONENT_LABELING_H_

#include <vector>
#include <utility>

std::vector<int> component_labeling_sequential(const std::vector<int>& image, 
                                                   size_t width, 
                                                   size_t height);
std::pair<std::vector<int>, std::vector<int> > first_pass(const std::vector<int>& image, 
                                                          size_t width, 
                                                          size_t height, 
                                                          size_t begin_label = 0);
std::vector<int> second_pass(std::vector<int> map, std::vector<int> disjoint_sets, size_t width, size_t height);
std::vector<int> component_labeling_parallel(const std::vector<int>& image, 
                                                 size_t width, 
                                                 size_t height);

#endif  // MODULES_TASK_3_ALIBEKOV_M_COMPONENT_LABELING_COMPONENT_LABELING_H_
