#pragma once

#include <vector>
#include "simulation/Agent.h"

//仿真引擎
class SimulationEngine
{
private:
    // 保存所有智能体
    std::vector<Agent> agents;

    // 当前仿真时间步
    int currentStep;

    // 仿真是否正在运行
    bool isRunning;

public:
    // 构造函数
    SimulationEngine();

    // 添加智能体
    bool addAgent(const Agent& agent);

    // 根据 ID 查找智能体
    Agent* getAgentById(int id);

    // 获取智能体数量
    int getAgentCount() const;

    // 获取当前时间步
    int getCurrentStep() const;

    // 判断仿真是否正在运行
    bool isSimulationRunning() const;

    // 启动仿真
    void start();

    // 暂停仿真
    void pause();

    // 推进一个时间步
    bool step();

    // 重置仿真
    void reset();
};