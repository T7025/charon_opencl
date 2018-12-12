//
// Created by thomas on 12/11/18.
//

#pragma once


#include <memory>
#include <settings/Settings.hpp>

class UniverseBase;

std::unique_ptr<UniverseBase> getConcreteUniverse(const Settings &settings);
