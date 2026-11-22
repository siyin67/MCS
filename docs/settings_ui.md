# 设置界面 / 多语言（SettingsState → SoundOptionsState / LanguageState）

仿原版「选项」的三级结构：**主菜单 / 暂停菜单 → 选项 → 音乐和声音…｜语言…**。
都是 pushState 压栈的叠加层，`StateManager::render` 会逐层绘制整个栈，所以下层界面作为背景透出来。

```
主菜单 ──Settings──> 选项 ──音乐和声音…──> 音乐和声音选项（5 条滑条）
  ↑                   │
  └───Done/ESC────────┴──语言…──────────> 语言（列表 + 滚动）

暂停菜单 ──Settings──> 选项 ──同上──> …
```

## 选项页（SettingsState）

两列按钮 + 底部 Done，全部 `menu_button` 贴图、白色字：

```
                      选项

[ 音乐和声音… ]            [ 语言… ]

[                  完成                  ]
```

* 每条 400x40，列间距 40，整体宽 840；Done 840x50，距底 60。
* 只列出真正实现了的项（没有视频/按键这些没做的功能）。

## 音乐和声音选项（SoundOptionsState）

```
                  音乐和声音选项

[ 主音量: 75% ================================ ]   <- 整行
[ 武器音量: 85% =========== ]  [ 敌对生物音量: 100% ]
[ 音乐: 20% =============== ]  [ 音效: 50% ======== ]

[                  完成                  ]
```

* 主音量独占一行（宽 840），其余 2x2（每格 400，行高 40、行距 72）。
* 文字**居中画在滑条上**，滑块是 8px 浅色竖条，位置 = 值 / 100 画在文字下面。
* **在滑条任意位置按下即跳到该值**，按住拖动实时生效；值域 0~100、步进 5。
* 悬停/拖动中的滑条加 1px 白描边；松开播放一次试听音（主音量/音效 → `ui_click`，
  武器 → `sword_swing`，怪物 → `zombie_hurt`，音乐不放音，避免和正在播的 BGM 打架）。
* Done / ESC 返回选项页，都带 `ui_click`。

## 语言页（LanguageState）

* 语言来自 `assets/lang/*.json`，一行一个，行高 30、列表宽 520，超出屏幕用滚轮滚动（右侧 6px 滚动条）。
* 选中行画一块深色底 `rgba(70,70,76,235)`，点击即 `Localization::setLanguage(code)` 并落盘。
* 底部提示「语言翻译不一定 100% 准确」，Done 返回。
* 换语言后所有已打开的界面通过 `langVersion_` 比较自动刷新文案。

## 音量模型

```
最终音效音量 = sfxVolume/100 × masterVolume/100 × 分类倍率 × 单次 volumeScale
最终音乐音量 = bgmVolume/100 × masterVolume/100
```

* 分类倍率按 id 查表：`weaponVolume`（15 个武器音效 id，含 `lightning_strike_1/2/3` 三个变体）、
  `monsterVolume`（43 个怪物音效 id），其余（UI、升级、拾取、喝药、游戏结束）走 100%。
* 同一件事有多套音效时用 `playSFXRandom({"a","b","c"}, scale)`：只从已加载的候选里随机挑一个，
  例如雷击武器每次在 `thunder1/2/3` 里随机，不会每次都听到同一段。
* `GameState` / `MenuState` 里原来写死的 `setSFXVolume(50)`、`setBGMVolume(20)` 已删除，
  否则每次开局都会覆盖玩家设置。

## 多语言：`assets/lang/*.json`

文件名即语言 code，加一门语言只要丢一个 json 进去（不用改代码、不用登记）：

```json
{
  "display": "简体中文（中国大陆）",
  "english": "Chinese (Simplified)",
  "strings": {
    "menu.start": "开始游戏",
    "options.title": "选项",
    "sound.master": "主音量"
  }
}
```

* `display` 是语言列表里显示的名字，`english` 备用；`strings` 是键值表。
* 查找顺序：当前语言 → `en_us` → 键名本身，所以缺翻译不会显示空白。
* 已用到的键：
  * 界面：`menu.*`、`pause.*`、`gameover.*`、`options.*`（title / sound / language / done）、
    `sound.*`（title / master / weapon / monster / bgm / sfx / done）、`language.*`（title / note / done）。
  * 游戏内：`hud.level`（`等级 {0}` / `Lv. {0}`）；
    `upgrade.title`、`upgrade.type_new`、`upgrade.type_upgrade`、`upgrade.level_from_to`、
    `upgrade.level_new`、`upgrade.stats`（占位符 `{0}`/`{1}`）；
    每把武器 `weapon.<id>.name`、`weapon.<id>.desc`（id 就是 `WeaponFactory` 的 key：
    sword / bow / shield / tnt / trident / clock / axe / dragon / seaheart / lava / mace / lightingrod）。
