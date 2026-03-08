#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "imgui.h"
#include "visualizer.h"
#include "static.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void VisualChoice(int choice, AppState& state) {
    switch (choice) {
        case 0: // Орбіти
            return GenerateOrbits(state);
        case 1: // Світловий спектр
        case 2: // Стовпчики
            return GenerateColumns(state);
        case 3: // Числа
            return GenerateNumbers(state);
    }
}

void GenerateColumns(AppState& state) {
    // ... (код GenerateColumns без змін) ...
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); 
    ImVec2 canvas_size = ImGui::GetContentRegionAvail(); 
    
    float col_width = canvas_size.x / state.arr.size();                    

    float real_max = state.arr[0];
    float real_min = state.arr[0];
    for (int val : state.arr) {
        if (val < real_min) real_min = val;
        if (val > real_max) real_max = val;
    }
    if (real_max == real_min) { real_max += 1.0f; real_min -= 1.0f; }
    float real_range = real_max - real_min;

    float geom_max = real_max < 0 ? 0 : real_max;
    float geom_min = real_min > 0 ? 0 : real_min;
    if (geom_max == geom_min) { geom_max += 1.0f; geom_min -= 1.0f; }
    float zero_y = canvas_pos.y + (geom_max / (geom_max - geom_min)) * canvas_size.y;

    for (size_t j = 0; j < state.arr.size(); j++) {
        float val = state.arr[j];
        ImVec2 p_min, p_max;
        
        float x_left = canvas_pos.x + j * col_width;
        float x_right = canvas_pos.x + (j + 1) * col_width - 1.0f;

        // --- ГЕОМЕТРІЯ ---
        if (state.current_vis == 1) { 
            p_min = ImVec2(x_left, canvas_pos.y);
            p_max = ImVec2(x_right, canvas_pos.y + canvas_size.y);
        } else {
            float col_height = (std::abs(val) / (geom_max - geom_min)) * canvas_size.y;
            if (val >= 0) {
                p_min = ImVec2(x_left, zero_y - col_height);
                p_max = ImVec2(x_right, zero_y);
            } else {
                p_min = ImVec2(x_left, zero_y);
                p_max = ImVec2(x_right, zero_y + col_height);
            }
        }

        // --- КОЛЬОРИ ТА НЕОН ---
        ImU32 fill_col, border_col;
        float border_thickness = state.enable_elements ? 1.5f : 0.0f;
        
        if (state.current_vis == 1) { 
            float normalized_val = (val - real_min) / real_range; 
            float h = 0.66f - (normalized_val * 0.66f); 
            float r, g, b;
            ImGui::ColorConvertHSVtoRGB(h, 1.0f, 1.0f, r, g, b);
            
            if (state.enable_elements) {
                fill_col = ImGui::GetColorU32(ImVec4(r, g, b, 0.7f)); 
                border_col = ImGui::GetColorU32(ImVec4(r, g, b, 1.0f)); 
            } else {
                fill_col = ImGui::GetColorU32(ImVec4(r, g, b, 1.0f)); 
            }

            if (state.is_sorting && (j == state.highlight_1 || j == state.highlight_2)) {
                fill_col = IM_COL32(255, 255, 255, 255); 
                border_col = IM_COL32(255, 255, 255, 255); 
            }

            if (state.is_animating_finish && (int)j == state.finish_anim_index) {
                fill_col = IM_COL32(255, 255, 255, 255); 
                border_col = IM_COL32(255, 255, 255, 255);
            }
        } 
        else { 
            if (state.enable_elements) {
                fill_col = IM_COL32(0, 200, 255, 40); 
                border_col = IM_COL32(0, 200, 255, 255);
            } else {
                fill_col = IM_COL32(100, 200, 255, 255); 
            }

            if (state.is_sorting && (j == state.highlight_1 || j == state.highlight_2)) {
                if (state.enable_elements) {
                    fill_col = IM_COL32(255, 0, 150, 100); 
                    border_col = IM_COL32(255, 0, 150, 255); 
                } else {
                    fill_col = IM_COL32(255, 255, 0, 255); 
                }
            }

            if (state.is_animating_finish && (int)j <= state.finish_anim_index) {
                fill_col = state.enable_elements ? IM_COL32(0, 255, 100, 80) : IM_COL32(50, 255, 50, 255);
                border_col = IM_COL32(0, 255, 100, 255);
            } else if (state.is_sorted && !state.is_animating_finish) {
                fill_col = state.enable_elements ? IM_COL32(0, 255, 100, 80) : IM_COL32(50, 255, 50, 255);
                border_col = IM_COL32(0, 255, 100, 255);
            }
        }

        draw_list->AddRectFilled(p_min, p_max, fill_col);
        if (state.enable_elements && state.current_vis != 1) { 
            draw_list->AddRect(p_min, p_max, border_col, 0.0f, 0, border_thickness);
        }
    }
}

