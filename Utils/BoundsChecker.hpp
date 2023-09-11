#ifndef ANTS_BOUNDSCHECKER_HPP
#define ANTS_BOUNDSCHECKER_HPP

#include <cstdint>
#include "IntVec.hpp"

constexpr bool IsInBounds(int n, int loN, int upN)
{
	return n >= loN && n < upN;
}

constexpr bool IsInBounds(int x, int y, int loX, int upX, int loY, int upY)
{
	return x >= loX && x < upX && y >= loY && y < upY;
}

#if 0
constexpr bool IsInBounds(size_t x, size_t y, size_t upX, size_t upY)
{
	return x < upX && y < upY;
}

constexpr bool IsInBounds(size_t n, size_t upN)
{
	return n < upN;
}
#endif

class BoundsChecker2D
{
public:
	BoundsChecker2D(int loX, int upX, int loY, int upY) :
			m_loX(loX), m_upX(upX), m_loY(loY), m_upY(upY) {}

	inline bool IsInBounds(int x, int y) const { return x >= m_loX && x < m_upX && y >= m_loY && y < m_upY; }
	inline bool IsInBounds(const IntVec2 &pos) const { return IsInBounds(pos.x, pos.y); }

private:
	int m_loX, m_upX;
	int m_loY, m_upY;
};

#endif //ANTS_BOUNDSCHECKER_HPP
