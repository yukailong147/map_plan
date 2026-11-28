#pragma once

struct Position
{
	int x = 0;
	int y = 0;

	//判断两个位置是否相同
	bool operator ==(const Position& other) const//重载运算符
	{
		return x == other.x && y == other.y;
	}

	//判断两个位置是否不同
	bool operator!=(const Position& other) const
	{
		return !(*this == other);//调用“==”再取反，逻辑更清楚
	}
};