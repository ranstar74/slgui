#include "slgui.h"
#include "imgui_gradient.h"

void ImGui::RenderGloss(const ImRect& bb, ImGuiCol col)
{
    // Extends half of the frame
    ImVec2 glossMin = ImVec2(bb.Min.x, bb.Min.y);
    ImVec2 glossMax = ImVec2(bb.Max.x, bb.Max.y - bb.GetHeight() * 0.5f);
    RenderFrame(glossMin, glossMax, GetColorU32(col), false);
}

void ImGui::RenderShadow(const ImRect& bb)
{
    ImRect shadowBb = bb;
    shadowBb.TranslateY(1);
    RenderFrame(shadowBb.Min, shadowBb.Max, GetColorU32(ImGuiCol_Shadow), false);
}