void GenerateOrbits(AppState& state) {
    // ... (код GenerateOrbits без змін) ...
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();

    ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x / 2.0f, canvas_pos.y + canvas_size.y / 2.0f);
    
    float max_r = std::min(canvas_size.x, canvas_size.y) / 2.0f - 20.0f; 
    float base_r = max_r * 0.15f; 

    float real_max = state.arr[0];
    float real_min = state.arr[0];
    for (int val : state.arr) {
        if (val > real_max) real_max = val;
        if (val < real_min) real_min = val;
    }
    
    if (real_max < 0) real_max = 0;
    if (real_min > 0) real_min = 0;
    if (real_max == real_min) { real_max += 1.0f; real_min -= 1.0f; }
    
    float range = real_max - real_min;
    float zero_r = base_r + (std::abs(real_min) / range) * (max_r - base_r);

    float angle_step = (2.0f * M_PI) / state.arr.size(); 
    float line_thickness = std::max(1.5f, (2.0f * (float)M_PI * max_r / state.arr.size()) * 0.6f);

    std::vector<ImVec2> tips;

    for (size_t j = 0; j < state.arr.size(); j++) {
        float val = state.arr[j];
        float angle = j * angle_step - (M_PI / 2.0f);
        float r = zero_r + (val / range) * (max_r - base_r);

        ImVec2 p_zero = ImVec2(center.x + zero_r * cos(angle), center.y + zero_r * sin(angle));
        ImVec2 p_val  = ImVec2(center.x + r * cos(angle), center.y + r * sin(angle));

        tips.push_back(p_val);

        ImU32 col = IM_COL32(100, 200, 255, 150); 
        if (state.is_sorting && (j == state.highlight_1 || j == state.highlight_2)) {
            col = IM_COL32(255, 255, 0, 255); 
        }
        if (state.is_animating_finish && (int)j <= state.finish_anim_index) {
            col = IM_COL32(50, 255, 50, 150);
        } else if (state.is_sorted && !state.is_animating_finish) {
            col = IM_COL32(50, 255, 50, 150); 
        }

        draw_list->AddLine(p_zero, p_val, col, line_thickness);
    }

    draw_list->AddPolyline(tips.data(), tips.size(), IM_COL32(255, 50, 200, 255), 0, 2.5f);

    if (real_min < 0 && real_max > 0) {
        draw_list->AddCircle(center, zero_r, IM_COL32(255, 255, 255, 100), 64, 1.5f);
    }
}

std::string FormatNum(int val, int sys) {
    // ... (код FormatNum без змін) ...
    std::stringstream ss;
    if (sys == 0) { ss << val; } 
    else if (sys == 1) { 
        ss << std::hex << std::uppercase << std::abs(val); 
        if (val < 0) return "-" + ss.str();
    } 
    else if (sys == 2) {
        if (val == 0) return "0";
        std::string b = "";
        int v = std::abs(val);
        while(v > 0) { b = (v % 2 == 0 ? "0" : "1") + b; v /= 2; }
        return (val < 0 ? "-" : "") + b;
    } 
    else if (sys == 3) { 
        ss << "0" << std::oct << std::abs(val); 
        if (val < 0) return "-" + ss.str();
    }
    return ss.str();
}

