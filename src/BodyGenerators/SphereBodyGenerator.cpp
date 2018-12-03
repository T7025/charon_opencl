//
// Created by thomas on 11/23/18.
//

#include "SphereBodyGenerator.hpp"
#include <utility>
#include <random>
#include <iostream>


SphereBodyGenerator::SphereBodyGenerator(Settings settings) : BodyGenerator{std::move(settings)} {};

std::tuple<fp, Vec3<fp>, Vec3<fp>> SphereBodyGenerator::getBody() {
    const fp radius = settings.universeRadius;  // Initial radius of the system.
    const fp G = settings.G;  // Gravitational constant.
    const fp M = settings.totalMass;  // Total mass of the system.

    std::uniform_real_distribution<fp> realDistribution{-radius, radius};
    std::normal_distribution<fp> normalDistribution{0, 1};

    fp mass = M / settings.numberOfBodies;

    // Pick a random point in the sphere.
    Vec3<fp> pos{realDistribution(engine), realDistribution(engine), realDistribution(engine)};
    while (pos.squareDistance({0, 0, 0}) >= (radius * radius)) {
        pos.x = realDistribution(engine);
        pos.y = realDistribution(engine);
        pos.z = realDistribution(engine);
    }

    Vec3<fp> vel{pos};  // Move to infinity ("escape velocity")
    vel.normalize();
    // Idea: Give particles velocity so they don't (immediately) collapse into each other. With "clustering" using
    // uniform distribution, expect interesting behaviour with clusters of bodies and voids.
    // g = G * M * r / R^3              (acceleration towards center inside a solid sphere)
    // v_initial = sqrt(2 * g * r)      (escape velocity)
    // v_initial = sqrt(2 * G * M r^2 / R^3)
    // Use (M - mass) instead of M: Don't view own mass as mass of the total system (when using one body as reference point).
    vel *= sqrt(2 * G * (M - mass) * pos.squareDistance({0, 0, 0}) / (radius * radius * radius));

    return std::make_tuple(mass, pos, vel);
}