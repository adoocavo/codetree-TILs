#include<iostream>
#include<map>
#include<vector>
#include<algorithm>
//#include<deque>
using namespace std;


#define N_MAX 20+1
#define M_MAX 30+1


int n;		//격자크기
int m;		//플레이어 수
int k;		//라운드 수

int mapp[N_MAX][N_MAX];							//1-m : 플레이어 위치, 0 : 빈칸
vector<vector<vector<int>>> guns(N_MAX, vector<vector<int>>(N_MAX));		// page, 행 고정, 오름차순으로 정렬(마지막 요소가 가장 큰 총)  	

map<int, pair<int, int>> players_pos;					// key번째 플레이어의 위치 저장
map<int, pair<int, int>> players_info;					// key번째 플레이어의 방향, 초기 능력치 저장
int players_gun[M_MAX];									// 플레이어가 갖고있는 총 공격력 저장

int points[M_MAX];										// key번째 플레이어의 획득 점수 총합


//방향 d는 0부터 3까지 순서대로 ↑, →, ↓, ←을 의미 
int off_row[4] = { -1, 0, 1, 0 };
int off_col[4] = { 0, 1, 0, -1 };


void move(void);		//플레이어 이동


int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n >> m >> k;
	
	int input_guns_power;
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> input_guns_power;
			if (input_guns_power > 0)
			{
				guns[i][j].push_back(input_guns_power);
			}
		}
	}


	int input_x, input_y, input_d, input_s;
	for (int i = 1; i <= m; ++i)
	{
		cin >> input_x >> input_y >> input_d >> input_s;
		players_pos[i] = { input_x , input_y };
		players_info[i] = { input_d , input_s };
		
		mapp[input_x][input_y] += i;
	}


	//2. 게임 수행
	while (k--)
	{
		//1. 첫 번째 플레이어부터 순차적으로 본인이 향하고 있는 방향대로 한 칸만큼 이동
		//2. 이동 후 처리
		move();
	}


	//3. 정답 출력
	for (int i = 1; i <= m; ++i)
	{
		cout << points[i] << ' ';
	}
	return 0;
}



