#pragma once
#include "../SDK/ISurface.h"
#include <d3d9.h>
namespace SDK
{
	class IMaterial;
}

enum circle_type
{
	full = 1, half, quarter
};
namespace RENDER
{
	unsigned int CreateF(std::string font_name, int size, int weight, int blur, int scanlines, int flags);
	void DrawF(int X, int Y, unsigned int Font, bool center_width, bool center_height, CColor Color, std::string Input);
	void DrawWF(int X, int Y, unsigned int Font, CColor Color, const wchar_t* Input);
	Vector2D GetTextSize(unsigned int Font, std::string Input);
	void CircleFilledDualColor(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2, IDirect3DDevice9 * m_device);

	void DrawLine(int x1, int y1, int x2, int y2, CColor color);

	void DrawEmptyRect(int x1, int y1, int x2, int y2, CColor color, unsigned char = 0); // the flags are for which sides to ignore in clockwise, 0b1 is top, 0b10 is right, etc.
	void DrawFilledRect(int x1, int y1, int x2, int y2, CColor color);
	void CircleFilledRainbowColor(float x, float y, float rad, float rotate, int type, int resolution, IDirect3DDevice9* m_device);
	void FillRectangle(int x1, int y2, int width, int height, CColor color);
	void DrawFilledRectOutline(int x1, int y1, int x2, int y2, CColor color);
	void DrawFilledRectArray(SDK::IntRect* rects, int rect_amount, CColor color);
	void DrawOutlineBox(int x, int y, int w, int h, CColor color);
	void DrawCoalBox(int x, int y, int w, int h, CColor color);
	void DrawCornerRect(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const bool outlined, const CColor& color, const CColor& outlined_color);
	void DrawEdges(float topX, float topY, float bottomX, float bottomY, float length, CColor color);

	void DrawCircle(int x, int y, int radius, int segments, CColor color);
	void DrawCircle2(float x, float y, float r, float s, CColor color);
	void DrawFilledCircle(int x, int y, int radius, int segments, CColor color);
	//void DrawFilledCircle(Vector2D center, CColor color, CColor outline, float radius, float points);

	void TexturedPolygon(int n, std::vector<SDK::Vertex_t> vertice, CColor color);
	void DrawSomething();
	void DrawFilled3DBox(Vector origin, int width, int height, CColor outline, CColor filling);
	bool WorldToScreen(Vector world, Vector &screen);
}