#include"common/Position.h"
#include <cassert>

int main()
{
	//测试1：创建两个位置
	Position a{ 1,2 };
	Position b{ 1,2 };
	Position c{ 3,4 };

	//测试2：相同位置判断
	assert(a == b);

	//测试3：不同位置判断
	assert(a != c);

	//测试4：默认初始化
	Position origin{};
	assert(origin.x == 0);
	assert(origin.y == 0);

	//测试5：对象复制
	Position d = a;
	assert(d == a);

	return 0;
}
