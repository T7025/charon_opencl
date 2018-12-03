//
// Created by thomas on 12/3/18.
//

#include "BinaryBodyGenerator.hpp"
#include <utility>
#include <random>
#include <iostream>


BinaryBodyGenerator::BinaryBodyGenerator(Settings settings) : BodyGenerator{std::move(settings)} {};

std::tuple<fp, Vec3<fp>, Vec3<fp>> BinaryBodyGenerator::getBody() {
    assert(settings.numberOfBodies == 2);

    Vec3<fp> pos{};
    Vec3<fp> vel{};
    if (bodyIndex++ == 0) {
        pos = {-1, 0, 0};
        vel = {0, -0.5, 0};
    }
    else {
        pos = {1, 0, 0};
        vel = {0, 0.5, 0};
    }
    return {1.0, pos, vel};
}