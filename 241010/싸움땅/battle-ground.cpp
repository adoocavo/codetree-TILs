#include<iostream>
#include<utility>
#include<queue>
#include<map>
#include<set>
#include<vector>
#include<algorithm>
#include<cmath>
using namespace std;

#define N_MAX (20+1)

// for input
int n, m, k;

// 플레이어 관련 ds, var
//// palyers_point => ans
map<int, pair<int, int>> players_pos;			//위치 -> 이동시 업데이트
map<int, int> players_force;					//초기 능력치
map<int, int> players_direc;					//플레이어 이동방향 -> 이동시 || 싸운 후처리 업데이트
map<int, int> players_gun_pow;					//갖고있는 총 능력치 -> 총 주울때 업데이트
map<int, int> players_point;					//획득 포인트 -> 싸운 후처리 업데이트


// 총 관련 ds, var
//// 3차원 벡터 사용
//// pos_to_guns[row][col].push_back()
//// 내림차순!
//// => 총 줍거나 버릴때 내림차순 배열로 update
vector<vector<vector<int>>> pos_to_guns(N_MAX, vector<vector<int>>(N_MAX));

// 상 / 우 / 하 / 좌 
//int off_r[4] = { -1, 0, 1, 1 };
int off_r[4] = { -1, 0, 1, 0 };
int off_c[4] = { 0, 1, 0, -1 };

// 0 <-> 2, 1 <-> 3
//// => 각 direc의 반대 방향 저장
int opp_direc[4] = {2, 3, 0, 1};


//
int mapp[N_MAX][N_MAX];		// 0은 빈 칸, 0보다 큰 값은 총의 공격력

//벡터 정렬
void sorrrrt(const int r, const int c, vector<vector<vector<int>>>& pos_to_guns);

// 플레이어 이동
//void move_players(const int tar_player);
pair<int, int> move_players(const int tar_player);

// 플레이어 이동 후처리
void after_move(const pair<int, int> nxt_pos, const int tar_player);

// param으로 주어지는 위치에, param으로 주어지는 플레이어를 제외한 사람 존재여부 확인
const int is_empty_pos(const int r, const int c, const int tar_player);

// 90도 회전
void rotate(const int loser);

// loser 이동 처리
void loser_move(const int loser);

int main()
{
	// ==========================================
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n >> m >> k;

	int in1;
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> in1;
			mapp[i][j] = in1;

			if (in1 > 0)
			{
				pos_to_guns[i][j].push_back(in1);
			}
		}

	}

	int x, y, d, s;
	for (int i = 1; i <= m; ++i)
	{
		cin >> x >> y >> d >> s;
		//players_pos[i] = { x,y };
		players_pos[i] = make_pair(x, y);
		players_direc[i] = d;
		players_force[i] = s;
		players_gun_pow[i] = 0;
		players_point[i] = 0;
	}

	// ==========================================


	//2. 게임 수행
	for (int i = 1; i <= k; ++i)
	{
		// 플레이어별 실행
		for (int j = 1; j <= players_pos.size(); ++j)
		{
			int tar_player = j;
			pair<int, int > nxt_node;	// 현재 플레이어(j)의 다음 위치 저장

			//// [1-1]  첫 번째 플레이어부터 순차적으로 본인이 향하고 있는 방향대로 한 칸만큼 이동
			////// [1-1-1] 격자를 벗어나는 경우인지 확인 ->  정반대 방향으로 방향을 바꾸어서 1만큼 이동
			//// => players_pos, players_direc 업데이트
			nxt_node = move_players(tar_player);

			////2. (1번)이동 후처리
			//// => 
			after_move(nxt_node, tar_player);

		}

	}


	//3. (게임 종료 후) 각 플레이어들이 획득한 포인트를 출력
	for (auto it_points : players_point)
	{
		cout << it_points.second << ' ';
	}
	cout << '\n';

	return 0;
}

void sorrrrt(const int r, const int c, vector<vector<vector<int>>>& pos_to_guns)
{
	sort(pos_to_guns[r][c].begin(), pos_to_guns[r][c].end());
}

