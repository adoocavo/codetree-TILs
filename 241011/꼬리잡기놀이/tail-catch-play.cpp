#include<iostream>
#include<queue>
#include<utility>
#include<map>
#include<deque>
#include<cmath>
using namespace std;


#define M_MAX (5+1)		//최대 팀 개수
#define N_MAX (20+1)	//최대 격자(r,c) 크기


/*
struct team_type {
	pair<int, int> head;
	pair<int, int> tail;

	int num_of_man;		// 팀 구성원 명수
	int score;			// 팀 획득 점수
};
*/
//팀 관련 DS, var
//// key : 팀 번호(5~)
//map<int, pair<int, int>> head;
//map<int, pair<int, int>> tail;
//map<int, int> team_size;						// 팀 구성원 명수
map<int, int> team_score;						// 팀 획득 점수
map<int, deque<pair<int, int>>> mem_list;		// 팀 구성원 목록 저장(front : 머리, back : 꼬리)
 
// 격자의 크기 n, 팀의 개수 m, 라운드 수 k
int n, m, k;

/*
// 팀 관련 정보 저장
//// idx : 팀 번호%팀 개수
//// 6-11 : 1-5 
team_type team[M_MAX];
*/

// mapp 관련 정보 저장
//// input : 0(빈칸), 1-4(1:해당 팀 머리, 3:해당 팀 꼬리, 2:해당팀 나머지, 4: 이동선)
//// => 4 : 이동선, 6-11 : 팀 번호
int mapp[N_MAX][N_MAX];

// 팀 만들기
//// => BFS 사용해서 if(mapp[][] == 4) -> 네트워크로 묶기 + 1,2,3을 팀 번호로 수정
void building_team(void);
// 팀 이동
void move_team(const int t_move);	
// 공 던지기 
void throw_ball(const int round);
// 공 맞으면 => (1) 점수 획득 (2) 방행 바꾸기
void bye_man(const int check_r, const int check_c);
// scaled된 라운드 값 -> 방향 정하기
const int for_direc(const int offset);


// 우 -> 상 -> 좌 -> 하
int off_r[4] = { 0, -1, 0, 1 };
int off_c[4] = { 1, 0, -1, 0 };




int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. 
	cin >> n >> m >> k;

	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> mapp[i][j];
		}
	}

	//2. 팀 만들기
	//// => mapp[][] : team idx가 저장됨
	building_team();

	//3. 게임 수행
	for (int i = 1; i <= k; ++i)
	{
		////1. 모든 팀이 머리사람 따라 이동
		for (auto it_team : mem_list)
		{
			move_team(it_team.first);
		}

		////2. 공 던지기 -> (공 받으면) (1)해당 팀 점수 획득 (2) 머리 <-> 꼬리 전환 
		throw_ball(i);
	}

	//4. 출력
	int ans_sum = 0;
	for (auto it_score : team_score)
	{
		ans_sum += it_score.second;
	}

	cout << ans_sum;

	return 0;
}


void building_team(void)
{
	//0-1.
	int tmp[N_MAX][N_MAX] = { 0, };
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			tmp[i][j] = mapp[i][j];
		}
	}

	//0-2. 
	bool v[N_MAX][N_MAX] = { false, };
	int team_number = 5;		//초기 팀 번호는 5번부터 시작			
	queue<pair<int, int>> q_nodes;


	//1.
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			//// 시작점 찾기
			if (mapp[i][j] == 1 && !v[i][j])					// 시작점 : 팀의 머리 && 미방문
			{
				q_nodes.push({ i,j });
				v[i][j] = true;
				
				mem_list[team_number].push_back({ i,j });		// 팀 머리 사람 저장
				tmp[i][j] = team_number;
				team_score[team_number] = 0;					//만들어질 팀 점수 초기화

				pair<int, int>cur_node;
				while (!q_nodes.empty())
				{
					//// 1. nxt 대상을 구하기 위한 cur node 구하기
					cur_node = q_nodes.front();
					q_nodes.pop();

					////
					for (int i = 0; i < 4; ++i)
					{
						int n_r = cur_node.first + off_r[i];
						int n_c = cur_node.second + off_c[i];

						////// 제한 : visit, 초과 / 네트워크 아님 
						if (v[n_r][n_c] || (n_r < 1 || n_r > n || n_c < 1 || n_c > n)) continue;
						//if (mapp[n_r][n_c] < 1 ) continue;
						if (!(mapp[n_r][n_c] >= 1 && mapp[n_r][n_c] <= 3)) continue;
						////// 제한 :  마지막 격자로서의 꼬리 || 몸통
						if (!((mapp[n_r][n_c] == 2) || (mapp[cur_node.first][cur_node.second] == 2 && mapp[n_r][n_c] == 3))) continue;

						////// 팀 정보 저장하기 : mem_list, tmp 업데이트
						mem_list[team_number].push_back({ n_r,n_c });
						tmp[n_r][n_c] = team_number;
						
						////// nxt queue 저장
						v[n_r][n_c] = true;
						q_nodes.push({ n_r, n_c });
					}
				
					//team_number++;
				}

				team_number++;
			}

			//team_number++;
		}
	}

	//2. tmp -> mapp
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			mapp[i][j] = tmp[i][j];
		}
	}

	return;

}

