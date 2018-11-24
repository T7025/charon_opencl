//
// Created by thomas on 11/23/18.
//

#pragma once

#include <base/BodyGenerator.hpp>
#include <random>

class CubeBodyGenerator : public BodyGenerator {
public:
    std::tuple<double, Vec3<double>, Vec3<double>> getBody() override {
        double radius = 5;  // Initial radius of the system.
        double G = 1;  // Gravitational constant.
        double M = settings().numberOfBodies;  // Total mass of the system.

        std::mt19937_64 engine{1302};
        std::uniform_real_distribution<double> realDistribution{-radius, radius};
        std::normal_distribution<double> normalDistribution{0, 1};

        Vec3<double> pos{realDistribution(engine), realDistribution(engine), realDistribution(engine)};
        Vec3<double> vel{normalDistribution(engine), normalDistribution(engine), normalDistribution(engine)};  // TODO: make perpendicular to line (0,0,0), (p1,p2,p3).

        vel *= sqrt(G * M * pos.distance({0, 0, 0}) / (radius * radius * radius)) / vel.norm();


        return std::make_tuple(1.0, pos, vel);
    }
};



