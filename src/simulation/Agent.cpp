#include "simulation/Agent.h"

//构造函数
Agent::Agent(int id, Position start, Position goal)
    :id(id),
    start(start),
    goal(goal),
    currentPosition(start),
    status(AgentStatus::READY){}

// 获取编号
int Agent::getId() const
{
    return id;
}

// 获取起点
const Position& Agent::getStart() const
{
    return start;
}

// 获取终点
const Position& Agent::getGoal() const
{
    return goal;
}

// 获取当前位置
const Position& Agent::getCurrentPosition() const
{
    return currentPosition;
}

// 获取状态
AgentStatus Agent::getStatus() const
{
    return status;
}

// 更新当前位置
void Agent::setCurrentPosition(Position position)
{
    currentPosition = position;
}

// 更新状态
bool Agent::setStatus(AgentStatus newStatus)
{
    if (status == AgentStatus::FINISHED &&
        newStatus != AgentStatus::FINISHED)
    {
        return false;
    }

    status = newStatus;
    return true;
}

// 判断是否完成任务
bool Agent::isFinished() const
{
    return status == AgentStatus::FINISHED;
}

//判断是否到达目标
bool Agent::hasReachedGoal() const
{
    return currentPosition == goal;
}

void Agent::reset()
{
    currentPosition = start;
    status = AgentStatus::READY;
}