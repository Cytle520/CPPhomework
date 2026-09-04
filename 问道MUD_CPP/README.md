# 《问道》— C++ 古典哲学 MUD 文字游戏

一款以中国诸子百家哲学为内核的单机 MUD 文字游戏，采用**数字菜单操作**，无需记忆任何指令。

---

## 编译与运行

### 方法一：Dev-C++（推荐，课程常用）
1. 打开 Dev-C++
2. 文件 → 新建 → 项目 → Console Application → C++ 项目
3. 把 `include/` 下所有 `.h` 和 `src/` 下所有 `.cpp` 以及 `main.cpp` 加入项目
4. 工具 → 编译选项 → 在"编译器"标签中勾选"编译时加入以下命令"，填入 `-std=c++17`
5. 按 F11 编译运行

### 方法二：MinGW / g++ 命令行
```bash
g++ -std=c++17 -I include -O2 src/*.cpp main.cpp -o game.exe
game.exe
```
或直接双击运行 `build.bat`

### 方法三：Visual Studio
1. 新建空项目（C++ 控制台应用）
2. 把所有 `.h` 加入头文件，所有 `.cpp` 加入源文件
3. 项目属性 → C/C++ → 语言 → C++ 标准 → 选择 `ISO C++17`
4. Ctrl+F5 运行

> **注意**：需要 C++17 标准（使用了结构化绑定等特性）

---

## 操作说明（全程数字菜单）

游戏主菜单：
```
  【主菜单】
  1. 移动        → 选择方向编号
  2. 对话        → 选择NPC编号
  3. 论道（战斗）→ 选择可战斗的NPC
  4. 拾取物品    → 选择物品编号
  5. 背包        → 查看/使用/装备
  6. 状态        → 查看角色属性
  7. 技能        → 查看已学招式
  8. 任务        → 查看当前目标
  9. 商店        → 购买物品（需在商人处）
  0. 休息        → 恢复气血和心神
  q. 退出游戏
```
- 任何菜单直接按**回车**可返回上一级
- 战斗中：1.直言 2.招式 3.物品 4.逃跑

---

## 项目结构

```
问道MUD_CPP/
├── main.cpp                  主入口
├── build.bat                 一键编译脚本（MinGW）
├── README.md                 本文件
│
├── include/                  头文件（.h）
│   ├── game_data.h           数据结构定义 + 数据加载器
│   ├── player.h              玩家类
│   ├── npc.h                 NPC类
│   ├── room.h                房间类
│   ├── item.h                物品类
│   ├── skill.h               技能类
│   ├── combat.h              战斗系统
│   ├── story.h               剧情系统
│   ├── ui.h                  界面渲染
│   └── game.h                游戏主控制器
│
├── src/                      实现文件（.cpp）
│   ├── game_data.cpp         INI解析 + 数据加载
│   ├── player.cpp            属性/背包/升级
│   ├── npc.cpp               对话/战斗AI
│   ├── room.cpp              场景/出口
│   ├── ui.cpp                彩色界面/数字菜单
│   ├── combat.cpp            回合制论道
│   ├── story.cpp             主线任务追踪
│   └── game.cpp              菜单调度/游戏循环
│
├── data/                     ★ 游戏数据（改内容只动这里）
│   ├── maps.txt              18个房间
│   ├── npcs.txt              19个NPC
│   ├── items.txt             15种物品
│   ├── skills.txt            14个技能
│   ├── levels.txt            9重境界
│   └── story.txt             6章主线剧情
│
└── 设计文档/
    └── 面向对象设计文档.md    WBS/用例图/UML类图/流程图
```

---

## ★ 关键元素修改指南

所有游戏内容存储在 `data/` 目录的文本文件中，格式为 `[节名]` + `key=value`。**修改内容无需改动代码**，重新编译运行即可。

### 1. 修改地图 / 添加房间 → `data/maps.txt`

```ini
[房间id]
name=房间名称
desc=房间描述（玩家进入时看到）
exit北=目标房间id
exit东=目标房间id
npc=npc_id1
item=物品id1
start=1    ← 只有出生房间加这行
```
- `exit` 后面跟方向名（支持任意中文方向）
- 一个房间可以有多个 `exit`、`npc`、`item` 行

