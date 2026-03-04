#include <iostream>
#include <random>
#include "algorithms.h"

using namespace std;

void GenerateArray(AppState& state) {
    state.arr.clear();
    
    state.is_sorted = false;
    state.is_animating_finish = false;
    state.is_sorting = false;
    state.bubble_i = 0;
    state.bubble_j = 0;
    state.highlight_1 = -1;
    state.highlight_2 = -1;

    random_device rd;
    mt19937 gen(rd());

    // Беремо ПРАВИЛЬНІ значення
    int min_v = state.min_value;
    int max_v = state.max_value;

    if(min_v > max_v) {
        swap(min_v, max_v);
        int temp_val = state.min_value;
        state.min_value = state.max_value;
        state.max_value = temp_val;
    }
    
    // ТУТ ПЕРЕДАЄМО НАШІ БЕЗПЕЧНІ ЛОКАЛЬНІ ЗМІННІ (min_v, max_v)
    uniform_int_distribution<int> dist(min_v, max_v);

    for (int i = 0; i < state.array_size; i++) {
        state.arr.push_back(dist(gen));
    }
    
    // Зберігаємо початковий вигляд масиву як текст
    state.initial_array_str = "";
    for (int i = 0; i < state.arr.size(); i++) {
        state.initial_array_str += std::to_string(state.arr[i]) + " ";
    }
    
    state.initial_arr = state.arr;

    // Додаємо системний лог
    state.log_history.clear(); // Очищаємо старі логи
    state.log_history.push_back("[System] Згенеровано новий масив розміром " + std::to_string(state.array_size));
}

void BubbleSort(AppState& state) {
    // Якщо не сортуємо, або масив замалий — нічого не робимо
    if (!state.is_sorting || state.arr.size() < 2) return;

    int size = (int)state.arr.size();
    // Аналог зовнішнього циклу (for int i = 0...)
    if (state.bubble_i < size - 1) {
        
        // Аналог внутрішнього циклу (for int j = 0...)
        if (state.bubble_j < size - state.bubble_i - 1) {
            
            // Підсвічуємо поточні елементи!
            state.highlight_1 = state.bubble_j;
            state.highlight_2 = state.bubble_j + 1;

            // Саме порівняння і перестановка
            if (state.arr[state.bubble_j] > state.arr[state.bubble_j + 1]) {
                state.is_swapping = true;

                if (state.detailed_logs) {
                    state.log_history.push_back("[Bubble] Переставляємо " + 
                        std::to_string(state.arr[state.bubble_j]) + " та " + 
                        std::to_string(state.arr[state.bubble_j + 1]));
                }       
                std::swap(state.arr[state.bubble_j], state.arr[state.bubble_j + 1]);
            }
            
            state.bubble_j++; // Крокуємо далі
        } else {
            // Внутрішній цикл дійшов до кінця, скидаємо j, збільшуємо i
            state.bubble_j = 0;
            state.bubble_i++;
            state.is_swapping = false;
        }
    } else {
        // СОРТУВАННЯ ЗАВЕРШЕНО!
        state.is_sorting = false;
        state.highlight_1 = -1;
        state.highlight_2 = -1;

        // ЗАПУСКАЄМО ЗЕЛЕНУ ХВИЛЮ!
        state.is_animating_finish = true;
        state.finish_anim_index = 0;
        state.log_history.push_back("[Success] Сортування завершено!");
    }
}
