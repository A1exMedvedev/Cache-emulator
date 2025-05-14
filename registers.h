#pragma once

#include <iostream>
#include <unordered_map>

using namespace std;

static int pc = 0;

static unordered_map<int, int> registers;

inline void get_register(int index, int value) {
    if (index == 0) {
        pc = value;
        registers[index] = 0;
        return;
    }
    registers[index] = value;
}