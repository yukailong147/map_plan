
#include "simulation/Agent.h"
#include <cassert>

int main()
{
    // 测试1：创建智能体
    Agent robot(1, { 0, 0 }, { 5, 5 });

    // 测试2：检查编号
    assert(robot.getId() == 1);

    // 测试3：检查起点
    Position expectedStart{ 0, 0 };
    assert(robot.getStart() == expectedStart);

    // 测试4：检查终点
    Position expectedGoal{ 5, 5 };
    assert(robot.getGoal() == expectedGoal);

    // 测试5：检查初始位置
    Position expectedCurrent{ 0, 0 };
    assert(robot.getCurrentPosition() == expectedCurrent);

    // 测试6：检查初始状态
    assert(robot.getStatus() == AgentStatus::READY);

    // 测试7：更新位置
    robot.setCurrentPosition({ 1, 0 });

    Position newPosition{ 1, 0 };
    assert(robot.getCurrentPosition() == newPosition);

    // 测试8：更新状态
    robot.setStatus(AgentStatus::MOVING);
    assert(robot.getStatus() == AgentStatus::MOVING);

    // 测试9：检查是否完成
    robot.setStatus(AgentStatus::FINISHED);
    assert(robot.isFinished());

    // 测试10：判断是否到达终点
    assert(robot.hasReachedGoal() == false);

    // 测试11：移动到终点
    robot.setCurrentPosition({ 5, 5 });
    assert(robot.hasReachedGoal());

    // 测试12：完成后不能恢复移动状态
    robot.setStatus(AgentStatus::FINISHED);
    assert(!robot.setStatus(AgentStatus::MOVING));
    assert(robot.isFinished());

    return 0;
}
