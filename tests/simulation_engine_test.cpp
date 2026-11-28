
#include "simulation/SimulationEngine.h"
#include <cassert>

int main()
{
    // 测试1：创建仿真引擎
    SimulationEngine engine;

    assert(engine.getAgentCount() == 0);
    assert(engine.getCurrentStep() == 0);
    assert(!engine.isSimulationRunning());

    // 测试2：创建三个智能体
    Agent robot1(1, { 0, 0 }, { 5, 5 });
    Agent robot2(2, { 1, 0 }, { 6, 5 });
    Agent robot3(3, { 2, 0 }, { 7, 5 });

    // 测试3：添加智能体
    assert(engine.addAgent(robot1));
    assert(engine.addAgent(robot2));
    assert(engine.addAgent(robot3));

    assert(engine.getAgentCount() == 3);

    // 测试4：检查重复 ID
    Agent duplicate(1, { 3, 0 }, { 8, 5 });
    assert(!engine.addAgent(duplicate));

    // 测试5：查找智能体
    Agent* found = engine.getAgentById(2);

    assert(found != nullptr);
    assert(found->getId() == 2);

    // 测试6：查找不存在的智能体
    assert(engine.getAgentById(100) == nullptr);

    // 测试7：未启动时不能推进时间
    assert(!engine.step());
    assert(engine.getCurrentStep() == 0);

    // 测试8：启动仿真
    engine.start();

    assert(engine.isSimulationRunning());

    // 测试9：推进时间步
    assert(engine.step());
    assert(engine.step());
    assert(engine.step());

    assert(engine.getCurrentStep() == 3);

    // 测试10：暂停仿真
    engine.pause();

    assert(!engine.isSimulationRunning());
    assert(!engine.step());
    assert(engine.getCurrentStep() == 3);

    // 测试11：模拟机器人移动和完成
    Agent* robot = engine.getAgentById(1);

    assert(robot != nullptr);

    robot->setCurrentPosition({ 5, 5 });
    assert(robot->hasReachedGoal());

    assert(robot->setStatus(AgentStatus::FINISHED));
    assert(robot->isFinished());

    // 测试12：重置仿真
    engine.reset();

    assert(engine.getCurrentStep() == 0);
    assert(!engine.isSimulationRunning());

    robot = engine.getAgentById(1);

    Position expectedStart{ 0, 0 };

    assert(robot != nullptr);
    assert(robot->getCurrentPosition() == expectedStart);
    assert(robot->getStatus() == AgentStatus::READY);

    // 测试13：重置后仍然保留三个智能体
    assert(engine.getAgentCount() == 3);

    return 0;
}
