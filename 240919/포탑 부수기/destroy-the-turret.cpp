#include<iostream>
#include<queue>
#include<stack>
#include<utility>
#include<vector>
using namespace std;

#define MAX_N 10+1
#define MAX_M 10+1


int N;			//input row
int M;			//input col
int K;			// 게임 턴 횟수

//int global_str_row;			// 레이저 공격시 초기 위치 알기위해 사용
//int global_str_col;			// 레이저 공격시 초기 위치 알기위해 사용

int num_of_alive;			// 부서지지 않은 포탑 개수
bool razer_success;			// 레이저 공격 성공 여부 확인

int mapp[MAX_N][MAX_M];						// mapp : 0: 부서짐, 1~5000 : 공격력
// bool visit[MAX_N][MAX_M];					// 레이저 공격 경로 탐색시 사용(방문여부 저장)

int attack_turn[MAX_N][MAX_M];				// 포탑의 공격 턴 저장 0 : 공격 X, 1~ : 공격했던 턴
//int dist[MAX_N][MAX_M];						// dist[a][b] : 시작점 ~ (a,b) 까지의 거리 저장
int back_row[MAX_N][MAX_M];					// back_row[a][b] : (a,b)로 도착하기 전 위치의 row 저장, for BFS 경로 역추적
int back_col[MAX_N][MAX_M];					// back_col[a][b] : (a,b)로 도착하기 전 위치의 col 저장, for BFS 경로 역추적
bool is_attacked[MAX_N][MAX_M];				// 공격 당했는지 여부 저장


queue<pair<int, int>> trace;				// 공격 무관포탑 저장

// 우 / 하 / 좌 / 상 탐색
int off_row[4] = { 0, 1, 0, -1 };
int off_col[4] = { 1, 0, -1, 0 };

void pick_attacker(const int turn, int* pick_row, int* pick_col);													// 공격 수행할 포탑 선정
void pick_target(const int turn, const int picked_row, const int picked_col, int* pick_row, int* pick_col);			// 공격 대상 포탑 선정
//void DFS(const int str_row, const int str_col, const int dest_row, const int dest_col);								// 레이저 공격 수행 
bool BFS(const int str_row, const int str_col, const int dest_row, const int dest_col);
void bomb(const int str_row, const int str_col, const int dest_row, const int dest_col);
void back_BFS(const int str_row, const int str_col, const int dest_row, const int dest_col);

int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> N >> M >> K;
	int input_power;
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= M; ++j)
		{
			cin >> input_power;
			mapp[i][j] = input_power;
		}
	}

	//2. 게임 수행
	for (int i = 1; i <= K; ++i)
	{
		//// 2-1. 공격 수행할 포탑 선정
		int pick_attacker_row = -1;			// 공격 수행할 포탑 row
		int pick_attacker_col = -1;			// 공격 수행할 포탑 col
		num_of_alive = 0;
		pick_attacker(i, &pick_attacker_row, &pick_attacker_col);

		if (num_of_alive == 1) break;

		//// 2-2. 공격 대상 포탑 선정
		int pick_tar_row = -1;			// 공격 대상 포탑 row
		int pick_tar_col = -1;			// 공격 대상 포탑 col
		pick_target(i, pick_attacker_row, pick_attacker_col, &pick_tar_row, &pick_tar_col);

		//global_str_row = pick_attacker_row;
		//global_str_col = pick_attacker_col;

		is_attacked[pick_attacker_row][pick_attacker_col] = true;
		//// 2-3. 레이저 공격 시도
		//DFS(pick_attacker_row, pick_attacker_col, pick_tar_row, pick_tar_col);
		razer_success = BFS(pick_attacker_row, pick_attacker_col, pick_tar_row, pick_tar_col);

		//// 2-4. 포탑공격 수행
		if (!razer_success)
		{
			bomb(pick_attacker_row, pick_attacker_col, pick_tar_row, pick_tar_col);
		}
		//// 레이저 공격 수행
		else
		{
			back_BFS(pick_attacker_row, pick_attacker_col, pick_tar_row, pick_tar_col);
		}

		//// 2-5. 포탑 재정비
		num_of_alive = 0;
		for (int j = 1; j <= N; ++j)
		{
			for (int k = 1; k <= M; ++k)
			{
				if (!is_attacked[j][k] && mapp[j][k] > 0)
				{
					mapp[j][k] += 1;
				}
				else if (is_attacked[j][k])
				{
					is_attacked[j][k] = false;
				}

				if (mapp[j][k] > 0)
				{
					++num_of_alive;
				}

				// 각 턴의 BFS 관련 DS, var 초기화
				back_row[j][k] = 0;
				back_col[j][k] = 0;
			}
		}
		if (num_of_alive == 1) break;
	}

	//3. 남아있는 포탑 중 가장 강한 포탑의 공격력을 출력
	int ans_max = -1;
	int ans_power;
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= M; ++j)
		{
			if (mapp[i][j] > 0)
			{
				if (mapp[i][j] > ans_max)
				{
					ans_max = mapp[i][j];
					ans_power = ans_max;
				}
			}
		}
	}

	cout << ans_power;


}