//const int is_empty_pos(const int tar_player)
const int is_empty_pos(const int r, const int c, const int tar_player)
{
	int versus_player = 0;
	for (auto it_player : players_pos)
	{
		if (it_player.first == tar_player) continue;

		//// 
		if (it_player.second == make_pair(r, c))
		{
			versus_player = it_player.first;
			break;
		}
	}
	return versus_player;
}

void loser_move(const int loser)
{
	// 상 -> 우 -> 하 -> 좌
	int dx[4] = {-1, 0, 1, 0};
	int dy[4] = { 0, 1, 0, -1 };
	
	//0. loser가 이동할 좌표, 방향 저장
	int nxt_l_r, nxt_l_c, ndirec;

	for (int i = 0; i < 4; ++i)
	{
		ndirec = (players_direc[loser] + i) % 4;
		nxt_l_r = players_pos[loser].first + dx[ndirec];
		nxt_l_c = players_pos[loser].second + dy[ndirec];
		
		//1. 제한 :  다른 플레이어가 있거나 격자 범위 밖
		if (nxt_l_r < 1 || nxt_l_r > n || nxt_l_c < 1 || nxt_l_c > n) continue;
		//if (is_empty_pos(nxt_l_r, nxt_l_c, -1) == 0) continue;
		if (is_empty_pos(nxt_l_r, nxt_l_c, -1) > 0) continue;		// 다른 플레이어 존재

		//2-1. 총 있는지 확인
		//// 총 get -> pos_to_guns.pop_back
		if (!pos_to_guns[nxt_l_r][nxt_l_c].empty())
		{
			players_gun_pow[loser] = pos_to_guns[nxt_l_r][nxt_l_c].back();
			pos_to_guns[nxt_l_r][nxt_l_c].pop_back();
		}

		//2-2. 업데이트
		players_pos[loser] = make_pair(nxt_l_r, nxt_l_c);
		players_direc[loser] = ndirec;

		//3.
		return;
	}
	
	return;

}

//void move_players(const int tar_player)
pair<int, int> move_players(const int tar_player)
{

	// [1-1]  첫 번째 플레이어부터 순차적으로 본인이 향하고 있는 방향대로 한 칸만큼 이동
	int nxt_r = players_pos[tar_player].first + off_r[players_direc[tar_player]];
	int nxt_c = players_pos[tar_player].second + off_c[players_direc[tar_player]];
	
	//// [1-1-1] 격자를 벗어나는 경우인지 확인 ->  정반대 방향으로 방향을 바꾸어서 1만큼 이동
	if (nxt_r < 1 || nxt_r > n || nxt_c < 1 || nxt_c > n)
	{
		players_direc[tar_player] = opp_direc[players_direc[tar_player]];
		nxt_r = players_pos[tar_player].first + off_r[players_direc[tar_player]];
		nxt_c = players_pos[tar_player].second + off_c[players_direc[tar_player]];
	}

	players_pos[tar_player].first = nxt_r;
	players_pos[tar_player].second = nxt_c;

	//return make_pair(players_pos[tar_player].first, players_pos[tar_player].first);
	return make_pair(players_pos[tar_player].first, players_pos[tar_player].second);
}

