// src/core/Types.h
#pragma once

#include <ostream>
#include <vector>

// ============================================================================
//  全组公共类型与常量 —— 【冻结】任何人不得私自修改，要改先喊一声
//
//  坐标约定（唯一一种）：
//      grid[y][x]   x 是列（向右增加），y 是行（向下增加）
//      Vec2{x, y} 与之一致，任何地方都不许写成 {行, 列}
//
//  代价约定：
//      全整数，以 10 为基准。直行 10，斜行 14（≈ 10·√2）
//      进入某格的代价 = 方向基准代价 × 该格地形倍率
// ============================================================================

struct Vec2 {
    int x = 0;   // 列
    int y = 0;   // 行

    constexpr Vec2() noexcept = default;
    constexpr Vec2(int x_, int y_) noexcept : x(x_), y(y_) {}

    constexpr bool operator==(const Vec2& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2& o) const noexcept { return !(*this == o); }
    constexpr Vec2 operator+(const Vec2& o) const noexcept { return Vec2{ x + o.x, y + o.y }; }
    constexpr Vec2 operator-(const Vec2& o) const noexcept { return Vec2{ x - o.x, y - o.y }; }
};

inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    return os << '(' << v.x << ',' << v.y << ')';
}

// ---- 方向表 ----------------------------------------------------------------
// 顺序 = A* 的展开顺序，会轻微影响路径形状。冻结后不许改。
inline constexpr Vec2 kDirs8[8] = {
    Vec2{ 0, -1}, Vec2{ 1, -1}, Vec2{ 1, 0}, Vec2{ 1, 1},
    Vec2{ 0,  1}, Vec2{-1,  1}, Vec2{-1, 0}, Vec2{-1,-1},
};
inline constexpr Vec2 kDirs4[4] = {
    Vec2{0, -1}, Vec2{1, 0}, Vec2{0, 1}, Vec2{-1, 0},
};

// ---- 代价常量 --------------------------------------------------------------
inline constexpr int kStepStraight = 10;
inline constexpr int kStepDiagonal = 14;
inline constexpr int kInvalidCost = -1;   // Grid 表示"走不了"的统一返回值

// 一个"方向"的基准代价
constexpr int dirBaseCost(const Vec2& d) noexcept {
    return (d.x != 0 && d.y != 0) ? kStepDiagonal : kStepStraight;
}

constexpr bool isAdjacent(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return dx <= 1 && dy <= 1 && (dx + dy) > 0;
}

// ---- 距离 ------------------------------------------------------------------
// Octile：八方向网格无障碍时的精确距离 = 10*max + 4*min = 10(dx+dy) - 6*min
// B 的八方向 A* 用它当启发函数 h（可采纳且一致）
constexpr int octile(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    const int mn = dx < dy ? dx : dy;
    return kStepStraight * (dx + dy) - (2 * kStepStraight - kStepDiagonal) * mn;
}

// Manhattan：四方向网格的精确距离，保底版 A* 用
constexpr int manhattan(const Vec2& a, const Vec2& b) noexcept {
    const int dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const int dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return kStepStraight * (dx + dy);
}

// ---- 路径 ------------------------------------------------------------------
// points[0] = 起点，points.back() = 终点。
// 相邻两点必须相邻；【相同 = 原地等待一拍】。
struct Path {
    std::vector<Vec2> points;
    int  cost = 0;
    bool valid = false;
};

// ---- 地图文件里的智能体描述（运行期 Agent 由 C 定义，别混用）---------------
struct AgentSpec {
    Vec2 start;
    Vec2 goal;
    int  priority = 0;   // 数字越小越优先
};