// Допоміжна функція для малювання одного блоку (квадрата з числом)
void DrawNumberBox(ImDrawList* draw_list, ImVec2 center, float size, int val, int index, AppState& state, ImFont* font, bool is_active = true, bool is_pivot = false) {
    std::string text = FormatNum(val, state.current_sys);
    float font_size = size * 0.45f;
    if (font_size < 12.0f) font_size = 12.0f;

    // Базові кольори з урахуванням прозорості (якщо елемент не активний - він темнішає)
    int alpha = is_active ? 255 : 60; // 60 - напівпрозорий привид
    
    ImU32 box_col = IM_COL32(40, 60, 100, alpha);
    ImU32 border_col = IM_COL32(100, 200, 255, alpha);
    ImU32 text_col = IM_COL32(255, 255, 255, alpha);

    // Логіка кольорів
    if (is_pivot) {
        // ОПОРНИЙ ЕЛЕМЕНТ (Pivot) для Quick Sort - Неоновий червоний/рожевий
        box_col = IM_COL32(200, 20, 80, 255);
        border_col = IM_COL32(255, 50, 150, 255);
        text_col = IM_COL32(255, 255, 255, 255);
        size *= 1.1f; 
    } 
    else if (state.is_sorting && (index == state.highlight_1 || index == state.highlight_2)) {
        // Елементи, які зараз порівнюються
        box_col = IM_COL32(255, 215, 0, 255);
        border_col = IM_COL32(255, 255, 255, 255);
        text_col = IM_COL32(0, 0, 0, 255);
        size *= 1.2f; 
        font_size *= 1.2f;
    } 
    else if (state.is_animating_finish && index <= state.finish_anim_index) {
        box_col = IM_COL32(50, 200, 50, 255);
        border_col = IM_COL32(100, 255, 100, 255);
    } 
    else if (state.is_sorted && !state.is_animating_finish) {
        box_col = IM_COL32(50, 200, 50, 255);
        border_col = IM_COL32(100, 255, 100, 255);
    } 
    else if (state.current_algo == 7 && !state.heap_phase_build && index > state.heap_i) {
        box_col = IM_COL32(50, 200, 50, 255);
        border_col = IM_COL32(100, 255, 100, 255);
    }
    // --- FIX: Зелений колір для відсортованої частини Heap Sort ---
    else if (state.current_algo == 7 && !state.heap_phase_build && index > state.heap_i) {
        box_col = IM_COL32(50, 200, 50, 255);
        border_col = IM_COL32(100, 255, 100, 255);
    }

    ImVec2 p_min = ImVec2(center.x - size / 2.0f, center.y - size / 2.0f);
    ImVec2 p_max = ImVec2(center.x + size / 2.0f, center.y + size / 2.0f);

    draw_list->AddRectFilled(p_min, p_max, box_col, 8.0f);
    draw_list->AddRect(p_min, p_max, border_col, 8.0f, 0, 2.0f);

    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());
    ImVec2 text_pos = ImVec2(center.x - text_size.x / 2.0f, center.y - text_size.y / 2.0f);
    draw_list->AddText(font, font_size, text_pos, text_col, text.c_str());
}

// 1. ЛІНІЙНИЙ ВИГЛЯД (Класичний)
void GenerateLinearNumbers(AppState& state, float t, float t_jump) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    ImFont* font = ImGui::GetFont();

    // FIX: Додаємо відступи, щоб краї не обрізалися
    float padding_x = 20.0f;
    float step_x = (canvas_size.x - 2 * padding_x) / state.arr.size();
    float base_center_y = canvas_pos.y + canvas_size.y / 2.0f;
    float box_size = std::min(step_x * 0.8f, 80.0f);

    for (size_t j = 0; j < state.arr.size(); j++) {
        float target_cx = canvas_pos.x + padding_x + j * step_x + (step_x / 2.0f);
        float target_cy = base_center_y;

        // Анімація стрибка (Swap)
        if (state.is_sorting && state.is_swapping) {
            float arc_height = canvas_size.y * 0.12f;
            if (j == state.highlight_1) {
                float start_cx = canvas_pos.x + padding_x + state.highlight_2 * step_x + (step_x / 2.0f);
                target_cx = start_cx + (target_cx - start_cx) * t_jump;
                target_cy -= sin(t_jump * M_PI) * arc_height;
            } else if (j == state.highlight_2) {
                float start_cx = canvas_pos.x + padding_x + state.highlight_1 * step_x + (step_x / 2.0f);
                target_cx = start_cx + (target_cx - start_cx) * t_jump;
                target_cy += sin(t_jump * M_PI) * arc_height;
            }
        }

        DrawNumberBox(draw_list, ImVec2(target_cx, target_cy), box_size, state.arr[j], j, state, font);
    }
}