void after_move(const pair<int, int> nxt_pos, const int tar_player)
{
	// 0. 현재 이동하는 플레이어가 이동하려는 위치 저장
	//// tar_player : 현재 이동하는 플레이어(nxt_pos에 이동하려는 플레이어)
	//// versus_player : 현재 이동하는 플레이어가 이동한 위치에, 이미 존재하는 플레이어
	int tr = nxt_pos.first;
	int tc = nxt_pos.second;

	int versus_player;

	// [2-1] 이동한 방향에 플레이어가 없는 경우
	//// = > pos_to_guns, players_gun_pow, players_pos 업데이트
	if ((versus_player = is_empty_pos(tr, tc, tar_player)) == 0)
	{
		//// [2-1-1] 이동한 칸에 총이 있는지 확인 -> 총을 획득(임시로 갖기-플레이어의 총이 되는건 확정X)
		//// 이동한 칸에 총 존재
		if (!pos_to_guns[tr][tc].empty())
		{
			// tar_player가 총 갖고있다
			if (players_gun_pow[tar_player] > 0)
			{
				//// [2-1-1-1] 이미 총을 가지고 있는 경우 -> 놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득, 나머지 총들은 해당 격자에 둡
				//// player 총 vs 격자 총 -> 큰거는 플레이어가 -> 작은거는 격자가 -> 격자 총 현황 업데이트 -> 정렬 
				int tmp = players_gun_pow[tar_player];
				players_gun_pow[tar_player] = max(tmp, pos_to_guns[tr][tc].back());
				pos_to_guns[tr][tc].back() = min(tmp, pos_to_guns[tr][tc].back());

				sorrrrt(tr, tc, pos_to_guns);
			}

			// tar_player가 총 갖고있지X
			else
			{
				//// [2-1-1-2] 총을 갖고있지 X ->  [2-1-1]에서 획득한 총 확정
				//// 총 획득 -> 격자 총 현황 업데이트 -> 정렬
				players_gun_pow[tar_player] = pos_to_guns[tr][tc].back();
				pos_to_guns[tr][tc].pop_back();
			}
		}

		//// 이동한 칸에 총 없음
		else
		{
			;
		}

		//// player_pos 업데이트
		players_pos[tar_player] = nxt_pos;
	}


	// [2-2] 이동한 방향에 플레이어가 있는 경우
	else
	{
		////1. 결투 : tar_player vs versus_player
		int versus_force = players_force[versus_player] + players_gun_pow[versus_player];
		int tar_force = players_force[tar_player] + players_gun_pow[tar_player];
		int winner;
		int loser;
		////// winner = versus_player
		if ((versus_force > tar_force) || ((versus_force == tar_force) && (players_force[versus_player] > players_force[tar_player])))
		{
			winner = versus_player;
			//loser = tar_force;
			loser = tar_player;
		}
		////// winner = tar_player
		else
		{
			//winner = tar_force;
			winner = tar_player;
			loser = versus_player;
		}

		////2. 이긴 플레이어는 각 플레이어의 초기 능력치와 가지고 있는 총의 공격력의 합의 차이만큼을 포인트로 획득
		players_point[winner] += abs(versus_force - tar_force);

		////3. 진 플레이어는 (1) 본인이 가지고 있는 총을 해당 격자에 내려놓기 (2) 해당 플레이어가 원래 가지고 있던 방향대로 한 칸 이동
		////// (1) 총 놓기 : pos_to_guns에 push_back -> players_gun_pow = 0 -> pos_to_guns sort
		if (players_gun_pow[loser] > 0)
		{
			pos_to_guns[players_pos[loser].first][players_pos[loser].second].push_back(players_gun_pow[loser]);
			players_gun_pow[loser] = 0;

			sorrrrt(players_pos[loser].first, players_pos[loser].second, pos_to_guns);
		}

		////// (2) 해당 플레이어가 원래 가지고 있던 방향대로 한 칸 이동
		////// => loser 관련 정보 업데이트까지 끝
		loser_move(loser);

		//// 4. 이긴 플레이어는 승리한 칸에 떨어져 있는 총들과 원래 들고 있던 총 중 가장 공격력이 높은 총을 획득하고, 나머지 총들은 해당 격자에 내려 놓
		int winner_r = players_pos[winner].first;
		int winner_c = players_pos[winner].second;
		if (!pos_to_guns[winner_r][winner_c].empty())
		{
			// winner가 총 갖고있다
			if (players_gun_pow[winner] > 0)
			{
				////  이미 총을 가지고 있는 경우 -> 놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득, 나머지 총들은 해당 격자에 둡
				int tmp = players_gun_pow[winner];
				players_gun_pow[winner] = max(tmp, pos_to_guns[winner_r][winner_c].back());
				pos_to_guns[winner_r][winner_c].back() = min(tmp, pos_to_guns[winner_r][winner_c].back());

				sorrrrt(winner_r, winner_c, pos_to_guns);
			}

			// winner 가 총 갖고있지X
			else
			{
				////  총을 갖고있지 X -> 총 줍기
				//// 총 획득 -> 격자 총 현황 업데이트 -> 정렬
				/*
				players_gun_pow[tar_player] = pos_to_guns[tr][tc].back();
				pos_to_guns[tr][tc].pop_back();
				*/
				//players_gun_pow[tar_player] = pos_to_guns[winner_r][winner_c].back();
				players_gun_pow[winner] = pos_to_guns[winner_r][winner_c].back();
				pos_to_guns[winner_r][winner_c].pop_back();
			}
		}
	}
}