#pragma once

#include "constant.h"
#include <vector>
#include <unordered_map>

#include "memory.h"

using namespace std;


struct CacheLine {
    bool valid = false;
    int tag = 0;
    int last_upd = 0;
    bool dirty = false;
    int index_plru = 0;

    bool last_plru = false;

    vector<byte> data;

    CacheLine() : data(CACHE_LINE_SIZE) {
    }

    void clear() {
        valid = false;
        last_upd = 0;
        dirty = false;
        tag = 0;
        data.resize(CACHE_LINE_SIZE);
    }
};


class Cache {
private:
    int time_ = 1;

    int get_tag(int address) {
        return (address & ((1 << 17) - (1 << 10))) >> 10;
    }

    int get_index(int address) {
        return (address & ((1 << 10) - (1 << 6))) >> 6;
    }

    int get_offset(int address) {
        return address & ((1 << 6) - 1);
    }

    bool is_hit(int address) {
        int tag = get_tag(address);
        int index = get_index(address);
        if (cache_[index].empty()) return false;
        if (cache_[index].find(tag) == cache_[index].end()) return false;
        return true;
    }

    void get_new_cache(int address) {
        int tag = get_tag(address);
        int index = get_index(address);
        address = address & 131008;
        if (cache_[index].size() == CACHE_WAY) {
            if (is_plru_) {
                get_plru(address);
                return;
            }
            get_lru(address);
            return;
        }
        cache_[index][tag] = CacheLine();
        cache_[index][tag].data = memory_.read_cache_line(address);
        cache_[index][tag].valid = true;
        cache_[index][tag].index_plru = cache_[index].size() - 1;
    }

    void get_lru(int address) {
        int key = 0;
        int min_time = 1e9;
        int tag = get_tag(address);
        int index = get_index(address);
        for (auto &[i, j]: cache_[index]) {
            if (j.last_upd < min_time) {
                min_time = j.last_upd;
                key = i;
            }
        }
        if (cache_[index][key].dirty) {
            int load_address = (key << 10) + (index << 6);
            memory_.load_cache_line(load_address, cache_[index][key].data);
        }
        cache_[index].erase(key);
        cache_[index][tag] = CacheLine();
        cache_[index][tag].data = memory_.read_cache_line(address);
        cache_[index][tag].valid = true;
        cache_[index][tag].index_plru = cache_[index].size() - 1;
    }

    void get_plru(int address) {
        int tag = get_tag(address);
        int index = get_index(address);
        int key = -1;
        int ma = 1e9;
        for (auto &[i, j]: cache_[index]) {
            if (j.last_plru == false && ma > j.index_plru) {
                key = i;
                ma = j.index_plru;
            }
        }
        if (key == -1) {
            for (auto &[i, j]: cache_[index]) {
                if (j.index_plru == 0) key = i;
                j.last_plru = false;
            }
        }
        if (cache_[index][key].dirty) {
            int load_address = (key << 10) + (index << 6);
            memory_.load_cache_line(load_address, cache_[index][key].data);
        }
        int ind = cache_[index][key].index_plru;
        cache_[index].erase(key);
        cache_[index][tag] = CacheLine();
        cache_[index][tag].data = memory_.read_cache_line(address);
        cache_[index][tag].valid = true;    
        cache_[index][tag].index_plru = ind;
    }

    vector<unordered_map<int, CacheLine> > cache_;

    Memory &memory_;
    bool is_plru_ = false;

    int data_hit_ = 0;
    int data_attempts_ = 0;
    int instraction_hit_ = 0;
    int instraction_attempts_ = 0;

public:
    Cache(Memory &memory, bool plru) : memory_(memory), is_plru_(plru) {
        cache_.resize(CACHE_SET_COUNT);
    }


    void write_data(int address, int value, int size) {
        int tag = get_tag(address);
        int index = get_index(address);
        int offset = get_offset(address);
        bool hitted = is_hit(address);
        data_hit_ += hitted;
        data_attempts_++;
        if (hitted) {
            for (int i = 0; i < size; ++i) {
                cache_[index][tag].data[offset + i] = byte(value);
                value >>= 8;
            }
            cache_[index][tag].last_upd = time_++;
            cache_[index][tag].dirty = true;
            cache_[index][tag].last_plru = true;
            return;
        }
        get_new_cache(address);
        for (int i = 0; i < size; ++i) {
            cache_[index][tag].data[offset + i] = byte(value);
            value >>= 8;
        }
        cache_[index][tag].last_upd = time_++;
        cache_[index][tag].dirty = true;
        cache_[index][tag].last_plru = true;
    }

    int read_data(int address, int size) {
        int tag = get_tag(address);
        int index = get_index(address);
        int offset = get_offset(address);
        bool hitted = is_hit(address);
        data_hit_ += hitted;
        data_attempts_ ++;
        int ans = 0;
        if (hitted) {
            for (int i = 0; i < size; i++) {
                ans <<= 8;
                ans += (int) cache_[index][tag].data[offset + size - 1 - i];
            }
            cache_[index][tag].last_upd = time_++;
            cache_[index][tag].last_plru = true;
            return ans;
        }
        get_new_cache(address);
        for (int i = 0; i < size; i++) {
            ans <<= 8;
            ans += (int) cache_[index][tag].data[offset + size - 1 - i];
        }
        cache_[index][tag].last_upd = time_++;
        cache_[index][tag].last_plru = true;
        return ans;
    }

    int read_instruction(int address) {
        int tag = get_tag(address);
        int index = get_index(address);
        int offset = get_offset(address);
        bool hitted = is_hit(address);
        instraction_hit_ += hitted;
        instraction_attempts_ ++;
        int ans = 0;
        if (hitted) {
            for (int i = 0; i < 4; i++) {
                ans <<= 8;
                ans += (int) cache_[index][tag].data[offset + 3 - i];
            }
            cache_[index][tag].last_upd = time_++;
            cache_[index][tag].last_plru = true;
            return ans;
        }
        get_new_cache(address);
        for (int i = 0; i < 4; i++) {
            ans <<= 8;
            ans += (int) cache_[index][tag].data[offset + 3 - i];
        }
        cache_[index][tag].last_upd = time_++;
        cache_[index][tag].last_plru = true;
        return ans;
    }

    double get_data_answer() {
        return abs(double(data_hit_) / double(data_attempts_));
    }

    double get_instraction_answer() {
        return abs(double(instraction_hit_) / double(instraction_attempts_));
    }

    double get_all_answer() {
        
        return abs(double(instraction_hit_ + data_hit_) / double(instraction_attempts_ + data_attempts_));
    }


    void debug() {
        cout << instraction_hit_ << " instr " << instraction_attempts_ << endl;
        cout << data_hit_ << " data " << data_attempts_ << endl;
    }

    void load_all() {
        for (size_t i = 0; i < cache_.size(); ++i) {
            for (auto& [k, j] : cache_[i]) {
                if (j.dirty) {
                    int address = (k << 10) + (i << 6);
                    memory_.load_cache_line(address, j.data);
                }
            }   
        }
    }

};
