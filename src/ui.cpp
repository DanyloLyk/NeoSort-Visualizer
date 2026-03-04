#include "imgui.h"
#include "ui.h"
#include "algorithms.h"
#include "visualizer.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Темний кібер-фон (Глибокий синьо-чорний)
    colors[ImGuiCol_WindowBg]       = ImVec4(0.04f, 0.05f, 0.09f, 1.00f);
    colors[ImGuiCol_ChildBg]        = ImVec4(0.04f, 0.05f, 0.09f, 1.00f);
    colors[ImGuiCol_PopupBg]        = ImVec4(0.04f, 0.05f, 0.09f, 0.96f);

    // Неонові рамки (Бірюзове світіння)
    colors[ImGuiCol_Border]         = ImVec4(0.00f, 1.00f, 0.80f, 0.40f);
    colors[ImGuiCol_BorderShadow]   = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Фони полів вводу (Dropdowns, Inputs)
    colors[ImGuiCol_FrameBg]        = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.20f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.00f, 0.50f, 0.80f, 1.00f);

    // Кнопки (Кібер-синій переходить у яскравий Бірюзовий при наведенні)
    colors[ImGuiCol_Button]         = ImVec4(0.00f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonHovered]  = ImVec4(0.00f, 0.60f, 0.80f, 1.00f);
    colors[ImGuiCol_ButtonActive]   = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);

    // Заголовки (Combo box, Radio buttons)
    colors[ImGuiCol_Header]         = ImVec4(0.00f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_HeaderHovered]  = ImVec4(0.00f, 0.60f, 0.80f, 1.00f);
    colors[ImGuiCol_HeaderActive]   = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);

    // Повзунок швидкості (Slider) - робимо його яскравим
    colors[ImGuiCol_SliderGrab]     = ImVec4(0.00f, 1.00f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]= ImVec4(0.00f, 0.80f, 1.00f, 1.00f);
    
    // Смуга прокрутки логів
    colors[ImGuiCol_ScrollbarBg]    = ImVec4(0.02f, 0.02f, 0.04f, 0.50f);
    colors[ImGuiCol_ScrollbarGrab]  = ImVec4(0.00f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.60f, 0.80f, 1.00f);

    // Текст (Світло-блакитний замість чисто білого)
    colors[ImGuiCol_Text]           = ImVec4(0.85f, 0.95f, 1.00f, 1.00f);

    // Геометрія: робимо все трішки кутастим, як у справжніх терміналах
    style.WindowRounding    = 4.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 2.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding      = 2.0f;
    
    // Вмикаємо товстіші рамки, щоб неон було видно
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
}

