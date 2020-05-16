#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr int SUCCESS_CODE = 200;
constexpr int PORT = 18080;

enum class Child
{
    LEFT = 0,
    RIGHT = 1
};

enum class InfoFields
{
    FORMULA,
    DEPTH,
    IS_SAT,
    EXEC_TIME,
    IS_CONSISTENT
};

#endif
