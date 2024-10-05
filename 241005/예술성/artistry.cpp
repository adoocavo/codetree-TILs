#include<iostream>
#include<queue>
#include<utility>
#include<vector>
#include<set>
using namespace std;

#define N_MAX (29+1)
//#define G_MAX (10+1)		// mapp에 주어지는 숫자의 max
//#define GROUP_MAX (841+1)	// mapp에 생기는 group 개수 max

int mapp[N_MAX][N_MAX];										// 1-10 : network 만들 정보 


int n;

int scores[4];		// 0 - 3 : 초기 예술 점수, 1/2/3회전 이후 예술 점수
//int score_init;		// 초기 예술 점수
//int score_1;		// 1회전 이후 예술 점수
//int score_2;		// 2회전 이후 예술 점수
//int score_3;		// 3회전 이후 예술 점수

// 상 -> 하 -> 좌 -> 우 탐색
int off_r[4] = { 1, -1, 0, 0 };
int off_c[4] = { 0, 0, -1, 1 };

int BFS_score();	// 예술점수 구하기
void rotate();		// mapp 회전 수행


int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n;

	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> mapp[i][j];
		}

	}

	//2. 게임 수행
	////1. 초기 예술점수 구하기
	scores[0] = BFS_score();

	////2. '회전 수행 -> 예술점수 구하기' 3번 반복
	for (int i = 1; i <= 3; ++i)
	{
		rotate();
		scores[i] = BFS_score();
	}

	//3.
	int fin_ans = 0;
	for (int i = 0; i < 4; ++i)
	{
		fin_ans += scores[i];
	}

	cout << fin_ans;

	return 0;

}




int BFS_score()
{
	bool visit[N_MAX][N_MAX] = { false, };
	queue<pair<int, int>> q_nodes;

	vector<set<pair<int, int>>> groups;			//groups.at(0) : group idx 0인 그룹의 모든 좌표 집합
	vector<int> group_mappInfo;					//group_mappInfo.at(0) : group idx 0인 그룹의 mapp[][] 값

	//1.네트워크 그룹 구하기 
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			if (visit[i][j]) continue;

			groups.push_back(set<pair<int, int>>());		// 새로운 그룹을 위한 빈 set 추가!
			group_mappInfo.push_back(mapp[i][j]);			// 새로운 그룹의 mapp[][] 숫자 값 추가!

			groups.back().insert({ i,j });
			q_nodes.push({ i,j });
			visit[i][j] = true;

			pair<int, int> cur_node;
			while (!q_nodes.empty())
			{
				////1. 
				cur_node = q_nodes.front();
				q_nodes.pop();

				////2. 
				for (int k = 0; k < 4; ++k)
				{
					int nxt_row = cur_node.first + off_r[k];
					int nxt_col = cur_node.second + off_c[k];

					//// 제한조건
					if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n)	continue;
					if (visit[nxt_row][nxt_col]) continue;
					if (mapp[cur_node.first][cur_node.second] != mapp[nxt_row][nxt_col]) continue;

					//// nxt 처리 
					groups.back().insert({ nxt_row , nxt_col });
					visit[nxt_row][nxt_col] = true;
					q_nodes.push({ nxt_row , nxt_col });
				}
			}
		}
	}


	//2. 그룹 쌍 간의 조화로움 합 구하기
	//// i : 비교대상 a의 group idx, j : 비교대상 b의 group idx
	//// a,b 맞닿은 변의 개수 구해서 -> (그룹 a에 속한 칸의 수 + 그룹 b에 속한 칸의 수) * (그룹 a를 이루고 있는 숫자 값) * (그룹 b를 이루고 있는 숫자 값)에 누적합으로 구하기
	int tmp_sum = 0;
	int ans_sum = 0;
	int point = 0;
	for (int i = 0; i < groups.size(); ++i)
	{
		for (int j = i + 1; j < groups.size(); ++j)
		{
			// a,b의 point 구하기
			point = (groups.at(i).size() + groups.at(j).size()) * group_mappInfo.at(i) * group_mappInfo.at(j);

			//tmp_sum = 0;
			int num_of_line = 0;
			for (auto it : groups.at(i))
			{
				for (int k = 0; k < 4; ++k)
				{
					//tmp_sum에 point 누적해가기 
					int tar_row = it.first + off_r[k];
					int tar_col = it.second + off_c[k];

					if (groups.at(j).find({ tar_row , tar_col }) != groups.at(j).end())
					{
						//tmp_sum += point;
						++num_of_line;
					}
				}
			}
			ans_sum += (num_of_line * point);

			////그룹 쌍 간의 조화로움 값이 0보다 큰 조합
			//if (tmp_sum != 0)	ans_sum += tmp_sum;
		}

		////그룹 쌍 간의 조화로움 값이 0보다 큰 조합
		//if (tmp_sum != 0)	ans_sum += tmp_sum;
	}


	return ans_sum;

}




