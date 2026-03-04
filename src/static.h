#pragma once
#include <vector>
#include <string>

struct AppState {
    // ==========================================
    // Змінні для базового налаштування та базової побудови
    // ==========================================
    int current_algo = 0;
    int current_vis = 0;
    int current_sys = 0;
    
    float speed = 10.0f;
    double last_step_time = 0.0;
    int array_size = 25; // Розмір нашого масиву (за замовчуванням 50)
    int min_value = -100;
    int max_value = 100;
    
    std::vector<int> arr; // Сам масив, який ми будемо сортувати
    
    std::string initial_array_str = ""; // Тут буде зберігатися масив текстом
    std::vector<int> initial_arr; // Наша резервна копія!

    std::vector<std::string> log_history; // Історія наших кроків
    bool detailed_logs = false; // Чи включені детальні логи

    // ==========================================
    // Змінні для керування алгоритмом
    // ==========================================
    bool is_sorting = false;
    bool is_swapping = false;
    bool is_sorted = false;           // Чи масив вже відсортований
    bool show_success_popup = false;  // Тригер для спливаючого вікна

    // Індекси для циклів Bubble Sort (заміна i та j)
    int bubble_i = 0;
    int bubble_j = 0;

    // Індекси елементів, які треба підсвітити жовтим (які зараз порівнюються)
    int highlight_1 = -1;
    int highlight_2 = -1;

    // --- Для ручного вводу ---
    char custom_array_input[512] = ""; // Буфер для тексту (наприклад: "10, 5, 20, 3")

    // --- Для зеленої хвилі (фініш) ---
    bool is_animating_finish = false;
    int finish_anim_index = -1; // Який стовпчик зараз зеленіє
    double last_anim_time = 0.0;
};