void move(void)
{
	/*
	//1. 플레이어 위치 확인
	for (int i = 1; i <= players_pos.size(); ++i)
	{
		//for (int j = 1; j <= n; ++j)
		//{
		int cur_idx;
		int cur_row, cur_col, cur_direction, cur_power;
		int nxt_row, nxt_col, nxt_direction;
		
		if (mapp[i][j] > 0)
		{
			//// 현재 위치, 정보 저장
			cur_idx = mapp[i][j]; mapp[i][j] -= cur_idx;
			cur_row = players_pos[cur_idx].first;
			cur_col = players_pos[cur_idx].second;
			cur_direction = players_info[cur_idx].first;
			cur_power = players_info[cur_idx].second;


			//// 이동할 위치 저장
			nxt_row = cur_row + off_row[cur_direction];
			nxt_col = cur_col + off_col[cur_direction];

			////// 제한
			if (nxt_row < 1 || nxt_row > N_MAX || nxt_col < 1 || nxt_col > N_MAX)
			{
				if (cur_direction == 0 || cur_direction == 1)
				{
					nxt_direction = cur_direction + 2;
					nxt_row = cur_row + off_row[nxt_direction];
					nxt_col = cur_col + off_col[nxt_direction];
				}
				else if (cur_direction == 2 || cur_direction == 3)
				{
					nxt_direction = cur_direction - 2;
					nxt_row = cur_row + off_row[nxt_direction];
					nxt_col = cur_col + off_col[nxt_direction];
				}
			}
			else { nxt_direction = cur_direction; }

			//// mapp에 이동 반영
			players_pos[cur_idx].first = nxt_row;
			players_pos[cur_idx].second = nxt_col;
			players_info[cur_idx].first = nxt_direction;

			mapp[nxt_row][nxt_col] += cur_idx;

		}
		//}
	}
	*/
	
	//1. 플레이어 이동할 위치 확인
	for (int i = 1; i <= players_pos.size(); ++i)
	{

		int cur_idx = i;
		int cur_row, cur_col, cur_direction, cur_power;
		int nxt_row, nxt_col, nxt_direction;


		//// 현재 위치, 정보 저장
		//cur_idx = mapp[i][j]; mapp[i][j] -= cur_idx;
		cur_row = players_pos[cur_idx].first;
		cur_col = players_pos[cur_idx].second;
		cur_direction = players_info[cur_idx].first;
		cur_power = players_info[cur_idx].second;

		mapp[cur_row][cur_col] = 0;		//이전 위치 0 초기화


		//// 이동할 위치 저장
		nxt_row = cur_row + off_row[cur_direction];
		nxt_col = cur_col + off_col[cur_direction];

		////// 제한
		if (nxt_row < 1 || nxt_row > N_MAX || nxt_col < 1 || nxt_col > N_MAX)
		{
			if (cur_direction == 0 || cur_direction == 1)
			{
				nxt_direction = cur_direction + 2;
				nxt_row = cur_row + off_row[nxt_direction];
				nxt_col = cur_col + off_col[nxt_direction];
			}
			else if (cur_direction == 2 || cur_direction == 3)
			{
				nxt_direction = cur_direction - 2;
				nxt_row = cur_row + off_row[nxt_direction];
				nxt_col = cur_col + off_col[nxt_direction];
			}
		}
		else { nxt_direction = cur_direction; }


		//2. 이동 후 처리

		// 플레이어가 있는 경우
		if (mapp[nxt_row][nxt_col] > 0)			
		{
			int winner, loser;

			// 2-1. 해당 플레이어의 초기 능력치와 가지고 있는 총의 공격력의 합을 비교
			//int full_power1 = players_info[cur_idx].first + players_gun[cur_idx];
			//int full_power2 = players_info[mapp[nxt_row][nxt_col]].first + players_gun[mapp[nxt_row][nxt_col]];
			int full_power1 = players_info[cur_idx].second + players_gun[cur_idx];
			int full_power2 = players_info[mapp[nxt_row][nxt_col]].second + players_gun[mapp[nxt_row][nxt_col]];


			if (full_power1 == full_power2)
			{
				//플레이어의 초기 능력치가 높은 플레이어가 승리
				//if (players_info[cur_idx].first > players_info[mapp[nxt_row][nxt_col]].first)
				if (players_info[cur_idx].second > players_info[mapp[nxt_row][nxt_col]].second)
				{
					winner = cur_idx; loser = mapp[nxt_row][nxt_col];
				}
				else
				{
					winner = mapp[nxt_row][nxt_col]; loser = cur_idx;
				}
			}
			else if (full_power1 > full_power2)
			{
				winner = cur_idx; loser = mapp[nxt_row][nxt_col];
			}
			else if (full_power1 < full_power2)
			{
				winner = mapp[nxt_row][nxt_col]; loser = cur_idx;
			}

			// 2-2. winner 점수 획득
			int getPoints = (players_info[winner].second + players_gun[winner]) - (players_info[loser].second + players_gun[loser]);
			points[winner] += getPoints;

			// 2-3. loser 후처리
			//// 본인이 가지고 있는 총을 해당 격자에 내려놓기
			int tmp = players_gun[loser];
			players_gun[loser] = 0;
			guns[nxt_row][nxt_col].push_back(tmp);

			sort(guns[nxt_row][nxt_col].begin(), guns[nxt_row][nxt_col].end());
		
			//// 해당 플레이어가 원래 가지고 있던 방향대로 한 칸 이동
			int loser_nxt_row, loser_nxt_col, loser_nxt_direc;
			for (int j = players_info[loser].first; j < 4; ++j)
			{
				loser_nxt_direc = j % 4;
				loser_nxt_row = players_pos[loser].first + off_row[j];
				loser_nxt_col = players_pos[loser].second + off_col[j];

				if (loser_nxt_row < 1 || loser_nxt_row > N_MAX || loser_nxt_col < 1 || loser_nxt_col > N_MAX) continue;
				if (mapp[loser_nxt_row][loser_nxt_col] > 0) continue;
				
				if (players_gun[loser] < guns[loser_nxt_row][loser_nxt_col].back())
				{
					int tmp = players_gun[loser];
					players_gun[loser] = guns[loser_nxt_row][loser_nxt_col].back();
					guns[loser_nxt_row][loser_nxt_col].back() = tmp;

					sort(guns[loser_nxt_row][loser_nxt_col].begin(), guns[loser_nxt_row][loser_nxt_col].end());
					
					//break;
				}
				break;
				
			}

			// 2-4. winner 후처리
			//// 승리한 칸에 떨어져 있는 총들과 원래 들고 있던 총 중 가장 공격력이 높은 총을 획득하고, 나머지 총들은 해당 격자에 내려 놓는다
			if (players_gun[winner] < guns[nxt_row][nxt_col].back())
			{
				int tmp = players_gun[winner];
				players_gun[winner] = guns[nxt_row][nxt_col].back();
				guns[nxt_row][nxt_col].back() = tmp;

				sort(guns[nxt_row][nxt_col].begin(), guns[nxt_row][nxt_col].end());
			}

			//2-6. loser 관련 mapp, players_pos, players_info 수정
			//// 직전 위치 초기화 
			mapp[nxt_row][nxt_col] = 0;
			mapp[loser_nxt_row][loser_nxt_col] = loser;

			players_pos[loser].first = loser_nxt_row;
			players_pos[loser].second = loser_nxt_col;
			players_info[loser].first = loser_nxt_direc;


			//2-7. winner 관련 mapp, players_pos, players_info 수정
			mapp[nxt_row][nxt_col] = winner;
			
			players_pos[winner].first = nxt_row;
			players_pos[winner].second = nxt_col;
			players_info[winner].first = nxt_direction;

			
		}

		//플레이어가 없는 경우
		else                                    
		{
			//총이 있는 경우
			if (!guns[nxt_row][nxt_col].empty())		
			{
				// case1. 플레이어에게 총이 없다 -> 획득
				if (players_gun[cur_idx] < 1)
				{
					players_gun[cur_idx] += guns[nxt_row][nxt_col].back();
					guns[nxt_row][nxt_col].pop_back();
				}

				// case2. 플레이어에게 총이 있다 -> 놓여있는 총들과 플레이어가 가지고 있는 총 가운데 공격력이 더 쎈 총을 획득
				else
				{
					if (players_gun[cur_idx] < guns[nxt_row][nxt_col].back())
					{
						int tmp = players_gun[cur_idx];
						players_gun[cur_idx] = guns[nxt_row][nxt_col].back();
						guns[nxt_row][nxt_col].back() = tmp;

						sort(guns[nxt_row][nxt_col].begin(), guns[nxt_row][nxt_col].end());
					}
				
				}
				//mapp, players_pos, players_info 수정
			}

			//총이 없는 경우
			else                                    
			{
				;//mapp, players_pos, players_info 수정
			}

			// mapp, players_pos, players_info 수정
			players_pos[cur_idx].first = nxt_row;
			players_pos[cur_idx].second = nxt_col;
			players_info[cur_idx].first = nxt_direction;

			mapp[nxt_row][nxt_col] = cur_idx;
		}

	}
	
	return;
}