#ifndef ANTS_COLORCONVERT_HPP
#define ANTS_COLORCONVERT_HPP

#include <imgui.h>
#include <raylib.h>

namespace ColorConvert
{
	struct ImGuiColor
	{
		float color[4];
	};

	inline unsigned char FloatToUchar(float value)
	{
		return static_cast<unsigned char>(value * 255.f);
	}

	inline float UcharToFloat(unsigned char value)
	{
		return static_cast<float>(value) / 255.f;
	}

	inline Color Float4ToRayColor(float value[4])
	{
		return {FloatToUchar(value[0]), FloatToUchar(value[1]), FloatToUchar(value[2]), FloatToUchar(value[3])};
	};

	inline ImGuiColor RayColorToFloat4(Color value)
	{
		return ImGuiColor{UcharToFloat(value.r), UcharToFloat(value.g), UcharToFloat(value.b), UcharToFloat(value.a)};
	}

	bool ImGuiRlColorEdit4(const char *label, Color &col, ImGuiColorEditFlags flags = 0)
	{
		auto converted = ColorConvert::RayColorToFloat4(col);
		bool ret       = ImGui::ColorEdit4(label, converted.color, flags);
		col = ColorConvert::Float4ToRayColor(converted.color);

		return ret;
	}
}


#endif //ANTS_COLORCONVERT_HPP
