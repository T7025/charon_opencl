//
// Created by thomas on 11/23/18.
//

#pragma once

#include <base/BodyGenerator.hpp>

class SphereBodyGenerator : public BodyGenerator {
public:
    explicit SphereBodyGenerator(Settings settings);

    std::tuple<fp, Vec3<fp>, Vec3<fp>> getBody() override;
};