void rotate()
{

	//0. tmp
	int tmp_mapp[N_MAX][N_MAX] = { 0, };
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			tmp_mapp[i][j] = mapp[i][j];
		}

	}

	int stan_row = n / 2 + 1;		//정중앙 row
	int stan_col = n / 2 + 1;		//정중앙 col
	
	// 4개 정사각형 회전할때 각 사각형의 좌상단 좌표  
	int str_row_arr[4] = { 1,1,stan_row + 1, stan_row + 1 };
	int str_col_arr[4] = { 1, stan_col + 1, 1, stan_col + 1 };


	//1. 십자 : 통째로 반시계 방향으로 90' 회전
	for (int i = 1; i <= n; ++i)
	{
		tmp_mapp[stan_row][i] = mapp[i][stan_col];
	}
	//int j_1 = 1;
	for (int i = n; i >= 1; --i)
	{
		//tmp_mapp[i][stan_col] = mapp[stan_row][j_1];
		tmp_mapp[i][stan_col] = mapp[stan_row][n - i + 1];
		//++j_1;
	}
	

	//2. 나머지 :  각각 개별적으로 시계 방향으로 90'씩 회전
	for (int i = 0; i < 4; ++i)
	{
		//// 회전 대상 사각형의 좌상단 시작 좌표
		int str_row = str_row_arr[i];
		int str_col = str_col_arr[i];
	
		//for (int j = 0; j < stan_row; ++j)
		for (int j = 0; j < stan_row-1; ++j)
		{
			//for (int k = 0; k < stan_col; ++k)
			for (int k = 0; k < stan_col-1; ++k)
			{
				//tmp_mapp[str_row + j][str_col + k] = mapp[str_col + stan_col - k - 1][str_col + j];
				//tmp_mapp[str_row + j][str_col + k] = mapp[str_col + (stan_col - 1) - k - 1][str_col + j];
				//tmp_mapp[str_row + j][str_col + k] = mapp[str_row + (stan_col - 2) - k - 1][str_col + j];
				tmp_mapp[str_row + j][str_col + k] = mapp[str_row + (stan_col - 2) - k][str_col + j];
			}
		}
	}



	/*
	int str_row, end_row;
	int str_col, end_col;
	//2. 나머지 :  각각 개별적으로 시계 방향으로 90'씩 회전
	//// (1)번 사각형
	str_row = 1; end_row = stan_row - 1;
	str_col = 1; end_col = stan_col - 1;
	for (int i = str_row; i <= end_row; ++i)
	{
		for (int j = str_col; j <= end_col; ++j)
		{
			tmp_mapp[i][j] = mapp[(end_row - i) + 1][j];
		}
	}

	//// (2)번 사각형
	str_row = 1; end_row = stan_row - 1;
	str_col = stan_col + 1; end_col = n;
	for (int i = str_row; i <= end_row; ++i)
	{
		for (int j = str_col; j <= end_col; ++j)
		{
			tmp_mapp[i][j] = mapp[(end_row - i) + 1][j];
		}
	}

	//// (3)번 사각형
	str_row = stan_row + 1; end_row = n;
	str_col = 1; end_col = stan_col - 1;
	for (int i = str_row; i <= end_row; ++i)
	{
		for (int j = str_col; j <= end_col; ++j)
		{
			tmp_mapp[i][j] = mapp[(end_row - i) + 1][j];
		}
	}


	//// (4)번 사각형
	str_row = stan_row + 1; end_row = n;
	str_col = stan_col + 1; end_col = n;
	for (int i = str_row; i <= end_row; ++i)
	{
		for (int j = str_col; j <= end_col; ++j)
		{
			tmp_mapp[i][j] = mapp[(end_row - i) + 1][j];
		}
	}
	*/


	//3. tmp -> mapp 
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			mapp[i][j] = tmp_mapp[i][j];
		}

	}
	
}