// 2. ДЕРЕВОПОДІБНИЙ ВИГЛЯД (Для Heap Sort)
void GenerateTreeNumbers(AppState& state, float t, float t_jump) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    ImFont* font = ImGui::GetFont();

    int n = state.arr.size();
    if (n == 0) return;

    // Розрахунок глибини дерева
    int max_depth = (int)floor(log2(n));
    if (max_depth < 3) max_depth = 3; // Щоб не було надто розтягнуто при малих N

    float level_height = canvas_size.y / (max_depth + 2); // +2 для відступів зверху/знизу
    float box_size = std::min(level_height * 0.7f, 60.0f);

    // Лямбда для розрахунку позиції вузла
    auto GetNodePos = [&](int idx) -> ImVec2 {
        int level = (int)floor(log2(idx + 1));
        int nodes_in_level = 1 << level; // 2^level
        int idx_in_level = idx - (nodes_in_level - 1);
        
        // Ширина сектора для одного вузла на цьому рівні
        float slice_width = canvas_size.x / nodes_in_level;
        
        float x = canvas_pos.x + slice_width * idx_in_level + slice_width / 2.0f;
        float y = canvas_pos.y + level_height * (level + 1); // +1 щоб відступити від верху
        return ImVec2(x, y);
    };

    // КРОК 1: Малюємо лінії зв'язків (спочатку, щоб були під блоками)
    for (int i = 1; i < n; i++) {
        int parent = (i - 1) / 2;
        ImVec2 p1 = GetNodePos(parent);
        ImVec2 p2 = GetNodePos(i);
        
        // Неонові лінії
        draw_list->AddLine(p1, p2, IM_COL32(0, 200, 255, 100), 2.0f);
    }

    // КРОК 2: Малюємо вузли
    for (int j = 0; j < n; j++) {
        ImVec2 pos = GetNodePos(j);

        // Анімація обміну в дереві
        if (state.is_sorting && state.is_swapping) {
            if (j == state.highlight_1) {
                // FIX: Елемент j зараз на highlight_1, але прийшов з highlight_2
                ImVec2 origin = GetNodePos(state.highlight_2);
                ImVec2 dest = GetNodePos(state.highlight_1);
                pos.x = origin.x + (dest.x - origin.x) * t_jump;
                pos.y = origin.y + (dest.y - origin.y) * t_jump;
            } else if (j == state.highlight_2) {
                // FIX: Елемент j зараз на highlight_2, але прийшов з highlight_1
                ImVec2 origin = GetNodePos(state.highlight_1);
                ImVec2 dest = GetNodePos(state.highlight_2);
                pos.x = origin.x + (dest.x - origin.x) * t_jump;
                pos.y = origin.y + (dest.y - origin.y) * t_jump;
            }
        }

        DrawNumberBox(draw_list, pos, box_size, state.arr[j], j, state, font);
    }
}