// => 1-1. 부서지지 않은 포탑(mapp[][] > 0) 중 가장 약한 포탑이 공격자로 선정
// => 1-2. 공격력이 가장 낮은 포탑
// => 1-3. 가장 최근에 공격한 포탑이 가장 약한 포탑(LIFO) -> global var로 처리
// => 1-4. 각 포탑 위치의 행과 열의 합이 가장 큰 포탑이 가장 약한 포탑
// => 1-5. 각 포탑 위치의 열 값이 가장 큰 포탑이 가장 약한 포탑
void pick_attacker(const int turn, int* pick_row, int* pick_col)
{
	int tar_row, tar_col;		// 공격 수행할 포탑

	int minPower = 5000 + 1;	// 포탑 공격력 최소값

	//int min_turn_offset = 1000;
	int max_turn = 0;			// 비교대상 포탑들의 최근 공격 턴 중 max 저장

	//1. 공격할 포탑 선정 조건 고려해서 pick
	for (int j = M; j >= 1; --j)		//col
	{
		for (int k = N; k >= 1; --k)	//row
		{
			// 부서지지 않은 포탑
			if (mapp[k][j] > 0)
			{
				++num_of_alive;

				// 공격력이 가장 낮은 포탑
				if (mapp[k][j] < minPower)
				{
					tar_row = k;
					tar_col = j;

					minPower = mapp[k][j];
					//max_turn = turn;
					max_turn = attack_turn[k][j];
				}
				else if (mapp[k][j] == minPower)
				{
					// 가장 최근에 공격한 포탑(attack_turn[][]이 가장 큰걸 찾는다)
					if (max_turn < attack_turn[k][j])
						//if(attack_turn[k][j] !=0 && max_turn < attack_turn[k][j])
						//if (turn > 1 && max_turn < attack_turn[k][j])
					{
						tar_row = k;
						tar_col = j;

						minPower = mapp[k][j];
						max_turn = turn;
					}
				}
			}
		}
	}

	//2. 선정된 포탑 관련 처리
	attack_turn[tar_row][tar_col] = turn;		// 선정된 포탑의 최근 공격 턴 수정
	mapp[tar_row][tar_col] += (N + M);			// 선정된 포탑의 공격력 N+M 증가

	*pick_row = tar_row;
	*pick_col = tar_col;

	return;
}

// => 1-1. 공격력이 가장 높은 포탑이 가장 강한 포탑
// => 1-2. 공격한지 가장 오래된 포탑이 가장 강한 포탑(FIFO) -> global var로 처리
// => 1-3. 각 포탑 위치의 행과 열의 합이 가장 작은 포탑이 가장 강한 포탑
// => 1-4. 각 포탑 위치의 열 값이 가장 작은 포탑이 가장 강한 포탑
void pick_target(const int turn, const int picked_row, const int picked_col, int* pick_row, int* pick_col)
{
	int tar_row, tar_col;		// 공격 대상 포탑
	int maxPower = 0;			// 포탑 공격력 최대값
	//int max_turn_offset = 0;
	int min_turn = 1000 + 1;

	//1. 공격할 포탑 선정 조건 고려해서 pick
	for (int j = 1; j <= M; ++j)		//col
	{
		for (int k = 1; k <= N; ++k)	//row
		{
			// 공격수행 포탑 제외
			if (picked_row == k && picked_col == j) continue;

			// 부서지지 않은 포탑
			if (mapp[k][j] > 0)
			{
				//++num_of_alive;

				// 공격력이 가장 높은 포탑
				if (mapp[k][j] > maxPower)
				{
					tar_row = k;
					tar_col = j;

					maxPower = mapp[k][j];
					//min_turn = turn;
					min_turn = attack_turn[k][j];
				}

				else if (mapp[k][j] == maxPower)
				{
					// 공격한지 가장 오래된 포탑(attack_turn[][]이 가장 작은걸 찾는다)
					if (min_turn > attack_turn[k][j])
						//if(attack_turn[k][j] != 0 && min_turn > attack_turn[k][j])
						//if (turn > 1 && min_turn > attack_turn[k][j])
					{
						tar_row = k;
						tar_col = j;

						maxPower = mapp[k][j];
						min_turn = attack_turn[k][j];
					}
				}
			}
		}
	}


	//2. 선정된 포탑 관련 처리
	//mapp[tar_row][tar_col] += (N + M);			// 선정된 포탑의 공격력 N+M 증가
	attack_turn[tar_row][tar_col] = turn;

	*pick_row = tar_row;
	*pick_col = tar_col;

	return;
}


