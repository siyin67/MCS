#include "Localization/UiText.h"

#include "Resourcemanager/ResourceManager.h"

#include <filesystem>
#include <vector>

namespace {

// 原版字体：资源表里的 main_font（assets/fonts/minecraft.ttf）
const sf::Font &mojangFont() {
    ResourceManager &rm = ResourceManager::GetInstance();
    if (rm.HasFont("main_font")) return rm.GetFont("main_font");
    static const sf::Font empty;
    return empty;
}

// GNU Unifont：只有需要兜底时才加载，文件不在就是 nullptr
const sf::Font *unifontFont() {
    static const sf::Font *font = []() -> const sf::Font * {
        const char *candidates[] = {"assets/fonts/unifont.otf", "assets/fonts/unifont.ttf"};
        for (const char *path : candidates) {
            std::error_code ec;
            if (!std::filesystem::exists(path, ec)) continue;
            auto *loaded = new sf::Font();
            if (loaded->openFromFile(path)) {
                return loaded;
            }
            delete loaded;
        }
        return nullptr;
    }();
    return font;
}

std::vector<char32_t> decodeUtf8(const std::string &s) {
    std::vector<char32_t> out;
    out.reserve(s.size());
    for (std::size_t i = 0; i < s.size();) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        char32_t cp = 0;
        std::size_t extra = 0;
        if (c < 0x80) {
            cp = c;
        } else if ((c & 0xE0) == 0xC0) {
            cp = c & 0x1F;
            extra = 1;
        } else if ((c & 0xF0) == 0xE0) {
            cp = c & 0x0F;
            extra = 2;
        } else if ((c & 0xF8) == 0xF0) {
            cp = c & 0x07;
            extra = 3;
        } else { // 坏的字节，跳过
            ++i;
            continue;
        }
        if (i + extra >= s.size()) break;
        for (std::size_t k = 1; k <= extra; ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
        out.push_back(cp);
        i += extra + 1;
    }
    return out;
}

bool needsFallback(const sf::Font &font, const std::string &utf8) {
    for (const char32_t cp : decodeUtf8(utf8)) {
        if (cp == U'\n' || cp == U'\r' || cp == U'\t') continue;
        // 非 ASCII（中文/日文/韩文/带音标的名字…）一律用 Unifont：
        // 原版字体里的汉字笔画太细，和像素风画面不搭。
        if (cp >= 0x80) return true;
        // 纯 ASCII 里遇到原版字体没有的字形（生僻符号）也交给 Unifont。
        if (!font.hasGlyph(cp)) return true;
    }
    return false;
}

} // namespace

void ui_text::set(sf::Text &text, const std::string &utf8) {
    const sf::Font &base = mojangFont();
    const sf::Font *fallback = unifontFont();
    // 只有原版字体缺字时才换成 Unifont；其余情况保持调用方原本的字体不变
    const bool useFallback = (fallback != nullptr && needsFallback(base, utf8));
    if (useFallback) text.setFont(*fallback);
    text.setString(sf(utf8));
    if (useFallback) {
        // Unifont 是 16x16 点阵，放大后关掉纹理平滑能少点灰边；
        // 必须放在 setString 之后（setString 会光栅化新字形，图集扩容时纹理会被重建）。
        const unsigned int size = text.getCharacterSize();
        if (size > 0) const_cast<sf::Texture &>(fallback->getTexture(size)).setSmooth(false);
    }
}
