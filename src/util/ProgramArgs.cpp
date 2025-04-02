#include "ProgramArgs.hpp"
#include <algorithm>
#include <iostream>
#include <string_view>

#ifndef EXECUTABLE_NAME
#define EXECUTABLE_NAME "app"
#endif

namespace App::ProgramArgs {
    std::vector<std::string_view> parse(int argc, char* argv[], AbbrMapType abbrMap)
    {
        std::vector<std::string_view> result;
        for (int i = 1; i < argc; ++i) {
            std::string_view arg = argv[i];
            // 处理缩写参数
            if (arg.starts_with("-")) {
                auto iter = abbrMap.find(arg);
                // 如果找到了缩写对应的完整参数，则将其添加到结果中
                if (iter != abbrMap.end())
                    result.push_back(iter->second);
                // 测试缩写是否是多字符缩写，如果是则逐个处理每个字符对应的完整参数
                else if (arg.size() > 2) {
                    for (int j = 1; j < arg.size(); ++j) {
                        std::string_view subArg = &"-"[arg[j]];
                        auto iter = abbrMap.find(subArg);
                        if (iter != abbrMap.end())
                            result.push_back(iter->second);
                        else
                            result.push_back(subArg);
                    }
                } else
                    result.push_back(arg);
            } else
                result.push_back(arg);
        }
        return result;
    }
    bool check(std::vector<std::string_view> args, std::vector<std::string_view> knownArgs, bool showErrorMsg)
    {
        for (auto& arg : args) {
            if (!arg.starts_with("-"))
                continue;
            if (arg.starts_with("--") && arg.find("=") != std::string_view::npos)
                arg = arg.substr(0, arg.find("="));
            if (std::find(knownArgs.begin(), knownArgs.end(), arg) == knownArgs.end()) {
                if (showErrorMsg)
                    std::cerr << "Unknown argument: " << arg << "\nTry `" << EXECUTABLE_NAME << "-h` for more information." << std::endl;
                return false;
            }
        }
        return true;
    }
    std::optional<std::string_view> getValue(std::vector<std::string_view> args, int& index, bool showErrorMsg)
    {
        if (index >= args.size()) {
            if (showErrorMsg)
                std::cerr << "Internal Error: Index out of range when getting argument value.(" << index << "/" << args.size() << ")" << std::endl;
            return {};
        }
        std::string_view arg = args[index];
        if (arg.starts_with("--")) {
            if (arg.find('=') != std::string_view::npos)
                return arg.substr(arg.find('=') + 1);
            if (showErrorMsg)
                std::cerr << "Error: Argument `" << arg << "` is missing its value." << '\n';
            return {};
        }
        if (arg.starts_with("-")) {
            if (index + 1 == args.size()) {
                if (showErrorMsg)
                    std::cerr << "Error: Argument `" << arg << "` is missing its value." << '\n';
                return {};
            }
            return args[++index];
        }
        if (showErrorMsg)
            std::cerr << "Internal Error: Argument `" << arg << "` is not a valid argument." << '\n';
        return {};
    }
} // namespace App::ProgramArgs
