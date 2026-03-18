#include "ui.h"
#include "static.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "font_data.h"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    long i = 0;

    // 1. Ініціалізуємо GLFW (бібліотеку для вікон)
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Створюємо вікно 1280x720
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sorting Visualizer C++", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Вмикаємо вертикальну синхронізацію (VSync)

    // 3. Налаштовуємо Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Додаємо підтримку кирилиці
    ImFontConfig font_config;
    font_config.OversampleH = 2; // для згладжування
    font_config.OversampleV = 2;
    font_config.FontDataOwnedByAtlas = false; 

    // ------------------------------
    // Беремо стандартний шрифт з твого Linux (шлях може трохи відрізнятися, 
    // але NotoSans або DejaVuSans зазвичай є у всіх)
    // io.Fonts->AddFontFromFileTTF("/usr/share/fonts/noto/NotoSans-Regular.ttf", 18.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());
    // ------------------------------
    // Конфіг потрібен, щоб ImGui не намагався видалити цей масив з пам'яті при виході
    // Завантажуємо шрифт ПРЯМО З ОПЕРАТИВНОЇ ПАМ'ЯТІ!
    io.Fonts->AddFontFromMemoryTTF(
        (void*)NotoSans_ttf,      // Назва масиву з нашого нового файлу
        NotoSans_ttf_len,         // Розмір масиву (теж з файлу)
        18.0f,                    // Розмір шрифту (зміни на свій, якщо треба)
        &font_config,
        io.Fonts->GetGlyphRangesCyrillic() // Це якщо потрібна підтримка кирилиці (укр/рос літер)
    );
    
    ImGui::StyleColorsDark(); // Вмикаємо темну тему 

    // Підключаємо ImGui до нашого вікна та OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Створюємо стан апки
    AppState app_state;

    // 4. ГОЛОВНИЙ ЦИКЛ ПРОГРАМИ (працює, поки вікно не закриють)
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Обробляємо кліки мишки/клавіатури

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ------------------------------
        RenderUI(app_state);
        // ------------------------------

        // Рендеримо (малюємо) все на екран
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        // Колір фону (темно-сірий)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window); // Оновлюємо екран
    }

    // 5. Прибираємо за собою після закриття вікна
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}