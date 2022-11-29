#pragma once

using namespace std;

bool cmdOptionExists(char** begin, char** end, const string& option) {
    return find(begin, end, option) != end;
}

char* getCmdOption(char** begin, int argc, const string& option) {
    char** itr = find(begin, begin + argc, option);
    if (itr != begin + argc && ++itr != begin + argc) {
        return *itr;
    }
    return 0;
}

char* getCmdOptionOrDefault(char** begin, int argc, const string& option, char* defaultValue) {
    char* found = getCmdOption(begin, argc, option);
    if (found == 0) {
        return defaultValue;
    }
    return found;
}