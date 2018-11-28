//
// Created by thomas on 11/14/18.
//

#pragma once

#include <base/Util.hpp>
#include <base/Universe.hpp>
#include <base/Vec3.hpp>
#include <vector>
#include <iostream>
#include <cassert>


template<typename FP>
class Universe<Algorithm::bruteForce, Platform::cpuSingleThread, FP> : public UniverseBase {
public:
    explicit Universe(Settings settings) : UniverseBase{std::move(settings)} {};

    void init(std::unique_ptr<BodyGenerator> bodyGenerator) override {
        for (unsigned i = 0; i < settings.numberOfBodies; ++i) {
            auto [m, pos, vel] = bodyGenerator->getBody();
            mass.emplace_back(m);
            position.emplace_back(pos);
            velocity.emplace_back(vel);
            acceleration.emplace_back(0,0,0);
        }

    }

    void logInternalState(std::ostream &out) override {
        assert(mass.size() == position.size() && mass.size() == velocity.size() && mass.size() == acceleration.size());

        out << "mass,xPos,yPos,zPos,xVel,yVel,zVel,xAcc,yAcc,zAcc\n";
        for (unsigned i = 0; i < mass.size(); ++i) {
            out << mass[i] << ',' << position[i] << ',' << velocity[i] << ',' << acceleration[i] << '\n';
        }
    }

    void step(unsigned int numSteps) override {
        for (unsigned step = 1; step < numSteps; ++step) {
            calcNextPosition();
            for (unsigned i = 0; i < mass.size(); ++i) {
                auto newAcceleration = calcAcceleration(i);
                velocity[i] += newAcceleration * settings.timeStep;
                acceleration[i] = newAcceleration;
            }
        }
    }


private:
    void calcNextPosition() {
        for (unsigned i = 0; i < mass.size(); ++i) {
            position[i] += velocity[i] * settings.timeStep + acceleration[i] * settings.timeStep * settings.timeStep / 2;
        }
    }
    Vec3<FP> calcAcceleration(const unsigned int target) const {
        Vec3<FP> newAcceleration{0, 0, 0};
        for (unsigned j = 0; j < mass.size(); ++j) {
            if (target == j) continue;
            const Vec3 diff = position[j] - position[target];
            fp norm = diff.norm();
            newAcceleration += diff * (mass[j] / (norm * norm * norm));
        }
        return newAcceleration;
    }

    std::vector<FP> mass;
    std::vector<Vec3<FP>> position;
    std::vector<Vec3<FP>> velocity;
    std::vector<Vec3<FP>> acceleration;
};