* `src/Localization/Localization.h` 提供单例和这些函数：
  * `tr(key)`：取不到就回退，最终返回 key 本身；
  * `trOr(key, fallback)`：取不到用 fallback（武器名/描述 fallback 到 `assets/config/weapon/*.json` 的英文 `name`）；
  * `trf(key, {"5"})` / `trfOr(key, fallback, args)`：把模板里的 `{0}`、`{1}` 换成实参；
  * `version()`：换语言时自增，各界面 render 里比对后自动刷新文字。
  启动时 `Application::Application` 里 `initialize("assets/lang", "assets/config/settings.json")`
  扫描目录并读回上次选的语言，日志 `Language loaded: zh_cn (简体中文（中国大陆）)`。
* 武器 id 记在 `Weapon::id_`（`WeaponFactory::create()` 创建后 `setId(id)`），
  升级界面因此能用 `weapon.<id>.name` 查本地化名字，不用靠英文名反查。

## 字体规则（`src/Localization/UiText.h`）

* **UTF-8 必须走 `sf::String::fromUtf8`**：`sf::Text::setString(std::string)` 会按 Latin-1
  逐字节解释，中文直接变乱码（这就是之前 `ăŌ¨ŀŧĿŏ` 的原因）。统一用 `ui_text::set(text, utf8)`。
* 字体选择：ASCII → `assets/fonts/minecraft.ttf`（原版像素字体）；
  文字里**只要有一个非 ASCII 字符**（中文/日文/带音标的名字）→ 整段改用
  `assets/fonts/unifont.otf`（GNU Unifont 16x16 点阵）；纯 ASCII 里遇到原版缺字形也交给 Unifont。
* Unifont 文件不存在时自动退回原版字体，不会崩。

## 配置持久化：`assets/config/settings.json`

```json
{
  "language": "zh_cn",
  "audio": { "master": 100, "weapon": 100, "monster": 100, "bgm": 20, "sfx": 50 }
}
```

| 时机 | 行为 |
| --- | --- |
| 启动（文件存在） | 读取，缺字段落回默认、越界值夹到 0~100；日志 `Settings loaded: ... (master 100, weapon 85, ...)` |
| 启动（文件不存在） | 用默认值并立刻写出一份，方便手动编辑；日志 `Settings file created: ...` |
| 启动（文件损坏） | 捕获 `nlohmann::json::parse_error` → 回退默认并重写文件；日志 `Settings file was invalid, rewritten with defaults: ...` |
| 拖滑条 / 换语言 | 值真正变化时才写文件（写前先 load 一次，避免把 language / audio 互相覆盖） |

解析在 `src/Config/SettingsStore.h`（header-only），路径由 `SoundManager::kSettingsPath` 指定。

## 素材与风格

| 用途 | 贴图 |
| --- | --- |
| 滑条底 | `assets/images/UI/slider_track.png`（原版 `gui/sprites/widget/slider.png`） |
| 滑块 | `assets/images/UI/slider_handle.png`（原版 `slider_handle.png`） |
| 按钮 | `assets/images/UI/menu_button.png`（与主菜单/暂停按钮同款） |
| 字体 | `assets/fonts/minecraft.ttf` + `assets/fonts/unifont.otf` |

选项页遮罩 `rgba(8,8,8,238)`（能看到菜单透出来）；两个二级页用不透明底 `rgba(12,12,14,255)`，
避免上一级按钮在文字后面晃。

滑块尺寸跟滑条框同高（`handles_[i].setSize({HANDLE_W, BAR_H})`，`HANDLE_W = 12`），
看着像原版那根竖条，而不是小方块。

升级界面（`UpgradeState`）：`upgrade_background` 是浅灰底图，所以面板保持白色、文字改用深色
（标题 `(45,45,45)`、武器名 `(30,30,30)`、描述 `(60,60,60)`、数值 `(95,95,95)`，`新武器` 绿色
`(0,120,0)`、`升级` 棕黄 `(165,105,0)`）；卡片、图标框和文字对象都在 `buildCards()` 里建一次，
`refreshTexts()` 只改字符串，`updateLayout()` 只改位置。

武器图标改成 `sf::Sprite` + 等比缩放（`scale = ICON_SIZE / max(texW, texH)`，再按包围盒居中），
不再用 `RectangleShape::setSize({80,80})` 硬拉成正方形 —— 这就是之前「贴图诡异拉伸」的原因。

## 武器栏冷却遮罩

原版没有冷却遮罩的贴图（`unusedassets/textures/gui/sprites/hud/` 里只有 `jump_bar_cooldown.png`，
那是跳跃蓄力条），原版是代码画的一块半透明白色矩形，所以这里也照着画：

