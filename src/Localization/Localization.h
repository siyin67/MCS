#pragma once
// 多语言：assets/lang/<code>.json
//
//   {
//     "display": "简体中文（中国大陆）",   // 语言列表里显示的名字
//     "english": "Chinese (Simplified)",   // 英文名（可省略）
//     "strings": { "menu.start": "开始游戏", ... }
//   }
//
// 丢一个新 json 进 assets/lang 就会被自动识别，不用改代码；
// 缺字符串时按 当前语言 -> en_us -> key 本身 的顺序回退。
#include <map>
#include <string>
#include <vector>

class Localization
{
public:
    struct LanguageInfo
    {
        std::string code;    // 文件名（不含扩展名），也是 settings.json 里存的值
        std::string display; // 列表里显示的名字
        std::string english; // 英文名，用于 "中文（English）" 这类拼接
    };

    static Localization &getInstance();

    // 扫描目录里的 *.json，并读取 settings.json 里记住的语言
    void initialize(const std::string &langDir = "assets/lang",
                    const std::string &settingsPath = "assets/config/settings.json");

    const std::vector<LanguageInfo> &languages() const { return languages_; }
    const std::string &getLanguage() const { return current_; }

    // 切换语言（能保住 settings.json 里其它分组），失败返回 false
    bool setLanguage(const std::string &code);

    // 每次切换语言 +1：界面据此判断要不要重新取字符串
    int version() const { return version_; }

    std::string tr(const std::string &key) const;

    // 这个 key 在当前语言（或 en_us）里有没有
    bool has(const std::string &key) const;

    // 取不到就用 fallback（例如武器 config 里的英文名 / 描述）
    std::string trOr(const std::string &key, const std::string &fallback) const;

    // 带占位符：trf("hud.level", {"5"}) → "等级 5"（模板里写 {0} {1} …）
    std::string trf(const std::string &key, const std::vector<std::string> &args) const;

    // 同上，但取不到 key 时用 fallback 当模板
    std::string trfOr(const std::string &key, const std::string &fallback,
                      const std::vector<std::string> &args) const;

private:
    Localization() = default;

    void loadFile(const std::string &path, const std::string &code);
    void saveLanguage() const;

    std::map<std::string, std::map<std::string, std::string>> tables_; // code -> key -> 文本
    std::vector<LanguageInfo> languages_;
    std::string current_ = "zh_cn";
    std::string langDir_ = "assets/lang";
    std::string settingsPath_ = "assets/config/settings.json";
    int version_ = 1;
};

// 语法糖：tr("menu.start")
inline std::string tr(const std::string &key) { return Localization::getInstance().tr(key); }

// 取不到 key 时用 fallback
inline std::string trOr(const std::string &key, const std::string &fallback)
{
    return Localization::getInstance().trOr(key, fallback);
}

// 占位符版本：trf("hud.level", {"5"})
inline std::string trf(const std::string &key, const std::vector<std::string> &args)
{
    return Localization::getInstance().trf(key, args);
}

inline std::string trfOr(const std::string &key, const std::string &fallback,
                         const std::vector<std::string> &args)
{
    return Localization::getInstance().trfOr(key, fallback, args);
}
