//
// Created by thomas on 11/23/18.
//

#pragma once

#include <base/BodyGenerator.hpp>
#include <utility>
#include <random>

class SphereBodyGenerator : public BodyGenerator {
public:
    explicit SphereBodyGenerator(Settings settings) : BodyGenerator{std::move(settings)} {};

    std::tuple<double, Vec3<double>, Vec3<double>> getBody() override {
        double radius = 5;  // Initial radius of the system.
        double G = 1;  // Gravitational constant.
        double M = settings().numberOfBodies;  // Total mass of the system.

        std::mt19937_64 engine{settings().rngSeed};
        std::uniform_real_distribution<double> realDistribution{-radius, radius};
        std::normal_distribution<double> normalDistribution{0, 1};

        // Pick a random point in the sphere.
        Vec3<double> pos{realDistribution(engine), realDistribution(engine), realDistribution(engine)};
        while (pos.squareDistance({0, 0, 0}) > radius * radius) {
            pos.x = realDistribution(engine);
            pos.y = realDistribution(engine);
            pos.z = realDistribution(engine);
        }

        Vec3<double> vel{pos.normalize()};  // Move to infinity ("escape velocity")
        // Idea: Give particles velocity so they don't (immediately) collapse into each other. With "clustering" using
        // uniform distribution, expect interesting behaviour with clusters of bodies and voids.
        // g = G * M * r / R^3              (acceleration towards center inside a solid sphere)
        // v_initial = sqrt(2 * g * r)      (escape velocity)
        // v_initial = sqrt(2 * G * M r^2 / R^3)
        vel *= sqrt(2 * G * M * pos.squareDistance({0, 0, 0}) / (radius * radius * radius));

        return std::make_tuple(1.0, pos, vel);
    }
};



