#ifndef MICROLUTION_INTVEC_HPP
#define MICROLUTION_INTVEC_HPP

#include <cmath>
#include <string>

struct IntVec2
{
	int x, y;

	IntVec2(int nx = 0, int ny = 0) :
			x(nx), y(ny) {}

	template<typename T>
	IntVec2(T nx = 0, T ny = 0) :
			x(nx), y(ny) {}

	const IntVec2 &operator+=(const IntVec2 &vec);
	const IntVec2 &operator-=(const IntVec2 &vec);
	bool operator==(const IntVec2 &vec) const;

	std::string ToString() const;
};

inline const IntVec2 &IntVec2::operator+=(const IntVec2 &vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

inline const IntVec2 &IntVec2::operator-=(const IntVec2 &vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

inline bool IntVec2::operator==(const IntVec2 &vec) const
{
	return x == vec.x && y == vec.y;
}

inline std::string IntVec2::ToString() const
{
	return "x:" + std::to_string(x) + " y:" + std::to_string(y);
}

const IntVec2 Zero  = IntVec2(0, 0);
const IntVec2 One   = IntVec2(1, 1);
const IntVec2 Up    = IntVec2(0, 1);
const IntVec2 Down  = IntVec2(0, -1);
const IntVec2 Left  = IntVec2(1, 0);
const IntVec2 Right = IntVec2(-1, 0);

inline float IV2Distance(const IntVec2 &v1, const IntVec2 &v2)
{
	return sqrtf(static_cast<float>(( v1.x - v2.x ) * ( v1.x - v2.x ) + ( v1.y - v2.y ) * ( v1.y - v2.y )));
}

inline float IV2SquareDistance(const IntVec2 &v1, const IntVec2 &v2)
{
	return static_cast<float>(( v1.x - v2.x ) * ( v1.x - v2.x ) + ( v1.y - v2.y ) * ( v1.y - v2.y ));
}

inline IntVec2 operator+(const IntVec2 &lhs, const IntVec2 &rhs)
{
	auto ret(lhs);
	ret += rhs;
	return ret;
}

inline IntVec2 operator-(const IntVec2 &lhs, const IntVec2 &rhs)
{
	auto ret(lhs);
	ret -= rhs;
	return ret;
}

inline std::ostream &operator<<(std::ostream &os, const IntVec2 &vec)
{
	os << vec.ToString();
	return os;
}

#endif //MICROLUTION_INTVEC_HPP
