#ifndef READ_HPP
#define READ_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> // for find_if
#include <cctype> // for isspace
#include <locale>
#include <string.h> // for memset

/**
 * @brief 输入一个字符串，根据给定的分隔符进行分割
 * @param input: 输入字符串
 * @param separator：输入的分隔符
 * @return 一个 vector
 * @from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
 */
std::vector<std::string> split(const std::string input, const std::string separator);

/**
 * @brief 输入一个字符串，去掉两边的不可见字符
 * @param input: 输入字符串
 * @return 处理好的字符串
 * @from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
 */
std::string trim(std::string input);

/**
 * @brief 一次性读取整个文件，并把文件存在 string 中
 * @param file_path: 文件的路径
 * @return 处理好的字符串
 */
std::string read_file(std::string file_path);

#endif