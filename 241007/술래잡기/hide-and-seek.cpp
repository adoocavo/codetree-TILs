#include<iostream>
#include<map>
#include<set>
#include<vector>
#include<utility>
#include<cmath>
#include<queue>
using namespace std;

#define N_MAX (99+1)
#define M_MAX ((99 * 99) + 1)		//도망자 수 최대
#define H_MAX ((99 * 99) + 1)		//나무 수 최대


int n, m, h, k;

// 술래 관련 ds, var
pair<int, int> catcher_pos;			//술래 위치
//int catcher_direc = 3;				//술래 방향(초기 : 상)
int catcher_direc = 0;				//술래 방향(초기 : 상)
int catcher_score;					//술래 획득 점수
bool forward_facing;                // 중앙 -> (1,1) 이동 : true, (1,1) -> 중앙 이동 : false


// 도망자 관련 ds, var
map<int, pair<int, int>> runner_pos;	// 도망자 위치
int runner_direc[M_MAX];				// 도망자 방향
int is_alive[M_MAX];					// 도망자 생존여부

//나무 관련 ds, var
//// 술래의 시야 범위 격자가 tree_pos에 존재하는지 여부만 확인!
set<pair<int, int>> tree_pos;			//나무 위치

// mapp
//// => 술래만 mapp에 표시
int mapp[N_MAX][N_MAX];					//mapp : 1(술래), 나머지 : 그냥 칸

// 도망자들 이동 구현
void runners_move(void);
// catcher_forw_direcs, catcher_rev_direcs에 술래 이동방향 저장
void init_catcher_direc(void);
// 술래 이동 구현
void cather_move(const int game_turn);

// 좌 우 하 상 ( 0, 1, 2, 3) for 도망자
//int off_row[] = { 0, 0, -1, 1 };
//int off_col[] = { -1, 1, 0, 0 };
int off_row[] = { 0, 0, 1, -1 };
int off_col[] = { -1, 1, 0, 0 };

// 상우하좌 순서대로 넣어줍니다. for 술래
//// (0 <-> 2) (1 <-> 3)
int dx[4] = { -1, 0, 1,  0 };
int dy[4] = { 0 , 1, 0, -1 };

// catcher의 이동방향 저장 mapp
//// (1,1) -> catcher_rev_direcs[1][1]
//// 중앙 ->  catcher_forw_direcs[][]
int catcher_forw_direcs[N_MAX][N_MAX];          // catcher_forw_direcs[i][j] : (정방향 이동시) mapp[i][j]에 도착했을때 바뀌어야할 방향
int catcher_rev_direcs[N_MAX][N_MAX];           //  catcher_rev_direcs[i][j] : (역방향 이동시) mapp[i][j]에 도착했을때 바뀌어야할 방향

int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n >> m >> h >> k;

	catcher_pos.first = n / 2 + 1;
	catcher_pos.second = n / 2 + 1;
	mapp[catcher_pos.first][catcher_pos.second] = 1;

	int in_x, in_y, in_d;
	//for (int i = 1; i < m; ++i)
	for (int i = 1; i <= m; ++i)
	{
		cin >> in_x >> in_y >> in_d;

		runner_pos[i].first = in_x;
		runner_pos[i].second = in_y;

		runner_direc[i] = in_d;

		is_alive[i] = true;
	}

	int in_x2, in_y2;
	for (int i = 1; i <= h; ++i)
		//for (int i = 1; i < h; ++i)
	{
		cin >> in_x2 >> in_y2;

		tree_pos.insert({ in_x2 , in_y2 });
	}


	//2. 게임 수행
	init_catcher_direc();		// 술래 이동방향 저장
	forward_facing = true;		// 술래 이동방법(중앙 -> ())

	int game_turn;
	for (int i = 1; i <= k; ++i)
	{
		game_turn = i;

		//1. m명의 도망자가 먼저 동시에 움직임
		//// 동시에 : tmp_pos 배열 사용해서, 각 도망자의 변경위치 저장 후, 모든 이동 종료 후 copy
		runners_move();

		//2. 술래가 움직임
		cather_move(game_turn);
	}

	//3. k번에 걸쳐 술래잡기를 진행하는 동안 술래가 총 얻게된 점수를 출력
	cout << catcher_score;

	return 0;
}




