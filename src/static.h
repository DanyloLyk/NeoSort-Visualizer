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
    int array_size = 50; // Розмір нашого масиву (за замовчуванням 50)
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
    bool sort_descending = false;     // Чи сортуємо за спаданням чи зростанням

    // Індекси для циклів Bubble Sort (заміна i та j)
    int bubble_i = 0;
    int bubble_j = 0;

    // Індекси для циклів Selection Sort (заміна i та j)
    int sel_i = 0;
    int sel_j = 1;
    int sel_min_idx = 0;   
    
    // Індекси для циклів Insertion Sort (заміна i та j)
    int ins_i = 1;              // Починаємо з 1-го елемента (0-й вже вважається "відсортованим")
    int ins_j = 1;              // Внутрішній індекс, який буде бігти вліво

    // Змінні для Merge Sort (ітеративний підхід) 
    int merge_curr_size = 1;    // Розмір блоків, які ми зараз зливаємо (1, 2, 4, 8...)
    int merge_left_start = 0;   // Початок лівого підмасиву
    int merge_i = 0;            // Внутрішні змінні для процесу злиття (merge)
    int merge_j = 0;            // ="="="="="="="="
    int merge_k = 0;            // ="="="="="="="="
    std::vector<int> merge_buffer; // Тимчасовий масив для злиття

    // Змінні для Quick Sort (ітеративний підхід) 
    std::vector<std::pair<int, int>> qs_stack; // Наш стек для заміни рекурсії
    bool qs_initialized = false;     // Чи закинули ми початковий масив у стек
    bool qs_is_partitioning = false; // Чи ми зараз ділимо масив на дві частини
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
    int shaker_dir = 1; // 1 = йдемо вправо, -1 = йдемо вліво
    bool shaker_swapped = false; // Оптимізація: якщо не було обмінів - сортування завершено

    // --- Змінні для Shell Sort ---
    int shell_gap = 0; // Наш "крок" стрибка
    int shell_i = 0;
    int shell_j = 0;

    // --- Змінні для Heap Sort ---
    bool heap_initialized = false;
    bool heap_phase_build = true; // true = Build Heap, false = Extract
    bool heap_in_heapify = false; // Чи ми зараз всередині процесу просіювання (heapify)?
    int heap_i = 0;               // Лічильник зовнішнього циклу
    int heap_current_root = 0;    // Поточний корінь для heapify
    int heap_current_n = 0;       // Поточний розмір купи для heapify

    // Індекси елементів, які треба підсвітити жовтим (які зараз порівнюються)
    int highlight_1 = -1;
    int highlight_2 = -1;

    // --- Для ручного вводу ---
    char custom_array_input[512] = ""; // Буфер для тексту (наприклад: "10, 5, 20, 3")

    // --- Для зеленої хвилі (фініш) ---
    bool is_animating_finish = false;
    int finish_anim_index = -1; // Який стовпчик зараз зеленіє
    double last_anim_time = 0.0;

    // --- Налаштування візуалу ---
    bool enable_cyber_bg = false;       // Чи малювати матричний фон
    bool enable_elements = false; // Чи робити елементи неоновими та прозорими
};