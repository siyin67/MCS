# 《C++课程设计》综合应用管理系统开发报告

**题目：Minecraft Survivor — 2D俯视角Minecraft主题生存游戏**

---

## 一、项目内容描述

Minecraft Survivor 是一款基于 C++ 和 SFML 3.0 图形库开发的2D俯视角生存游戏，灵感来源于《吸血鬼幸存者》（Vampire Survivors）与《我的世界》（Minecraft）。玩家操控Minecraft风格的角色在程序化生成的地图上移动，武器自动攻击周围的怪物，击杀怪物获得经验值，升级后可选择并强化武器。游戏目标是存活尽可能久、击败尽可能多的怪物。

**核心玩法：**
- 玩家使用 WASD 键移动，鼠标控制攻击方向
- 12种武器各有独特的攻击方式和冷却时间
- 怪物按波次（Wave）生成，共4个等级梯度（Common、Uncommon、Rare、Boss）
- 击杀怪物掉落经验球，满经验自动升级，随机3把武器选择1把
- 怪物掉落药水（加速、加攻、无敌、治疗）
- 支持输入Minecraft正版ID，通过Mojang API获取角色皮肤
- 5000×5000程序化生成地形，包含草地、泥土、石头、水域、岩浆等地形

---

## 二、小组成员及其分工

| 学号 | 姓名 | 工作量占比 | 工作内容 |
|------|------|------------|----------|
| （填写） | （填写） | （填写） | 项目架构设计、Application层、状态管理系统（StateManager/IState/MenuState/GameState/GameOverState/PauseState/UpgradeState）、游戏主循环 |
| （填写） | （填写） | （填写） | 玩家系统（移动/血量/无敌帧/击退/经验升级）、武器系统（基类+12种武器+工厂+管理器）、投射物系统（ProjectileManager+9种投射物） |
| （填写） | （填写） | （填写） | 怪物系统（基类+14种怪物+生成器Spawner）、怪物AI与技能、Buff系统（9种状态效果） |
| （填写） | （填写） | （填写） | 碰撞检测系统（ICollidable接口+AABB快速排除+SAT精确检测+CollisionResolver）、程序化地图生成（Perlin噪声+分形噪声）、UI系统（HUD血条/经验条/武器栏/Buff图标/计时器） |
| （填写） | （填写） | （填写） | 资源管理（ResourceManager单例+JSON配置加载）、音效系统（SoundManager单例+BGM播放列表+音效管理）、网络模块（Mojang API皮肤获取）、掉落物系统（经验球+药水） |

> 说明：以上分工为建议划分，具体工作量占比由组长根据实际贡献评定，每位成员工作量占比范围为0.3-1，全组贡献度之和为1。

---

## 三、需求分析

### 3.1 功能需求

**F1 — 玩家操控**
- 支持 WASD 四方向移动，对角线移动自动归一化
- 鼠标控制攻击方向，显示方向指示箭头
- 玩家有生命值（20点）、无敌帧（受伤后0.2秒）、击退效果

**F2 — 武器系统**
- 12种武器：铁剑、弓、盾牌、TNT、三叉戟、时钟、飞斧、龙息、海洋之心、熔岩桶、重锤、避雷针
- 每种武器有独立的攻击方式、冷却时间、伤害值、攻击范围
- 武器支持升级，增加伤害和范围
- 武器自动攻击范围内的怪物
- 最多携带6把武器

**F3 — 怪物系统**
- 14种怪物：僵尸、骷髅、蜘蛛、史莱姆、溺尸、旋风人、苦力怕、末影人、女巫、烈焰人、凋零骷髅、恶魂、凋零、末影龙
- 怪物具有不同属性：速度、血量、伤害、经验奖励
- 部分怪物具有特殊技能：远程攻击（骷髅）、爆炸（苦力怕）、药水投掷（女巫）、火焰弹（烈焰人/恶魂）、龙息（末影龙）、凋零之首（凋零）等
- 冻结机制：部分武器可冻结怪物

**F4 — 波次生成系统**
- 按时间推进波次，每波约30秒
- 4级梯度：Common（波1起）、Uncommon（波7起）、Rare（波15起）、Boss（波20起）
- 各梯度按概率随机抽取，随波次提升稀有怪物比例
- 怪物在玩家视野外环形区域随机生成
- 每波怪物数量上限递增，生成间隔递减

