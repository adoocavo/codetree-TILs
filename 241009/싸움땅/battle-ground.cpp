#include<iostream>
#include<utility>
#include<queue>
#include<map>
#include<set>
#include<vector>
#include<algorithm>
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

//
int mapp[N_MAX][N_MAX];		// 0은 빈 칸, 0보다 큰 값은 총의 공격력

//벡터 정렬
void sorrrrt(const int r, const int c, vector<vector<vector<int>>>& pos_to_guns);
// 플레이어 이동
void move_players(const int tar_player);
// 플레이어 이동 후처리
void after_move(const int tar_player);
// param으로 주어지는 위치에, param으로 주어지는 플레이어를 제외한 사람 존재여부 확인
const int is_empty_pos(const int r, const int c, const int tar_player);
// 90도 회전
//pair<int, int> rotate(const int r, const int c, const int direc);
void rotate(const int loser);

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

			////1. 첫 번째 플레이어부터 순차적으로 본인이 향하고 있는 방향대로 한 칸만큼 이동
			//// => players_pos, players_direc 업데이트
			move_players(tar_player);

			////2. (1번)이동 후처리
			//// => 
			after_move(tar_player);

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
	/*
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			sort(pos_to_guns[i][j].rbegin(), pos_to_guns[i][j].rend());
		}
	}
	*/
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


//pair<int, int> rotate(const int r, const int c, const int direc)
void rotate(const int loser)
{
	// 상 우 하 좌
	int dx[4] = { -1, 0, 1, 0 };
	int dy[4] = { 0, 1, 0, -1 };


	// param 좌표 : loser의 현재 위치
	// => loser의 현재 격자 위치에서, 현재 방향 포함 시계방향 90도씩 위치 탐색
	//// => 기존 방향의 탐색은 어차피 조건에 부적합해서 제외될거니깐 예외처리 신경끄기 -> ㄴㄴ 이전 조건이 안결러지므로 탐색대상에서 제외
	//// =>  loser의 직전 위치가 비어있으므로, 갈 격자는 나온다!!
	int cur_r = players_pos[loser].first;
	int cur_c = players_pos[loser].second;
	int cur_direc = players_direc[loser];

	int n_r;
	int n_c;


	//for (int i = 1; i <= 3; ++i)
	for (int i = 0; i < 4; ++i)
	{
		n_r = cur_r + dx[(cur_direc + i) % 4];
		n_c = cur_c + dy[(cur_direc + i) % 4];

		//빈칸 여부 확인 : 총, 사람 X
		if (n_r < 1 || n_r > n || n_c < 1 || n_c > n) continue;
		if (pos_to_guns[n_r][n_c].empty() && is_empty_pos(n_r, n_c, -1) == 0)
		{
			// loser_pos, loser_direc 처리
			players_pos[loser] = make_pair(n_r, n_c);
			players_direc[loser] = (cur_direc + i) % 4;

			return;
		}
	}

	return;

	/*
	// 원래 이동한 위치 구하기
	//int n_r = r + off_r[direc];
	//int n_c = c + off_c[direc];

	//loser의 현재 위치에서, 현재 방향의 90도 1/2/3번 회전한것들 상태 보기
	int n_r;
	int n_c;

	// 빈 칸 보이면 바로 이동
	for (int i = 1; i <= 3; ++i)
	{
		n_r = r + dx[(direc + i) % 4];
		n_c = c + dy[(direc + i) % 4];

		//빈칸 여부 확인 : 총, 사람 X
		if (pos_to_guns[n_r][n_c].empty() && is_empty_pos(n_r, n_c, -1) == 0)
		{
			// 빈 칸 보여서 이동
			return make_pair(n_r, n_c);
		}
	}

	// 빈 칸 없어서 기존위치 ㅠ
	return make_pair(n_r, n_c);
	*/

}



