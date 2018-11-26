#include <utility>

//
// Created by thomas on 11/23/18.
//

#pragma once


//template <typename FP>
//struct Vec3 {
//    FP x;
//    FP y;
//    FP z;
//};
//
//template <typename FP>
//struct Body {
//    FP mass;
//    FP posX;
//    FP posY;
//    FP
//};
//extern template class Body<float>;
//extern template class Body<double>;

#include <settings/Settings.hpp>
#include <random>
#include "Vec3.hpp"

//template<typename FP>
class BodyGenerator {
public:
    explicit BodyGenerator(Settings settings) : settings{std::move(settings)}, engine{settings.rngSeed} {};
//    struct Vec3 {
//        double x, y, z;
//    };
    struct Body {
        double mass;
        Vec3<double> pos;
        Vec3<double> vel;
        Vec3<double> acc;
    };

    virtual ~BodyGenerator() = default;

    virtual std::tuple<fp, Vec3<fp>, Vec3<fp>> getBody() = 0;

protected:
    Settings settings;
    std::mt19937_64 engine;
};


//extern template
//class BodyGenerator<float>;
//
//extern template
//class BodyGenerator<double>;
