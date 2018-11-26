//
// Created by thomas on 11/24/18.
//

#pragma once

#include <ostream>
#include <tuple>
#include <cmath>
#include <iomanip>


template <typename FP>
struct Vec3 {
    Vec3() = default;
    Vec3(FP x, FP y, FP z) : x{x}, y{y}, z{z} {};
    
    template <typename FP2>
    explicit Vec3(Vec3<FP2> vec) : Vec3{vec.x, vec.y, vec.z} {};

    Vec3 &operator+=(const Vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    };

    Vec3 &operator-=(const Vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3 &operator*=(const Vec3 &other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vec3 &operator/=(const Vec3 &other) {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    Vec3 &operator*=(FP scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3 &operator/=(FP scalar) {
        const auto inverse = FP(1.0)/scalar;
        x *= inverse;
        y *= inverse;
        z *= inverse;
        return *this;
    }

    bool operator==(const Vec3 &rhs) const {
        return std::tie(x, y, z) == std::tie(rhs.x, rhs.y, rhs.z);
    }

    bool operator!=(const Vec3 &rhs) const {
        return !(rhs == *this);
    }

    FP norm() const {
        return sqrt(x * x + y * y + z * z);
    }

    Vec3 &normalize() {
        return *this /= norm();
    }

    FP squareDistance(const Vec3 &rhs) const {
        return (x - rhs.x) * (x - rhs.x)
               + (y - rhs.y) * (y - rhs.y)
               + (z - rhs.z) * (z - rhs.z);
    }

    FP distance(const Vec3 &rhs) const {
        return sqrt(squareDistance(rhs));
    }

    Vec3 &min(const Vec3 &rhs) {
        x = std::min(x, rhs.x);
        y = std::min(y, rhs.y);
        z = std::min(z, rhs.z);
        return *this;
    }

    Vec3 &max(const Vec3 &rhs) {
        x = std::max(x, rhs.x);
        y = std::max(y, rhs.y);
        z = std::max(z, rhs.z);
        return *this;
    }

    Vec3 operator+(const Vec3 &rhs) const {
        auto result = *this;
        return result += rhs;
    }

    Vec3 operator-(const Vec3 &rhs) const {
        auto result = *this;
        return result -= rhs;
    }
    Vec3 operator*(const Vec3 &rhs) const {
        auto result = *this;
        return result *= rhs;
    }
    Vec3 operator/(const Vec3 &rhs) const {
        auto result = *this;
        return result /= rhs;
    }
    Vec3 operator*(FP scalar) const {
        auto result = *this;
        return result *= scalar;
    }

    /*Vec3 operator*(FP scalar, Vec3 rhs) {
        return rhs *= scalar;
    }*/

    Vec3 operator/(FP scalar) const {
        auto result = *this;
        return result /= scalar;
    }
    
    FP x, y, z;

};

template <typename FP>
std::ostream &operator<<(std::ostream &out, const Vec3<FP> &vec) {
    out << vec.x << ',' << vec.y << ',' << vec.z;
    return out;
}