void move_players(const int tar_player)
{
	// 0. 
	/*
	int nxt_r = players_pos[tar_player].first + players_direc[tar_player];
	int nxt_c = players_pos[tar_player].second + players_direc[tar_player];
	*/
	int nxt_r = players_pos[tar_player].first + off_r[players_direc[tar_player]];
	int nxt_c = players_pos[tar_player].second + off_c[players_direc[tar_player]];

	// 1. 만약 해당 방향으로 나갈 때 격자를 벗어나는 경우에는 정반대 방향으로 방향을 바꾸어서 1만큼 이동
	//// => 0 <-> 2, 1 <-> 3
	//// 상(0) 우(1) 하(2) 좌(3)
	if (nxt_r < 1 || nxt_r > n || nxt_c < 1 || nxt_c > n)
	{
		//// 방향 바꾸기
		if (players_direc[tar_player] == 0 || players_direc[tar_player] == 1)
		{
			players_direc[tar_player] += 2;
		}
		else if (players_direc[tar_player] == 2 || players_direc[tar_player] == 3)
		{
			players_direc[tar_player] -= 2;
		}

		////바뀐 방향 반영
		/*
		nxt_r = players_pos[tar_player].first + players_direc[tar_player];
		nxt_c = players_pos[tar_player].second + players_direc[tar_player];
		*/
		nxt_r = players_pos[tar_player].first + off_r[players_direc[tar_player]];
		nxt_c = players_pos[tar_player].second + off_c[players_direc[tar_player]];
	}

	//2. nxt_r,c -> players_pos[tar_player]
	players_pos[tar_player].first = nxt_r;
	players_pos[tar_player].second = nxt_c;

	return;
}

