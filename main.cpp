#include <iostream>
#include <fstream>
#include "cpu.h"
#include <algorithm>
#include "parser.h"
#include "create_file.h"


using namespace std;


bool check_data(int argc, char** argv) {
    argc--;
    if (argc != 2 && argc != 6 && argc != 4) {
        cerr << "incorrect number of arguments" << endl;
        return true;
    }

    for (int i = 1; i <= argc; ++i) {
        if (string(argv[i]) == "-i") {
            if (argc - i == 0) {
                cerr << "no input file" << endl;
                return true;
            }
            ifstream file(string(argv[i + 1]), ios::binary);
            if (!file.is_open()) {
                cerr << "incorrect input file" << endl;
            }
        }
        if (string(argv[i]) == "-o") {
            if (argc - i < 3) {
                cerr << "incorrect number of arguments output file" << endl;
                return true;
            }
            int address = stoi(string(argv[i + 2]));
            int sz = stoi(string(argv[i + 3]));
            if (address < 0 || sz < 0) {
                cerr << "incorrect address" << endl;
                return true;
            }
        }
    }

    return false;
}

int main(int argc, char** argv) {

    if (check_data(argc, argv)) {
        return 0;
    }

    string input_file = "";
    string output_file = "";
    int start_address = -1;
    int size = -1;

    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-i") {
            input_file = string(argv[i + 1]);
        }
        if (string (argv[i]) == "-o") {
            output_file = string(argv[i + 1]);
            start_address = stoi(string(argv[i + 2]));
            size = stoi(string(argv[i + 3]));
        }
    }
    
    
    CPU cpu1(0);

    load_fragment(input_file, cpu1);

    int pc_c = pc;
    int ra_c = ra;
    unordered_map<int, int> registers_c = registers;

    try {
        cpu1.start();
    }
    catch(exception& e) {
        cerr << e.what() << endl;
        return 0;
    }

    pc = pc_c;
    ra = ra_c;
    registers = registers_c;

    CPU cpu2(1);

    load_fragment(input_file, cpu2);

     try {
        cpu2.start();
    }
    catch(exception& e) {
        cerr << e.what() << endl;
        return 0;
    }

    printf("replacement\thit rate\thit rate (inst)\thit rate (data)\n");

    printf("        LRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n",
           cpu1.get_all_answer() * 100, cpu1.get_instraction_answer() * 100, cpu1.get_data_answer() * 100);   

    printf("      bpLRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n",
           cpu2.get_all_answer() * 100, cpu2.get_instraction_answer() * 100, cpu2.get_data_answer() * 100);


    if (start_address != -1) cpu1.load_all();

    cpu1.write_ram(output_file, start_address, size);
    
    return 0;
}