**F5 — 经验和升级**
- 击杀怪物掉落经验球（XP Orb），玩家自动拾取
- 升级阈值 = 当前等级 × 100
- 升级时弹出武器选择界面，3选1
- 2%概率掉落药水（加速/加攻/无敌/治疗）

**F6 — 碰撞检测**
- AABB 包围盒快速排除（第一级）
- SAT 分离轴定理精确检测（第二级，仅对投射物）
- Player-Monster 碰撞：玩家受伤 + 击退
- Bullet-Monster 碰撞：投射物造成伤害

**F7 — 地图生成**
- 基于Perlin噪声 + 分形噪声的程序化地形生成
- 7种地形：草地、泥土、石头、圆石、沙地、水域、岩浆
- 高度图 + 湿度图双参数控制
- 地图边界为水域
- 312×312 tile，16×16像素/tile，有效5000×5000

**F8 — UI系统**
- 红心血条（满/半/空心）
- 经验条 + 等级显示
- 物品栏（最多6个武器图标 + 等级）
- 游戏计时器（MM:SS）
- Buff 图标显示（屏幕右侧）

**F9 — 音效与音乐**
- BGM播放列表，随机切换，自动循环
- 独立打击音效（怪物受伤/死亡、武器挥舞等）
- 音效与BGM独立音量控制

**F10 — 网络功能**
- 输入Minecraft正版ID → 调用Mojang API获取UUID → 获取皮肤纹理
- 异步加载，不阻塞UI

### 3.2 非功能需求
- 帧率限制60FPS，保证流畅体验
- 窗口分辨率1280×720，支持最大2560×1440
- 无外部依赖安装需求（DLL随exe分发）

---

## 四、设计与实现

### 4.1 系统功能划分 — 模块结构

```
Minecraft Survivor
│
├── Application 层（应用框架）
│   ├── Application          — 游戏主循环、窗口管理、全局事件
│   ├── StateManager         — 状态栈管理器（push/pop/change/clearAndPush）
│   └── States/              — 5种游戏状态
│       ├── IState           — 状态接口（handleInput/update/render/handleEvent）
│       ├── MenuState        — 主菜单（输入ID获取皮肤、开始游戏、退出）
│       ├── GameState        — 核心游戏状态（地图/战斗/碰撞/波次/UI/掉落）
│       ├── PauseState       — 暂停界面
│       ├── UpgradeState     — 武器升级选择（3选1）
│       └── GameOverState    — 游戏结束界面
│
├── 实体系统（Entity）
│   ├── Player               — 玩家（移动/血量/无敌帧/击退/经验/升级/武器持有）
│   ├── Monster/             — 怪物系统
│   │   ├── Monster          — 怪物基类（属性/技能/冻结/Buff/碰撞接口）
│   │   ├── Spawner          — 波次生成器（梯度概率/CD/数量控制）
│   │   └── 14种怪物子类     — Zombie/Skeleton/Spider/Slimes/Drowned/Breeze/
│   │                           Creeper/Enderman/Witch/Blaze/WitherSkeleton/
│   │                           Ghast/Wither/Enderdragon
│   └── Projectile/          — 投射物系统
│       ├── Projectile       — 投射物基类
│       ├── ProjectileManager— 投射物管理器
│       └── 9种投射物        — Arrow/Axe/Bubble/Trident/TNT/Lava/
│                               DragonBreath/MonsterProjectile
│
├── 武器系统（Weapon）
│   ├── Weapon               — 武器基类（伤害/CD/范围/升级/投射物回调）
│   ├── WeaponManager        — 武器管理器（最多6把，更新/渲染/升级）
│   ├── WeaponFactory        — 武器工厂（单例，注册+创建模式）
│   └── 12种武器子类
│
├── 碰撞系统（Collision）
│   ├── ICollidable          — 碰撞接口（getGlobalBounds/getVertices/getType）
│   ├── Collision            — AABB快速排除 + SAT精确检测 + 批量检测
│   └── CollisionResolver    — 碰撞结果处理（玩家受伤/击退/死亡清理/掉落）
│
├── 物品系统（Item）
│   ├── DropItem             — 掉落物基类
│   ├── DropItemManager      — 掉落物管理器
│   ├── XpOrb                — 经验球（自动追踪玩家）
│   └── PotionItem           — 药水（加速/加攻/无敌/治疗Buff）
│
├── Buff系统
│   └── BuffManager          — Buff管理器（SpeedUp/Down, AttackUp/Down,
│                               Invincible/Heal/Burn/Poison/Wither，叠加+Tick）
│
├── UI系统
│   └── UIManager            — HUD（红心血条/经验条/武器图标/等级/计时器/Buff图标）
│
├── 资源管理
│   ├── ResourceManager      — 资源管理器（单例，纹理/字体/JSON缓存）
│   └── ConfigLoader         — JSON配置加载器（nlohmann/json）
│
├── 音频系统
│   └── SoundManager         — 音效管理器（单例，BGM播放列表/随机切换/循环/
│                               音效播放/独立音量）
│
└── 网络模块
    └── MojangAPI            — Mojang API（正版ID→UUID→皮肤纹理，异步加载）
```