void after_move(const int tar_player)
{
	//0. 후처리 위치 저장
	/*
	int tar_r = players_pos[tar_player].first + players_direc[tar_player];
	int tar_c = players_pos[tar_player].second + players_direc[tar_player];
	*/
	/*
	int tar_r = players_pos[tar_player].first + off_r[players_direc[tar_player]];
	int tar_c = players_pos[tar_player].second + off_c[players_direc[tar_player]];
	*/
	int tar_r = players_pos[tar_player].first;
	int tar_c = players_pos[tar_player].second;

	//1. 이동한 위치에 다른 플레이어 있는지 확인
	int versus_player = is_empty_pos(tar_r, tar_c, tar_player);

	//2. 
	//// case2 : 이동한 방향에 플레이어가 있다
	if (versus_player > 0)
	{
		//// 0. 각 플레이어의 공격력 구하기
		int force_of_tar_player = players_force[tar_player] + players_gun_pow[tar_player];
		int force_of_versus_player = players_force[versus_player] + players_gun_pow[versus_player];

		//// 동작1 : 제한1-2에 맞춰, 두 플레이어가 싸운다
		////=> winner, loser 구하기
		int winner, loser;
		if (force_of_tar_player > force_of_versus_player)
		{
			winner = tar_player;
			loser = versus_player;
		}
		else if (force_of_tar_player < force_of_versus_player)
		{
			winner = versus_player;
			loser = tar_player;
		}
		else if (force_of_tar_player == force_of_versus_player)
		{
			if (players_force[tar_player] > players_force[versus_player])
			{
				winner = tar_player;
				loser = versus_player;
			}
			else
			{
				winner = versus_player;
				loser = tar_player;
			}

		}

		//// 동작2 : 이긴 플레이어는, 각 플레이어의 초기 능력치와 가지고 있는 총의 공격력의 합의 차이만큼을 포인트로 획득
		int getPoint = (players_force[winner] + players_gun_pow[winner]) - (players_force[loser] + players_gun_pow[loser]);
		players_point[winner] += getPoint;

		//// 동작3: 진 플레이어는, (1)본인이 가지고 있는 총을 해당 격자에 내려놓 (2) 해당 플레이어가 원래 가지고 있던 방향대로 한 칸 이동
		////// (1) 본인이 가지고 있는 총을 해당 격자에 내려놓
		if (players_gun_pow[loser] > 0)
		{
			int bye_gun = players_gun_pow[loser];
			players_gun_pow[loser] = 0;
			pos_to_guns[players_pos[loser].first][players_pos[loser].second].push_back(bye_gun);

			sorrrrt(players_pos[loser].first, players_pos[loser].second, pos_to_guns);
		}
		////// (2) 해당 플레이어가 원래 가지고 있던 방향대로 한 칸 이동
		/*
		int loser_nxt_r = players_pos[loser].first + players_direc[loser];
		int loser_nxt_c = players_pos[loser].second + players_direc[loser];
		*/
		int loser_nxt_r = players_pos[loser].first + off_r[players_direc[loser]];
		int loser_nxt_c = players_pos[loser].second + off_c[players_direc[loser]];

		//////// case1 : 이동하려는 칸에, 다른 플레이어가 있 || 격자 범위 밖 -> 오른쪽으로 90도씩 회전하여 빈 칸이 보이는 순간 이동
		///////// => pos, direc 변경!
		if ((loser_nxt_r < 1 || loser_nxt_r > n || loser_nxt_c < 1 || loser_nxt_c > n) ||
			is_empty_pos(loser_nxt_r, loser_nxt_c, loser) > 0)
		{
			//make_pair(loser_nxt_r, loser_nxt_c) = rotate(players_pos[loser].first, players_pos[loser].second, players_direc[loser]);
			rotate(loser);
		}


		////////case2 : 이동한 칸에, 총이 있 -> 가장 공격력이 높은 총을 획득하고 나머지 총들은 해당 격자에 내려 놓
		////////// pos, players_gun_pow, pos_to_guns 변경
		else if (!pos_to_guns[loser_nxt_r][loser_nxt_c].empty())
		{
			/*
			players_gun_pow[loser] = pos_to_guns[tar_r][tar_c].back();
			pos_to_guns[loser_nxt_r][loser_nxt_c].pop_back();
			*/
			//players_gun_pow[loser] = pos_to_guns[loser_nxt_r][loser_nxt_r].back();
			players_gun_pow[loser] = pos_to_guns[loser_nxt_r][loser_nxt_c].back();
			pos_to_guns[loser_nxt_r][loser_nxt_c].pop_back();

			players_pos[loser] = make_pair(loser_nxt_r, loser_nxt_c);
		}

		//////// case1,2 모두 아닌경우 이동 반영
		//players_pos[loser] = make_pair(loser_nxt_r, loser_nxt_c);
		else
		{
			players_pos[loser] = make_pair(loser_nxt_r, loser_nxt_c);
		}
		


		//// 동작4 : 이긴 플레이어는 승리한 칸에 떨어져 있는 총들과 원래 들고 있던 총 중 가장 공격력이 높은 총을 획득하고, 나머지 총들은 해당 격자에 내려 놓
		//// 플레이어가 이미 총을 가지고 있다 -> 놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득(나머지 총들은 해당 격자에)
		int winner_r = tar_r;
		int winner_c = tar_c;
		if (!pos_to_guns[winner_r][winner_c].empty())
		{
			//// case 1-1-2 :  플레이어 총 있다
			if (players_gun_pow[winner] > 0)
			{

				////  놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득
				int tmp = players_gun_pow[winner];
				/*
				players_gun_pow[winner] = max(players_gun_pow[winner], pos_to_guns[winner_r][winner_c].back());
				pos_to_guns[winner_r][winner_c].back() = min(players_gun_pow[winner], pos_to_guns[winner_r][winner_c].back());
				*/
				players_gun_pow[winner] = max(tmp, pos_to_guns[winner_r][winner_c].back());
				pos_to_guns[winner_r][winner_c].back() = min(tmp, pos_to_guns[winner_r][winner_c].back());

				//// (나머지 총들은 해당 격자에)
				sorrrrt(winner_r, winner_c, pos_to_guns);
			}
			//// case 1-1-2 :  플레이어 총 없다 -> 그냥 획득
			else
			{
				players_gun_pow[winner] = pos_to_guns[winner_r][winner_c].back();
				pos_to_guns[winner_r][winner_c].pop_back();
			}
		}
		else { ; }
	}
	//// case1 : 이동한 방향에 플레이어가 없다
	else
	{
		//// case1-1 : 해당 칸에 총이 있다
		if (!pos_to_guns[tar_r][tar_c].empty())
		{
			//// case 1-1-1 : 플레이어가 이미 총을 가지고 있다 -> 놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득(나머지 총들은 해당 격자에)
			if (players_gun_pow[tar_player] > 0)
			{
				////  놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득
				int tmp = players_gun_pow[tar_player];
				//players_gun_pow[tar_player] = max(players_gun_pow[tar_player], pos_to_guns[tar_r][tar_c].back());
				//pos_to_guns[tar_r][tar_c].back() = min(players_gun_pow[tar_player], pos_to_guns[tar_r][tar_c].back());

				players_gun_pow[tar_player] = max(tmp, pos_to_guns[tar_r][tar_c].back());
				pos_to_guns[tar_r][tar_c].back() = min(tmp, pos_to_guns[tar_r][tar_c].back());
				//// (나머지 총들은 해당 격자에)
				sorrrrt(tar_r, tar_c, pos_to_guns);
			}
			//// case 1-1-2 :  플레이어 총 없다 -> 그냥 획득
			else
			{
				players_gun_pow[tar_player] = pos_to_guns[tar_r][tar_c].back();
				pos_to_guns[tar_r][tar_c].pop_back();
			}

		}
		//// case1-2 : 해당 칸에 총이 없다
		else
		{
			//끝
			return;
		}

		return;
	}

}