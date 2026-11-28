#pragma once
// ============================================================================
//  Grid.h —— 网格地图
//
//  这个类对外只回答两个问题（B 和 C 只会用到这两个）：
//      1) passable(p)     这一格能站人吗？
//      2) moveCost(a, b)  从 a 走到相邻格 b 要多少代价？（-1 = 走不了）
//
//  别的都是 A 自己用的（地形编辑、文件读写）。
//  ★ 接口越小，队友越不容易用错。
// ============================================================================

#include <cstddef>
#include <string>
#include <vector>

#include "Types.h"

// ---------------------------------------------------------------------------
// 地形
//   枚举值 = 进入这一格的【代价倍率】。
//   Wall = -1 是个不会被误当成代价的哨兵值（代价不可能是 -1）。
// ---------------------------------------------------------------------------
enum class Terrain : int {
    Wall = -1,   // 墙，不可通行
    Free = 1,    // 空地，倍率 1
    Grass = 2,    // 草地，倍率 2
    Swamp = 5,    // 沼泽，倍率 5
};

class Grid {
public:
    Grid() = default;

    // 构造一张 w×h 的地图，默认全是空地。
    // w 或 h <= 0 时构造出一张空地图（empty() 为 true）。
    Grid(int width, int height, Terrain fill = Terrain::Free);

    int  width()  const noexcept { return w_; }
    int  height() const noexcept { return h_; }
    bool empty()  const noexcept { return cells_.empty(); }

    // 坐标是否在地图内
    bool inBounds(const Vec2& p) const noexcept {
        return p.x >= 0 && p.x < w_ && p.y >= 0 && p.y < h_;
    }

    // 这一格能不能站人。
    // ★ 内部已经包了 inBounds，所以 A* 在四个角上展开邻居时不用自己判边界。
    bool passable(const Vec2& p) const noexcept {
        return inBounds(p) && at(p) != Terrain::Wall;
    }

    // 越界一律返回 Wall，调用方不用先判边界。
    Terrain terrain(const Vec2& p) const noexcept {
        return inBounds(p) ? at(p) : Terrain::Wall;
    }

    // 改地形（编辑器用）。越界调用在 Debug 下会断言失败。
    void setTerrain(const Vec2& p, Terrain t);

    // 进入 p 的代价（按直行 10 计算）；不可站人返回 kInvalidCost(-1)。
    //   空地 10 / 草地 20 / 沼泽 50
    int enterCost(const Vec2& p) const noexcept;

    // 从 a 走到相邻格 b 的代价；非法移动返回 kInvalidCost(-1)。
    //   非法 = 越界 / 目标格是墙 / 不相邻 / 斜向穿墙角
    // ★ 三条规则都收在这个函数里，B 写 A* 时只需要判 if (cost < 0)。
    int moveCost(const Vec2& a, const Vec2& b) const noexcept;

    // 从文件读地形。成功返回 true 并把 error 清空；
    // 失败返回 false 并写入错误信息（第几行、期望什么、实际什么）。
    //
    // 文件格式：
    //     第 1 行（可跳过空行和 ';' 开头的注释行）：  宽 高
    //     之后恰好「高」行，每行恰好「宽」个字符：
    //         '.' 空地    '#' 墙    ',' 草地    '~' 沼泽
    //     之后的内容（例如 '--' 之后的 agent 段）本函数不读，由 MapIO 负责。
    bool loadFromFile(const std::string& path, std::string* error = nullptr);

    // 只写地形段（不会写 agent 段）。
    bool saveToFile(const std::string& path) const;

private:
    // 内部访问：调用者必须先保证 inBounds
    Terrain& at(const Vec2& p)       noexcept { return cells_[idx(p)]; }
    const Terrain& at(const Vec2& p) const noexcept { return cells_[idx(p)]; }

    // 行优先一维存储：index = y * w + x
    std::size_t idx(const Vec2& p) const noexcept {
        return static_cast<std::size_t>(p.y) * static_cast<std::size_t>(w_)
            + static_cast<std::size_t>(p.x);
    }

    int w_ = 0;
    int h_ = 0;
    std::vector<Terrain> cells_;
};
