//
// File: slgui.h
//
// Copyright (C) 2023 ranstar74. All rights violated.
//
// Part of "Rage Am" Research Project.
//
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

struct ImGradient
{
	ImVec4 Start;
	ImVec4 End;
};

inline ImGradient ImLerp(const ImGradient& a, const ImGradient& b, float t) { return { ImLerp(a.Start, b.Start, t), ImLerp(a.End, b.End, t) }; }

namespace ImGui
{
	inline ImGradient ColorConvertU32ToGradient(ImU32 start, ImU32 end) { return { ColorConvertU32ToFloat4(start),ColorConvertU32ToFloat4(end) }; }

    void ShadeVerts(const ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    void RenderFrameGradient(const ImRect& bb, ImU32 col0, ImU32 col1, ImGuiAxis axis = ImGuiAxis_Y, float bias = 1.0f, float offset = 0.0f);
	void RenderRectGradient(const ImRect& bb, ImU32 col0, ImU32 col1, ImGuiAxis axis = ImGuiAxis_Y, float bias = 1.0f, float offset = 0.0f);

	void ShadeVerts(const ImRect& bb, const ImGradient& col, int vtx0, int vtx1, float bias = 1, float shift = 0, ImGuiAxis axis = ImGuiAxis_Y);

	ImVec4 StorageGetVec4(ImGuiID id);
	void StorageSetVec4(ImGuiID id, const ImVec4& vec);
	ImGradient StorageGetGradient(ImGuiID id);
	void StorageSetGradient(ImGuiID id, const ImGradient& col);
	ImGradient StorageLerpGradient(const char* strId, const ImGradient& col, float time = 1.0f);

	void RenderFrame(const ImRect& bb, const ImGradient& col, float bias = 1, float shift = 0, ImGuiAxis axis = ImGuiAxis_Y);
	void RenderBorder(const ImRect& bb, const ImGradient& col, float bias = 1, float shift = 0, ImGuiAxis axis = ImGuiAxis_Y);

	// Fills previous drawn item with gradient
	void ShadeItem(const ImGradient& col);
}
