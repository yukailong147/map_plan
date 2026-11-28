#pragma once
// ============================================================================
//  Types.h —— 全组公共类型与常量
//
//  【冻结文件】任何人都不能私自修改。要加东西，先在群里说一声。
//
//  1) 坐标约定（全项目唯一一种，不许有第二种）
//        grid[y][x]   x = 列（向右增大）
//                     y = 行（向下增大）
//        Vec2{x, y}   与之一致。
//        ★ 任何地方都不许写成 {行, 列}，写反了整个项目全错，而且很难查。
//
//  2) 代价约定（全整数，不使用浮点）
//        直行 10，斜行 14（≈ 10·√2）
//        进入某格的代价 = 方向基准代价 × 该格地形倍率
//        例：斜着走进草地 = 14 × 2 = 28
//        ★ 全整数的原因：A* 靠 f = g + h 比大小，浮点会有 1.0000001 的抖动，
//          导致路径不稳定、单元测试断言写不出来。
// ============================================================================

#include <ostream>
#include <vector>

// ---------------------------------------------------------------------------
// 二维坐标 / 向量
//   选 struct 而不是 class：它是个纯数据聚合，没有需要保护的不变量。
//   故意做成"可比较、可相加、可打印"的小对象，方便放进 vector、当 map 的键。
// ---------------------------------------------------------------------------
struct Vec2 {
    int x = 0;   // 列
    int y = 0;   // 行

    constexpr Vec2() noexcept = default;
    constexpr Vec2(int x_, int y_) noexcept : x(x_), y(y_) {}

    constexpr bool operator==(const Vec2& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2& o) const noexcept { return !(*this == o); }
    constexpr Vec2 operator+ (const Vec2& o) const noexcept { return Vec2{ x + o.x, y + o.y }; }
    constexpr Vec2 operator- (const Vec2& o) const noexcept { return Vec2{ x - o.x, y - o.y }; }
};

// 调试打印：(3,5)
// ★ 写在头文件里的普通函数必须 inline，否则被两个 .cpp 包含就重复定义（LNK2005）
inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    return os << '(' << v.x << ',' << v.y << ')';
}

// ---------------------------------------------------------------------------
// 方向表
//   kDirs8 的顺序 = A* 的展开顺序，会轻微影响路径形状。
//   【冻结后不许改】，改了要通知全组。
// ---------------------------------------------------------------------------
inline constexpr Vec2 kDirs8[8] = {
    Vec2{ 0, -1}, Vec2{ 1, -1}, Vec2{ 1, 0}, Vec2{ 1, 1},
    Vec2{ 0,  1}, Vec2{-1,  1}, Vec2{-1, 0}, Vec2{-1,-1},
};

inline constexpr Vec2 kDirs4[4] = {
    Vec2{0, -1}, Vec2{1, 0}, Vec2{0, 1}, Vec2{-1, 0},
};

// ---------------------------------------------------------------------------
// 代价常量（单位：1/10 步）
// ---------------------------------------------------------------------------
inline constexpr int kStepStraight = 10;   // 直行
inline constexpr int kStepDiagonal = 14;   // 斜行 ≈ 10·√2
inline constexpr int kInvalidCost = -1;   // Grid 表示"走不了"的统一返回值

// 一个"方向"的基准代价（不看地形）
constexpr int dirBaseCost(const Vec2& d) noexcept {
    return (d.x != 0 && d.y != 0) ? kStepDiagonal : kStepStraight;
}

// 两点是否相邻（含斜向）；自己到自己返回 false
constexpr bool isAdjacent(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return dx <= 1 && dy <= 1 && (dx + dy) > 0;
}

// ---------------------------------------------------------------------------
// 距离函数
// ---------------------------------------------------------------------------

// Octile 距离：八方向网格在【空地图】上的精确最短代价。
//   直走 max 步 + 斜走 min 步 = 10·max + 4·min = 10(dx+dy) - 6·min
// B 的八方向 A* 用它当启发函数 h：
//   - 它删掉了墙、穿角限制、地形倍率，是"放松版问题"的精确解
//   - 所以它永远【不高估】真实剩余代价（可采纳性），A* 因此保证最短路
constexpr int octile(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    const int mn = dx < dy ? dx : dy;
    return kStepStraight * (dx + dy) - (2 * kStepStraight - kStepDiagonal) * mn;
}

// Manhattan 距离：四方向网格的精确距离，保底版四方向 A* 用。
// ★ 八方向地图上【不要】用它，会高估，A* 就不保证最优了。
constexpr int manhattan(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return kStepStraight * (dx + dy);
}

// ---------------------------------------------------------------------------
// 路径
//   points[0]     = 起点
//   points.back() = 终点
//   相邻两点必须相邻；【两点相同 = 原地等待一拍】。
//   用"重复点"表示等待，好处是普通 A* 和时空 A* 的输出格式完全一致，
//   仿真循环里只需要看 points[pathIndex + 1] 一个东西。
// ---------------------------------------------------------------------------
struct Path {
    std::vector<Vec2> points;
    int  cost = 0;
    int  startTick = 0;     // 路径第 0 个点对应的时刻（重规划时用；第一版恒为 0）
    bool valid = false; // false 表示无解（不抛异常，用返回值表达失败）
};

// ---------------------------------------------------------------------------
// 地图文件里描述一个智能体的四元组
//   ★ 运行期的 Agent（含路径、当前进度、等待帧数）由 C 在 Simulation 里定义，
//     不要和这个混用。
// ---------------------------------------------------------------------------
struct AgentSpec {
    Vec2 start;
    Vec2 goal;
    int  priority = 0;   // 数字越小越优先（越先规划）
};