void move_team(const int t_move)
{
	//1. 꼬리 pop -> mapp 반영
	pair<int, int> tar_pop = mem_list[t_move].back();
	mem_list[t_move].pop_back();
	mapp[tar_pop.first][tar_pop.second] = 4;

	//2. 머리에서 경로탐색 -> push 경로 찾음 -> 머리에 push -> mapp 반영
	pair<int, int> cur_head = mem_list[t_move].front();
	pair<int, int> tar_push;
	//// 머리에서 경로탐섹
	for (int i = 0; i < 4; ++i)
	{
		tar_push.first = cur_head.first + off_r[i];
		tar_push.second = cur_head.second + off_c[i];
	
		//제한 : 범위내에 존재 && 4
		if (!(tar_push.first < 1 || tar_push.first > n || tar_push.second < 1 || tar_push.second > n) &&
			mapp[tar_push.first][tar_push.second] == 4)
		{
			break;
		}
	}

	//// 머리에 push
	mem_list[t_move].push_front({ tar_push.first , tar_push.second });
	//// mapp 반영
	/*
	mapp[cur_head.first][cur_head.second] = 2;
	mapp[tar_push.first][tar_push.second] = 1;
	*/
	mapp[cur_head.first][cur_head.second] = t_move;
	mapp[tar_push.first][tar_push.second] = t_move;


	return;
}

const int for_direc(const int scaled_round)
{
	if (scaled_round >= 1 && scaled_round <= n) return 0;
	else if (scaled_round >= n + 1 && scaled_round <= 2 * n) return 1;
	else if (scaled_round >= 2 * n + 1 && scaled_round <= 3 * n) return 2;
	else return 3;
}

void bye_man(const int check_r, const int check_c)
{
	int teamidx = mapp[check_r][check_c];

	//1. 점수 획득
	//// 해당 사람이 머리사람을 시작으로 팀 내에서 몇 번째?
	int k = 0;		//몇 번째 사람인지 저장
	for (int i = 0; i < mem_list[teamidx].size(); ++i)
	{
		pair<int, int> tmp = *(mem_list[teamidx].begin() + i);
		if (tmp == make_pair(check_r, check_c))
		{
			k = (i+1);
			break;
		}
	}
	int getSocre = k*k;
	team_score[teamidx] += getSocre;

	//2. 앞, 뒤 바꾸기 
	pair<int, int>c_head = mem_list[teamidx].front();
	pair<int, int>c_tail = mem_list[teamidx].back();
	
	mem_list[teamidx].back() = c_head;
	mem_list[teamidx].front() = c_tail;

	//3. mapp 반영
	/*
	mapp[c_tail.first][c_tail.second] = 1;
	mapp[c_head.first][c_head.second] = 3;
	*/
	mapp[c_tail.first][c_tail.second] = teamidx;
	mapp[c_head.first][c_head.second] = teamidx;

	return;
}


void throw_ball(const int round)
{
	//1. 라운드 -> 방향결정, 방향 -> (공이 던저질)row,col 결정
	int scaled_round = ((round - 1) % (4 * n)) + 1;			// all 라운드 -> offset : 1-28 숫자로 치환 
	int direc = for_direc(scaled_round);
	int str = ((scaled_round - 1) % n) + 1;					// all scaled_round -> 1-7 숫자로 치환

	int sr, sc;
	if (direc == 0)
	{
		sr = str;
		sc = 0;
	}
	else if (direc == 1)
	{
		sr = n;
		sc = str;
	}
	else if (direc == 2)
	{
		sr = str;
		sc = n;
	}
	else
	{
		sr = 0;
		sc = str;
	}


	//2. sr, sc, direc 사용해서 -> mapp탐색
	for (int i = 0; i <= n-1; ++i)
	{
		/*
		int check_r = sr + off_r[i];
		int check_c = sc + off_c[i];
		*/
		int check_r = sr + (off_r[direc] * i);
		int check_c = sc + (off_c[direc] * i);

		if (mapp[check_r][check_c] >= 5)
		{
			bye_man(check_r, check_c);
		
			return;
		}
	}

	return;

}