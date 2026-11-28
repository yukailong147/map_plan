
#include "simulation/SimulationEngine.h"

// 构造函数
SimulationEngine::SimulationEngine()
    : currentStep(0),
    isRunning(false)
{
}

// 添加智能体
bool SimulationEngine::addAgent(const Agent& agent)
{
    // 检查是否存在相同 ID
    for (const Agent& existingAgent : agents)
    {
        if (existingAgent.getId() == agent.getId())
        {
            return false;
        }
    }

    agents.push_back(agent);
    return true;
}

// 根据 ID 查找智能体
Agent* SimulationEngine::getAgentById(int id)
{
    for (Agent& agent : agents)
    {
        if (agent.getId() == id)
        {
            return &agent;
        }
    }

    return nullptr;
}

// 获取智能体数量
int SimulationEngine::getAgentCount() const
{
    return static_cast<int>(agents.size());
}

// 获取当前时间步
int SimulationEngine::getCurrentStep() const
{
    return currentStep;
}

// 判断仿真是否正在运行
bool SimulationEngine::isSimulationRunning() const
{
    return isRunning;
}

// 启动仿真
void SimulationEngine::start()
{
    isRunning = true;
}

// 暂停仿真
void SimulationEngine::pause()
{
    isRunning = false;
}

// 推进一个时间步
bool SimulationEngine::step()
{
    if (!isRunning)
    {
        return false;
    }

    ++currentStep;

    return true;
}

// 重置仿真
void SimulationEngine::reset()
{
    isRunning = false;
    currentStep = 0;

    for (Agent& agent : agents)
    {
        agent.reset();
    }
}