void runners_move()
{
	//0.
	pair<int, int> tmp_pos[N_MAX];			//각 도망자들의 변경 위치 저장
	set<int> is_moved;						//이동한 도망자들의 idx 정보 저장

	//1. m명의 도망자 움직이기 : tmp에 반영
	for (int i = 1; i <= m; ++i)
	{
		////0. 이동 가능 여부 판단
		if (!is_alive[i]) continue;
		int dist = abs(runner_pos[i].first - catcher_pos.first) +
			abs(runner_pos[i].second - catcher_pos.second);
		if (dist > 3) continue;


		////1. 현재위치, nxt위치 구하기
		int cur_idx = i;						//현재 움직이는 도망자 idx
		int c_row = runner_pos[i].first;		//현재 움직이는 도망자 row
		int c_col = runner_pos[i].second;		//현재 움직이는 도망자 col
		int c_direc = runner_direc[i];			//현재 움직이는 도망자 이동방향

		//is_moved.insert(i);
		int nxt_row = c_row + off_row[c_direc];
		int nxt_col = c_col + off_col[c_direc];
		

		////2. 움직이는 동작 이후 상황
		// 격자를 벗어나는 경우
		if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n)
		{
			// 방향 틀기
			if (c_direc == 1 || c_direc == 3)
			{
				runner_direc[cur_idx] = c_direc - 1;
				c_direc = runner_direc[cur_idx];
			}

			else if (c_direc == 0 || c_direc == 2)
			{
				runner_direc[cur_idx] = c_direc + 1;
				c_direc = runner_direc[cur_idx];
			}

			// 바뀐 방향으로 1칸 움직이기
			int n_row = c_row + off_row[c_direc];
			int n_col = c_col + off_col[c_direc];

			// 술래 존재 여부에 따라 이동 결정하기
			if (mapp[n_row][n_col] == 1)
			{
				//is_moved.erase(cur_idx);
				continue;
			}
			else
			{
				tmp_pos[cur_idx] = { n_row , n_col };
				is_moved.insert(cur_idx);
			}
		}
		// 격자를 벗어나지 않는 경우
		else
		{
			if (mapp[nxt_row][nxt_col] == 1)
			{
				//is_moved.erase(cur_idx);
				continue;
			}
			else
			{
				tmp_pos[cur_idx] = { nxt_row , nxt_col };
				is_moved.insert(cur_idx);
			}
		}
	}


	//2. tmp_pos -> runner_pos
	for (auto i : is_moved)
	{
		// 이동 반영
		runner_pos[i].first = tmp_pos[i].first;
		runner_pos[i].second = tmp_pos[i].second;

	}

	return;
}


// catcher_forw_direcs, catcher_rev_direcs에 술래 이동방향 저장
void init_catcher_direc()
{

	// 시작 위치와 방향, 해당 방향으로 이동할 횟수를 설정합니다. 
	//// 1회 이동마다 : 이동방향이 바뀜
	//// 2회 이동 마다 : ++move_num
	int cur_row = n / 2 + 1, cur_col = n / 2 + 1;
	int move_dir = 0;               // 1번 이동시 각 칸에 저장시킬 이동방향 
	int move_num = 1;               // 1번 이동시 몇 칸 이동할지 

	// flag 역할 : move_cnt == 2 -> ++move_num, move_cnt = 0 으로 초기화
	int move_cnt = 0;

	while (1)
	{
		/*
		//0. (1,1) 도착시 종료
		if (cur_row == 1 && cur_col == 1)   break;
		*/

		//1. move_num 만큼 이동(1번 이동)
		//// 1번 이동시 : (1) 현재 위치의 이동방향 저장 (2) 다음 위치의 역방향(다음 위치에서 현재 위치로 오기위한 방향) 저장
		for (int i = 0; i < move_num; ++i)
		{
			//0. (1,1) 도착시 종료
			//if (cur_row == 1 && cur_col == 1)   break;
			if (cur_row == 1 && cur_col == 1)   return;

			//// (이동 전) 현재 위치에 이동방향 저장
			catcher_forw_direcs[cur_row][cur_col] = move_dir;

			//// 이동 후에 위치할 좌표 구하기
			////// 현재 위치의 move_dir 사용해서!
			int nxt_row = cur_row + dx[move_dir];
			int nxt_col = cur_col + dy[move_dir];

			//// 다음 위치의 역방향(다음 위치에서 현재 위치로 오기위한 방향) 저장
			int tmp_direc;
			if (move_dir == 0 || move_dir == 1)
			{
				tmp_direc = move_dir + 2;
			}
			else if (move_dir == 2 || move_dir == 3)
			{
				tmp_direc = move_dir - 2;
			}
			catcher_rev_direcs[nxt_row][nxt_col] = tmp_direc;


			////nxt_row, nxt_col ->  cur_row, cur_col
			cur_row = nxt_row;
			cur_col = nxt_col;

		}
		++move_cnt;

		//2. 이동방향 변경(1회 이동마다)
		move_dir = (move_dir + 1) % 4;

		//3. move_num 변경(2회 이동마다)
		if (move_cnt == 2)
		{
			++move_num;
			move_cnt = 0;
		}

	}
}