// 3. РОЗДІЛЕНИЙ ВИГЛЯД (Для Merge Sort та Quick Sort)
void GeneratePartitionNumbers(AppState& state, float t, float t_jump) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    ImFont* font = ImGui::GetFont();

    // FIX: Додаємо відступи
    float padding_x = 20.0f;
    float step_x = (canvas_size.x - 2 * padding_x) / state.arr.size();
    float base_center_y = canvas_pos.y + canvas_size.y / 2.0f - 40.0f; 
    float box_size = std::min(step_x * 0.8f, 80.0f);
    
    float active_offset_y = 100.0f; // Сильніше опускаємо активні блоки для контрасту
    float split_gap_x = std::min(20.0f, step_x * 0.5f); // Розрив для Merge Sort

    // Лямбда для X-координати (з урахуванням розривів)
    auto GetX = [&](int idx) -> float {
        float x = canvas_pos.x + padding_x + idx * step_x + (step_x / 2.0f);
        if (state.current_algo == 3 && state.merge_curr_size < state.arr.size()) { 
            int left = state.merge_left_start;
            int size = state.merge_curr_size;
            int mid = left + size;
            int right_end = std::min((int)state.arr.size(), left + 2 * size);
            
            // Якщо ми всередині зони злиття, розсуваємо ліву і праву половини
            if (idx >= left && idx < right_end) {
                if (idx >= mid) x += split_gap_x; // Праву половину вправо
                else x -= split_gap_x;            // Ліву вліво
            }
        }
        return x;
    };

    for (size_t j = 0; j < state.arr.size(); j++) {
        float target_cx = GetX(j);
        float target_cy = base_center_y;

        bool is_active = false;
        bool is_pivot = false;

        // --- ЛОГІКА ДЛЯ MERGE SORT ---
        if (state.current_algo == 3) {
            int left = state.merge_left_start;
            int size = state.merge_curr_size;
            int right_end = std::min((int)state.arr.size(), left + 2 * size);

            if (state.merge_curr_size >= state.arr.size() || state.is_sorted) {
                is_active = true; // Коли все відсортовано, все активне
            } else if (j >= left && j < right_end) {
                is_active = true;
                target_cy += active_offset_y; 
            }
        }
        // --- ЛОГІКА ДЛЯ QUICK SORT ---
        else if (state.current_algo == 4) {
            if (state.is_sorted) {
                is_active = true;
            } else if (j >= state.qs_low && j <= state.qs_high) {
                is_active = true;
                target_cy += active_offset_y; 
                
                // Вказуємо опорний елемент
                if (state.qs_is_partitioning && j == state.qs_high) {
                    is_pivot = true;
                }
            }
        }

        // Анімація стрибків (переважно для Quick Sort)
        if (state.is_sorting && state.is_swapping && is_active) {
             float arc_height = canvas_size.y * 0.15f;
             if (j == state.highlight_1) {
                 float start_cx = GetX(state.highlight_2);
                 target_cx = start_cx + (target_cx - start_cx) * t_jump;
                 target_cy -= sin(t_jump * M_PI) * arc_height;
             } else if (j == state.highlight_2) {
                 float start_cx = GetX(state.highlight_1);
                 target_cx = start_cx + (target_cx - start_cx) * t_jump;
                 target_cy += sin(t_jump * M_PI) * arc_height;
             }
        }

        // Малюємо кубик (передаємо is_active та is_pivot)
        DrawNumberBox(draw_list, ImVec2(target_cx, target_cy), box_size, state.arr[j], j, state, font, is_active, is_pivot);
    }
    
    // Гарний підпис зони, яка обробляється
    if (state.is_sorting && !state.is_sorted) {
        std::string label = (state.current_algo == 3) ? "MERGING BLOCK" : "PARTITIONING RANGE";
        ImVec2 text_sz = font->CalcTextSizeA(18.0f, FLT_MAX, 0.0f, label.c_str());
        ImVec2 label_pos = ImVec2(canvas_pos.x + canvas_size.x / 2.0f - text_sz.x / 2.0f, base_center_y + active_offset_y + box_size);
        draw_list->AddText(font, 18.0f, label_pos, IM_COL32(0, 255, 200, 200), label.c_str());
    }
}

// ГОЛОВНА ФУНКЦІЯ-ДИСПЕТЧЕР
void GenerateNumbers(AppState& state) {
    // Рахуємо загальний таймер кроку (від 0.0 до 1.0)
    double current_time = ImGui::GetTime();
    double delay = 0.05 / state.speed;
    float t = (float)((current_time - state.last_step_time) / delay);
    if (t > 1.0f || !state.is_sorting) t = 1.0f;

    // Розрахунок фази стрибка (t_jump)
    float t_jump = 0.0f;
    if (t < 0.2f) t_jump = 0.0f; 
    else if (t > 0.8f) t_jump = 1.0f; 
    else t_jump = (t - 0.2f) / 0.6f; 

    // Вибір режиму візуалізації залежно від алгоритму
    // 3 = Merge Sort, 4 = Quick Sort
    if (state.current_algo == 3 || state.current_algo == 4) {
        GeneratePartitionNumbers(state, t, t_jump);
    }
    // 7 = Heap Sort (Припустимо, що ти додаси його під цим індексом)
    else if (state.current_algo == 7) {
        GenerateTreeNumbers(state, t, t_jump);
    }
    // Всі інші (Bubble, Selection, Insertion, Shell, Shaker)
    else {
        GenerateLinearNumbers(state, t, t_jump);
    }
}

