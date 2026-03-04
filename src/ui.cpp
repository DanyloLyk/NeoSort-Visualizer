#include "imgui.h"
#include "ui.h"
#include "algorithms.h"
#include "visualizer.h"
#include <vector>
#include <algorithm>
#include <sstream>

void RenderUI(AppState& state) {
    // --- ТАЙМЕР АНІМАЦІЇ ---
    double current_time = ImGui::GetTime(); // Отримуємо час від запуску програми (в секундах)
    
    // Базова затримка = 0.05 секунди. 
    // Якщо спід = 1.0 (норма) -> затримка 0.05с.
    // Якщо спід = 5.0 (макс) -> затримка 0.01с (літає).
    // Якщо спід = 0.1 (мін) -> затримка 0.5с (повзе).
    double delay = 0.1 / state.speed; 
    if(state.speed == 10.0) delay = 0;

    // Робимо крок алгоритму ТІЛЬКИ якщо ми сортуємо І пройшло достатньо часу
    // Робимо крок алгоритму ТІЛЬКИ якщо ми сортуємо І пройшло достатньо часу
    if (state.is_sorting && (current_time - state.last_step_time) >= delay) {
        
        // Викликаємо потрібний алгоритм залежно від вибору в меню
        if (state.current_algo == 0) {
            SelectionSort(state);
        } else if (state.current_algo == 1) {
            InsertionSort(state);
        } else if (state.current_algo == 2) {
            BubbleSort(state); // Бульбашка тепер третя в списку (індекс 2)
        } else if (state.current_algo == 3) {
            MergeSort(state);
        }
        
        state.last_step_time = current_time; 
    }
    
    // --- ТАЙМЕР ЗЕЛЕНОЇ ХВИЛІ ---
    if (state.is_animating_finish) {
        // Хвиля біжить швидко (кожні 0.02 секунди)
        if (current_time - state.last_anim_time >= 0.02) {
            state.finish_anim_index++;
            state.last_anim_time = current_time;
            
            // Якщо хвиля дійшла до кінця
            if (state.finish_anim_index >= state.arr.size()) {
                state.is_animating_finish = false;
                state.is_sorted = true;          // Фіксуємо зелений колір!
                state.show_success_popup = true; // Викликаємо вікно!
            }
        }
    }

    // --- ГОЛОВНЕ ВІКНО НА ВЕСЬ ЕКРАН ---
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    // Робимо вікно без заголовка, рамок і можливості його тягати
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    
    ImGui::Begin("Головний робочий простір", nullptr, window_flags);

        // ==========================================
        // 1. ЛІВА ПАНЕЛЬ (Керування)
        // ==========================================
        ImGui::BeginChild("LeftPanel", ImVec2(300, 0), true);
        ImGui::Text("Керування алгоритмом");
        ImGui::Separator();
        ImGui::Spacing();
        
        if(ImGui::Button(state.is_sorted ? "Відсортувати заново" : "Старт", ImVec2(-1, 30))) {
            // Якщо масив вже був зелений, скидаємо прогрес, щоб "перепрогнати" його
            if (state.is_sorted) {
                state.arr = state.initial_arr;
                ClearStates(state);
            }
            state.is_sorting = true;
            state.log_history.push_back("[System] Сортування запущено!");
        }
        if(ImGui::Button("Пауза", ImVec2(-1, 30))) {
            state.is_sorting = false;
            state.log_history.push_back("[System] Пауза...");
        }
        if(ImGui::Button("Згенерувати масив", ImVec2(-1, 30))) {
            GenerateArray(state);
        }
        
        ImGui::Spacing(); ImGui::Spacing();
        
        // Змінні для зберігання поточного вибору (static, щоб не скидались кожен кадр)
        const char* algos[] = { 
            "Selection Sort (Лаба 1)", 
            "Insertion Sort (Лаба 2)", 
            "Bubble Sort (Лаба 3)", 
            "Merge Sort (Лаба 4)", 
            "Quick Sort (Лаба 5)", 
            "Shell Sort (Лаба 6)", 
            "Cocktail Shaker (Лаба 7)" 
        };
        ImGui::Text("Алгоритм:");
        if(ImGui::Combo("##algo", &state.current_algo, algos, IM_ARRAYSIZE(algos))) { // ## приховує стандартний підпис збоку
            if (!state.is_sorting) {
                state.arr = state.initial_arr;
                ClearStates(state);
            }
        } 
        
        ImGui::Spacing();
        
        const char* visuals[] = { "Орбіти", "Світловий спектр", "Стовпчики", "Звичайні числа" };
        ImGui::Text("Візуалізація:");
        ImGui::Combo("##vis", &state.current_vis, visuals, IM_ARRAYSIZE(visuals));
        
        ImGui::Spacing();
                
        const char* systems[] = { "Десяткова (Dec)", "Шістнадцяткова (Hex)", "Двійкова (Bin)", "Вісімкова (Oct)" };
        ImGui::Text("Система числення:");
        ImGui::Combo("##sys", &state.current_sys, systems, IM_ARRAYSIZE(systems));

        ImGui::Spacing();

        ImGui::Text("Розмір масиву:");
        ImGui::InputInt("##arr_size", &state.array_size, 1, 10);
        // Для чисел макс 20, для решти макс 200
        int max_size = (state.current_vis == 3) ? 20 : 200; 
        
        if (state.array_size < 5) state.array_size = 5;
        if (state.array_size > max_size) {
            state.array_size = max_size;
            if (state.arr.size() > max_size) {
                state.arr.resize(max_size);
                state.initial_arr.resize(max_size);
            }
        }
        
        ImGui::Spacing();

        ImGui::Text("Мінімальне значення:");
        ImGui::InputInt("##min_value", &state.min_value, 1, 10);

        ImGui::Spacing();

        ImGui::Text("Максимальне значення:");
        ImGui::InputInt("##max_value", &state.max_value, 1, 10);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Свій масив (через пробіл):");
        // Поле для вводу тексту
        ImGui::InputText("##custom_arr", state.custom_array_input, IM_ARRAYSIZE(state.custom_array_input));
        
        if (ImGui::Button("Застосувати свій масив", ImVec2(-1, 30))) {
            OwnArray(state);
        }

        ImGui::EndChild();
        
        // --- СПЛИВАЮЧЕ ВІКНО УСПІХУ ---
        if (state.show_success_popup) {
            ImGui::OpenPopup("Успіх!"); // Кажемо ImGui відкрити вікно
            state.show_success_popup = false; // Одразу вимикаємо тригер, щоб не відкривало нескінченно
        }

        // Налаштування самого спливаючого вікна (модальне - блокує все інше, поки не закриєш)
        ImGuiWindowFlags popup_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
        if (ImGui::BeginPopupModal("Успіх!", NULL, popup_flags)) {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "🎉 ВІТАЄМО!");
            ImGui::Text("Алгоритм %s успішно завершив роботу!", algos[state.current_algo]);
            ImGui::Separator();
            ImGui::Text("Розмір масиву: %d елементів", (int)state.arr.size());
            ImGui::Spacing();
            
            // Кнопка закриття по центру
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) * 0.5f);
            if (ImGui::Button("Супер!", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        // ------------------------------

        ImGui::SameLine(); // Не переносити на новий рядок!

        // ==========================================
        // 2. ПРАВА ЗОНА (Візуалізатор + Код + Логи)
        // ==========================================
        ImGui::BeginGroup();
            
            // --- ВЕРХНІЙ БЛОК (Полотно + Код) ---
            ImGui::BeginChild("TopRightArea", ImVec2(0, -200), false); // false, бо рамка буде у внутрішніх вікон
                
                // Отримуємо доступну ширину і віддаємо 70% під візуалізатор
                float visualizer_width = ImGui::GetContentRegionAvail().x * 0.7f;
                
                // Полотно візуалізатора (Ліворуч)
                ImGui::BeginChild("VisualizerArea", ImVec2(visualizer_width, 0), true);
                
                // Якщо масив не пустий, починаємо малювати
                if (!state.arr.empty()) {
                    VisualChoice(state.current_vis, state);        
                } else {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Згенеруй масив!");
                }

                ImGui::EndChild();

                ImGui::SameLine();

            // Панель коду (Праворуч)
                // Додаємо прапорець ImGuiWindowFlags_NoBackground, щоб ми могли самі намалювати фон (якщо захочемо)
                ImGui::BeginChild("CodeArea", ImVec2(0, 0), true);
                
                // --- МАЛЮЄМО "ШАПКУ" ЯК У macOS ---
                // Отримуємо доступ до списку малювання
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                // Отримуємо позицію курсора (лівий верхній кут цього дочірнього вікна)
                ImVec2 p = ImGui::GetCursorScreenPos();
                
                // Малюємо три кружечки (червоний, жовтий, зелений)
                // IM_COL32 - це макрос для створення кольору (R, G, B, Alpha)
                draw_list->AddCircleFilled(ImVec2(p.x + 12, p.y + 12), 6.0f, IM_COL32(255, 95, 86, 255));  // Red
                draw_list->AddCircleFilled(ImVec2(p.x + 32, p.y + 12), 6.0f, IM_COL32(255, 189, 46, 255)); // Yellow
                draw_list->AddCircleFilled(ImVec2(p.x + 52, p.y + 12), 6.0f, IM_COL32(39, 201, 63, 255));  // Green
                
                // Відступаємо місце після кружечків
                ImGui::Dummy(ImVec2(0, 25)); 
                // ----------------------------------

                ImGui::Text(" Файл: bubble_sort.cpp");
                ImGui::Separator();
                ImGui::Spacing();

                // Визначаємо круті кольори для коду
                ImVec4 col_keyword = ImVec4(0.86f, 0.44f, 0.84f, 1.0f); // Фіолетовий (void, for, if)
                ImVec4 col_type    = ImVec4(0.34f, 0.65f, 0.92f, 1.0f); // Синій (int)
                ImVec4 col_active  = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);    // Яскраво-жовтий (активний рядок)
                ImVec4 col_normal  = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);    // Білий
                ImVec4 col_green   = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);    // Зелений

                // Імітація підсвічування коду. 
                // Щоб зробити різні кольори в одному рядку, використовуємо ImGui::SameLine()
                
                // void bubbleSort(int arr[], int n) {
                ImGui::TextColored(col_keyword, "void "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "bubbleSort("); ImGui::SameLine();
                ImGui::TextColored(col_type, "int "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine();
                ImGui::TextColored(col_type, "int "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "n) {");
                
                // for (int i = 0; i < n - 1; i++) {
                ImGui::Text("  "); ImGui::SameLine(); // Відступ
                ImGui::TextColored(col_keyword, "for "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "("); ImGui::SameLine();
                ImGui::TextColored(col_type, "int "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "i = 0; i < n - 1; i++) {");

                // for (int j = 0; j < n - i - 1; j++) {
                ImGui::Text("    "); ImGui::SameLine();
                ImGui::TextColored(col_keyword, "for "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "("); ImGui::SameLine();
                ImGui::TextColored(col_type, "int "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "j = 0; j < n - i - 1; j++) {");
                
                ImGui::Spacing();
                // АКТИВНИЙ РЯДОК (Світиться жовтим)
                // if (arr[j] > arr[j + 1]) {
                ImGui::Text("      "); ImGui::SameLine();
                ImGui::TextColored(col_active, "if (arr[j] > arr[j + 1]) { "); ImGui::SameLine();
                ImGui::TextColored(col_green, "// ПОРІВНЯННЯ"); 
                
                // swap(arr[j], arr[j + 1]);
                ImGui::Text("        "); ImGui::SameLine();
                ImGui::TextColored(col_normal, "swap(arr[j], arr[j + 1]);");

                ImGui::TextColored(col_normal, "      }");
                ImGui::TextColored(col_normal, "    }");
                ImGui::TextColored(col_normal, "  }");
                ImGui::TextColored(col_normal, "}");

                ImGui::EndChild();
            ImGui::EndChild(); // Кінець верхнього блоку

            // --- НИЖНІЙ БЛОК (Логи та Швидкість) ---
            ImGui::BeginChild("LogArea", ImVec2(0, 0), true);
            
            // --- ЗАКРІПЛЕНА ШАПКА ---
            ImGui::Text("Швидкість анімації:");
            // Робимо повзунок трохи вужчим, щоб влізла галочка
            ImGui::PushItemWidth(700); 
            ImGui::SliderFloat("##speed", &state.speed, 0.1f, 10.0f, "x%.1f"); 
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            ImGui::Checkbox("Детальні логи", &state.detailed_logs);
            ImGui::Separator();
            
            // --- ЗОНА ПРОКРУТКИ ЛОГІВ ---
            // false - без рамки, ImGuiWindowFlags_AlwaysVerticalScrollbar - завжди показувати скрол
            ImGui::BeginChild("LogScroll", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[System] Візуалізатор успішно запущено.");
            ImGui::Text("[Action] Обрано алгоритм: %s", algos[state.current_algo]); 
            ImGui::Text("[Action] Обрано візуалізацію: %s", visuals[state.current_vis]);
            
            // Виводимо наш зафіксований початковий масив
            if (!state.initial_array_str.empty()) {
                ImGui::Text("[Data] Початковий масив: %s", state.initial_array_str.c_str());
            }

            // Виводимо всю історію кроків
            for (const auto& log_msg : state.log_history) {
                ImGui::Text("%s", log_msg.c_str());
            }
            
            // Автоматична прокрутка до самого низу, коли додаються нові логи
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
            
            ImGui::EndChild(); // Кінець зони прокрутки
            ImGui::EndChild(); // Кінець загальної зони логів

        ImGui::EndGroup();
    ImGui::End();
}