void RenderUI(AppState& state) {
    static bool last_theme_state = !state.enable_elements; 

    if (state.enable_elements != last_theme_state) {
        if (state.enable_elements) {
            ApplyTheme(); // Заливаємо все неоном!
        } else {
            // Повертаємо дефолтну темну тему ImGui та скидаємо заокруглення
            ImGui::StyleColorsDark(); 
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowRounding = 0.0f;
            style.ChildRounding = 0.0f;
            style.FrameRounding = 0.0f;
            style.PopupRounding = 0.0f;
        }
        last_theme_state = state.enable_elements;
    }

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
        } else if (state.current_algo == 4) {
            QuickSort(state);
        } else if (state.current_algo == 5) {
            ShellSort(state);
        } else if (state.current_algo == 6) {
            CocktailShakerSort(state);
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
        if(ImGui::Combo("##algo", &state.current_algo, algos, IM_ARRAYSIZE(algos))) { 
            state.arr = state.initial_arr;
            state.is_sorting = false;
            state.is_animating_finish = false; // Зупиняємо зелену хвилю, якщо вона була
            ClearStates(state);
            state.log_history.push_back("[System] Алгоритм змінено. Масив скинуто.");
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Напрямок сортування:");
        if (ImGui::RadioButton("Зростання", !state.sort_descending)) {
            state.sort_descending = false;
            // Скидаємо масив, якщо вже відсортовано
            if (!state.is_sorting && state.is_sorted) { state.arr = state.initial_arr; ClearStates(state); }
            state.log_history.push_back("[Action] Напрямок змінено: Зростання");
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Спадання", state.sort_descending)) {
            state.sort_descending = true;
            if (!state.is_sorting && state.is_sorted) { state.arr = state.initial_arr; ClearStates(state); }
            state.log_history.push_back("[Action] Напрямок змінено: Спадання");
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Кібер-Налаштування:");
        ImGui::Checkbox("Хакерський фон", &state.enable_cyber_bg);
        ImGui::Checkbox("Неонові елементи", &state.enable_elements);

        const char* visuals[] = { "Орбіти", "Світловий спектр", "Стовпчики", "Звичайні числа" };
        ImGui::Text("Візуалізація:");
        ImGui::Combo("##vis", &state.current_vis, visuals, IM_ARRAYSIZE(visuals));
        
        ImGui::Spacing();
        ImGui::Separator();
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
                
                // --- BACKGROUND ---
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 v_min = ImGui::GetCursorScreenPos(); 
                ImVec2 v_max = ImVec2(v_min.x + ImGui::GetContentRegionAvail().x, v_min.y + ImGui::GetContentRegionAvail().y);
                
                if (state.enable_cyber_bg) {
                    float t = (float)ImGui::GetTime(); 

                    // 1. АНІМОВАНИЙ ГРАДІЄНТНИЙ ФОН
                    ImU32 col_top_left  = ImGui::GetColorU32(ImVec4(0.01f, 0.01f, 0.03f, 1.0f)); 
                    ImU32 col_top_right = ImGui::GetColorU32(ImVec4(0.05f, 0.01f, 0.05f, 1.0f)); 
                    ImU32 col_bot_left  = ImGui::GetColorU32(ImVec4(0.0f, 0.1f + sin(t*0.5f)*0.05f, 0.15f, 1.0f)); 
                    ImU32 col_bot_right = ImGui::GetColorU32(ImVec4(0.0f, 0.05f, 0.1f, 1.0f));   
                    draw_list->AddRectFilledMultiColor(v_min, v_max, col_top_left, col_top_right, col_bot_right, col_bot_left);

                    // 2. РУХОМА ПАРАЛАКС-СІТКА
                    float grid_sz = 60.0f;
                    float speed = 30.0f;
                    float offset_y = fmodf(t * speed, grid_sz);
                    for (float y = v_min.y + offset_y; y < v_max.y; y += grid_sz) {
                        float alpha_factor = (y - v_min.y) / (v_max.y - v_min.y); 
                        draw_list->AddLine(ImVec2(v_min.x, y), ImVec2(v_max.x, y), ImGui::GetColorU32(ImVec4(0.0f, 0.8f, 1.0f, 0.02f + alpha_factor * 0.15f)), 1.0f);
                    }
                    for (float x = v_min.x; x < v_max.x; x += grid_sz) {
                        float dist_to_center = std::abs((x - v_min.x) - (v_max.x - v_min.x)/2.0f) / ((v_max.x - v_min.x)/2.0f);
                        draw_list->AddLine(ImVec2(x, v_min.y), ImVec2(x, v_max.y), ImGui::GetColorU32(ImVec4(0.0f, 0.8f, 1.0f, std::max(0.0f, 0.2f - dist_to_center * 0.18f))), 1.0f);
                    }

                    // 3. ЦИФРОВИЙ ПИЛ
                    for (int i = 0; i < 40; i++) {
                        float px = v_min.x + fmodf(std::abs(sin(i * 123.456f + t*0.2f)) * 9999.0f, v_max.x - v_min.x);
                        float py = v_min.y + fmodf(std::abs(cos(i * 789.123f - t*0.3f)) * 9999.0f, v_max.y - v_min.y);
                        float p_alpha = (sin(t * 3.0f + i) + 1.0f) * 0.3f; 
                        draw_list->AddCircleFilled(ImVec2(px, py), 1.5f, ImGui::GetColorU32(ImVec4(0.5f, 1.0f, 1.0f, p_alpha)));
                    }

                    // 4. ЛАЗЕРНИЙ СКАНЕР
                    float scanline_h = 120.0f; 
                    float scanline_y = v_min.y + fmodf(t * 200.0f, (v_max.y - v_min.y) + scanline_h) - scanline_h;
                    if (scanline_y > v_min.y - scanline_h && scanline_y < v_max.y) {
                        draw_list->AddRectFilledMultiColor(ImVec2(v_min.x, scanline_y), ImVec2(v_max.x, scanline_y + scanline_h/2), IM_COL32(0,0,0,0), IM_COL32(0,0,0,0), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 0.1f)), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 0.1f)));
                        draw_list->AddLine(ImVec2(v_min.x, scanline_y + scanline_h/2), ImVec2(v_max.x, scanline_y + scanline_h/2), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 0.8f)), 2.0f);
                        draw_list->AddRectFilledMultiColor(ImVec2(v_min.x, scanline_y + scanline_h/2), ImVec2(v_max.x, scanline_y + scanline_h), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 0.1f)), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.8f, 0.1f)), IM_COL32(0,0,0,0), IM_COL32(0,0,0,0));
                    }
                } else {
                    // ЯКЩО ФОН ВИМКНЕНО - просто малюємо звичайний темний колір
                    draw_list->AddRectFilled(v_min, v_max, IM_COL32(15, 15, 20, 255));
                }
                // --- КІНЕЦЬ ФОНУ ---

                // Якщо масив не пустий, починаємо малювати
                if (!state.arr.empty()) {
                    VisualChoice(state.current_vis, state);        
                } else {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Згенеруй масив!");
                }

                ImGui::EndChild();

                ImGui::SameLine();

            // Панель коду (Праворуч)
                ImGui::BeginChild("CodeArea", ImVec2(0, 0), true);
                RenderCodePanel(state); // <--- Викликаємо нашу нову функцію!
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
            
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[System] Візуалізатор успішно запущено.");
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[Action] Обрано алгоритм: %s", algos[state.current_algo]); 
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "[Action] Обрано візуалізацію: %s", visuals[state.current_vis]);
            
            // Виводимо наш зафіксований початковий масив
            if (!state.initial_array_str.empty()) {
                ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "[Data] Початковий масив: %s", state.initial_array_str.c_str());
            }

            // Виводимо всю історію кроків
            for (const auto& log_msg : state.log_history) {
                if (log_msg.find("[Success]") == 0) {
                    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%s", log_msg.c_str()); // Зелений
                } else if (log_msg.find("[Action]") == 0) {
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "%s", log_msg.c_str()); // Жовтий
                } else if (log_msg.find("[Data]") == 0) {
                    ImGui::TextColored(ImVec4(0.3f, 0.6f, 1.0f, 1.0f), "%s", log_msg.c_str()); // Блакитний
                } else if (log_msg.find("[System]") == 0) {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", log_msg.c_str()); // Світло-сірий
                } else {
                    ImGui::Text("%s", log_msg.c_str()); // Стандартний (для алгоритмів)
                }
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