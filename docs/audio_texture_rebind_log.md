# 音效 / 贴图重绑定修复日志

范围：本文件的音效与贴图绑定修复（**地图贴图、怪物贴图及其绑定未改动**）。
日期：2026-11-22　构建：`.\build.ps1` → `=== Build OK (805 KB) ===`

---

## 1. 问题定位（修复前）

| 类别 | 数量 | 说明 |
|---|---|---|
| 空绑定：已绑表但全代码从不播放 | 3 | `wither_shoot`、`enderdragon_shoot`、`enderman_teleport` |
| 空绑定：有玩法事件但无音效 | 12 处 | 6 件武器出手/命中、盾格挡、拾取、喝药、4 个界面的按钮 |
| 语义错绑 | 4 | `sword_swing`→命中声、凋灵骷髅借普通骷髅声、`heart_empty`→载具容器图、`buff_burn`→抗火图标 |
| 音量缺陷 | 1 | `GameOverState` 用 `setSFXVolume(20)` 永久改写全局音量；`GameState` 又把全局 SFX 设成 5（≈5%），导致所有音效几乎听不见 |
| 脏文件 / 死条目 | 3 | `assets/Sound/weapon/shield/shield.png`（PNG 在音效目录）、`point_arrow`（零引用）、`assets/resources.txt`（旧格式，代码不读） |

## 2. 目录改造（音效扁平化）

```
assets/
├─ audio/
│  ├─ sfx/    ← 新建，单层扁平，64 个文件，语义命名（slash.ogg / shield_block.ogg / zombie_hurt.wav …）
│  └─ bgm/    ← 原 assets/bgm 的 7 首 mp3 迁入
├─ images/    ← 结构不变，仅换 2 张贴图
├─ config/ fonts/
├─ resources.json
└─ resources.txt   （未删，代码不读，仅留存对照）
```

- 旧 `assets/Sound/` 整树删除；`assets/bgm/` 删除（内容已迁入 `assets/audio/bgm/`）
- 全部音效保留原扩展名（.wav / .ogg 混用，SFML 自动识别），**未做转码**
- 未新建 `assets/images/ui/`：Windows 文件系统不区分大小写，会与现有 `images/UI/` 冲突

## 3. 新增绑定（16 条 id，均为原来完全没有音效的事件）

| id | 文件 | 取自 unusedassets | 触发点 |
|---|---|---|---|
| `mace_swing` | mace_swing.ogg | sounds/item/mace/smash_air1.ogg | `MaceWeapon::attack` |
| `mace_hit` | mace_hit.ogg | sounds/item/mace/smash_ground1.ogg | `MaceWeapon::attack`（真命中时，每次挥击一次） |
| `lightning_strike_1/2/3` | lightning_strike_1.ogg / _2 / _3 | sounds/ambient/weather/thunder1.ogg、thunder2.ogg、thunder3.ogg（每次雷击三选一随机播放，`playSFXRandom`，音量 ×0.85） | `LightningRodWeapon::attack` |
| `axe_throw` | axe_throw.ogg | sounds/item/trident/throw1.ogg | `AxeWeapon::attack` |
| `tnt_throw` | tnt_throw.ogg | sounds/random/fuse.ogg | `TNTWeapon::attack` |
| `lava_throw` | lava_throw.ogg | sounds/item/bucket/empty_lava1.ogg | `LavaWeapon::attack` |
| `seaheart_shoot` | seaheart_shoot.ogg | sounds/block/bubble_column/upwards_inside.ogg | `SeaHeartWeapon::attack`（仅真正发射时） |
| `dragon_breath` | dragon_breath.ogg | sounds/random/breath.ogg | `DragonBreathWeapon::attack` |
| `shield_block` | shield_block.ogg | sounds/item/shield/block1.ogg | `ShieldWeapon::updateShields` 命中 |
| `xp_pickup` | xp_pickup.ogg | sounds/random/orb.ogg | `GameState::update` 拾取经验球 |
| `potion_drink` | potion_drink.ogg | sounds/random/drink.ogg | `PotionItem::onCollect`（所有掉落药水） |
| `ui_click` | ui_click.ogg | sounds/random/click.ogg | 主菜单 3 键 / 暂停 2 键 / 死亡界面按钮 / 跳卡 |
| `upgrade_select` | upgrade_select.ogg | sounds/block/enchantment_table/enchant1.ogg | `UpgradeState::applyOption` |
| `wither_skeleton_hurt` | wither_skeleton_hurt.ogg | sounds/mob/wither_skeleton/hurt1.ogg | `WitherSkeleton::playHitSound` |
| `wither_skeleton_death` | wither_skeleton_death.ogg | sounds/mob/wither_skeleton/death1.ogg | `WitherSkeleton::playDeathSound` |
| `boss_spawn` | boss_spawn.ogg | sounds/mob/wither/spawn.ogg | `Spawner::update` 的 `case Tier::Boss` |
| `slime_big_hurt` / `slime_big_death` | slime_big_hurt.ogg / slime_big_death.ogg | sounds/mob/slime/big1.ogg / big2.ogg | BigSlime / KingSlime |

