#pragma once

#include  <iostream>
#include  <fstream>
#include  "registers.h"
#include  "cpu.h"

using namespace std;


inline void load_fragment(string filename, CPU& cpu) {
    ifstream file(filename, ios::binary);
    int cur_value = 0;
    for (int i = 0; i < 32; i++) {
        file.read(reinterpret_cast<char*>(&cur_value), sizeof(int));

        get_register(i, cur_value);
    }

    int address = 0;
    int sz = 0;


    while (file.read(reinterpret_cast<char*>(&address), sizeof(int)) &&
           file.read(reinterpret_cast<char*>(&sz), sizeof(int))) {
        vector<byte> data(sz);
        file.read(reinterpret_cast<char*>(data.data()), sz);
        cpu.write_instruction(address, data);
    }

    file.close();
}