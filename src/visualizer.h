#pragma once
#include "static.h"

void VisualChoice(int choice, AppState& state);
void GenerateColumns(AppState& state);
void GenerateOrbits(AppState& state);
void GenerateNumbers(AppState& state);
void RenderCodePanel(AppState& state);
void GeneratePartitionNumbers(AppState& state, float t, float t_jump);
void GenerateTreeNumbers(AppState& state, float t, float t_jump);
void GenerateLinearNumbers(AppState& state, float t, float t_jump);
void DrawNumberBox(ImDrawList* draw_list, ImVec2 center, float size, int value, int index, const AppState& state, ImFont* font);
std::string FormatNum(int val, int sys);
void ExplodeConfetti(AppState& state);
void UpdateSearchAnimation(AppState& state);