## 4. 修正绑定（语义纠正）

| id | 修复前 | 修复后 | 原因 |
|---|---|---|---|
| `sword_swing` | weapon/sword/**sword_hit.wav** | `audio/sfx/slash.ogg`（entity/player/attack/sweep1.ogg） | 原来拿"剑命中"声当"挥砍"声 |
| `hit` | mob/player/hit1.wav | `audio/sfx/player_hurt.ogg`（damage/hit1.ogg） | 换成 MC 通用受伤声 |
| `wither_shoot` | 绑定但从不播放 | `audio/sfx/wither_shoot.ogg`（mob/wither/shoot.ogg） | 补播放点：凋灵三种弹幕每轮齐射播一次 |
| `enderdragon_shoot` | 绑定但从不播放 | `audio/sfx/enderdragon_shoot.ogg`（mob/enderdragon/growl1.ogg） | 素材库无末影龙开火声，用咆哮替代 |
| `enderman_teleport` | 绑定但从不播放 | `audio/sfx/enderman_teleport.ogg`（mob/endermen/portal.ogg） | 补播放点：末影人传送瞬移处 |
| `enderdragon_death` | mob/enderdragon/enderdragon_death.wav | mob/enderdragon/end.ogg | 原版龙死亡音 |
| `blaze_shoot` | blaze_shoot.wav | mob/blaze/breathe1.ogg | 烈焰人喷火球音 |
| `ghast_shoot` | ghast_shoot.wav | mob/ghast/fireball4.ogg | 恶魂火球音 |
| `creeper_fuse` | creeper_fuse.wav | mob/creeper/say1.ogg | 引信嘶声 |
| `slime_hurt/death` | slime_*.wav | mob/slime/small1.ogg / small2.ogg | 小型史莱姆专用 |
| `mace_swing`/`lightning_strike_*` | 借用 `sword_swing` | 各自专属音 | 消除语义错绑 |

**贴图**

| id | 修复前 | 修复后 |
|---|---|---|
| `heart_empty` | `images/UI/vehicle_container.png`（取自载具/硬核容器精灵，虽与原版 container.png 像素相同，但语义路径错） | `images/UI/heart_empty.png` ← `gui/sprites/hud/heart/container.png` |
| `buff_burn` | `mob_effect/fire_resistance.png`（"抗火"图标当"燃烧"） | `images/buff/buff_burn.png` ← `item/fire_charge.png` |

保留未动：`bubble_button`（虽名为 button，但 `BubbleProjectile.cpp:9` 的 `setTextureId("bubble_button")` 在用，内容即 `particle/bubble.png`）；`path → sand.png`（语义瑕疵，但属地图贴图绑定，红线不动）。

## 5. 代码改动清单（23 个文件，均为补播放点 / 改路径，未改架构）

| 文件 | 改动 |
|---|---|
| `src/Sound/SoundManager.h` / `.cpp` | `playSFX(id, volumeScale = 1.0f)`：新增单条音效相对音量，不动全局音量 |
| `src/Application/States/GameState.cpp` | BGM 路径→`assets/audio/bgm/`；`setSFXVolume(5)`→`50`；`setBGMVolume(5)`→`20`；拾取时 `xp_pickup` |
| `src/Application/States/MenuState.cpp` | BGM 路径；3 个按钮 `ui_click` |
| `src/Application/States/PauseState.cpp` | 引入 SoundManager；2 个按钮 `ui_click` |
| `src/Application/States/UpgradeState.cpp` | 引入 SoundManager；`applyOption`→`upgrade_select`，`skip`→`ui_click` |
| `src/Application/States/GameOverState.cpp` | 删除 `setSFXVolume(20)`，改 `playSFX("gameover", 0.2f)`；按钮 `ui_click` |
| `src/Weapon/AxeWeapon.cpp` | `axe_throw` |
| `src/Weapon/TNTWeapon.cpp` | `tnt_throw` |
| `src/Weapon/LavaWeapon.cpp` | `lava_throw` |
| `src/Weapon/SeaHeartWeapon.cpp` | `seaheart_shoot` |
| `src/Weapon/DragonBreathWeapon.cpp` | `dragon_breath` |
| `src/Weapon/ShieldWeapon.cpp` | 格挡 `shield_block` |
| `src/Weapon/MaceWeapon.cpp` | `sword_swing`→`mace_swing`；新增 `mace_hit`（`hitAny` 标记，每次挥击只响一次） |
| `src/Weapon/LightningRodWeapon.cpp` | `sword_swing`→`lightning_strike_1/2/3`（`playSFXRandom` 三选一） |
| `src/Weapon/ClockWeapon.cpp` | BGM 路径→`assets/audio/bgm/heaven.mp3` |
| `src/Monster/Enderman.cpp` | 传送时 `enderman_teleport` |
| `src/Monster/Wither.cpp` | 三种弹幕模式每轮齐射各播一次 `wither_shoot`（不在 `fireFromAngle` 内，避免每颗弹幕叠音） |
| `src/Monster/Enderdragon.cpp` | 同上，`enderdragon_shoot` |
| `src/Monster/WitherSkeleton.cpp` | 改用 `wither_skeleton_hurt/death` |
| `src/Monster/Slimes.h` | BigSlime / KingSlime 改用 `slime_big_hurt/death` |
| `src/Monster/Spawner.cpp` | 引入 SoundManager；Boss 出场 `boss_spawn` |
| `src/Item/PotionItem.cpp` | 引入 SoundManager；`onCollect` 播 `potion_drink` |
| `assets/resources.json` | sounds 段全量重写（47→64 条，全部指向 `assets/audio/sfx/`）；`heart_empty` 改指；移除死条目 `point_arrow` |

## 6. 验证结果

1. 构建：`.\build.ps1` → **Build OK (805 KB)**，零警告输出。
2. 启动实测（7 秒抓 stdout）：`Loaded 134 resources from assets/resources.json`，**无 `Failed to load`、无 `Sound buffer not found`**；`Playing BGM: assets/audio/bgm/13 Wet Hands.mp3` 说明新 BGM 路径生效。
3. 静态交叉校验：代码中 `playSFX("…")` 共 **64 个 id**，与 resources.json 的 **64 条 sounds 绑定双向一一对应**（无缺失、无死绑定）。
4. 资源文件自检：textures 68 + fonts 1 + sounds 64 条路径全部存在；`assets/audio/sfx` 内 64 个文件全部被引用，无孤儿文件。

## 7. 需人工试听确认（自动化无法覆盖）

- 音量基准：全局 SFX 由 5 改为 50、BGM 由 5 改为 20，若觉得过响/过轻可直接改 `GameState.cpp` 的 `setSFXVolume/setBGMVolume`。
- 素材音色：`enderdragon_shoot`（用咆哮代替原版开火声）、`creeper_fuse`（用 say1 代替引信嘶声）、`buff_burn`（火焰弹图标）三处是"最接近的替代品"，如耳感不符可在 `assets/audio/sfx/` 直接替换同名文件，无需改代码。
- 逐个触发：受击、拾取经验球、喝药、升级选卡、12 件武器、盾格挡、凋灵骷髅、末影人传送、凋灵/末影龙齐射、20/25 波 Boss 出场、三个界面按钮。
