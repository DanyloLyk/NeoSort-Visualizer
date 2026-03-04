cmake_minimum_required(VERSION 3.10)
project(Algorithms)

set(CMAKE_CXX_STANDARD 17)

# Вказуємо, де лежать заголовні файли ImGui
include_directories(vendor/imgui)
include_directories(vendor/imgui/backends)

# Збираємо всі потрібні файли ImGui в одну змінну
set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

# Шукаємо встановлені в системі бібліотеки для графіки
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Створюємо наш виконуваний файл (додаємо твій main.cpp та файли ImGui)
add_executable(Algorithms src/main.cpp src/ui.cpp src/algorithms.cpp src/visualizer.cpp ${IMGUI_SOURCES})

# Лінкуємо системні бібліотеки до нашого проекту
target_link_libraries(Algorithms PRIVATE glfw OpenGL::GL)