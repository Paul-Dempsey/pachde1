#pragma once
#include <rack.hpp>

namespace pachde {

// int version of Rack Vec
struct Point {
	int x = 0;
	int y = 0;

	Point() {}
	Point(int xy) : x(xy), y(xy) {}
	Point(int x, int y) : x(x), y(y) {}

	int& operator[](int i) {
		return (i == 0) ? x : y;
	}
	const int& operator[](int i) const {
		return (i == 0) ? x : y;
	}
	/** Negates the vector.
	Equivalent to a reflection across the `y = -x` line.
	*/
	Point neg() const {
		return Point(-x, -y);
	}
	Point plus(Point b) const {
		return Point(x + b.x, y + b.y);
	}
	Point minus(Point b) const {
		return Point(x - b.x, y - b.y);
	}
	Point mult(int s) const {
		return Point(x * s, y * s);
	}
	Point mult(Point b) const {
		return Point(x * b.x, y * b.y);
	}
	Point div(int s) const {
		return Point(x / s, y / s);
	}
	Point div(Point b) const {
		return Point(x / b.x, y / b.y);
	}
	int dot(Point b) const {
		return x * b.x + y * b.y;
	}
	int area() const {
		return x * y;
	}
	float distance(Point b) {
		float dx = std::abs(b.x - x);
		float dy = std::abs(b.y - y);
		return ::rack::simd::sqrt(dx * dx + dy * dy);
	}

	/** Swaps the coordinates.
	Equivalent to a reflection across the `y = x` line.
	*/
	Point flip() const {
		return Point(y, x);
	}
	Point min(Point b) const {
		return Point(std::min(x, b.x), std::min(y, b.y));
	}
	Point max(Point b) const {
		return Point(std::max(x, b.x), std::max(y, b.y));
	}
	Point abs() const {
		return Point(std::abs(x), std::abs(y));
	}
	bool equals(Point b) const {
		return x == b.x && y == b.y;
	}
	bool isZero() const {
		return x == 0 && y == 0;
	}
    bool isPositive() const {
        return x >= 0 && y >= 0;
    }
	// Method aliases
	bool isEqual(Point b) const {
		return equals(b);
	}
};

}