### 2. 修改NPC / 添加人物 → `data/npcs.txt`

```ini
[npc_id]
name=人物名
title=身份头衔
desc=外貌描述
type=enemy          ← mentor/enemy/merchant/npc 四种
level=2
hp=120
attack=12
defense=8
exp=30              ← 击败后获得的经验
dialogue=第一句对话
dialogue=第二句对话  ← 多次对话循环显示
defeat=被击败后说的话（enemy类型用）
skill=技能id1
skill=技能id2
drop=掉落物品id
shop=商品id1        ← merchant类型用
shop=商品id2
```

### 3. 修改物品 → `data/items.txt`

```ini
[物品id]
name=物品名
type=consumable     ← consumable(消耗品)/equipment(装备)/quest(任务物品)
desc=描述
slot=weapon         ← 装备类型用：weapon/armor
usable=1            ← 1可使用 0不可使用
effect=heal:50      ← heal:数值 / buff:属性:数值:回合 / exp:数值
price=25
```

### 4. 修改技能 → `data/skills.txt`

```ini
[技能id]
name=技能名
desc=描述
type=attack         ← attack/heal/buff
damage=1.5          ← 伤害倍率（attack类型）
cost=8              ← 消耗心神
heal=25             ← 治疗量（heal类型）
buff_stat=defense   ← buff类型
buff_value=8
duration=3
unlock=2            ← 哪个境界解锁
```

### 5. 修改升级境界 → `data/levels.txt`

```ini
[base]              ← 玩家初始属性
hp=80
mp=20
attack=8
defense=5
money=50

[level1]            ← 按level1~level9顺序
name=初学
exp=0               ← 升到这级需要的经验
hp_grow=10          ← 升级时增加的属性
mp_grow=5
atk_grow=2
def_grow=1
desc=境界描述
unlock=技能id       ← 到达此境界自动学会
unlock=技能id2
```

### 6. 修改剧情 → `data/story.txt`

```ini
[ch1]
title=第一章 · 标题
step1_text=步骤描述
step1_objective=目标说明
step1_type=talk     ← talk/defeat/get_item/enter_room/use_item_on
step1_target=npc_id ← 目标NPC/物品/房间ID
step1_dialogue=触发时显示的对话（可选）
step1_reward_exp=30 ← 完成奖励经验（可选）
step1_reward_item=物品id ← 完成奖励物品（可选）
step2_text=...
```

### 7. 修改战斗机制 → `src/combat.cpp`
- 伤害公式：`calc_damage()` 函数
- 战斗流程：`start()` 函数
- NPC AI：`src/npc.cpp` 的 `choose_skill()` 函数

### 8. 修改界面风格 → `src/ui.cpp`
- 颜色：`include/ui.h` 的 `Color` 命名空间
- 标题画面：`show_title()` 函数
- 菜单样式：`main_menu()`、`move_menu()` 等函数

### 9. 添加新菜单项 → `src/game.cpp`
- 在 `main_menu()`（ui.cpp）中添加选项
- 在 `game_loop()`（game.cpp）的 switch 中添加 case
- 实现对应的 `do_xxx()` 方法

---

## 游戏世界观

玩家从白鹿书院出发，历经六章：
1. **初入书院** — 拜师颜夫子，研读《易经》
2. **格物致知** — 论道场切磋，问道青云道人
3. **下山历练** — 游历清平镇，体察民间疾苦
4. **朝堂风云** — 入都城，辩儒法之争
5. **兼爱非攻** — 赴边关，见墨将军，悟战争与和平
6. **问道** — 归书院，与自己论道，融会百家

升级境界取自《大学》八条目：初学→格物→致知→诚意→正心→修身→齐家→治国→平天下。

---

## 技术说明

- **语言**：C++17（无第三方依赖）
- **架构**：数据驱动 + MVC思想 + 面向对象
- **数据格式**：自定义INI格式（UTF-8编码）
- **界面**：终端彩色文字（ANSI转义码，Windows 10+原生支持）
- **类数量**：10个核心类（GameData/Player/NPC/Room/Item/Skill/Combat/StoryManager/UI/Game）