**各模块输入输出关系：**

| 模块 | 输入 | 输出 |
|------|------|------|
| StateManager | 键盘事件、窗口引用 | 当前状态的handleInput/update/render调用 |
| Player | 键盘输入(WASD)、鼠标位置、deltaTime、碰撞事件 | 位置、血量状态、攻击方向、武器攻击触发 |
| Monster | 玩家位置、deltaTime、同行怪物列表 | 位置、血量状态、对玩家伤害、死亡掉落 |
| Spawner | 玩家位置、deltaTime、当前怪物列表 | 新怪物实例、波次状态 |
| WeaponManager | 玩家位置、攻击方向、怪物列表、deltaTime | 投射物（通过回调）、怪物伤害 |
| ProjectileManager | 投射物（来自武器/怪物）、deltaTime | 碰撞检测、范围伤害、渲染 |
| Collision系统 | ICollidable对象列表 | 碰撞对列表 → CollisionResolver处理 |
| UIManager | 玩家引用、deltaTime | HUD渲染 |
| ResourceManager | 资源路径 | 纹理/字体/JSON对象引用 |

### 4.2 详细设计

#### 4.2.1 类设计

**核心类关系图：**

```
                    ┌──────────────┐
                    │  Application │  (游戏主循环 + 窗口)
                    └──────┬───────┘
                           │ 持有
                    ┌──────▼───────┐
                    │ StateManager │  (状态栈)
                    └──────┬───────┘
                           │ 管理
                    ┌──────▼───────┐
                    │    IState    │  (状态接口)
                    └──────┬───────┘
           ┌────────┬──────┼──────┬──────────┬────────┐
           │        │      │      │          │        │
      MenuState  GameState PauseState UpgradeState GameOverState
                      │
         ┌────────────┼────────────────┐
         │            │                 │
    ┌────▼───┐  ┌─────▼──────┐  ┌──────▼────────┐
    │Player  │  │Spawner     │  │ProjectileMgr  │
    └───┬────┘  └─────┬──────┘  └──────┬────────┘
        │             │                 │
   ┌────▼────┐  ┌─────▼──────┐  ┌──────▼────────┐
   │WeaponMgr│  │14种Monster │  │9种Projectile  │
   └────┬────┘  └────────────┘  └───────────────┘
        │
   ┌────▼──────┐
   │12种Weapon │
   └───────────┘

   所有实体类（Player/Monster/Bullet/DropItem）实现 ICollidable 接口
                    ┌──────────────┐
                    │ ICollidable  │  <<interface>>
                    ├──────────────┤
                    │+getGlobalBounds() → FloatRect
                    │+getVertices()     → vector<Vector2f>
                    │+getType()         → CollisionType
                    └──────────────┘
```

**关键类的成员与职责：**

