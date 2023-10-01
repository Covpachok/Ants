#ifndef ANTS_COLORMAP_HPP
#define ANTS_COLORMAP_HPP

#include <raylib.h>
#include <cstdint>

#include "IntVec.hpp"

class ColorMap
{
public:
	ColorMap(size_t width, size_t height, const Color &defaultColor);
	~ColorMap();

	void Update();

	void Clear();

	void UpdatePixel(int x, int y);
	inline void UpdatePixel(const IntVec2 &pos);

	void Add(int n, const Color &color);
	inline void Add(int x, int y, const Color &color);
	inline void Add(const IntVec2 &pos, const Color &intensity);

	void Set(int n, const Color &color);
	inline void Set(int x, int y, const Color &color);
	inline void Set(const IntVec2 &pos, const Color &intensity);

	inline void UnsafeSet(int x, int y, const Color &color) { m_colors[y * m_width + x] = color; };

	Color &GetMutable(int n);
	inline Color &GetMutable(int x, int y);
	inline Color &GetMutable(const IntVec2 &pos);

	void Draw() const;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	int m_width, m_height;
	int m_size;

//	float m_screenToMapRatio;

	Color m_defaultColor;
	Color m_errorColor = BLACK;

	Texture m_texture;
	Color   *m_colors;

	Rectangle m_drawSrc, m_drawDest;
};

inline void ColorMap::UpdatePixel(const IntVec2 &pos)
{
	UpdatePixel(pos.x, pos.y);
}

void ColorMap::Add(int x, int y, const Color &color)
{
	Add(y * m_width + x, color);
}

void ColorMap::Add(const IntVec2 &pos, const Color &intensity)
{
	Add(pos.y * m_width + pos.x, intensity);
}

void ColorMap::Set(int x, int y, const Color &color)
{
	Set(y * m_width + x, color);
}

void ColorMap::Set(const IntVec2 &pos, const Color &intensity)
{
	Set(pos.y * m_width + pos.x, intensity);
}

inline Color &ColorMap::GetMutable(int x, int y)
{
	return GetMutable(y * m_width + x);
}

inline Color &ColorMap::GetMutable(const IntVec2 &pos)
{
	return GetMutable(pos.y * m_width + pos.x);
}

#endif //ANTS_COLORMAP_HPP
