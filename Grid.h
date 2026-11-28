// src/core/Grid.h
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "Types.h"

// 地形：枚举值就是「进入这一格的代价倍率」，-1 专表示不可通行。
//   用 enum class 而不是 #define，避免和别的常量撞名。
enum class Terrain : int {
    Wall = -1,   // 墙
    Free = 1,    // 空地，倍率 1
    Grass = 2,    // 草地，倍率 2
    Swamp = 5,    // 沼泽，倍率 5
};

class Grid {
public:
    Grid() = default;
    Grid(int width, int height, Terrain fill = Terrain::Free);

    int  width()  const noexcept { return w_; }
    int  height() const noexcept { return h_; }
    bool empty()  const noexcept { return cells_.empty(); }

    bool inBounds(const Vec2& p) const noexcept {
        return p.x >= 0 && p.x < w_ && p.y >= 0 && p.y < h_;
    }

    // 这一格能不能站人（越界一律 false —— B 展开邻居时不用自己判边界）
    bool passable(const Vec2& p) const noexcept {
        return inBounds(p) && at(p) != Terrain::Wall;
    }

    // 越界返回 Wall，调用方不用先判断
    Terrain terrain(const Vec2& p) const noexcept {
        return inBounds(p) ? at(p) : Terrain::Wall;
    }

    void setTerrain(const Vec2& p, Terrain t);

    // 进入 p 的代价（直行基准）；不可站人返回 kInvalidCost(-1)
    int enterCost(const Vec2& p) const noexcept;

    // 从 a 走到相邻格 b 的代价；非法移动返回 -1
    //   非法 = 越界 / 目标是墙 / 不相邻 / 斜向穿墙角
    int moveCost(const Vec2& a, const Vec2& b) const noexcept;

    // 只读写地形段；agent 段由 MapIO 负责
    bool loadFromFile(const std::string& path, std::string* error = nullptr);
    bool saveToFile(const std::string& path) const;

private:
    // 内部访问：调用者必须先保证 inBounds
    Terrain& at(const Vec2& p)       noexcept { return cells_[idx(p)]; }
    const Terrain& at(const Vec2& p) const noexcept { return cells_[idx(p)]; }
    std::size_t    idx(const Vec2& p) const noexcept {
        return static_cast<std::size_t>(p.y) * static_cast<std::size_t>(w_)
            + static_cast<std::size_t>(p.x);
    }

    int w_ = 0;
    int h_ = 0;
    std::vector<Terrain> cells_;   // 行优先：index = y * w + x
};
