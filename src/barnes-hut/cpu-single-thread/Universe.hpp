//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/Vec3.hpp>
#include <vector>
#include <ostream>
#include <cassert>



template<typename FP>
class Universe<Algorithm::bruteForce, Platform::cpuSingleThread, FP> : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{settings} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override {
        for (int i = 0; i < settings().numberOfBodies; ++i) {
//            double m;
//            Vec3<FP> pos, vel;
//            std::tie(m, pos, vel) = bodyGenerator->getBody();
            auto [m, pos, vel] = bodyGenerator->getBody();
            mass.emplace_back(m);
            position.emplace_back(pos);
            velocity.emplace_back(vel);
            acceleration.emplace_back({0,0,0});
        }


    }

    void logInternalState(std::ostream &out) override {
        assert(mass.size() == position.size() && mass.size() == velocity.size() && mass.size() == acceleration.size());

        out << "mass,xPos,yPos,zPos,xVel,yVel,zVel,xAcc,yAcc,zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ',' << position[i] << ',' << velocity[i] << ',' << acceleration[i] << '\n';
        }
    }

    void step() override {

    }


private:

    std::vector<FP> mass;
    std::vector<Vec3<FP>> position;
    std::vector<Vec3<FP>> velocity;
    std::vector<Vec3<FP>> acceleration;
};

