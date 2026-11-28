#pragma once

#include "common/Position.h"

//智能体运行状态
enum class AgentStatus
{
	READY,
	MOVING,
	WAITING,
	FINISHED,
	BLOCKED
};

//智能体类
class Agent
{
private:
	int id;
	Position start;
	Position goal;
	Position currentPosition;
	AgentStatus status;

public:
	//构造函数
	Agent(int id, Position start, Position goal);

	//获取智能体编号
	int getId() const;

	//获取起点
	const Position& getStart()const;

	//获取终点
	const Position& getGoal()const;

	//获取当前位置
	const Position& getCurrentPosition() const;

	//获取当前状态
	AgentStatus getStatus() const;

	//更新当前位置
	void setCurrentPosition(Position position);

	//更新状态
	bool setStatus(AgentStatus newStatus);

	//判断是否完成任务
	bool isFinished()const;

	//判断是否到达目标
	bool hasReachedGoal() const;

	// 恢复智能体初始状态
	void reset();
};