void RenderCodePanel(AppState& state) {
// ... (RenderCodePanel залишається без змін) ...
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    
    // --- МАЛЮЄМО "ШАПКУ" ЯК У macOS ---
    draw_list->AddCircleFilled(ImVec2(p.x + 12, p.y + 12), 6.0f, IM_COL32(255, 95, 86, 255));  // Red
    draw_list->AddCircleFilled(ImVec2(p.x + 32, p.y + 12), 6.0f, IM_COL32(255, 189, 46, 255)); // Yellow
    draw_list->AddCircleFilled(ImVec2(p.x + 52, p.y + 12), 6.0f, IM_COL32(39, 201, 63, 255));  // Green
    ImGui::Dummy(ImVec2(0, 25)); 

    // Визначаємо круті кольори для коду
    ImVec4 col_keyword = ImVec4(0.86f, 0.44f, 0.84f, 1.0f); // Фіолетовий (void, for, if)
    ImVec4 col_type    = ImVec4(0.34f, 0.65f, 0.92f, 1.0f); // Синій (int)
    ImVec4 col_active  = ImVec4(1.0f, 0.9f, 0.2f, 1.0f);    // Яскраво-жовтий (активний рядок)
    ImVec4 col_normal  = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);    // Білий
    ImVec4 col_green   = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);    // Зелений

    // Перемикаємось залежно від обраного алгоритму
    switch (state.current_algo) {
        case 0: // Selection Sort
            ImGui::Text(" Файл: selection_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();
            
            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "selectionSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n) {");
            
            ImGui::TextColored(col_keyword, "    for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "i = 0; i < n - 1; i++) {");
            
            ImGui::TextColored(col_type, "        int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "min_idx = i;");

            ImGui::TextColored(col_keyword, "        for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "j = i + 1; j < n; j++) {");
            
            // Активний рядок
            ImGui::TextColored(col_active, "            if (arr[j] < arr[min_idx]) { "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// ПОШУК МІНІМУМУ"); 
            
            ImGui::TextColored(col_normal, "                min_idx = j;");
            ImGui::TextColored(col_normal, "            }");
            ImGui::TextColored(col_normal, "        }");
            
            ImGui::TextColored(col_normal, "        swap(arr[i], arr[min_idx]);");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 1: // Insertion Sort
            ImGui::Text(" Файл: insertion_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();
            
            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "insertionSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n) {");

            ImGui::TextColored(col_keyword, "    for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "i = 1; i < n; i++) {");

            ImGui::TextColored(col_type, "        int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "key = arr[i];");

            ImGui::TextColored(col_type, "        int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "j = i - 1;");

            ImGui::TextColored(col_keyword, "        while "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(j >= 0 && arr[j] > key) {");

            // Активний рядок
            ImGui::TextColored(col_active, "            arr[j + 1] = arr[j]; "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// ТЯГНЕМО ВЛІВО");

            ImGui::TextColored(col_normal, "            j--;");
            ImGui::TextColored(col_normal, "        }");
            ImGui::TextColored(col_normal, "        arr[j + 1] = key;");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 2: // Bubble Sort
            ImGui::Text(" Файл: bubble_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();
            
            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "bubbleSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n) {");
            
            ImGui::TextColored(col_keyword, "    for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "i = 0; i < n - 1; i++) {");

            ImGui::TextColored(col_keyword, "        for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "j = 0; j < n - i - 1; j++) {");
            
            // Активний рядок
            ImGui::TextColored(col_active, "            if (arr[j] > arr[j + 1]) { "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// ПОРІВНЯННЯ ТА ОБМІН"); 
            
            ImGui::TextColored(col_normal, "                swap(arr[j], arr[j + 1]);");

            ImGui::TextColored(col_normal, "            }");
            ImGui::TextColored(col_normal, "        }");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 3: // Merge Sort
            ImGui::Text(" Файл: merge_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();
            
            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "mergeSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "l, "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "r) {");

            ImGui::TextColored(col_keyword, "    if "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(l < r) {");

            ImGui::TextColored(col_type, "        int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "m = l + (r - l) / 2;");

            ImGui::TextColored(col_normal, "        mergeSort(arr, l, m);");
            ImGui::TextColored(col_normal, "        mergeSort(arr, m + 1, r);");

            // Активний рядок
            ImGui::TextColored(col_active, "        merge(arr, l, m, r); "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// ПРОЦЕС ЗЛИТТЯ");

            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 4: // Quick Sort
            ImGui::Text(" Файл: quick_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();
            
            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "quickSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "low, "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "high) {");

            ImGui::TextColored(col_keyword, "    if "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(low < high) {");

            ImGui::TextColored(col_type, "        int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "pi = partition(arr, low, high);");

            // Активний рядок
            ImGui::TextColored(col_active, "        quickSort(arr, low, pi - 1); "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// РЕКУРСІЯ ЛІВО");

            ImGui::TextColored(col_normal, "        quickSort(arr, pi + 1, high);");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 5: // Shell Sort
            ImGui::Text(" Файл: shell_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();

            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "shellSort("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n) {");

            ImGui::TextColored(col_keyword, "    for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "gap = n / 2; gap > 0; gap /= 2) {");

            ImGui::TextColored(col_keyword, "        for "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "i = gap; i < n; i++) {");

            ImGui::TextColored(col_type, "            int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "temp = arr[i];");
            
            ImGui::TextColored(col_type, "            int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "j;");

            // Активний рядок
            ImGui::TextColored(col_active, "            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) { "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// СТРИБОК");

            ImGui::TextColored(col_normal, "                arr[j] = arr[j - gap];");
            ImGui::TextColored(col_normal, "            }");
            ImGui::TextColored(col_normal, "            arr[j] = temp;");
            ImGui::TextColored(col_normal, "        }");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 6: // Cocktail Shaker Sort
            ImGui::Text(" Файл: cocktail_shaker.cpp");
            ImGui::Separator(); ImGui::Spacing();

            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "cocktailShaker("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "arr[], "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n) {");

            ImGui::TextColored(col_keyword, "    bool "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "swapped = "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_keyword, "true"); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, ";");

            ImGui::TextColored(col_type, "    int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "start = 0, end = n - 1;");

            ImGui::TextColored(col_keyword, "    while "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(swapped) {");

            ImGui::TextColored(col_normal, "        swapped = "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_keyword, "false"); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, ";");

            // Активний рядок (Вправо)
            ImGui::TextColored(col_active, "        for (int i = start; i < end; ++i) { "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// РУХ ВПРАВО");

            ImGui::TextColored(col_normal, "            if (arr[i] > arr[i + 1]) swap(arr[i], arr[i + 1]);");
            ImGui::TextColored(col_normal, "        }");

            ImGui::TextColored(col_normal, "        end--;");

            // Активний рядок (Вліво)
            ImGui::TextColored(col_active, "        for (int i = end - 1; i >= start; --i) { "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// РУХ ВЛІВО");

            ImGui::TextColored(col_normal, "            if (arr[i] > arr[i + 1]) swap(arr[i], arr[i + 1]);");
            ImGui::TextColored(col_normal, "        }");

            ImGui::TextColored(col_normal, "        start++;");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;

        case 7: // Heap Sort
            ImGui::Text(" Файл: heap_sort.cpp");
            ImGui::Separator(); ImGui::Spacing();

            ImGui::TextColored(col_keyword, "void "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "heapify("); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "n, "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_type, "int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "i) {");

            ImGui::TextColored(col_type, "    int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "largest = i;");
            ImGui::TextColored(col_type, "    int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "l = 2 * i + 1;");
            ImGui::TextColored(col_type, "    int "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "r = 2 * i + 2;");

            ImGui::TextColored(col_keyword, "    if "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(l < n && arr[l] > arr[largest]) largest = l;");
            ImGui::TextColored(col_keyword, "    if "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(r < n && arr[r] > arr[largest]) largest = r;");

            ImGui::TextColored(col_keyword, "    if "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_normal, "(largest != i) {");
            ImGui::TextColored(col_active, "        swap(arr[i], arr[largest]); "); ImGui::SameLine(0, 0);
            ImGui::TextColored(col_green, "// SIFT DOWN");
            ImGui::TextColored(col_normal, "        heapify(arr, n, largest);");
            ImGui::TextColored(col_normal, "    }");
            ImGui::TextColored(col_normal, "}");
            break;
    }
}
