#pragma once

#include <iostream>
#include <ranges>
#include <exception>
#include <fstream>

#include "cache.h"
#include "registers.h"


using namespace std;

class CPU {
private:

    bool stop_ = false;

    Memory memory_;
    Cache cache_;

    int bin_to_int(string bin) {
        int result = 0;
        for (int i = 0; i < bin.length(); i++) {
            if (bin[i] == '1') {
                result += (1 << (bin.length() - i - 1));
            }
        }
        return result;
    }


    int sext(int value, int bits) {
        return(value << (32 - bits)) >> (32 - bits);
    }


    void lui(string val, string reg) {
        int value = bin_to_int(val) << 12;
        int ind = bin_to_int(reg);
        registers[ind] = value;
        pc += 4;
    }

    void auipc(string val, string reg) {
        int value = bin_to_int(val) << 12;
        int ind = bin_to_int(reg);
        registers[ind] = value + pc;
        pc += 4;
    }

    void addi(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = registers[inds] + value;
        pc += 4;
    }

    void slti(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        if (registers[inds] < value) {
            registers[ind] = 1;
        }
        else {
            registers[ind] = 0;
        }
        pc += 4;
    }

    void sltiu(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        if ((unsigned int) registers[inds] < (unsigned int) value) {
            registers[ind] = 1;
        }
        else {
            registers[ind] = 0;
        }
        pc += 4;
    }

    void xori(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = registers[inds] ^ value;
        pc += 4;
    }

    void ori(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = registers[ind] | value;
        pc += 4;
    }

    void andi(string val, string regs, string reg) {
        int value = bin_to_int(val);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = registers[ind] & value;
        pc += 4;
    }

    void slli(string shamt, string regs, string reg) {
        int value = bin_to_int(shamt);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        registers[ind] = registers[ind] << value;
        pc += 4;
    }

    void srli(string shamt, string regs, string reg) {
        int value = bin_to_int(shamt);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        registers[ind] = (int) ((unsigned int) registers[inds] >> value);
        pc += 4;
    }

    void srai(string shamt, string regs, string reg) {
        int value = bin_to_int(shamt);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        registers[ind] = registers[inds] >> value;
        pc += 4;
    }

