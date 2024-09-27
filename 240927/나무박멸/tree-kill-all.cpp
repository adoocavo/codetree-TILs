#include<iostream>
#include<utility>
#include<queue>
#include<vector>
using namespace std;


#define MAX_N 20+1
#define MAX_M 1000+1

int n;		//격자 크기
int m;		//게임 진행 년수
int k;		//제초제 확산 범위(대각선 4방향으로)
int c;		//제초제가 지속되는 년수

int mapp[MAX_N][MAX_N]; 		// 1-100 : 나무 그루 수, -1 : 벽, 0: 빈칸
int killing[MAX_N][MAX_N]; 		// 제초제 뿌려진 위치에 제초제가 없어지는 년도 저장(해당 위치 나무는 성장, 번식 x)
int sum_of_killed;				// 게임진행으로 박멸한 총 나무 그루 수



// 대각선 이동 offset
int off_diagonal_row[4] = { -1,-1,1,1 };
int off_diagonal_col[4] = { -1, 1, 1, -1 };

// 상 -> 우 -> 하 -> 좌
int off_row[4] = { -1,0,1,0 };
int off_col[4] = { 0,1,0,-1 };


void grow_tree(void);			// 나무 성장 (1,600,000)
void breeding_tree(void);		// 나무 번식 (1,600,000) 
void find_kill_tar(void);		// 제초제 뿌릴 위치 정하고 killing 수정


int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1.input
	cin >> n >> m >> k >> c;
	c = -1 * (c + 1);							// c + 1년째가 될 때 사라짐 반영
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> mapp[i][j];
			if (mapp[i][j] == -1)				// 벽 -> 나무 성장, 번식 불가 => 벽이 있는 격자는 게임이 끝날때까지 제초제가 있다고 설정해놓기
			{
				//mapp[i][j] *= MAX_M;
				killing[i][j] *= MAX_M;
			}
		}

	}


	//2. 나무 박멸 진행  
	for (int i = 1; i <= m; ++i)
	{
		////0. 제초제 수명 업데이트
		for (int j = 1; j <= n; ++j)
		{
			for (int k = 1; k <= n; ++k)
			{
				//if (killing[j][k] < 0 || mapp[j][k] != -1)
				if (killing[j][k] < 0)
				{
					++killing[j][k];
				}
			}

		}

		////1. 나무 성장
		grow_tree();

		////2. 나무 번식 
		breeding_tree();


		////3. 나무가 가장 많이 박멸되는 칸을 찾고, 제초제를 뿌린다.
		find_kill_tar();
	}


	//3. 정답 출력
	cout << sum_of_killed;


	return 0;


}




void grow_tree(void)
{
	//0. 동시에 -> copy 사용
	int tmp_mapp[MAX_N][MAX_N];
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			tmp_mapp[i][j] = mapp[i][j];
		}
	}


	//1. 성장할 대상 나무 찾기
	//// 제한 1. mapp[][] 범위 확인  -> 성장 크기 찾기위함
	//// 제한 2. mapp[][] == -1 -> 벽 
	//// 제한 3. killing[][] > 0  -> 성장 불가
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			if (mapp[i][j] > 0)						//성장할 대상 나무
			{
				//if (killing[i][j] > 0) continue;
				if (killing[i][j] < 0) continue;

				int cur_row = i;
				int cur_col = j;

				for (int k = 0; k < 4; ++k)			//2. 성장 크기 찾기
				{
					int nxt_row = cur_row + off_row[k];
					int nxt_col = cur_col + off_col[k];

					if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n) continue;
					//if (mapp[nxt_row][nxt_col] == -1) continue;
					//if (mapp[nxt_row][nxt_col] < 0) continue;						
					if (mapp[nxt_row][nxt_col] <= 0) continue;						// 벽 || 빈칸 제외
					//if (killing[nxt_row][nxt_col] > 0) continue;
					if (killing[nxt_row][nxt_col] < 0) continue;					// 제초제 뿌려진곳 제외
					

					//tmp_mapp[cur_row][cur_col] += mapp[nxt_row][nxt_col];		
					tmp_mapp[cur_row][cur_col] += 1;		  //3. 나무 성장
				}
			}
		}
	}


	//4. tmp_mapp -> mapp
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			mapp[i][j] = tmp_mapp[i][j];
		}
	}

	return;

}



