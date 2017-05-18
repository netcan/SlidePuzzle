/*************************************************************************
    > File Name: eight.cpp
      > Author: Netcan
      > Blog: http://www.netcan666.com
      > Mail: 1469709759@qq.com
      > Created Time: 2016-04-07 四 18:24:18 CST
 ************************************************************************/

#include <iostream> // C++的IO输入输出
#include <cstdio> // C的输入输出
#include <vector> // 存储open表的数据结构
#include <map> // 存储closed表的数据结构
#include <cstring> // 含memcpy的接口
#include <algorithm> // 排序
using namespace std;

typedef int board[4][4]; // 二维数组，下标从1开始，用于存放八数码的状态
int movx[] = {-1, 0, 1, 0}; // 空格下右上左四个方向移动
int movy[] = {0, 1, 0, -1};
bool success(const int &s, const int &e); // 判断两个状态是否相等

struct Status {
    int s; // 利用一维数据存储当前状态
    int g, h; // f = g + h
    int cid, pid; // 当前状态id，其父节点id
    Status(const int s, int g, int h, int cid, int pid): s(s), g(g), h(h), cid(cid), pid(pid) {}
    bool operator<(const Status &b) const { // 根据状态排序
        return s < b.s;
    }
    bool operator==(const Status &b) const { // 重载==，find查找元素需要
        return success(s, b.s);
    }
};

// 将opening表分成两部分来维护，提高效率
multimap<int, Status> opening_key; // F-value
map<Status, int> opening_value; // value-F

map<int, bool> closed; // 记录使用过的状态
vector<Status> path; // 保存路径，即closed表的详尽结构

void ser2board(int s, board t);

void input(board s) { // 输入状态
    for(int i=1; i<=3; ++i)
        for(int j=1; j<=3; ++j)
            scanf("%1d", &s[i][j]);
}
void output(const int &s) { // 输出状态
	board tmp;
	ser2board(s, tmp);
    for(int i=1; i<=3; ++i) {
        for(int j=1; j<=3; ++j)
            printf("%2d", tmp[i][j]);
        puts("");
    }
}

int H(int cur, int e) { // 评估函数，当H==0的时候说明状态相等，H需要收敛
	int d = 0;
	for(int i=0; i<9; ++i) {
		if(cur % 10 != e % 10) ++d;
		cur/=10;
		e/=10;
	}
	return d;
}

int F(const Status &s) { // F = G + H
    return s.g + s.h;
}

bool success(const int &cur, const int &e) { // 判断是否到达终点
    return H(cur, e) == 0;
}

int serialization(const board &cur) { // 状态序列化成一维数字，方便存储。
    int sr = 0;
    for(int i=1; i<=3; ++i)
        for(int j=1; j<=3; ++j)
            sr = sr * 10 + cur[i][j];
    return sr; // 序列化结果
}

void ser2board(int s, board t) {
	// printf("%d\n", s);
	for(int i=3; i>=1; --i)
		for(int j=3; j>=1; --j) {
			t[i][j] = s%10;
			// printf("%d\n", t[i][j]);
			s /= 10;
		}
}

bool checkvalid(const int &s, const int &e) { // 判断是否有解
    int ss = 0, ee = 0;
	board tmps, tmpe;
	ser2board(s, tmps);
	ser2board(e, tmpe);
    for(int i=0; i<9; ++i)
        for(int j=0; j<i; ++j) {
            if(tmps[j/3+1][j%3+1] != 0 && tmps[j/3+1][j%3+1] < tmps[i/3+1][i%3+1]) ++ss;
            if(tmpe[j/3+1][j%3+1] != 0 && tmpe[j/3+1][j%3+1] < tmpe[i/3+1][i%3+1]) ++ee;
        }
	printf("ss:%d ee:%d\n", ss, ee);
    return (ss&1) == (ee&1); // 同奇同偶有解
}