```
remain = clamp(weapon->getCurrentCooldown() / weapon->getAttackCooldown(), 0, 1)
遮罩高度 = 槽内高 * remain，贴着槽底对齐（顶边随 remain 变小而往下退）
颜色     = rgba(255,255,255,130)，左右/上下各内缩 3px 不糊住槽位边框
```

绘制位置在 `src/UI/UIManager.cpp` 的武器图标之后、等级数字之前，所以冷却中图标会被压暗、
等级数字仍然清楚。冷却走完 `remain = 0` 直接不画。

## 新增 / 修改的文件

| 文件 | 说明 |
| --- | --- |
| `src/Application/States/SettingsState.h/.cpp` | 选项页（两列按钮） |
| `src/Application/States/SoundOptionsState.h/.cpp` | 音乐和声音选项（5 条滑条） |
| `src/Application/States/LanguageState.h/.cpp` | 语言列表（滚动 + 选中） |
| `src/Localization/Localization.h/.cpp` | 语言文件扫描、`tr/trOr/trf`、当前语言落盘 |
| `src/Localization/UiText.h/.cpp` | UTF-8 解码 + 字体回退 |
| `assets/lang/zh_cn.json`、`en_us.json` | 语言文件（界面 + `hud.*` / `upgrade.*` / `weapon.*`） |
| `src/Application/States/UpgradeState.h/.cpp` | 白面板 + 深色文字、图标等比缩放、文案 `tr/trOr/trf` |
| `src/UI/UIManager.cpp` | HUD 等级文字 `trf("hud.level", …)`；武器栏冷却遮罩 |
| `src/Weapon/weapon.h`、`WeaponFactory.cpp` | 记录工厂 id（`Weapon::id_`），供 `weapon.<id>.*` 查表 |
| `src/Config/SettingsStore.h` | settings.json 读写与容错 |
| `src/Sound/SoundManager.h/.cpp` | 总线 + 分类音量，setter 自动落盘，playBGM 随 master 变化 |
| `src/Application/States/MenuState.cpp`、`PauseState.cpp`、`GameOverState.cpp` | 文案改 `tr()`，增加 Settings 入口，换语言自动刷新 |
| `src/Application/Application.cpp` | 启动时初始化 Localization |
| `assets/resources.json` | 新增 `slider_track`、`slider_handle` 两个贴图 id |
| `assets/config/settings.json` | 音量 + 语言 |
| `build.ps1` | 增量编译（见下） |

## 构建：增量编译

`build.ps1` 现在按文件编译：

* 每个 `src/**.cpp` 编到 `obj/<同名>.o`；`.o` 比源文件新、且比 `src/` 下**最新**的 `.h`
  还新就跳过（没有依赖表，用“全工程最新头文件”做保守判断）。
* 最后把所有 `.o` 一次性链接成 `bin\game.exe`，链接标志与原来一致。
* 全是增量时只做链接。

| 场景 | 耗时 |
| --- | --- |
| 无改动（只链接） | ~1.4s |
| 改 1 个 .cpp | ~11s |
| 改 1 个 .h（全量 64 个 .o） | 一次全编 |

另外两处坑（都已写进脚本）：

* 受限沙箱里 `& $Gpp @args 2>&1 | ...` 会报 `拒绝访问` / `StandardOutputEncoding ...`，
  改用 `Start-Process -NoNewWindow -Wait`（子进程直接继承控制台）后无需提权即可编译。
* 编译中间文件默认落系统 TEMP，C 盘满时报 `No space left on device`，
  现在把子进程的 `TEMP`/`TMP` 指到工程内 `.tmp\`（E 盘）。

## 验证

`docs/ui_smoke_test.ps1` 跑一整轮（20 个检查点，截图当场存 `docs/screenshots/`，看完即清）：
菜单 → 选项 → 声音页点/拖滑条 → 选项 → 语言页 en/zh 来回切 → 换回中文 → 开始游戏 →
游戏里按 `9` 触发升级界面（截图 + 选一张）→ ESC 暂停 → 暂停内设置 → 声音页 → 返回 →
继续 → ESC → 回主菜单。

* `settings.json` 落盘 `master 75 / weapon 85 / language zh_cn`（(850-220)/840=0.75、(560-220)/400=0.85 精确命中）。
* 日志 `Language loaded: zh_cn` → `Language switched: en_us` → `zh_cn`。
* 升级界面（按 9）验证了 `upgrade.title` / `weapon.<id>.name|desc` / `upgrade.stats` 的中文渲染，
  HUD 等级显示为 `等级 1`。
* 字体规则实测：纯 ASCII 用 `minecraft.ttf`，出现非 ASCII（中文）整段换 `assets/fonts/unifont.otf`；
  文本一律经 `ui_text::set()`（内部 `sf::String::fromUtf8`）写进 `sf::Text`，直接 `setString(std::string)` 会乱码。