void breeding_tree(void)
{
	//0. 동시에 -> copy 사용
	int tmp_mapp[MAX_N][MAX_N];
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			tmp_mapp[i][j] = mapp[i][j];
		}
	}
	//1. 번식할 나무 찾기
	//// 번식 대상 칸 제한 
	//// 제한 1. mapp[][] 범위 확인 
	//// 제한 2. mapp[][] == -1 -> 벽
	//// 제한 3. mapp[][] > 0 -> 해당 칸에 번식 불가 
	//// 제한 3. killing[][] > 0  -> 해당 칸에 번식 불가 
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			if (mapp[i][j] > 0)						//번식할 대상 나무
			{
				//if (killing[i][j] > 0) continue;
				if (killing[i][j] < 0) continue;

				int cur_row = i;
				int cur_col = j;
				int num_of_breedable = 0;
				//queue<pair<int, int>> breed_pos;
				vector<pair<int, int>>breed_pos;


				for (int k = 0; k < 4; ++k)			//2. 번식 가능 칸 찾기
				{
					int nxt_row = cur_row + off_row[k];
					int nxt_col = cur_col + off_col[k];

					if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n) continue;
					if (mapp[nxt_row][nxt_col] == -1) continue;
					if (mapp[nxt_row][nxt_col] > 0) continue;
					//if (killing[nxt_row][nxt_col] > 0) continue;
					if (killing[nxt_row][nxt_col] < 0) continue;

					++num_of_breedable;
					breed_pos.push_back({ nxt_row , nxt_col });
				}

				// 3. 모든 나무 동시에 번식
				if (num_of_breedable > 0)
				{
					int breed_plus = mapp[cur_row][cur_col] / num_of_breedable;
					for (int k = 0; k < breed_pos.size(); ++k)
					{
						tmp_mapp[breed_pos.at(k).first][breed_pos.at(k).second] += breed_plus;
					}

				}
			}
		}
	}


	//4. tmp_mapp -> mapp
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			mapp[i][j] = tmp_mapp[i][j];
		}
	}

	return;

}



void find_kill_tar(void)
{
	int max_num_of_kill = 0;
	int max_row;
	int max_col;
	//int tmp_max_num_of_kill;

	//1. 제초제 뿌릴 타겟 후보 찾기
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			//if (mapp[i][j] > 0)											////1. 제초제 뿌릴 후보 지정
			if (mapp[i][j] > 0 && killing[i][j] >= 0)						////1. 제초제 뿌릴 후보 지정
			{

				int str_row = i;
				int str_col = j;

				int new_k = k;
				//int tmp_max_num_of_kill = 0;
				int tmp_max_num_of_kill = mapp[i][j];

				//for (int new_k = 1; new_k <= k; ++new_k)					////2. k번만큼 대각선 4 방향으로 뻗어가면서 박멸 개수 구하기
				for (int l = 0; l < 4; ++l)						// 방향
				{
					//for (int l = 0; l < 4; ++l)
					for (int new_k = 1; new_k <= k; ++new_k)	// k만큼 특정 대각선 방향 이동
					{
						int nxt_row = str_row + (new_k * off_diagonal_row[l]);
						int nxt_col = str_col + (new_k * off_diagonal_col[l]);

						//제한조건들
						if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n) continue;
						//// 벽이 있거나 나무가 아얘 없는 칸이 있는 경우, 이후의 칸으로는 제초제가 전파되지 않음
						if (mapp[nxt_row][nxt_col] == -1 || mapp[nxt_row][nxt_col] == 0)
						{
							//if(mapp[nxt_row][nxt_col] == 0)	killing[nxt_row][nxt_col] += c;
							break;			//해당 방향 종료
						}

						tmp_max_num_of_kill += mapp[nxt_row][nxt_col];
					}
				}

				////3. 제초제 뿌릴 타겟 수정
				if (tmp_max_num_of_kill > max_num_of_kill)
				{
					max_row = str_row;
					max_col = str_col;
					max_num_of_kill = tmp_max_num_of_kill;
				}

			}
		}
	}

	if (max_num_of_kill == 0)		//격자 내에 나무가 없음
	{
		return;
	}


	//2. 제초제 살포 처리
	int str_row = max_row;
	int str_col = max_col;

	killing[str_row][str_col] += c;

	for (int i = 0; i < 4; ++i)						// 방향
	{
		for (int j = 1; j <= k; ++j)	// k만큼 특정 대각선 방향 이동
		{
			int nxt_row = str_row + (j * off_diagonal_row[i]);
			int nxt_col = str_col + (j * off_diagonal_col[i]);

			//제한조건들
			if (nxt_row < 1 || nxt_row > n || nxt_col < 1 || nxt_col > n) continue;

			//// 벽이 있거나 나무가 아예 없는 칸이 있는 경우, 이후의 칸으로는 제초제가 전파되지 않음
			if (mapp[nxt_row][nxt_col] == -1 || mapp[nxt_row][nxt_col] == 0)
			{
				if (mapp[nxt_row][nxt_col] == 0)	killing[nxt_row][nxt_col] += c;
				break;			//해당 방향 종료
			}
			killing[nxt_row][nxt_col] += c;
		}
	}

	//3. 박멸한 나무의 그루 수 수정
	sum_of_killed += max_num_of_kill;
}