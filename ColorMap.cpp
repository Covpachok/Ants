#include "ColorMap.hpp"
#include "BoundsChecker.hpp"
#include "Settings.hpp"

ColorMap::ColorMap(size_t width, size_t height, const Color &defaultColor)
		:
		m_width(static_cast<int>(width)), m_height(static_cast<int>(height)),
		m_size(m_width * m_height), m_defaultColor(defaultColor)
{
	m_screenToMapRatio = Settings::Instance().GetWorldSettings().screenToMapRatio;

	m_drawSrc  = {0, 0,
	              static_cast<float>(m_width),
	              static_cast<float>(m_height)};
	m_drawDest = {0, 0,
	              static_cast<float>(m_width) * m_screenToMapRatio,
	              static_cast<float>(m_height) * m_screenToMapRatio};

	Image image = GenImageColor(m_width, m_height, defaultColor);

	m_texture = LoadTextureFromImage(image);
	m_colors  = LoadImageColors(image);

	UnloadImage(image);
}

ColorMap::~ColorMap()
{
	UnloadImageColors(m_colors);
	UnloadTexture(m_texture);
}

void ColorMap::Update()
{
	UpdateTexture(m_texture, m_colors);
}

void ColorMap::Clear()
{
	for ( int i = 0; i < m_size; ++i )
	{
		m_colors[i] = m_defaultColor;
	}
}

void ColorMap::UpdatePixel(int x, int y)
{
	UpdateTextureRec(m_texture, {static_cast<float>(x), static_cast<float>(y), 1, 1}, &m_colors[y * m_width + x]);
}

void ColorMap::Add(int n, const Color &color)
{
	if ( !IsInBounds(n, 0, m_size))
	{
		return;
	}

	m_colors[n].r = std::min(m_colors[n].r + color.r, 255);
	m_colors[n].g = color.g;
	m_colors[n].b = color.b;
	m_colors[n].a = color.a;
}

void ColorMap::Set(int n, const Color &color)
{
	if ( !IsInBounds(n, 0, m_size))
	{
		return;
	}

	m_colors[n] = color;
}

Color &ColorMap::GetMutable(int n)
{
	if ( !IsInBounds(n, 0, m_size))
	{
		return m_errorColor;
	}

	return m_colors[n];
}

void ColorMap::Draw() const
{
	DrawTexturePro(m_texture, m_drawSrc, m_drawDest, {0, 0}, 0, WHITE);
}
