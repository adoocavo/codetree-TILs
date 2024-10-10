#include<iostream>
#include<set>
#include<utility>
#include<map>
#include<queue>
using namespace std;

#define N_MAX (10+1)		//row
#define M_MAX (10+1)		//col

int mapp[N_MAX][M_MAX];		// 0~5000

//포탑 관련 ds, var
map<pair<int, int>, int> top_force;		// 특정 위치 포탑의 공격력
map<pair<int, int>, bool> is_alive;		// 특정 위치 포탑의 생존 여부
map<pair<int, int>, int> last_attack;	// 특정 위치 포탑의 마지막 공격 턴
map<pair<int, int>, int> is_related;	// 특정 위치 포탑의 현재 턴에서 공격과 연관성 (턴 단위로 초기화)
set<pair<int, int>> is_related_set;		

int N, M, K;

//공격 대상 선정 
pair<int, int> select_attacker();
//공격 받는 대상 선정 
pair<int, int> select_attacked(const pair<int, int> attacker_top);
//공격자가 공격 수행
bool BFS_attack(const pair<int, int> attacker_top, const pair<int, int> attacked_top);
// BFS 최단경로 역추적 -> 경로 내 포탑들 공격력 수정
void BFS_back(const pair<int, int> attacker_top, const pair<int, int> attacked_top, const int damage);
// 포탄공격
void bomb(const pair<int, int> attacker_top, const pair<int, int> attacked_top);


// 우 -> 하 -> 좌 -> 상 이동
int off_row[4] = { 0, 1, 0, -1 };
int off_col[4] = { 1, 0, -1, 0 };

// 직전 좌표 저장 (for 역추적 : attacked_top -> attacker_top)
//// => 특정 격자에 이전 격자 위치정보가 저장됨과 동시에, visit 처리되므로 -> 중간에 수정되거나 경로가 꼬이는 걱정 ㄴㄴ
pair<int, int> back[N_MAX][M_MAX];

//
int alive_cnt;				//남아있는 포탄 개수 저장 -> 1개만 남으면 개임 종료!

int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> N >> M >> K;

	int in_force;
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= M; ++j)
		{
			cin >> in_force;

			mapp[i][j] = in_force;

			top_force[{i, j}] = in_force;
			if (in_force > 0)	is_alive[{i, j}] = true;
			last_attack[{i, j}] = 0;
			is_related[{i, j}] = false;
		}
	}


	//2. 게임 수행
	for (int i = 1; i <= K; ++i)
	{
		////0. 게임 전 초기화
		is_related_set.clear();
		alive_cnt = 0;

		/*
		for (auto it_clear : is_related)
		{
			is_related[{it_clear.first.first, it_clear.first.second}] = false;
		}
		*/

		////1. 공격자 선정
		pair<int, int> attacker_top = select_attacker();
		is_related_set.insert({ attacker_top.first, attacker_top.second });
		//is_related[{attacker_top.first, attacker_top.second}] = true;
		////// 최근 공격 턴 업데이트
		last_attack[{attacker_top.first, attacker_top.second}] = i;

		////2-1. 공격대상 선정
		pair<int, int> attacked_top = select_attacked(attacker_top);
		is_related_set.insert({ attacked_top.first, attacked_top.second });
		//is_related[{attacked_top.first, attacked_top.second}] = true;

		////2-2. 공격자 -> 공격대상 공격 수행
		if (BFS_attack(attacker_top, attacked_top) == true)		//레이저 공격
		{
			;
		}
		else
		{
			// 포탄공격
			bomb(attacker_top, attacked_top);
		}

		////3. 포탑 부서짐 처리
		////// => top_force[] < 1 -> mapp = 0, is_alive[] = false 로 수정
		for (auto it_top : top_force)
		{
			if (top_force[{it_top.first.first, it_top.first.second}] < 1)
			{
				mapp[it_top.first.first][it_top.first.second] = 0;
				is_alive[{it_top.first.first, it_top.first.second}] = false;
			}
		}

		////4-1. 포탑 정비	
		////// => is_alive[] == true && is_related[] == false -> 공격력이 1씩 증가
		//alive_cnt = 0;
		for (auto it_top : is_alive)
		{
			if (is_alive[{it_top.first.first, it_top.first.second}])
			{
				++alive_cnt;
				if (is_related_set.find({ it_top.first.first, it_top.first.second }) == is_related_set.end())		//공격과 무관
				{
					top_force[{it_top.first.first, it_top.first.second}] += 1;
				}
				/*
				if (!is_related[{it_top.first.first, it_top.first.second}])
				{
					top_force[{it_top.first.first, it_top.first.second}] += 1;
				}
				*/
			}

		}

		/*
		////4-2. 초기화 
		for (auto it_clear : is_related)
		{
			is_related[{it_clear.first.first, it_clear.first.second}] = false;
		}
		*/

		////4-3.
		if (alive_cnt == 1)	break;
	}

	//3. 전체 과정이 종료된 후 남아있는 포탑 중 가장 강한 포탑의 공격력을 출력
	int max_force = -1;
	for (auto it_top : is_alive)
	{
		if (is_alive[{it_top.first.first, it_top.first.second}])
		{
			if (max_force < top_force[{it_top.first.first, it_top.first.second}])
			{
				max_force = top_force[{it_top.first.first, it_top.first.second}];
			}
		}
	}

	cout << max_force;

	return 0;
}