void cather_move(const int game_turn)
{
	//1. forward ? reverse ? 확인
	//forward
	if (forward_facing == true)
	{
		int cur_direc = catcher_forw_direcs[catcher_pos.first][catcher_pos.second];
		//mapp[catcher_pos.first][catcher_pos.second] = 0;

		//2. 한 칸 이동
		int nxt_row = catcher_pos.first + dx[cur_direc];
		int nxt_col = catcher_pos.second + dy[cur_direc];

		//// 방향 update
		//// => (1, 1) || (중심, 중심)->forward_facing 수정
		if (nxt_row == 1 && nxt_col == 1)
		{
			forward_facing = false;
			catcher_direc = catcher_rev_direcs[nxt_row][nxt_col];
		}
		else
		{
			catcher_direc = catcher_forw_direcs[nxt_row][nxt_col];
		}

		//3. catcher_pos, mapp 수정
		mapp[catcher_pos.first][catcher_pos.second] = 0;
		catcher_pos.first = nxt_row;
		catcher_pos.second = nxt_col;
		mapp[catcher_pos.first][catcher_pos.second] = 1;

	}
	// reverse
	else
	{
		int cur_direc = catcher_rev_direcs[catcher_pos.first][catcher_pos.second];

		//2. 한 칸 이동
		int nxt_row = catcher_pos.first + dx[cur_direc];
		int nxt_col = catcher_pos.second + dy[cur_direc];

		//// 방향 update
		//// => (1, 1) || (중심, 중심)->forward_facing 수정
		if (nxt_row == (n / 2 + 1) && nxt_col == (n / 2 + 1))
		{
			forward_facing = true;
			catcher_direc = catcher_forw_direcs[nxt_row][nxt_col];
		}
		else
		{
			catcher_direc = catcher_rev_direcs[nxt_row][nxt_col];
		}

		//3. catcher_pos, mapp 수정
		mapp[catcher_pos.first][catcher_pos.second] = 0;
		catcher_pos.first = nxt_row;
		catcher_pos.second = nxt_col;
		mapp[catcher_pos.first][catcher_pos.second] = 1;

	}

	//4. 시야 내에 도망자 탐색
	//// => 격자 크기 초과 확인 
	//// => 나무가 놓여 있는 칸 확인
	//// => is_alive, runner_pos 수정
	int num_of_catch = 0;			// 현재 턴에서 잡힌 도망자의 수
	set<int> erase_idx;				// 잡힌 도망자들의 idx 저장
	for (int i = 0; i < 3; ++i)
	{
		// 시야 범위 내의 좌표 구하기
		int tar_row = catcher_pos.first + (i * dx[catcher_direc]);
		int tar_col = catcher_pos.second + (i * dy[catcher_direc]);

		// 범위 초과 여부 확인
		if (tar_row < 1 || tar_row > n || tar_col < 1 || tar_col > n) break;

		// 도망자 존재 여부 확인
		for (auto it : runner_pos)
		{
			if ((it.second.first == tar_row && it.second.second == tar_col) && is_alive[it.first])		//도망자 존재 
			{
				if (tree_pos.find({ tar_row , tar_col }) == tree_pos.end())								//나무 존재 X
				{
					++num_of_catch;
					is_alive[it.first] = false;
					//runner_pos.erase(it.first);
					erase_idx.insert(it.first);
				}
			}
		}
	}
	// 사라질 대상들 없애기	
	for (auto it : erase_idx)
	{
		runner_pos.erase(it);
	}



	//5. 술래 점수 획득
	//catcher_score = game_turn * num_of_catch;
	catcher_score += game_turn * num_of_catch;

	return;

}