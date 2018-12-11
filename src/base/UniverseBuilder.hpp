//
// Created by thomas on 12/11/18.
//

#pragma once


#include <memory>
#include <settings/Settings.hpp>

class UniverseBase;

struct UniverseBuilder {
    std::unique_ptr<UniverseBase> operator()(const Settings &settings);
};