//// => 레이저 이동 제한1 : 상하좌우의 4개의 방향으로 움직일 수 있다
//// => 레이저 이동 제한2 : 부서진 포탑이 있는 위치는 지날 수 없다
//// => 레이저 이동 제한3 : 가장자리에서 막힌 방향으로 진행하고자 한다면, 반대편으로 나옵
//// => 레이저 이동 제한4 : 최단경로 존재X -> 포탑공격 시도(3 수행)
//// => 레이저 이동 제한5 : 동일한 최단경로 여러개 -> 우/하/좌/상의 우선순위대로 먼저 움직인 경로가 선택
bool BFS(const int str_row, const int str_col, const int dest_row, const int dest_col)
{
	//0. 초기 세팅
	queue<pair<int, int>> q_nodes;
	bool visit[MAX_N][MAX_M] = { false, };
	int dist[MAX_N][MAX_M] = { 0, };

	visit[str_row][str_col] = true;
	q_nodes.push({ str_row, str_col });
	dist[str_row][str_col] = 0;

	//1. BFS 수행
	while (!q_nodes.empty())
	{
		pair<int, int> cur_node = q_nodes.front();
		q_nodes.pop();

		for (int i = 0; i < 4; ++i)
		{
			int nxt_row = cur_node.first + off_row[i];
			int nxt_col = cur_node.second + off_col[i];

			//// 튕김 처리
			if (nxt_row < 1) nxt_row = N;
			else if (nxt_row > N) nxt_row = 1;
			if (nxt_col < 1) nxt_col = M;
			else if (nxt_col > M) nxt_col = 1;

			//// 부서진 포탑 처리
			if (mapp[nxt_row][nxt_col] == 0) continue;
			//// visit 처리
			if (visit[nxt_row][nxt_col]) continue;


			//// nxt 처리
			dist[nxt_row][nxt_col] = dist[cur_node.first][cur_node.second] + 1;
			back_row[nxt_row][nxt_col] = cur_node.first;
			back_col[nxt_row][nxt_col] = cur_node.second;

			visit[nxt_row][nxt_col] = true;
			q_nodes.push({ nxt_row , nxt_col });
		}

		// BFS 종료시키기
		if (dist[dest_row][dest_col] != 0) break;
	}

	if (dist[dest_row][dest_col] == 0) return false;
	return true;
}


//// => 공격 대상 포탑 :  공격자의 공격력 만큼의 피해
//// => 레이저 경로 내의 포탑 : 공격자의 공격력을 2로 나눈 몫 만큼의 피해
void back_BFS(const int str_row, const int str_col, const int dest_row, const int dest_col)
{
	int surplus_damage = mapp[str_row][str_col] / 2;

	queue <pair<int, int>> q_nodes;
	q_nodes.push({ dest_row , dest_col });
	mapp[dest_row][dest_col] -= mapp[str_row][str_col];
	is_attacked[dest_row][dest_col] = true;

	pair<int, int> cur_node;
	while (!q_nodes.empty())
	{
		cur_node = q_nodes.front();
		q_nodes.pop();

		int nxt_row = back_row[cur_node.first][cur_node.second];
		int nxt_col = back_col[cur_node.first][cur_node.second];

		if (nxt_row == 0 || nxt_col == 0) break;
		if (nxt_row == str_row && nxt_col == str_col) break;

		q_nodes.push({ nxt_row , nxt_col });
		mapp[nxt_row][nxt_col] -= surplus_damage;
		is_attacked[nxt_row][nxt_col] = true;
	}
	return;
}

//// => 공격 대상 포탑 :  공격자의 공격력 만큼의 피해
//// => 공격 대상의 주위 8방향 포탑 : 공격자의 공격력을 2로 나눈 몫 만큼의 피해
//// => 공격자는 해당 공격에 영향을 받지 않음
//// => 가장자리에 포탄이 떨어졌다면, 위에서의 레이저 이동처럼 포탄의 추가 피해가 반대편 격자에 미치게 됨
void bomb(const int str_row, const int str_col, const int dest_row, const int dest_col)
{
	int off_8_row[8] = { 0, 1, 0, -1, -1, 1, 1, -1 };
	int off_8_col[8] = { 1, 0, -1, 0, 1, 1, -1, -1 };
	int surplus_damage = mapp[str_row][str_col] / 2;

	//1. 공격 대상 포탑 :  공격자의 공격력 만큼의 피해
	mapp[dest_row][dest_col] -= mapp[str_row][str_col];
	is_attacked[dest_row][dest_col] = true;

	//2. 
	int surplus_row;
	int surplus_col;
	for (int i = 0; i < 8; ++i)
	{
		surplus_row = dest_row + off_8_row[i];
		surplus_col = dest_col + off_8_col[i];

		//// 튕김 처리
		if (surplus_row < 1) surplus_row = N;
		else if (surplus_row > N) surplus_row = 1;
		if (surplus_col < 1) surplus_col = M;
		else if (surplus_col > M) surplus_col = 1;

		//// 부서진 포탑 처리
		// if (mapp[surplus_row][surplus_col] == 0) continue;
		if (mapp[surplus_row][surplus_col] <= 0) continue;

		//// 공격 대상의 주위 8방향 포탑 처리
		mapp[surplus_row][surplus_col] -= surplus_damage;
		is_attacked[surplus_row][surplus_col] = true;
	}

	return;
}