**Player 类**（[player.h](src/Player/player.h)）
| 成员 | 类型 | 说明 |
|------|------|------|
| shape_ | sf::RectangleShape | 玩家矩形（64×64，缩放0.8） |
| speed_ | float | 基础移动速度 500px/s |
| health_ / maxHealth_ | int | 当前/最大生命值 20 |
| isInvincible_ | bool | 无敌帧标志（受伤后0.2秒） |
| knockbackVelocity_ | sf::Vector2f | 击退速度（指数衰减） |
| buffManager_ | BuffManager | 玩家Buff管理器 |
| weaponManager_ | WeaponManager | 武器管理器（最多6把） |
| currentXP_ / level_ | int | 经验值/等级 |
| attackDirection_ | sf::Vector2f | 鼠标指向的攻击方向 |

**Monster 基类**（[Monster.h](src/Monster/Monster.h)）
| 成员 | 类型 | 说明 |
|------|------|------|
| shape_ | sf::RectangleShape | 怪物矩形 |
| speed_ | float | 移动速度 |
| health_ / maxHealth_ | int | 血量（随波次增加） |
| damage_ | int | 对玩家造成的伤害 |
| expReward_ | int | 击杀经验奖励 |
| skillFunc_ | std::function | 技能函数（可配置） |
| frozen_ / frozenTimer_ | bool/float | 冻结状态 |
| buffManager_ | BuffManager | 怪物Buff管理器 |

**Weapon 基类**（[weapon.h](src/Weapon/weapon.h)）
| 成员 | 类型 | 说明 |
|------|------|------|
| baseDamage_ / damage_ | int | 基础伤害/当前伤害 |
| level_ | int | 武器等级 |
| attackCooldown_ | float | 攻击冷却时间 |
| currentCooldown_ | float | 当前冷却计时 |
| range_ | float | 攻击范围 |
| addProjectileCallback_ | function | 投射物生成回调 |
| levelUpCfg_ | json | 升级配置（从JSON加载） |

#### 4.2.2 核心算法 — 碰撞检测

采用**两级检测策略**，兼顾性能与精度：

```
checkCollision(objA, objB):
    // 第一级：AABB 快速排除（O(1)，利用SFML内置）
    if NOT aabbCollide(boundsA, boundsB):
        return false
    
    // 第二级：仅当涉及投射物时启用 SAT 精确检测
    if involvesBullet(typeA, typeB):
        return satCollide(vertsA, vertsB)
    
    // 其他情况（Player-Monster）：AABB相交即认为碰撞
    return true
```

**SAT 算法流程：**

```
satCollide(vertsA, vertsB):
    1. 收集所有分离轴：
       对于多边形A的每条边 → 计算法线轴(-edge.y, edge.x) → 归一化 → 加入axes
       对于多边形B的每条边 → 同上
    2. 在每个轴上检测投影区间重叠：
       projectOntoAxis(vertsA, axis) → [minA, maxA]
       projectOntoAxis(vertsB, axis) → [minB, maxB]
       若 maxA < minB 或 maxB < minA → 存在分离轴 → return false
    3. 所有轴上投影均重叠 → return true
```

#### 4.2.3 核心算法 — 程序化地图生成

```
generateMap():
    对每个坐标 (x, y):
        height = getHeight(x, y)    // 分形噪声合成高度值
        moisture = getMoisture(x, y) // 独立噪声生成湿度值
        tileType = getTileType(height, moisture)
        根据tileType绘制对应纹理tile到sf::RenderTexture
    将RenderTexture转换为sf::Texture作为地图精灵

getHeight(x, y):
    mainHeight = fractalNoise(x*0.3, y*0.3, 4)  // 4 octaves 主地形
    detail     = fractalNoise(x*1.2, y*1.2, 2) * 0.2  // 细节扰动
    return clamp(mainHeight*0.8 + detail, -1.0, 1.0)

fractalNoise(x, y, octaves):
    value = 0; amplitude = 1.0; persistence = 0.5
    对 i = 0..octaves-1:
        value += amplitude * (PerlinNoise(x*freq, y*freq)*2-1)
        amplitude *= persistence; frequency *= 2
    return value

getTileType(height, moisture):
    height < -0.40: 水
    height < -0.25: 沙滩
    height <  0.10: 草地
    height <  0.20: moisture>0.5?草地:沙滩
    height <  0.35: 草地
    height <  0.50: 泥土
    height <  0.65: 石头
    height <  0.80: moisture>0.5?石头:圆石
    其他: 圆石（山顶）
```

