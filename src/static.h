#pragma once
#include <vector>
#include <string>
#include "imgui.h"

struct AppState {
    // ==========================================
    // Змінні для базового налаштування та базової побудови
    // ==========================================
    int current_algo = 0;
    int current_vis = 1;
    int current_sys = 0;
    
    float speed = 1.0f;
    double last_step_time = 0.0;
    int array_size = 100;
    int min_value = -100;
    int max_value = 100;
    
    std::vector<int> arr;
    
    std::string initial_array_str = "";
    std::vector<int> initial_arr;

    std::vector<std::string> log_history;
    bool detailed_logs = false;

    // ==========================================
    // Змінні для керування алгоритмом
    // ==========================================
    bool is_sorting = false;
    bool is_swapping = false;
    bool is_sorted = false;
    bool show_success_popup = false;
    bool sort_descending = false;

    // Bubble Sort
    int bubble_i = 0;
    int bubble_j = 0;

    // Selection Sort
    int sel_i = 0;
    int sel_j = 1;
    int sel_min_idx = 0;   
    
    // Insertion Sort
    int ins_i = 1;
    int ins_j = 1;

    // Merge Sort
    int merge_curr_size = 1;
    int merge_left_start = 0;
    int merge_i = 0;
    int merge_j = 0;
    int merge_k = 0;
    std::vector<int> merge_buffer;

    // Quick Sort
    std::vector<std::pair<int, int>> qs_stack;
    bool qs_initialized = false;
    bool qs_is_partitioning = false;
    int qs_low = 0;
    int qs_high = 0;
    int qs_i = 0;
    int qs_j = 0;
    int qs_pivot = 0;

    // --- Змінні для Cocktail Shaker Sort ---
    bool shaker_initialized = false;
    int shaker_left = 0;
    int shaker_right = 0;
    int shaker_i = 0;
    int shaker_dir = 1;
    bool shaker_swapped = false;

    // --- Змінні для Shell Sort ---
    int shell_gap = 0;
    int shell_i = 0;
    int shell_j = 0;

    // --- Змінні для Heap Sort ---
    bool heap_initialized = false;
    bool heap_phase_build = true;
    bool heap_in_heapify = false;
    int heap_i = 0;
    int heap_current_root = 0;
    int heap_current_n = 0;

    // Елементи для підсвітки
    int highlight_1 = -1;
    int highlight_2 = -1;

    // --- Для ручного вводу ---
    char custom_array_input[1024] = "";

    // --- Для зеленої хвилі (фініш) ---
    bool is_animating_finish = false;
    int finish_anim_index = -1;
    double last_anim_time = 0.0;

    // --- Налаштування візуалу ---
    bool enable_cyber_bg = false;
    bool enable_elements = false;

    // ==========================================
    // --- ЗМІННІ ДЛЯ АЛГОРИТМІВ ПОШУКУ ---
    // ==========================================
    bool is_searching = false;
    bool is_found = false;
    int search_target = 0;      
    int search_result = -1;     
    
    // Змінні для Лінійного пошуку
    int search_index = 0;       
    
    // Змінні для Бінарного пошуку
    bool is_presorting = false;
    int search_l = 0;           
    int search_r = 0;           
    int search_m = 0;           
    int search_presort_algo = 4; 

    // ==========================================
    // --- АНІМАЦІЯ МИГАННЯ (FLASHING) ---
    // ==========================================
    bool is_animating_search_success = false; 
    bool is_animating_search_fail = false;    
    int flash_count = 0;                      
    double last_flash_time = 0;               
    bool flash_state = false;                 

    // ==========================================
    // --- СИСТЕМА ЧАСТИНОК (КОНФЕТІ) ---
    // ==========================================
    struct Confetti {
        float x, y, vx, vy, size;
        ImU32 color;
    };
    std::vector<Confetti> particles;
    bool search_popup_shown = false; // Щоб вікно пошуку відкрилося лише один раз
};