int run(const int &s,const int &e) { // Astar
	if(!checkvalid(s, e)) return -1; // 判断是否有解，无解返回-1
	// 清理Open/Close/Path表
	opening_key.clear();
	opening_value.clear();
	closed.clear();
	path.clear();
	int id = 0; // 每使用一个id，就++，随着状态转移，id只会越来越大
	Status st(s, 0, H(s, e), id, id++);
	opening_key.insert(make_pair<int, Status>(H(s, e), st)); // 初始状态放入closed表中
	opening_value.insert(make_pair<Status, int>(st, H(s, e))); // 初始状态放入closed表中

	while(opening_key.size()) { // 直到表空，如果无解的话是不可能表空的= =
		Status p = opening_key.begin()->second; // 取出最小F值的状态

		opening_key.erase(opening_key.begin()); // 删除最小F值的状态
		opening_value.erase(opening_value.lower_bound(p));

		path.push_back(p); // 放到path数据结构中，用来保存路径
		closed[p.s] = true; // 将状态s放入closed表中
		if(success(p.s, e)) return p.g; // 找到目标状态，返回最少步骤
		int cx = -1, cy = -1; // 寻找空格的位置（即0）
		board tmp;
		ser2board(p.s, tmp);
		for(int i=1; i<=3; ++i) {
			if(cx != -1) break;
			for(int j=1; j<=3; ++j)
				if(tmp[i][j] == 0) { // 找到空格
					cx = i;
					cy = j;
					break;
				}
		}
		for(int k=0; k<4; ++k) { // 生成4个移动到相邻格子的状态（相当于空格上下左右移动）
			int nx = cx + movx[k];
			int ny = cy + movy[k];
			if(nx >= 1 && nx <= 3 && ny >= 1 && ny <= 3) { // 移动判断是否越界
				swap(tmp[cx][cy], tmp[nx][ny]); // 开始移动
				int tmps = serialization(tmp);
				Status n(tmps, p.g+1, H(tmps, e), id++, p.cid); // 生成的状态
				swap(tmp[cx][cy], tmp[nx][ny]); // 恢复原位
				int f = F(n); // f=g+h
				// 查找Open表中是否有这个状态，没有就添加，有的话更新最小值
				if(!closed.count(tmps)) { // 先查找是否在closed表中
					// vector<Status>::iterator it = find(opening.begin(), opening.end(), n); // 遍历查找状态
					map<Status, int>::iterator itv = opening_value.lower_bound(n);
					map<int, Status>::iterator itk;
					if(itv != opening_value.end() && itv->first == n) { // 找到
						if(F(itv->first) > f) { // 有最优解
							for(itk = opening_key.lower_bound(F(itv->first)); itk != opening_key.upper_bound(F(itv->first)); ++itk)
								if(itk->second == n) break;
							opening_value.erase(itv); // 删除
							opening_key.erase(itk);
							opening_key.insert(make_pair<int, Status>(f, n)); // 初始状态放入closed表中
							opening_value.insert(make_pair<Status, int>(n, f)); // 初始状态放入closed表中
						}
					}
					else {
						opening_key.insert(make_pair<int, Status>(f, n)); // 初始状态放入closed表中
						opening_value.insert(make_pair<Status, int>(n, f)); // 初始状态放入closed表中
					}
				}
			}
		}
	}
	return -1; // 无解= =
}

void outpath(int pid, int ss, int step) { // 后序递归寻找路径
	if(step < 0) return; // 无解
	else if(step == 0) { // 初始状态
		printf("Step(%d) ==> \n", step);
		output(path[ss].s); // 输出状态
		return;
	}
	for(int i=ss; i>=0; --i) // 往前查找父状态
		if(path[i].cid == pid)  // 找到父状态
			outpath(path[i].pid, i, step - 1); // 后序递归
	printf("Step(%d) ==> \n", step);
	output(path[ss].s); // 输出状态
}

int main() {
	freopen("eight.in", "r", stdin); // 从文件中读取数据
    board start, end;
    cout << "输入起始状态(9个数，0表示空格）：" << endl;
    input(start);
    cout << "输入目标状态(9个数，0表示空格）：" << endl;
    input(end);

	int step = run(serialization(start), serialization(end)); // Astar
	// cout << step << endl;
	if(step != -1)
		outpath((path.end()-1)->pid, path.size() - 1, step); // 输出路径
	else
		cout << "无解！" << endl;

    return 0;
}
