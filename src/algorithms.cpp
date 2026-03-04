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

void OwnArray(AppState& state) {
    vector<int> new_arr;
    string input(state.custom_array_input);
    
    // Замінюємо коми на пробіли, щоб можна було вводити "10, 20, 30"
    replace(input.begin(), input.end(), ',', ' ');
    
    stringstream ss(input);
    string temp;
    
    // Розбиваємо рядок на числа і пушимо в новий масив
    while (ss >> temp) {
        try {
            new_arr.push_back(std::stoi(temp)); // Конвертуємо текст у число
        } catch (...) { /* Ігноруємо те, що не є числом (букви і т.д.) */ }
    }
    
    // Якщо вдалося знайти хоч одне число, оновлюємо масив!
    if (!new_arr.empty()) {
        state.arr = new_arr;
        state.initial_arr = new_arr;
        state.array_size = new_arr.size(); // Оновлюємо повзунок розміру
        
        // Скидаємо стани
        state.is_animating_finish = false;
        state.is_sorting = false;
        ClearStates(state);
        
        // Оновлюємо логи
        state.initial_array_str = input;
        state.log_history.clear();
        state.log_history.push_back("[System] Завантажено власний масив (" + std::to_string(state.array_size) + " елементів)");
    }
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
            
            state.bubble_j++; // Крокуємо далі
        } else {
            // Внутрішній цикл дійшов до кінця, скидаємо j, збільшуємо i
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
            
            // Підсвічуємо поточний знайдений мінімум (жовтим) і елемент, який перевіряємо зараз
            state.highlight_1 = state.sel_min_idx;
            state.highlight_2 = state.sel_j;
            state.is_swapping = false; // Поки що тільки шукаємо, не стрибаємо

            // Якщо знайшли число, яке ще менше за наш поточний мінімум — запам'ятовуємо його індекс!
            bool condition = state.sort_descending ? (state.arr[state.sel_j] > state.arr[state.sel_min_idx]) : (state.arr[state.sel_j] < state.arr[state.sel_min_idx]);
            if (condition) {
                state.sel_min_idx = state.sel_j;
            }
            state.sel_j++; 
            
        } else {
            // Ми дійшли до кінця масиву і точно знаємо, де найменше число.
            // Тепер міняємо його місцями з початком нашої невідсортованої частини (sel_i)
            state.highlight_1 = state.sel_i;
            state.highlight_2 = state.sel_min_idx;
            if (state.sel_min_idx != state.sel_i) {
                state.is_swapping = true; // Робимо красивий стрибок!
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Selection] Знайшли мінімум: " + 
                        std::to_string(state.arr[state.sel_min_idx]) + 
                        ". Міняємо з " + std::to_string(state.arr[state.sel_i]));
                }
                std::swap(state.arr[state.sel_i], state.arr[state.sel_min_idx]);
            } else {
                state.is_swapping = false; // Число вже на своєму місці
            }

            // Переходимо до наступного кроку зовнішнього циклу
            state.sel_i++;
            state.sel_min_idx = state.sel_i; // Новий пошук починаємо з наступного елемента
            state.sel_j = state.sel_i + 1;
        }
    } else {
        EndSort(state);    
    }
}

void InsertionSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // Зовнішній цикл: беремо елементи від 1 до кінця
    if (state.ins_i < n) {
        
        // Внутрішній цикл: тягнемо елемент ВЛІВО, поки він менший за свого лівого сусіда
        bool condition = false;
        if (state.ins_j > 0) { // СПОЧАТКУ перевіряємо, чи ми не вийшли за межі
            condition = state.sort_descending ? (state.arr[state.ins_j - 1] < state.arr[state.ins_j]) : (state.arr[state.ins_j - 1] > state.arr[state.ins_j]);
        }
        if (state.ins_j > 0 && condition) {
            
            // Підсвічуємо поточну пару (вони сусідні)
            state.highlight_1 = state.ins_j - 1;
            state.highlight_2 = state.ins_j;
            state.is_swapping = true; // Робимо стрибок!

            if (state.detailed_logs) {
                state.log_history.push_back("[Insertion] Тягнемо вліво: міняємо " + 
                    std::to_string(state.arr[state.ins_j - 1]) + " та " + 
                    std::to_string(state.arr[state.ins_j]));
            }
            // Міняємо місцями
            std::swap(state.arr[state.ins_j - 1], state.arr[state.ins_j]);
            
            // Крокуємо далі вліво
            state.ins_j--;
        } 
        else {
            // Елемент знайшов своє місце! Переходимо до наступного (ins_i++)
            state.is_swapping = false;
            
            state.ins_i++;
            state.ins_j = state.ins_i; // Новий елемент почне свій шлях вліво зі своєї позиції
            
            // Просто підсвітимо жовтим елемент, який ми зараз готуємось перевіряти
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

    // Перевіряємо, чи ми ще не відсортували весь масив
    if (state.merge_curr_size < n) {
        
        if (state.merge_left_start < n - 1) {
            // Визначаємо межі для злиття
            int left = state.merge_left_start;
            int mid = std::min(left + state.merge_curr_size - 1, n - 1);
            int right = std::min(left + 2 * state.merge_curr_size - 1, n - 1);

            // Якщо ми тільки починаємо нове злиття, готуємо буфер
            if (state.merge_buffer.empty()) {
                state.merge_i = left;
                state.merge_j = mid + 1;
                state.merge_k = left;
                state.merge_buffer = state.arr; // Робимо копію поточного масиву
            }

            // САМ ПРОЦЕС ЗЛИТТЯ (покроково)
            if (state.merge_i <= mid && state.merge_j <= right) { 
                // Підсвічуємо елементи, які зараз порівнюємо
                state.highlight_1 = state.merge_i;
                state.highlight_2 = state.merge_j;
                state.is_swapping = false; // У Merge Sort немає класичних стрибків
                
                bool condition = state.sort_descending ? (state.merge_buffer[state.merge_i] >= state.merge_buffer[state.merge_j]) : (state.merge_buffer[state.merge_i] <= state.merge_buffer[state.merge_j]);

                if (condition) {
                    state.arr[state.merge_k] = state.merge_buffer[state.merge_i];
                    state.merge_i++;
                } else {
                    state.arr[state.merge_k] = state.merge_buffer[state.merge_j];
                    state.merge_j++;
                }
                
                // Якщо увімкнені логи, спамимо
                if (state.detailed_logs) {
                    state.log_history.push_back("[Merge] Записуємо " + std::to_string(state.arr[state.merge_k]) + " на позицію " + std::to_string(state.merge_k));
                }
                
                state.merge_k++;
                return; // Чекаємо наступного кадру!
            }

            // Дописуємо залишки з лівої частини (якщо є)
            if (state.merge_i <= mid) {
                state.highlight_1 = state.merge_i;
                state.highlight_2 = -1;
                state.arr[state.merge_k] = state.merge_buffer[state.merge_i];
                state.merge_i++;
                state.merge_k++;
                return; // Чекаємо наступного кадру
            }
            
            // Дописуємо залишки з правої частини (якщо є)
            if (state.merge_j <= right) {
                state.highlight_1 = -1;
                state.highlight_2 = state.merge_j;
                state.arr[state.merge_k] = state.merge_buffer[state.merge_j];
                state.merge_j++;
                state.merge_k++;
                return; // Чекаємо наступного кадру
            }

            // Злиття цього блоку закінчено! 
            // Переходимо до наступної пари блоків
            state.merge_left_start += 2 * state.merge_curr_size;
            state.merge_buffer.clear(); // Очищаємо буфер для наступного злиття
            
        } else {
            // Ми пройшли весь масив з поточним розміром блоку.
            // Збільшуємо розмір блоку вдвічі (1 -> 2 -> 4 -> 8...)
            state.merge_curr_size *= 2;
            state.merge_left_start = 0; // Починаємо знову з початку масиву
        }
    } else {
        // Ми відсортували весь масив!
        state.log_history.push_back("[Merge] Очищення буфера пам'яті...");
        state.merge_buffer.clear(); 
        
        EndSort(state); // Викликаємо твою круту функцію!
    }
}

void QuickSort(AppState& state) {
    if (!state.is_sorting || state.arr.size() < 2) return;

    int n = (int)state.arr.size();

    // 1. Ініціалізація: закидаємо весь масив (від 0 до n-1) у стек як перше "завдання"
    if (!state.qs_initialized) {
        state.qs_stack.push_back({0, n - 1});
        state.qs_initialized = true;
        state.qs_is_partitioning = false;
    }

    // 2. Якщо ми зараз в процесі розділення (partition)
    if (state.qs_is_partitioning) {
        
        // Поки j не дійшов до кінця нашого шматка
        if (state.qs_j <= state.qs_high - 1) {
            state.highlight_1 = state.qs_j;
            state.highlight_2 = state.qs_high; // Опорний елемент завжди підсвічений!
            state.is_swapping = false;

            // Якщо поточний елемент менший за опорний
            bool condition = state.sort_descending ? (state.arr[state.qs_j] > state.qs_pivot) : (state.arr[state.qs_j] < state.qs_pivot);
            if (condition) {
                state.qs_i++;
                state.is_swapping = true; // Робимо стрибок
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Quick] Міняємо " + std::to_string(state.arr[state.qs_i]) + " та " + std::to_string(state.arr[state.qs_j]));
                }
                std::swap(state.arr[state.qs_i], state.arr[state.qs_j]);
                
                // Перевизначаємо підсвітку для красивого стрибка
                state.highlight_1 = state.qs_i;
                state.highlight_2 = state.qs_j;
            }
            state.qs_j++;
        } 
        else {
            // Дійшли до кінця: ставимо опорний елемент на його ЗАКОННЕ місце
            state.qs_i++;
            std::swap(state.arr[state.qs_i], state.arr[state.qs_high]);
            
            state.is_swapping = true; // Стрибок опорного елемента
            state.highlight_1 = state.qs_i;
            state.highlight_2 = state.qs_high;
            
            if (state.detailed_logs) {
                state.log_history.push_back("[Quick] Опорний елемент " + std::to_string(state.arr[state.qs_i]) + " став на своє місце!");
            }

            // Тепер наш масив розділений!
            // Додаємо ліву частину в стек завдань (якщо там більше 1 елемента)
            if (state.qs_i - 1 > state.qs_low) {
                state.qs_stack.push_back({state.qs_low, state.qs_i - 1});
            }
            // Додаємо праву частину в стек
            if (state.qs_i + 1 < state.qs_high) {
                state.qs_stack.push_back({state.qs_i + 1, state.qs_high});
            }

            state.qs_is_partitioning = false; // Завдання виконано
        }
    } 
    // 3. Якщо ми не ділимо, значить треба взяти нове завдання зі стеку
    else {
        if (!state.qs_stack.empty()) {
            // Беремо останнє завдання
            auto bounds = state.qs_stack.back();
            state.qs_stack.pop_back();

            // Розпаковуємо межі
            state.qs_low = bounds.first;
            state.qs_high = bounds.second;
            
            // Встановлюємо опорний елемент (останній у шматку)
            state.qs_pivot = state.arr[state.qs_high];
            
            // Налаштовуємо індекси для нового проходу
            state.qs_i = state.qs_low - 1;
            state.qs_j = state.qs_low;

            state.qs_is_partitioning = true; // Запускаємо процес
            
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

    // 1. Ініціалізація: починаємо з кроку (gap), який дорівнює половині масиву
    if (state.shell_gap == 0 && state.shell_i == 0) {
        state.shell_gap = n / 2;
        state.shell_i = state.shell_gap;
        state.shell_j = state.shell_i;
    }

    // 2. Поки крок більше нуля
    if (state.shell_gap > 0) {
        
        // Йдемо по масиву
        if (state.shell_i < n) {
            
            // Порівнюємо елементи на відстані 'gap'
            bool condition = false;
            if (state.shell_j >= state.shell_gap) {
                condition = state.sort_descending ? (state.arr[state.shell_j - state.shell_gap] < state.arr[state.shell_j]) : (state.arr[state.shell_j - state.shell_gap] > state.arr[state.shell_j]);
            }
            if (state.shell_j >= state.shell_gap && condition) {
                
                state.highlight_1 = state.shell_j - state.shell_gap;
                state.highlight_2 = state.shell_j;
                state.is_swapping = true; // Робимо довгий красивий стрибок!
                
                if (state.detailed_logs) {
                    state.log_history.push_back("[Shell] Міняємо на відстані " + std::to_string(state.shell_gap) + 
                        ": " + std::to_string(state.arr[state.shell_j - state.shell_gap]) + 
                        " та " + std::to_string(state.arr[state.shell_j]));
                }

                // Міняємо місцями
                std::swap(state.arr[state.shell_j - state.shell_gap], state.arr[state.shell_j]);
                
                // Відступаємо назад на крок gap, щоб перевірити попередні
                state.shell_j -= state.shell_gap;
            } 
            else {
                // Більше обмінів для цього елемента не треба, беремо наступний
                state.is_swapping = false;
                state.shell_i++;
                state.shell_j = state.shell_i;
                
                // Просто підсвічуємо поточний елемент
                if (state.shell_i < n) {
                    state.highlight_1 = state.shell_i;
                    state.highlight_2 = state.shell_i;
                }
            }
        } 
        else {
            // Пройшли весь масив з поточним кроком. Зменшуємо крок вдвічі!
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
        state.shaker_dir = 1; // Починаємо рух вправо
        state.shaker_swapped = false;
        state.shaker_initialized = true;
    }

    // 2. Якщо ліва і права межа зустрілися — масив відсортовано!
    if (state.shaker_left >= state.shaker_right) {
        EndSort(state);
        return;
    }

    // 3. РУХ ВПРАВО (як класична Бульбашка)
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
            // Дійшли до правого краю!
            if (!state.shaker_swapped) {
                EndSort(state); // Якщо по дорозі нікого не поміняли, все готово
                return;
            }
            // Готуємось іти назад
            state.shaker_right--; // Найбільший елемент вже на місці, звужуємо межу
            state.shaker_dir = -1; // Міняємо напрямок на "Вліво"
            state.shaker_i = state.shaker_right; // Ставимо індекс на новий край
            state.shaker_swapped = false; // Скидаємо прапорець для нового проходу
        }
    }
    // 4. РУХ ВЛІВО (тягнемо найменший елемент на початок)
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
            // Дійшли до лівого краю!
            if (!state.shaker_swapped) {
                EndSort(state); // Якщо нічого не змінили - готово
                return;
            }
            // Готуємось іти вперед
            state.shaker_left++; // Найменший елемент на місці, звужуємо межу
            state.shaker_dir = 1; // Міняємо напрямок на "Вправо"
            state.shaker_i = state.shaker_left;
            state.shaker_swapped = false;
        }
    }
}

void EndSort(AppState& state) {
    // СОРТУВАННЯ ЗАВЕРШЕНО!
    state.is_sorting = false;
    state.highlight_1 = -1;
    state.highlight_2 = -1;
    
    // Запускаємо зелену хвилю!
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
}