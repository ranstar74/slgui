#include "imgui_gradient.h"

void ImGui::ShadeVerts(const ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
	ImVec2 gradient_extent = gradient_p1 - gradient_p0;
	float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
	ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
	ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
	const int col0_r = (int)(col0 >> IM_COL32_R_SHIFT) & 0xFF;
	const int col0_g = (int)(col0 >> IM_COL32_G_SHIFT) & 0xFF;
	const int col0_b = (int)(col0 >> IM_COL32_B_SHIFT) & 0xFF;
	const int col0_a = (int)(col0 >> IM_COL32_A_SHIFT) & 0xFF;
	const int col_delta_r = ((int)(col1 >> IM_COL32_R_SHIFT) & 0xFF) - col0_r;
	const int col_delta_g = ((int)(col1 >> IM_COL32_G_SHIFT) & 0xFF) - col0_g;
	const int col_delta_b = ((int)(col1 >> IM_COL32_B_SHIFT) & 0xFF) - col0_b;
	const int col_delta_a = ((int)(col1 >> IM_COL32_A_SHIFT) & 0xFF) - col0_a;
	for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
	{
		float d = ImDot(vert->pos - gradient_p0, gradient_extent);
		float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
		int r = (int)((float)col0_r + (float)col_delta_r * t);
		int g = (int)((float)col0_g + (float)col_delta_g * t);
		int b = (int)((float)col0_b + (float)col_delta_b * t);
		int a = (int)((float)col0_a + (float)col_delta_a * t);

		// To preserve round borders
		int oldAlpha = (vert->col >> IM_COL32_A_SHIFT) & 0xFF;
		if (oldAlpha == 0)
			a = 0;

		vert->col = IM_COL32(r, g, b, a);
	}
}

void ImGui::RenderFrameGradient(const ImRect& bb, ImU32 col0, ImU32 col1, ImGuiAxis axis, float bias, float offset)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiStyle& style = GImGui->Style;

	int v0 = window->DrawList->VtxBuffer.Size;
	window->DrawList->AddRectFilled(bb.Min, bb.Max, IM_COL32_WHITE, style.FrameRounding);
	int v1 = window->DrawList->VtxBuffer.Size;

	ShadeVerts(bb, ColorConvertU32ToGradient(col0, col1), v0, v1, bias, offset, axis);
}

void ImGui::RenderRectGradient(const ImRect& bb, ImU32 col0, ImU32 col1, ImGuiAxis axis, float bias, float offset)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiStyle& style = GImGui->Style;

	int v0 = window->DrawList->VtxBuffer.Size;
	window->DrawList->AddRect(bb.Min, bb.Max, IM_COL32_WHITE, style.FrameRounding);
	int v1 = window->DrawList->VtxBuffer.Size;

	ShadeVerts(bb, ColorConvertU32ToGradient(col0, col1), v0, v1, bias, offset, axis);
}

ImVec4 ImGui::StorageGetVec4(ImGuiID id)
{
	PushOverrideID(id);
	ImGuiID x = GetID("x");
	ImGuiID y = GetID("y");
	ImGuiID z = GetID("z");
	ImGuiID w = GetID("w");
	ImVec4 result;
	ImGuiStorage* st = GetStateStorage();
	result.x = st->GetFloat(x);
	result.y = st->GetFloat(y);
	result.z = st->GetFloat(z);
	result.w = st->GetFloat(w);
	PopID();
	return result;
}

void ImGui::StorageSetVec4(ImGuiID id, const ImVec4& vec)
{
	PushOverrideID(id);
	ImGuiID x = GetID("x");
	ImGuiID y = GetID("y");
	ImGuiID z = GetID("z");
	ImGuiID w = GetID("w");
	ImGuiStorage* st = GetStateStorage();
	st->SetFloat(x, vec.x);
	st->SetFloat(y, vec.y);
	st->SetFloat(z, vec.z);
	st->SetFloat(w, vec.w);
	PopID();
}

ImGradient ImGui::StorageGetGradient(ImGuiID id)
{
	PushOverrideID(id);
	ImVec4 start = StorageGetVec4(GetID("Start"));
	ImVec4 end = StorageGetVec4(GetID("End"));
	PopID();
	return { start, end };
}

void ImGui::StorageSetGradient(ImGuiID id, const ImGradient& col)
{
	PushOverrideID(id);
	StorageSetVec4(GetID("Start"), col.Start);
	StorageSetVec4(GetID("End"), col.End);
	PopID();
}

ImGradient ImGui::StorageLerpGradient(const char* strId, const ImGradient& col, float time)
{
	float t = GetIO().DeltaTime * 15 * (1.0f / time);
	ImGuiID id = GetID(strId);
	PushOverrideID(id);
	ImGradient current = StorageGetGradient(id);
	current = ImLerp(current, col, t);
	StorageSetGradient(id, current);
	PopID();
	return current;
}

void ImGui::ShadeVerts(const ImRect& bb, const ImGradient& col, int vtx0, int vtx1, float bias, float shift, ImGuiAxis axis)
{
	ImGuiWindow* window = GetCurrentWindow();

	ImVec2 p0 = bb.Min;
	ImVec2 p1 = axis == ImGuiAxis_X ? bb.GetTR() : bb.GetBL();
	p1 = ImLerp(p0, p1, bias);

	float shiftDist = fabsf(p1.y - p0.y) * shift;

	p0.y += shiftDist;
	p1.y += shiftDist;

	ImU32 col0 = ColorConvertFloat4ToU32(col.Start);
	ImU32 col1 = ColorConvertFloat4ToU32(col.End);

	ShadeVerts(window->DrawList, vtx0, vtx1, p0, p1, col0, col1);
}

void ImGui::RenderFrame(const ImRect& bb, const ImGradient& col, float bias, float shift, ImGuiAxis axis)
{
	ImGuiWindow* window = GetCurrentWindow();

	int vtx0 = window->DrawList->VtxBuffer.Size;
	window->DrawList->AddRectFilled(bb.Min, bb.Max, IM_COL32_WHITE, GImGui->Style.FrameRounding);
	int vtx1 = window->DrawList->VtxBuffer.Size;

	ShadeVerts(bb, col, vtx0, vtx1, bias, shift, axis);
}

void ImGui::RenderBorder(const ImRect& bb, const ImGradient& col, float bias, float shift, ImGuiAxis axis)
{
	ImGuiWindow* window = GetCurrentWindow();
	ImGuiStyle& style = GImGui->Style;

	int vtx0 = window->DrawList->VtxBuffer.Size;
	window->DrawList->AddRect(bb.Min, bb.Max, IM_COL32_WHITE, style.FrameRounding, 0, style.FrameBorderSize);
	int vtx1 = window->DrawList->VtxBuffer.Size;

	ShadeVerts(bb, col, vtx0, vtx1, bias, shift, axis);
}

void ImGui::ShadeItem(const ImGradient& col)
{
	ImGuiWindow* window = GetCurrentWindow();
	const ImRect& rect = window->ClipRect;
	RenderFrame(rect, col);
}
