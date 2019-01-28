//
// Created by thomas on 28/01/19.
//

#pragma once

template <typename T, typename S>
T &lshift(T &val, const S &shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T lshift(T &&val, const S &shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T &lshift(T &val, S &&shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T lshift(T &&val, S &&shift) {
    val <<= shift - (shift > 0);
    val <<= shift > 0;
    return val;
};

template <typename T, typename S>
T &rshift(T &val, const S &shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T rshift(T &&val, const S &shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T &rshift(T &val, S &&shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};

template <typename T, typename S>
T rshift(T &&val, S &&shift) {
    val >>= shift - (shift > 0);
    val >>= shift > 0;
    return val;
};
