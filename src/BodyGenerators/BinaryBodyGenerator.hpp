//
// Created by thomas on 12/3/18.
//

#pragma once

#include <base/BodyGenerator.hpp>

class BinaryBodyGenerator : public BodyGenerator {
public:
    explicit BinaryBodyGenerator(Settings settings);

    std::tuple<fp, Vec3<fp>, Vec3<fp>> getBody() override;

private:
    unsigned bodyIndex = 0;
};