    void add(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] + registers[ind2];
        pc += 4;
    }

    void sub(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] - registers[ind2];
        pc += 4;
    }

    void sll(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] << (registers[ind2] & 31);
        pc += 4;
    }

    void slt(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        if (registers[ind1] < registers[ind2]) {
            registers[ind] = 1;
        }
        else {
            registers[ind] = 0;
        }
        pc += 4;
    }

    void sltu(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        if ((unsigned int) registers[ind1] < (unsigned int) registers[ind2]) {
            registers[ind] = 1;
        }
        else {
            registers[ind] = 0;
        }
        pc += 4;
    }

    void xorr(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] ^ registers[ind2];
        pc += 4;
    }

    void srl(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = (int) ((unsigned int) registers[ind1] >> ((unsigned int) registers[ind2] & 31));
        pc += 4;
    }

    void sra(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] >> (registers[ind2] & 31);
        pc += 4;
    }

    void orr(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] | registers[ind2];
        pc += 4;
    }

    void andd(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] & registers[ind2];
        pc += 4;
    }

    void fence() {
        pc += 4;
    }

    void fencetso() {
        pc += 4;
    }

    void pause() {
        pc += 4;
    }

    void ecall() {
        stop_ = true;
    }

    void ebreak() {
        stop_ = true;
    }

    void lb(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = sext(cache_.read_data(registers[inds] + value, 1), 8);
        pc += 4;
    }

    void lh(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = sext(cache_.read_data(registers[inds] + value, 2), 16);
        pc += 4;
    }

    void lw(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = cache_.read_data(registers[inds] + value, 4);
        pc += 4;
    }

    void lbu(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = cache_.read_data(registers[inds] + value, 1);
        pc += 4;
    }

    void lhu(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = cache_.read_data(registers[inds] + value, 2);
        pc += 4;
    }

    void lwu(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        registers[ind] = cache_.read_data(registers[inds] + value, 4);
        pc += 4;
    }

    void sb(string offset, string reg2, string reg1) {
        int value = bin_to_int(offset);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 12);
        cache_.write_one_byte(registers[ind1] + value, byte(registers[ind2]));
        pc += 4;
    }

    void sh(string offset, string reg2, string reg1) {
        int value = bin_to_int(offset);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 12);
        cache_.write_two_bytes(registers[ind1] + value, short(registers[ind2]));
        pc += 4;
    }

    void sw(string offset, string reg2, string reg1) {
        int value = bin_to_int(offset);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 12);
        cache_.write_four_bytes(registers[ind1] + value, registers[ind2]);
        pc += 4;
    }

    void jal(string offset, string reg) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind = bin_to_int(reg);
        value = sext(value, 20);
        registers[ind] = pc + 4;
        pc += value;

    }

    void jalr(string offset, string regs, string reg) {
        int value = bin_to_int(offset);
        int inds = bin_to_int(regs);
        int ind = bin_to_int(reg);
        value = sext(value, 12);
        int t = pc + 4;
        pc = (registers[inds] + value) & ~1;
        registers[ind] = t;
    }

    void beq(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if (registers[ind1] == registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void bne(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if (registers[ind1] != registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void blt(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if (registers[ind1] < registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void bge(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if (registers[ind1] >= registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void bltu(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if ((unsigned int) registers[ind1] < (unsigned int) registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void bgeu(string offset, string reg1, string reg2) {
        int value = bin_to_int(offset);
        value <<= 1;
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        value = sext(value, 13);
        if ((unsigned int) registers[ind1] >= (unsigned int) registers[ind2]) {
            pc += value;
        }
        else {
            pc += 4;
        }
    }

    void mul(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] * registers[ind2];
        pc += 4;
    }

    void mulh(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = (int) ((long long) registers[ind1] * (long long) registers[ind2] >> 32);
        pc += 4;
    }

    void mulhsu(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = (int) ((unsigned long long) registers[ind1] * (unsigned long long) registers[ind2] >> 32);
        pc += 4;
    }

    void mulhu(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        unsigned long long t = 32;
        registers[ind] = (int) (((unsigned long long) registers[ind1] * (unsigned long long) registers[ind2]) >> t);
        pc += 4;
    }

    void div(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] / registers[ind2];
        pc += 4;
    }

    void divu(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = (int)((unsigned int) registers[ind1] / (unsigned int) registers[ind2]);
        pc += 4;
    }

    void rem(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = registers[ind1] % registers[ind2];
        pc += 4;
    }

    void remu(string reg2, string reg1, string reg) {
        int ind = bin_to_int(reg);
        int ind1 = bin_to_int(reg1);
        int ind2 = bin_to_int(reg2);
        registers[ind] = (int)((unsigned int) registers[ind1] % (unsigned int) registers[ind2]);
        pc += 4;
    }


public:
    CPU(bool plru) : cache_(memory_, plru) {}

    string make_comand(int comand) {
        string ans = "";
        unsigned int cur = (unsigned int) comand;
        for (int i = 31; i >= 0; i--) {
            ans += to_string((cur >> i) & 1);
        }
        return ans;
    }

    void start() {
        while (pc != ra && !stop_) {
            int comand = cache_.read_instruction(pc);
            pars_command(make_comand(comand));
        }
    }

    void write_instruction(int address, vector<byte>& value) {
        memory_.load_instraction(address, value);
    }

    void pars_command(string command) {
        if (command.substr(25, 7) == "0110111") {
            lui(command.substr(0, 20), command.substr(20, 5));
            return;
        }
        if (command.substr(25, 7) == "0010111") {
            auipc(command.substr(0, 20), command.substr(20, 5));
            return;
        }
        if (command.substr(25, 7) == "0010011") {
            if (command.substr(17, 3) == "000") {
                addi(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "010") {
                slti(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "011") {
                sltiu(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "100") {
                xori(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "110") {
                ori(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "111") {
                andi(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "001") {
                slli(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "101") {
                if (command.substr(0, 5) == "00000") {
                    srli(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 5) == "01000") {
                    srai(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
        }
        if (command.substr(25, 7) == "0110011") {
            if (command.substr(17, 3) == "000") {
                if (command.substr(0, 7) == "0000000") {
                    add(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0100000") {
                    sub(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    mul(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "001") {
                if (command.substr(0, 7) == "0000001") {
                    mulh(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000000") {
                    sll(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "010") {
                if (command.substr(0, 7) == "0000000") {
                    slt(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    mulhsu(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "011") {
                if (command.substr(0, 7) == "0000000") {
                    sltu(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    mulhu(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "100") {
                if (command.substr(0, 7) == "0000000") {
                    xorr(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    div(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "101") {
                if (command.substr(0, 7) == "0000000") {
                    srl(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0100000") {
                    sra(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    divu(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "110") {
                if (command.substr(0, 7) == "0000000") {
                    orr(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    rem(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
            if (command.substr(17, 3) == "111") {
                if (command.substr(0, 7) == "0000000") {
                    andd(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
                if (command.substr(0, 7) == "0000001") {
                    remu(command.substr(7, 5), command.substr(12, 5), command.substr(20, 5));
                    return;
                }
            }
        }
        if (command.substr(25, 7) == "0001111") {
            fence();
            return;
        }
        if (command == "00000000000000000000000001110011") {
            ecall();
            return;
        }
        if (command == "00000000000100000000000001110011") {
            ebreak();
            return;
        }
        if (command.substr(25, 7) == "0000011") {
            if (command.substr(17, 3) == "000") {
                lb(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "001") {
                lh(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "010") {
                lw(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "100") {
                lbu(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
            if (command.substr(17, 3) == "101") {
                lhu(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
                return;
            }
        }
        if (command.substr(25, 7) == "0100011") {
            if (command.substr(17, 3) == "000") {
                sb(command.substr(0, 7) + command.substr(20, 5), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "001") {
                sh(command.substr(0, 7) + command.substr(20, 5), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "010") {
                sw(command.substr(0, 7) + command.substr(20, 5), command.substr(7, 5), command.substr(12, 5));
                return;
            }
        }
        if (command.substr(25, 7) == "1101111") {
            jal(command.substr(0, 1) + command.substr(12, 8) + command.substr(11, 1) + command.substr(1, 10), command.substr(20, 5));
            return;
        }
        if (command.substr(25, 7) == "1100111") {
            jalr(command.substr(0, 12), command.substr(12, 5), command.substr(20, 5));
            return;
        }
        if (command.substr(25, 7) == "1100011") {
            if (command.substr(17, 3) == "000") {
                beq(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "001") {
                bne(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "100") {
                blt(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "101") {
                bge(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "110") {
                bltu(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
            if (command.substr(17, 3) == "111") {
                bgeu(command.substr(0, 1) + command.substr(24, 1) + command.substr(1, 6) + command.substr(20, 4), command.substr(7, 5), command.substr(12, 5));
                return;
            }
        }
        throw runtime_error("Unknown command " + command);
    }

    double get_data_answer() {
        return cache_.get_data_answer();
    }

    double get_instraction_answer() {
        return cache_.get_instraction_answer();
    }

    double get_all_answer() {

        return cache_.get_all_answer();
    }

    void debug() {
        cache_.debug();
    }

    void load_all() {
        cache_.load_all();
    }

    void write_ram(string output_file, int start_address, int size) {
        memory_.write_ram(output_file, start_address, size);
    }
};
