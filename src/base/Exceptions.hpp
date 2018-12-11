//
// Created by thomas on 12/11/18.
//

#pragma once

#include <exception>
#include <settings/Settings.hpp>

class CharonException : public std::exception {
public:
    explicit CharonException(std::string &&errorString) : errorString{errorString} {}

    const char *what() const noexcept override {
        return errorString.c_str();
    };

protected:
    const std::string errorString;
};

class NotImplementedUniverseException : public CharonException {
public:
    explicit NotImplementedUniverseException(const Settings &settings) :
            CharonException{"The Universe with options:\n"
                            "\talgorithm = '" + settings.algorithm + "',\n" +
                            "\tplatform = '" + settings.platform + "',\n" +
                            "\tfloatingPointType = '" + settings.floatingPointType + "'\n" +
                            "is not implemented."} {}
};

