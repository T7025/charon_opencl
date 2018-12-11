//
// Created by thomas on 12/11/18.
//

#pragma once

#include <exception>

class NotImplementedUniverseException : public std::exception {
//    [[nothrow]]
    const char *what() const noexcept override  {
        return "";
    };
};

