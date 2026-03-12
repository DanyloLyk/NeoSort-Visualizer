#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <sstream>
#include "algorithms.h"

using namespace std;

void GenerateArray(AppState& state) {
    state.arr.clear();
    
    state.is_animating_finish = false;
    state.is_sorting = false;
    ClearStates(state);

    random_device rd;
    mt19937 gen(rd());

    int min_v = state.min_value;
    int max_v = state.max_value;

    if (min_v > max_v) {
        swap(min_v, max_v);
        int temp_val = state.min_value;
        state.min_value = state.max_value;
        state.max_value = temp_val;
    }
    
    uniform_int_distribution<int> dist(min_v, max_v);

    for (int i = 0; i < state.array_size; i++) {
        state.arr.push_back(dist(gen));
    }
    
    state.initial_array_str = "";
    for (int i = 0; i < state.arr.size(); i++) {
        state.initial_array_str += std::to_string(state.arr[i]) + " ";
    }
    
    state.initial_arr = state.arr;

    state.log_history.clear();
    state.log_history.push_back("[System] Згенеровано новий масив розміром " + std::to_string(state.array_size));
}

void OwnArray(AppState& state) {
    vector<int> new_arr;
    string input(state.custom_array_input);
    
    replace(input.begin(), input.end(), ',', ' ');
    
    stringstream ss(input);
    string temp;
    
    while (ss >> temp) {
        try {
            new_arr.push_back(std::stoi(temp));
        } catch (...) { }
    }
    
    if (!new_arr.empty()) {
        state.arr = new_arr;
        state.initial_arr = new_arr;
        state.array_size = new_arr.size(); 
        
        state.is_animating_finish = false;
        state.is_sorting = false;
        ClearStates(state);
        
        state.initial_array_str = input;
        state.log_history.clear();
        state.log_history.push_back("[System] Завантажено власний масив (" + std::to_string(state.array_size) + " елементів)");
    }
}

void BubbleSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int size = (int)state.arr.size();
    if (state.bubble_i < size - 1) {
        if (state.bubble_j < size - state.bubble_i - 1) {
            
            state.highlight_1 = state.bubble_j;
            state.highlight_2 = state.bubble_j + 1;

            bool condition = state.sort_descending ? (state.arr[state.bubble_j] < state.arr[state.bubble_j + 1]) : (state.arr[state.bubble_j] > state.arr[state.bubble_j + 1]);
            if (condition) {
                state.is_swapping = true;
                if (state.detailed_logs) {
                    state.log_history.push_back("[Bubble] Переставляємо " + 
                        std::to_string(state.arr[state.bubble_j]) + " та " + 
                        std::to_string(state.arr[state.bubble_j + 1]));
                }       
                std::swap(state.arr[state.bubble_j], state.arr[state.bubble_j + 1]);
            }
            
            state.bubble_j++;
        } else {
            state.bubble_j = 0;
            state.bubble_i++;
            state.is_swapping = false;
        }
    } else {
        EndSort(state);
    }
}

void SelectionSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    if (state.sel_i < n - 1) {
        if (state.sel_j < n) {
            state.highlight_1 = state.sel_min_idx;
            state.highlight_2 = state.sel_j;
            state.is_swapping = false;

            bool condition = state.sort_descending ? (state.arr[state.sel_j] > state.arr[state.sel_min_idx]) : (state.arr[state.sel_j] < state.arr[state.sel_min_idx]);
            if (condition) {
                state.sel_min_idx = state.sel_j;
            }
            state.sel_j++; 
        } else {
            state.highlight_1 = state.sel_i;
            state.highlight_2 = state.sel_min_idx;
            if (state.sel_min_idx != state.sel_i) {
                state.is_swapping = true;
                if (state.detailed_logs) {
                    state.log_history.push_back("[Selection] Знайшли мінімум: " + 
                        std::to_string(state.arr[state.sel_min_idx]) + 
                        ". Міняємо з " + std::to_string(state.arr[state.sel_i]));
                }
                std::swap(state.arr[state.sel_i], state.arr[state.sel_min_idx]);
            } else {
                state.is_swapping = false;
            }

            state.sel_i++;
            state.sel_min_idx = state.sel_i;
            state.sel_j = state.sel_i + 1;
        }
    } else {
        EndSort(state);    
    }
}

void InsertionSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    if (state.ins_i < n) {
        bool condition = false;
        if (state.ins_j > 0) {
            condition = state.sort_descending ? (state.arr[state.ins_j - 1] < state.arr[state.ins_j]) : (state.arr[state.ins_j - 1] > state.arr[state.ins_j]);
        }

        if (state.ins_j > 0 && condition) {
            state.highlight_1 = state.ins_j - 1;
            state.highlight_2 = state.ins_j;
            state.is_swapping = true;

            if (state.detailed_logs) {
                state.log_history.push_back("[Insertion] Тягнемо вліво: міняємо " + 
                    std::to_string(state.arr[state.ins_j - 1]) + " та " + 
                    std::to_string(state.arr[state.ins_j]));
            }
            std::swap(state.arr[state.ins_j - 1], state.arr[state.ins_j]);
            state.ins_j--;
        } 
        else {
            state.is_swapping = false;
            state.ins_i++;
            state.ins_j = state.ins_i;
            
            if (state.ins_i < n) {
                state.highlight_1 = state.ins_i;
                state.highlight_2 = state.ins_i;
            }
        }
    } else {
        EndSort(state);
    }
}

void MergeSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    if (state.merge_curr_size < n) {
        
        if (state.merge_left_start < n - 1) {
            int left = state.merge_left_start;
            int mid = std::min(left + state.merge_curr_size - 1, n - 1);
            int right = std::min(left + 2 * state.merge_curr_size - 1, n - 1);

            if (state.merge_buffer.empty()) {
                state.merge_i = left;
                state.merge_j = mid + 1;
                state.merge_k = left;
                state.merge_buffer = state.arr;
            }

            if (state.merge_i <= mid && state.merge_j <= right) { 
                state.highlight_1 = state.merge_i;
                state.highlight_2 = state.merge_j;
                state.is_swapping = false; 
                
                bool condition = state.sort_descending ? (state.merge_buffer[state.merge_i] >= state.merge_buffer[state.merge_j]) : (state.merge_buffer[state.merge_i] <= state.merge_buffer[state.merge_j]);

                if (condition) {
                    state.arr[state.merge_k] = state.merge_buffer[state.merge_i];
                    state.merge_i++;
                } else {
                    state.arr[state.merge_k] = state.merge_buffer[state.merge_j];
                    state.merge_j++;
                }
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Merge] Записуємо " + std::to_string(state.arr[state.merge_k]) + " на позицію " + std::to_string(state.merge_k));
                }
                
                state.merge_k++;
                return; 
            }

            // Залишки з лівої частини
            if (state.merge_i <= mid) {
                state.highlight_1 = state.merge_i;
                state.highlight_2 = -1;
                state.arr[state.merge_k] = state.merge_buffer[state.merge_i];
                state.merge_i++;
                state.merge_k++;
                return;
            }
            
            // Залишки з правої частини
            if (state.merge_j <= right) {
                state.highlight_1 = -1;
                state.highlight_2 = state.merge_j;
                state.arr[state.merge_k] = state.merge_buffer[state.merge_j];
                state.merge_j++;
                state.merge_k++;
                return;
            }

            state.merge_left_start += 2 * state.merge_curr_size;
            state.merge_buffer.clear();
            
        } else {
            state.merge_curr_size *= 2;
            state.merge_left_start = 0;
        }
    } else {
        state.log_history.push_back("[Merge] Очищення буфера пам'яті...");
        state.merge_buffer.clear(); 
        
        EndSort(state);
    }
}

void QuickSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // 1. Ініціалізація
    if (!state.qs_initialized) {
        state.qs_stack.push_back({0, n - 1});
        state.qs_initialized = true;
        state.qs_is_partitioning = false;
    }

    // 2. Partitioning
    if (state.qs_is_partitioning) {
        if (state.qs_j <= state.qs_high - 1) {
            state.highlight_1 = state.qs_j;
            state.highlight_2 = state.qs_high;
            state.is_swapping = false;

            bool condition = state.sort_descending ? (state.arr[state.qs_j] > state.qs_pivot) : (state.arr[state.qs_j] < state.qs_pivot);
            if (condition) {
                state.qs_i++;
                state.is_swapping = true;
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Quick] Міняємо " + std::to_string(state.arr[state.qs_i]) + " та " + std::to_string(state.arr[state.qs_j]));
                }
                std::swap(state.arr[state.qs_i], state.arr[state.qs_j]);
                
                state.highlight_1 = state.qs_i;
                state.highlight_2 = state.qs_j;
            }
            state.qs_j++;
        } 
        else {
            state.qs_i++;
            std::swap(state.arr[state.qs_i], state.arr[state.qs_high]);
            
            state.is_swapping = true;
            state.highlight_1 = state.qs_i;
            state.highlight_2 = state.qs_high;
            
            if (state.detailed_logs) {
                state.log_history.push_back("[Quick] Опорний елемент " + std::to_string(state.arr[state.qs_i]) + " став на своє місце!");
            }

            if (state.qs_i - 1 > state.qs_low) {
                state.qs_stack.push_back({state.qs_low, state.qs_i - 1});
            }
            if (state.qs_i + 1 < state.qs_high) {
                state.qs_stack.push_back({state.qs_i + 1, state.qs_high});
            }

            state.qs_is_partitioning = false;
        }
    } 
    // 3. Отримання нового завдання зі стеку
    else {
        if (!state.qs_stack.empty()) {
            auto bounds = state.qs_stack.back();
            state.qs_stack.pop_back();

            state.qs_low = bounds.first;
            state.qs_high = bounds.second;
            
            state.qs_pivot = state.arr[state.qs_high];
            
            state.qs_i = state.qs_low - 1;
            state.qs_j = state.qs_low;

            state.qs_is_partitioning = true;
            
            if (state.detailed_logs) {
                state.log_history.push_back("[Quick] Беремо шматок [" + std::to_string(state.qs_low) + "..." + std::to_string(state.qs_high) + "]. Опорний: " + std::to_string(state.qs_pivot));
            }
        } 
        else {
            EndSort(state);
        }
    }
}

void ShellSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // 1. Ініціалізація gap
    if (state.shell_gap == 0 && state.shell_i == 0) {
        state.shell_gap = n / 2;
        state.shell_i = state.shell_gap;
        state.shell_j = state.shell_i;
    }

    if (state.shell_gap > 0) {
        if (state.shell_i < n) {
            
            bool condition = false;
            if (state.shell_j >= state.shell_gap) {
                condition = state.sort_descending ? (state.arr[state.shell_j - state.shell_gap] < state.arr[state.shell_j]) : (state.arr[state.shell_j - state.shell_gap] > state.arr[state.shell_j]);
            }
            if (state.shell_j >= state.shell_gap && condition) {
                
                state.highlight_1 = state.shell_j - state.shell_gap;
                state.highlight_2 = state.shell_j;
                state.is_swapping = true;
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Shell] Міняємо на відстані " + std::to_string(state.shell_gap) + 
                        ": " + std::to_string(state.arr[state.shell_j - state.shell_gap]) + 
                        " та " + std::to_string(state.arr[state.shell_j]));
                }
                std::swap(state.arr[state.shell_j - state.shell_gap], state.arr[state.shell_j]);
                
                state.shell_j -= state.shell_gap;
            } 
            else {
                state.is_swapping = false;
                state.shell_i++;
                state.shell_j = state.shell_i;
                
                if (state.shell_i < n) {
                    state.highlight_1 = state.shell_i;
                    state.highlight_2 = state.shell_i;
                }
            }
        } 
        else {
            state.shell_gap /= 2;
            
            if (state.shell_gap > 0) {
                state.shell_i = state.shell_gap;
                state.shell_j = state.shell_i;
                if (state.detailed_logs) {
                    state.log_history.push_back("[Shell] Зменшуємо відстань кроку до: " + std::to_string(state.shell_gap));
                }
            }
        }
    } 
    else {
        EndSort(state);
    }
}

void CocktailShakerSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // 1. Ініціалізація
    if (!state.shaker_initialized) {
        state.shaker_left = 0;
        state.shaker_right = n - 1;
        state.shaker_i = 0;
        state.shaker_dir = 1;
        state.shaker_swapped = false;
        state.shaker_initialized = true;
    }

    if (state.shaker_left >= state.shaker_right) {
        EndSort(state);
        return;
    }

    // Рух вправо
    if (state.shaker_dir == 1) {
        if (state.shaker_i < state.shaker_right) {
            state.highlight_1 = state.shaker_i;
            state.highlight_2 = state.shaker_i + 1;
            state.is_swapping = false;

            bool condition = state.sort_descending ? (state.arr[state.shaker_i] < state.arr[state.shaker_i + 1]) : (state.arr[state.shaker_i] > state.arr[state.shaker_i + 1]);
            if (condition) {
                state.is_swapping = true;
                std::swap(state.arr[state.shaker_i], state.arr[state.shaker_i + 1]);
                state.shaker_swapped = true;

                if (state.detailed_logs) {
                    state.log_history.push_back("[Shaker] Вправо: міняємо " + std::to_string(state.arr[state.shaker_i]) + " та " + std::to_string(state.arr[state.shaker_i+1]));
                }
            }
            state.shaker_i++;
        } else {
            if (!state.shaker_swapped) {
                EndSort(state);
                return;
            }
            state.shaker_right--;
            state.shaker_dir = -1;
            state.shaker_i = state.shaker_right;
            state.shaker_swapped = false;
        }
    }
    // Рух вліво
    else if (state.shaker_dir == -1) {
        if (state.shaker_i > state.shaker_left) {
            state.highlight_1 = state.shaker_i - 1;
            state.highlight_2 = state.shaker_i;
            state.is_swapping = false;

            bool condition = state.sort_descending ? (state.arr[state.shaker_i - 1] < state.arr[state.shaker_i]) : (state.arr[state.shaker_i - 1] > state.arr[state.shaker_i]);
            if (condition) {
                state.is_swapping = true;
                std::swap(state.arr[state.shaker_i - 1], state.arr[state.shaker_i]);
                state.shaker_swapped = true;

                if (state.detailed_logs) {
                    state.log_history.push_back("[Shaker] Вліво: міняємо " + std::to_string(state.arr[state.shaker_i-1]) + " та " + std::to_string(state.arr[state.shaker_i]));
                }
            }
            state.shaker_i--;
        } else {
            if (!state.shaker_swapped) {
                EndSort(state);
                return;
            }
            state.shaker_left++;
            state.shaker_dir = 1;
            state.shaker_i = state.shaker_left;
            state.shaker_swapped = false;
        }
    }
}

void HeapSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // 1. Ініціалізація
    if (!state.heap_initialized) {
        state.heap_i = n / 2 - 1;
        state.heap_phase_build = true;
        state.heap_in_heapify = false;
        state.heap_initialized = true;
    }

    // 2. Heapify
    if (state.heap_in_heapify) {
        int i = state.heap_current_root;
        int size = state.heap_current_n;
        int largest = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;

        state.highlight_1 = i;
        state.highlight_2 = -1;
        state.is_swapping = false;

        if (l < size) {
            bool condition = state.sort_descending ? (state.arr[l] < state.arr[largest]) : (state.arr[l] > state.arr[largest]);
            if (condition) largest = l;
        }
        if (r < size) {
            bool condition = state.sort_descending ? (state.arr[r] < state.arr[largest]) : (state.arr[r] > state.arr[largest]);
            if (condition) largest = r;
        }

        if (largest != i) {
            if (state.arr[i] != state.arr[largest] || i != largest) {
                state.highlight_2 = largest;
                state.is_swapping = true;
                std::swap(state.arr[i], state.arr[largest]);
            }
            state.heap_current_root = largest;
            return;
        } else {
            state.heap_in_heapify = false;
        }
    }

    if (!state.heap_in_heapify) {
        // ФАЗА 1: Побудова купи (Build Heap)
        if (state.heap_phase_build) {
            if (state.heap_i >= 0) {
                state.heap_current_root = state.heap_i;
                state.heap_current_n = n;
                state.heap_in_heapify = true;
                state.heap_i--;
                return;
            } else {
                state.heap_phase_build = false;
                state.heap_i = n - 1;
                if (state.detailed_logs) state.log_history.push_back("[Heap] Купа побудована. Починаємо сортування.");
            }
        }

        // ФАЗА 2: Екстракція (Extract Max)
        if (!state.heap_phase_build) {
            if (state.heap_i > 0) {
                if (state.heap_i != 0) {
                    std::swap(state.arr[0], state.arr[state.heap_i]);
                    state.highlight_1 = 0;
                    state.highlight_2 = state.heap_i;
                    state.is_swapping = true;
                }

                state.heap_current_root = 0;
                state.heap_current_n = state.heap_i;
                state.heap_in_heapify = true;
                state.heap_i--;
                return;
            } else {
                EndSort(state);
            }
        }
    }
}

void EndSort(AppState& state) {
    state.is_sorting = false;
    state.highlight_1 = -1;
    state.highlight_2 = -1;
    
    state.is_animating_finish = true;
    state.finish_anim_index = 0;
    
    std::string final_arr_str = "";
    for (int val : state.arr) {
        final_arr_str += std::to_string(val) + " ";
    }
    state.log_history.push_back("[Data] Відсортований масив: " + final_arr_str);
    state.log_history.push_back("[Success] Сортування успішно завершено!");
}

void ClearStates(AppState& state) {
    state.is_sorted = false;
    state.highlight_1 = -1;
    state.highlight_2 = -1;

    state.bubble_i = 0;
    state.bubble_j = 0;

    state.sel_i = 0;
    state.sel_j = 1;

    state.ins_i = 1;
    state.ins_j = 1;
    state.sel_min_idx = 0;
    
    state.merge_curr_size = 1;
    state.merge_left_start = 0;
    state.merge_buffer.clear();

    state.qs_stack.clear();
    state.qs_initialized = false;
    state.qs_is_partitioning = false;
    state.qs_low = 0;
    state.qs_high = 0;
    state.qs_i = 0;
    state.qs_j = 0;
    state.qs_pivot = 0;

    state.shell_gap = 0;
    state.shell_i = 0;
    state.shell_j = 0;
    
    state.shaker_initialized = false;
    state.shaker_left = 0;
    state.shaker_right = 0;
    state.shaker_i = 0;
    state.shaker_dir = 1;
    state.shaker_swapped = false;

    state.heap_initialized = false;
    state.heap_phase_build = true;
    state.heap_in_heapify = false;
    state.heap_i = 0;
    state.heap_current_root = 0;
    state.heap_current_n = 0;
}