#### 4.2.4 核心算法 — 波次怪物生成

```
Spawner.update(deltaTime, playerPos, monsters):
    waveTimer += deltaTime
    若 waveTimer >= waveDuration:
        currentWave++
        maxAlive = 80 + currentWave * 12
        spawnInterval = max(0.1, 0.4 - currentWave * 0.02)
        waveDuration = max(15, 30 - currentWave * 0.3)
    
    若 存活怪物数 < maxAlive 且 spawnTimer >= spawnInterval:
        tier = rollTier()        // 按概率随机梯度
        monster = 从该梯度可用工厂中随机选择一个创建
        monster.setPosition(getSpawnPosition(playerPos)) // 玩家视野外环形区域
        monster.addHealth(按tier*currentWave加成)
        monsters.push_back(monster)

rollTier():
    概率分布随波次变化:
    波1-3:   Common 80%, Uncommon 20%
    波4-6:   Common 70%, Uncommon 28%, Rare 2%
    波7-9:   Common 60%, Uncommon 35%, Rare 5%
    波10-12: Common 50%, Uncommon 40%, Rare 9%, Boss 0.5%
    波13-15: Common 40%, Uncommon 45%, Rare 14%, Boss 1%
    波16-18: Common 30%, Uncommon 45%, Rare 23%, Boss 1.5%
    波19+:   Common 20%, Uncommon 45%, Rare 30%, Boss 2%
```

#### 4.2.5 重点函数 — 游戏主循环 (Application::run)

```cpp
void Application::run() {
    while (window_.isOpen() && isRunning_) {
        float deltaTime = clock_.restart().asSeconds();

        handleGlobalEvents();           // 1. 处理窗口事件
        stateManager_.handleInput(window_);  // 2. 当前状态处理输入
        stateManager_.update(deltaTime);     // 3. 当前状态更新逻辑
        window_.clear();
        stateManager_.render(window_);       // 4. 当前状态渲染
        window_.display();
    }
}
```

#### 4.2.6 重点函数 — GameState::update (游戏核心更新)

该函数是游戏的核心逻辑入口，每帧调用一次。其执行流程为：

```
GameState::update(deltaTime):
    1. 确保所有武器有投射物回调
    2. 更新玩家（移动/无敌帧/击退/Buff tick）
    3. 更新武器攻击
    4. 更新盾牌（特殊处理）
    5. 更新怪物生成器
    6. 更新所有怪物（移动/技能/状态）
    7. 更新投射物管理器（移动/追踪/索敌/碰撞/死亡清理）
    8. 更新掉落物管理器 + 检测玩家拾取
    9. 若经验满 → 弹出升级界面
    10. AABB/SAT碰撞检测 → 碰撞解决（玩家受伤/击退）
    11. 清理死亡怪物 → 生成经验球和药水
    12. 处理延迟生成（怪物技能产生的子怪物/投射物/外力）
    13. 怪物投射物与玩家碰撞检测
    14. 区域效果（龙息等）对玩家Buff刷新
    15. 岩浆地板对怪物灼烧
    16. 更新相机跟随玩家
    17. 更新UI
    18. 检查BGM循环
```

### 4.3 功能实现

#### 4.3.1 主菜单功能

- 背景显示、游戏标题、Minecraft风格字体
- 文本输入框：输入Minecraft正版ID
- 点击"确认"按钮 → 异步调用Mojang API获取皮肤 → 头像预览
- 点击"开始游戏" → 进入GameState
- 点击"退出" → 关闭窗口
- 窗口缩放时自动重新布局

#### 4.3.2 玩家战斗系统

- **移动**：WASD输入 → 归一化对角线 → 乘以有效速度（受Speed Buff影响）→ 边界限制(5000×5000)
- **无敌帧**：受伤触发0.2秒无敌，红白闪烁视觉效果
- **击退**：受击方向 × 400速度，指数衰减（decay=10.0）
- **经验/升级**：拾取XP Orb → currentXP累加 → 超过阈值(level×100)则level++，返回升级次数 → 弹出UpgradeState

#### 4.3.3 武器攻击模式详解

