#pragma once

// 文案写入 + 字体兜底。
//
// 1) sf::Text::setString(std::string) 是按 Latin-1/ANSI 解释字节的，直接把 UTF-8
//    塞进去会变成 "Ĉ-Ăŀ..." 这样的乱码（每个字节当成一个字符）。所有文案都走 set()，
//    内部用 sf::String::fromUtf8 正确解码。
//
// 2) 字体策略：ASCII 用 Mojang 的 assets/fonts/minecraft.ttf（原版像素字体）；
//    只要这段文字里出现非 ASCII 字符（中文、日文、带音标的名字…），
//    整段改用 assets/fonts/unifont.otf（GNU Unifont，16x16 点阵），
//    因为原版字体的汉字笔画太细，和像素画面不搭。
//    原版字体缺字形（生僻符号）时同样交给 Unifont 兜底。
//    unifont 文件不存在时保持原版字体，不会崩。

#include <SFML/Graphics.hpp>
#include <string>

namespace ui_text {

inline sf::String sf(const std::string &utf8) {
    return sf::String::fromUtf8(utf8.begin(), utf8.end());
}

// 写入文案；原版字体缺字时自动切到 Unifont，否则保持调用方原有的字体。
void set(sf::Text &text, const std::string &utf8);

} // namespace ui_text