pair<int, int> select_attacker(void)
{
	int min_force = 5000 + 10;
	pair<int, int> tar_top;			// 선정되는 공격자 포탑 저장 -> return 
	int latest = -1;				// 가장 최근 공격턴 저장(공격턴이 클수록 최근이다)
	int max_sum_of_rc = 0;			// 각 포탑 위치의 행과 열의 합  최대 
	int max_c = 0;					// 각 포탑 위치의 열 값 최대

	//동작1 : 부서지지 않은 포탑 중 가장 약한 포탑 선택
	for (auto it : top_force)
	{

		if (!is_alive[{it.first.first, it.first.second}])	continue;
		if ((top_force[{it.first.first, it.first.second}] < min_force) ||
			(((top_force[{it.first.first, it.first.second}] == min_force) && (last_attack[{it.first.first, it.first.second}] > latest))) ||
			(((top_force[{it.first.first, it.first.second}] == min_force) && (last_attack[{it.first.first, it.first.second}] == latest)) && (it.first.first + it.first.second) > max_sum_of_rc) ||
			(((top_force[{it.first.first, it.first.second}] == min_force) && (last_attack[{it.first.first, it.first.second}] == latest)) && ((it.first.first + it.first.second) == max_sum_of_rc) && max_c < it.first.second)
			)
		{
			max_c = it.first.second;
			max_sum_of_rc = (it.first.first + it.first.second);
			latest = last_attack[{it.first.first, it.first.second}];
			min_force = top_force[{it.first.first, it.first.second}];
			tar_top = { it.first.first, it.first.second };

		}
	}

	// 동작2 : 공격자로 선정된 포탑의 공격력이, N+M만큼 증가
	top_force[{tar_top.first, tar_top.second}] += (N + M);

	return tar_top;
}

pair<int, int> select_attacked(const pair<int, int> attacker_top)
{
	int max_force = -1;
	pair<int, int> tar_top;			// 선정되는 공격대상 포탑 저장 -> return 
	int latest = 1000 + 1;			// 가장 최근 공격턴 저장(공격턴이 작을수록 오래됨)
	int min_sum_of_rc = 0;			// 각 포탑 위치의 행과 열의 합 최소 
	int min_c = 0;					// 각 포탑 위치의 열 값 최소

	for (auto it : top_force)
	{
		//if (is_alive[{it.first, it.second}])
		// 제한0 : 자신을 제외
		if (it.first.first == attacker_top.first && it.first.second == attacker_top.second) continue;
		if ((top_force[{it.first.first, it.first.second}] > max_force) ||
			(((top_force[{it.first.first, it.first.second}] == max_force) && (last_attack[{it.first.first, it.first.second}] < latest))) ||
			(((top_force[{it.first.first, it.first.second}] == max_force) && (last_attack[{it.first.first, it.first.second}] == latest)) && (it.first.first + it.first.second) < min_sum_of_rc) ||
			(((top_force[{it.first.first, it.first.second}] == max_force) && (last_attack[{it.first.first, it.first.second}] == latest)) && ((it.first.first + it.first.second) == min_sum_of_rc) && min_c > it.first.second)
			)
		{
			min_c = it.first.second;
			min_sum_of_rc = (it.first.first + it.first.second);
			latest = last_attack[{it.first.first, it.first.second}];
			max_force = top_force[{it.first.first, it.first.second}];
			tar_top = { it.first.first, it.first.second };

		}
	}

	return tar_top;
}