| 武器 | 攻击方式 | 代码实现要点 |
|------|----------|-------------|
| 铁剑 | 近战扇形范围攻击 | 遍历怪物，计算距离和点积判断是否在攻击扇形内 |
| 弓 | 直线箭矢 | 创建Arrow投射物，沿攻击方向飞行 |
| 盾牌 | 360°旋转碰撞 | 圆形环绕玩家旋转，与怪物AABB交叉即伤害 |
| TNT | 投掷爆炸 | 创建TNT投射物，落地检测范围内所有怪物 |
| 三叉戟 | 追踪最近怪物 | 每帧找到最近怪物，调整飞行方向 |
| 时钟 | 重置所有冷却 | 遍历WeaponManager所有武器，resetCooldown() |
| 飞斧 | 回旋往返 | 投射物飞出→达到最大距离→飞回，往返均伤害 |
| 龙息 | 放置毒雾区域 | 通过spawnAreaCallback创建AreaEffect，每0.6秒刷新Buff |
| 海洋之心 | 弹射水滴 | 命中怪物后弹向最近的其他怪物，有弹射次数限制 |
| 熔岩桶 | 放置熔岩地板 | 在地面放置LavaFloor，每0.6秒对踩踏怪物施加燃烧 |
| 重锤 | 圆形范围重击 | 以玩家为中心，检测圆形范围内所有怪物 |
| 避雷针 | 连锁闪电 | 命中怪物后跳跃到范围内最近的另一怪物，有跳跃次数限制 |

#### 4.3.4 异常情况处理

- **空指针检查**：所有 `ICollidable*`、`Monster*`、`Weapon*` 使用前检查非空
- **边界检查**：玩家位置限制在地图范围内（halfSize ~ 5000-halfSize），怪物生成位置同样限制
- **武器数量上限**：最多6把武器，超过不再添加
- **怪物数量上限**：每波maxAlive控制，防止无限堆积
- **经验溢出**：while循环处理连续升级（`while(currentXP >= getXPToNext())`）
- **资源加载失败**：ConfigLoader/ResourceManager输出cerr日志，返回空json/nullptr
- **JSON解析异常**：try-catch捕获 `nlohmann::json::parse_error`
- **零向量防护**：计算方向前检查向量长度 > 0.01f，避免除零
- **音频文件缺失**：SoundManager内部容错，不影响游戏运行

---

## 五、项目展示

### 5.1 主菜单界面

（请在此处粘贴主菜单运行截图）

**功能说明**：显示游戏标题、开始/退出按钮、Minecraft ID输入框及皮肤预览区域。

### 5.2 游戏主界面

（请在此处粘贴游戏运行截图，包含HUD）

**功能说明**：显示地图、玩家角色、怪物、投射物、HUD（左上红心血条、经验条、底部物品栏、右上Buff图标、顶部计时器）。

### 5.3 升级选择界面

（请在此处粘贴升级选择截图）

**功能说明**：升级时弹出3把随机武器，鼠标悬停高亮，点击选择。已有武器则升级该武器。

### 5.4 暂停与游戏结束界面

（请在此处分别粘贴暂停界面和游戏结束界面截图）

---

### 5.5 运行说明

**环境要求**：Windows 10/11，无需安装任何依赖

**启动方式**：双击 `game.exe` 启动游戏

**操作指南**：

| 操作 | 按键 |
|------|------|
| 移动 | W / A / S / D |
| 攻击方向 | 鼠标 |
| 暂停/继续 | Esc |
| 选择升级武器 | 鼠标点击 |
| 恢复血量（调试） | Space |
| 生成全部怪物（调试） | M |
| 触发升级（调试） | 9 |
| 触发结束（调试） | 0 |

---

## 六、心得体会

（以下由各成员分别填写）

### 成员1：（姓名）
**收获**：（填写本次实验中掌握的知识、技能或工程经验）

**不足**：（填写遇到的困难、未解决的问题或可改进之处）

---

### 成员2：（姓名）
**收获**：（填写）

**不足**：（填写）

---

### 成员3：（姓名）
**收获**：（填写）

**不足**：（填写）

---

### 成员4：（姓名）
**收获**：（填写）

**不足**：（填写）

---

### 成员5：（姓名）
**收获**：（填写）

**不足**：（填写）

---

*报告完成日期：2026年6月*
