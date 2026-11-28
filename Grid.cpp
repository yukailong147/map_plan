#include "Grid.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <utility>

namespace {

    char charOf(Terrain t) noexcept {
        switch (t) {
        case Terrain::Wall:  return '#';
        case Terrain::Grass: return ',';
        case Terrain::Swamp: return '~';
        case Terrain::Free:  return '.';
        }
        return '.';
    }

    bool terrainOf(char c, Terrain& out) noexcept {
        switch (c) {
        case '.': out = Terrain::Free;  return true;
        case '#': out = Terrain::Wall;  return true;
        case ',': out = Terrain::Grass; return true;
        case '~': out = Terrain::Swamp; return true;
        default:  return false;
        }
    }

    void fail(std::string* error, std::string msg) {
        if (error != nullptr) *error = std::move(msg);
    }

    // 空行或以 ';' 开头的注释行
    bool blankOrComment(const std::string& s) {
        for (char c : s) {
            if (c == ' ' || c == '\t') continue;
            return c == ';';
        }
        return true;
    }

}  // namespace

Grid::Grid(int width, int height, Terrain fill)
    : w_(width > 0 ? width : 0),
    h_(height > 0 ? height : 0),
    cells_(static_cast<std::size_t>(w_)* static_cast<std::size_t>(h_), fill) {
}

void Grid::setTerrain(const Vec2& p, Terrain t) {
    assert(inBounds(p) && "Grid::setTerrain 越界");
    if (!inBounds(p)) return;   // Release 下也别写坏内存
    at(p) = t;
}

int Grid::enterCost(const Vec2& p) const noexcept {
    if (!passable(p)) return kInvalidCost;
    return kStepStraight * static_cast<int>(at(p));
}

int Grid::moveCost(const Vec2& a, const Vec2& b) const noexcept {
    if (!passable(a) || !passable(b)) return kInvalidCost;

    const int dx = b.x - a.x;
    const int dy = b.y - a.y;

    if (dx == 0 && dy == 0) return kInvalidCost;                      // 原地不算移动
    if (dx < -1 || dx > 1 || dy < -1 || dy > 1) return kInvalidCost;  // 不相邻

    const bool diagonal = (dx != 0 && dy != 0);
    if (diagonal) {
        // 防穿墙角：两条直角边都必须能走
        if (!passable(Vec2{ a.x + dx, a.y })) return kInvalidCost;
        if (!passable(Vec2{ a.x, a.y + dy })) return kInvalidCost;
    }

    const int base = diagonal ? kStepDiagonal : kStepStraight;
    return base * static_cast<int>(at(b));   // × 目标格的地形倍率
}

bool Grid::loadFromFile(const std::string& path, std::string* error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        fail(error, "打不开地图文件：" + path);
        return false;
    }

    // 先把所有行读进来（顺便去掉 Windows 的 '\r'）
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(std::move(line));
    }

    // 跳过空行 / 注释行，定位到「宽 高」
    std::size_t i = 0;
    while (i < lines.size() && blankOrComment(lines[i])) ++i;
    if (i == lines.size()) {
        fail(error, "地图文件是空的：" + path);
        return false;
    }

    int w = 0;
    int h = 0;
    {
        std::istringstream header(lines[i]);
        if (!(header >> w >> h)) {
            fail(error, "第 " + std::to_string(i + 1) + " 行应为「宽 高」，实际是：" + lines[i]);
            return false;
        }
        ++i;
    }

    if (w <= 0 || h <= 0 || w > 512 || h > 512) {
        fail(error, "地图尺寸非法：" + std::to_string(w) + " x " + std::to_string(h)
            + "（要求 1..512）");
        return false;
    }

    const std::size_t rows = static_cast<std::size_t>(h);
    if (i + rows > lines.size()) {
        fail(error, "地图行数不够：声明 " + std::to_string(h) + " 行，实际只剩 "
            + std::to_string(lines.size() - i) + " 行");
        return false;
    }

    // 先解析到临时对象，全部成功才替换 —— 解析失败不会破坏原来的地图
    Grid loaded(w, h);
    for (int y = 0; y < h; ++y) {
        const std::string& row = lines[i + static_cast<std::size_t>(y)];
        if (row.size() != static_cast<std::size_t>(w)) {
            fail(error, "第 " + std::to_string(i + static_cast<std::size_t>(y) + 1)
                + " 行宽度是 " + std::to_string(row.size())
                + "，应为 " + std::to_string(w));
            return false;
        }
        for (int x = 0; x < w; ++x) {
            const char c = row[static_cast<std::size_t>(x)];
            Terrain t = Terrain::Free;
            if (!terrainOf(c, t)) {
                fail(error, "第 " + std::to_string(i + static_cast<std::size_t>(y) + 1)
                    + " 行第 " + std::to_string(x + 1) + " 列是非法字符 '"
                    + std::string(1, c) + "'（只允许 . # , ~）");
                return false;
            }
            loaded.setTerrain(Vec2{ x, y }, t);
        }
    }

    *this = std::move(loaded);
    if (error != nullptr) error->clear();
    return true;
}

bool Grid::saveToFile(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    out << w_ << ' ' << h_ << '\n';
    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) out << charOf(at(Vec2{ x, y }));
        out << '\n';
    }
    return static_cast<bool>(out);
}
