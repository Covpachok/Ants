#ifndef ANTS_BOUNDSCHECKER_HPP
#define ANTS_BOUNDSCHECKER_HPP

#include <cstdint>

constexpr bool IsInBounds(int n, int loN, int upN)
{
	return n >= loN && n < upN;
}

constexpr bool IsInBounds(int x, int y, int loX, int upX, int loY, int upY)
{
	return x >= loX && x < upX && y >= loY && y < upY;
}

constexpr bool IsInBounds(size_t x, size_t y, size_t upX, size_t upY)
{
	return x < upX && y < upY;
}

constexpr bool IsInBounds(size_t n, size_t upN)
{
	return n < upN;
}

#endif //ANTS_BOUNDSCHECKER_HPP
