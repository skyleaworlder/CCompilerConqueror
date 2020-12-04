#include "utils.hpp"

std::vector<std::string> split(
    std::string input,
    std::string separator
) {
    // pos_start 表示开始寻找的位置
    // 而 pos_end 是 separator 出现的末尾位置，一开始没有赋值
    size_t pos_start = 0, pos_end;
    size_t sepa_len = separator.length();

    std::vector<std::string> to_ret;
    while ((pos_end = input.find(separator, pos_start)) != std::string::npos) {
        std::string token = input.substr(pos_start, pos_end-pos_start);
        pos_start = pos_end + sepa_len;
        to_ret.push_back(token);
    }

    // 收尾工作
    to_ret.push_back(input.substr(pos_start));
    return to_ret;
}

inline std::string ltrim(std::string input) {
    input.erase(input.begin(), std::find_if(input.begin(), input.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return input;
}

inline std::string rtrim(std::string input) {
    input.erase(std::find_if(input.rbegin(), input.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), input.end());
    return input;
}

std::string trim(std::string input) {
    return rtrim(ltrim(input));
}