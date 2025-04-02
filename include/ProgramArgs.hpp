#pragma once
#include <map>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

/**
 * @deprecated use Arguments instead
 */
namespace App {
namespace [[deprecated("Use Arguments instead.")]] ProgramArgs {
    // abbrMap: 缩写映射表，例如：{"-h", "--help"} 表示 "-h" 是 "--help" 的缩写
    using AbbrMapType = std::unordered_map<std::string_view, std::string_view>;

    // 解析命令行参数，返回一个包含所有参数的 vector<string_view> 对象
    std::vector<std::string_view> parse(int argc, char* argv[], AbbrMapType abbrMap = {});
    // 检查参数是否合法，knownArgs 为合法的参数列表，showErrorMsg 为是否显示错误信息
    bool check(std::vector<std::string_view> args, std::vector<std::string_view> knownArgs, bool showErrorMsg = true);
    // 获取参数的值。兼容--key=value和-k value两种格式。注：传入index会被更改（为了for循环中遇到-k的参数）
    std::optional<std::string_view> getValue(std::vector<std::string_view> args, int& index, bool showErrorMsg = true);
}
}

// ----------

/**
 * @brief 用于处理程序参数
 */
namespace Arguments {
    struct BasicArgument {
        // 参数名或内容
        std::string_view content;
    };
    struct Option : BasicArgument {
        bool isAbbr;
        std::optional<std::string_view> value;
    };
    struct PositionalArgument : BasicArgument {
        size_t index; // 第几个位置参数，从0开始
    };
    using Arguments = std::vector<BasicArgument*>;

    struct BasicArgumentTemplate {
        /**
         * @brief 参数名
         * 如果是Option，则为选项全称（如--help）
         * 如果是PositionalArgument，则为参数名（如file）
         */
        std::string_view name;
        std::string_view description;
        bool required = false;
    };
    struct OptionTemplate : BasicArgumentTemplate {
        // 缩写（如-h）
        std::optional<std::string_view> abbr;
        // 是否有值
        bool hasValue = false;
    };
    struct PositionalArgumentTemplate : BasicArgumentTemplate { };
    using ArgumentTemplates = std::vector<BasicArgumentTemplate*>;

    std::map<BasicArgumentTemplate*, bool> _getRequiredStatus(const ArgumentTemplates& templates)
    {
        std::map<BasicArgumentTemplate*, bool> result;
        for (const auto& tem : templates) {
            result[tem] = tem->required;
        }
        return result;
    }

    std::optional<Arguments> parse(int argc, char** argv, const ArgumentTemplates& templates)
    {
        bool parseOption = true;
        size_t positionalIndex = 0;
    }

} // namespace Arguments