bool BFS_attack(const pair<int, int> attacker_top, const pair<int, int> attacked_top)
{
	//0-1. back 초기화
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= M; ++j)
		{
			back[i][j] = { 0,0 };
		}
	}

	//0-2. visit, queue 설정

	//bool visit[N_MAX][M_MAX];
	bool visit[N_MAX][M_MAX] = { false, };
	queue<pair<int, int>>q_nodes;

	visit[attacker_top.first][attacker_top.second] = true;
	q_nodes.push({ attacker_top.first , attacker_top.second });

	//1. BFS
	pair<int, int> cur_node;
	while (!q_nodes.empty())
	{
		cur_node = q_nodes.front();
		q_nodes.pop();

		//정답 처리
		if (cur_node.first == attacked_top.first &&
			cur_node.second == attacked_top.second)
		{
			//// 공격 대상 : 공격자의 공격력 만큼, 공격력 하락
			int attack_power1 = top_force[{attacker_top.first, attacker_top.second}];

			top_force[{attacked_top.first, attacked_top.second}] -= attack_power1;

			//// 공격 대상을 제외한 레이저 경로에 있는 포탑 :  '공격자의 공격력 / 2' 만큼 공격력 하락
			int attack_power2 = attack_power1 / 2;
			BFS_back(attacker_top, attacked_top, attack_power2);

			return true;
		}


		//nxt 위치 탐색
		for (int i = 0; i < 4; ++i)
		{
			//// 가장자리에서 막힌 방향으로 진행하고자 한다면, 반대편으로 나오는 제한 우선 반영하여 nxt 구하기
			int nxt_row = (((cur_node.first + off_row[i]) - 1 + N) % N) + 1;
			int nxt_col = (((cur_node.second + off_col[i]) - 1 + M) % M) + 1;

			//// 반대편으로 나오는 제한이 고려된 nxt 좌표로 나머지 제한 조건 확인하기
			////제한 0 : 이미 방문한 위치는 방문 불가 
			if (visit[nxt_row][nxt_col]) continue;
			////제한 1 : 부서진 포탑이 있는 위치는 지날 수 없습니다.
			if (is_alive[{nxt_row, nxt_col}] == false) continue;

			//// nxt 처리(visit, queue) + 역추적 위한 좌표 처리
			visit[nxt_row][nxt_col] = true;
			q_nodes.push({ nxt_row, nxt_col });

			back[nxt_row][nxt_col] = { cur_node.first , cur_node.second };
		}

	}

	return false;
}

void BFS_back(const pair<int, int> attacker_top, const pair<int, int> attacked_top, const int damage)
{
	//0.
	queue<pair<int, int>> q_nodes;

	q_nodes.push({ back[attacked_top.first][attacked_top.second].first, back[attacked_top.first][attacked_top.second].second });

	//1. 
	pair<int, int> tar_node;
	while (!q_nodes.empty())
	{

		//// pop -> target 구하기
		tar_node = q_nodes.front();
		q_nodes.pop();

		//// 정답처리
		if (tar_node.first == attacker_top.first && tar_node.second == attacker_top.second)
		{
			return;
		}

		//// target 처리 : damage
		top_force[{tar_node.first, tar_node.second}] -= damage;
		//// is_related 처리
		//is_related[{tar_node.first, tar_node.second}] = true;
		is_related_set.insert({ tar_node.first, tar_node.second });

		//// nxt 구해서 push
		q_nodes.push({ back[tar_node.first][tar_node.second].first, back[tar_node.first][tar_node.second].second });
	}
}

void bomb(const pair<int, int> attacker_top, const pair<int, int> attacked_top)
{
	// 
	int dr[8] = { -1,-1,-1,0,0,1,1,1 };
	int dc[8] = { -1,0,1,-1,1,-1,0,1 };



	// 공격 대상 : 공격자의 공격력 만큼, 공격력 하락
	int attack_power1 = top_force[{attacker_top.first, attacker_top.second}];
	top_force[{attacked_top.first, attacked_top.second}] -= attack_power1;

	// 공격 대상을 제외한 8경로에 있는 포탑 : '공격자의 공격력 / 2' 만큼 공격력 하락
	int attack_power2 = attack_power1 / 2;
	//for (int i = 0; i < 8; ++i)
	for (int i = 0; i < 8; ++i)
	{
		//// 가장자리에서 막힌 방향으로 진행하고자 한다면, 반대편으로 나오는 처리 먼저 수행
		int tar_r = (((attacked_top.first + dr[i]) - 1 + N) % N) + 1;
		int tar_c = ((attacked_top.second + dc[i]) - 1 + M) % M + 1;

		//int tar_r = ((attacked_top.first + dr[i])) % N;
		//int tar_c = ((attacked_top.second + dc[i])) % M;
		//// 제한 처리 : 공격자는 해당 공격에 영향을 받지 않는다. 
		if (tar_r == attacker_top.first && tar_c == attacker_top.second) continue;

		//// 공격 처리
		top_force[{tar_r, tar_c}] -= attack_power2;
		//// is_related 처리
		//is_related[{tar_r, tar_c}] = true;
		is_related_set.insert({ tar_r, tar_c });
	}
	
	return;
}