#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include "registers.h"

#include "constant.h"

using namespace std;

class Memory {
private:
    vector<byte> memory_;

    void load_byte(int address, byte value) {
        memory_[address] = value;
    }

    byte read_byte(int address) {
        return memory_[address];
    }

public:
    Memory() : memory_(MEMORY_SIZE) {
    }

    void load_instraction(int address, vector<byte>& instr) {
        for (size_t i = 0; i < instr.size(); i++) {
            load_byte(address + i, instr[i]);
        }
    }

    void load_cache_line(int address, const vector<byte> &line) {
        for (size_t i = 0; i < CACHE_LINE_SIZE; i++) {
            load_byte(address + i, line[i]);
        }
    }

    vector<byte> read_cache_line(int address) {
        vector<byte> result;
        for (size_t i = 0; i < CACHE_LINE_SIZE; i++) {
            result.push_back(read_byte(address + i));
        }
        return result;
    }

    void write_ram(string output_file, int start_address, int size) {
        ofstream file(output_file, ios::binary);
        file.write(reinterpret_cast<char*>(&pc), 4);
        for (int i = 1; i < 32; ++i) {
            file.write(reinterpret_cast<char*>(&registers[i]), 4);
        }
        file.write(reinterpret_cast<char*>(&start_address), 4);
        file.write(reinterpret_cast<char*>(&size), 4);
        file.write(reinterpret_cast<char*>(memory_.data() + start_address), size);
    }
};
