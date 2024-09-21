#include<iostream>
#include<queue>
#include<utility>
#include<cmath>
using namespace std;

#define MAX_N 10+1				// mapp row, col 최대 크기

int N;		// input 미로 크기
int M;		// input 참가자 수
int K;		// input 게임 시간(1초당 게임 1회)

int exit_row, exit_col;		//출구 좌표

int mapp[MAX_N][MAX_N];					// mapp : 0(빈칸, 이동 가능), -1(사람), 1~9(내구도), -11 : 출구
int sum_of_move;						// 각 참가자들이 이동할때마다 +1
int num_of_alive;						//각 참가자들이 탈출할때마다 -1


// 상 / 하 / 좌 / 우
int off_row[4] = { -1, 1, 0, 0 };
int off_col[4] = { 0, 0, -1, 1 };


void move_runners(void);					// 모든 참가자들이 한 칸씩 ‘동시에’ 이동
void rotate_maze(void);							// 미로 회전
bool is_possible_rotate(const int str_row, const int str_col, const int select_N);	// 회전 가능 확인
void rotate(const int, const int, const int);									// 회전 대상 정사각형을 회전 수행

int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> N >> M >> K;
	num_of_alive = M;
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= N; ++j)
		{
			cin >> mapp[i][j];				//input 미로 정보
		}
	}

	int input_row, input_col;
	for (int i = 0; i < M; ++i)
	{
		cin >> input_row >> input_col;		//input 참가자 좌표
		//mapp[input_row][input_col] = -1;
		mapp[input_row][input_col] += -1;
	}
	cin >> exit_row >> exit_col;			//input 출구 좌표
	mapp[exit_row][exit_col] = -11;

	//2. 게임 수행
	for (int i = 1; i <= K; ++i)
	{
		//2-1. 참가자들의 이동
		move_runners();
		if (num_of_alive == 0) break;

		//2-2. 미로 회전
		rotate_maze();

	}

	//3. 게임 종료 후, 이동 거리 합과 출구 좌표 출력
	cout << sum_of_move << '\n';
	cout << exit_row << ' ' << exit_col << '\n';

	return 0;

}


void move_runners(void)
{
	//0. 셋팅
	//queue<pair<int, int>> q_nxtPos;			//각 참가자들이 현재 턴(초)에서 이동할 위치 저장
	int tmp_new_mapp[MAX_N][MAX_N] = { 0, };
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= N; ++j)
		{
			tmp_new_mapp[i][j] = mapp[i][j];
		}
	}

	// 1. 사람 탐색
	// pair<int, int> cur_pos;
	for (int i = 1; i <= N; ++i)			//row
	{
		for (int j = 1; j <= N; ++j)		//col
		{
			if (mapp[i][j] <= -1 && mapp[i][j] > -11)		//mapp에 사람 있는 칸
			{
				int cur_min_dist = abs(i - exit_row) + abs(j - exit_col);

				// 2. 이동할 위치 탐색
				int tar_nxt_row, tar_nxt_col;
				bool is_moved = false;
				for (int k = 0; k < 4; ++k)
				{
					int nxt_row = i + off_row[k];
					int nxt_col = j + off_col[k];

					// 제한0
					if (nxt_row < 1 || nxt_row > N || nxt_col < 1 || nxt_col > N) continue;
					// 제한1
					if (mapp[nxt_row][nxt_col] >= 1) continue;
					// 제한2
					int tmp_dist = abs(nxt_row - exit_row) + abs(nxt_col - exit_col);
					//if (cur_min_dist < tmp_dist) continue;
					if (cur_min_dist <= tmp_dist) continue;

					cur_min_dist = tmp_dist;
					is_moved = true;
					tar_nxt_row = nxt_row;
					tar_nxt_col = nxt_col;
				}

				// 3. '동시에' 이동 반영
				//// => 출구인 경우, 아닌경우 모두 처리해야함 -> 공통되는 처리 먼저 수행
				if (!is_moved) continue;				// 움직이지 않는 경우

				//sum_of_move += abs(mapp[nxt_row][nxt_col]);
				sum_of_move += abs(mapp[i][j]);
				tmp_new_mapp[i][j] -= mapp[i][j];		// 이전 좌표 이동처리

				//if (mapp[nxt_row][nxt_col] == -11)		// 출구인경우
				if (mapp[tar_nxt_row][tar_nxt_col] == -11)
				{
					num_of_alive += mapp[i][j];
				}
				else									// 빈칸 혹은 사람 있는 곳일 경우
				{
					//tmp_new_mapp[nxt_row][nxt_col] += mapp[i][j];
					tmp_new_mapp[tar_nxt_row][tar_nxt_col] += mapp[i][j];
				}

			}

		}

	}

	//4. tmp_new_mapp -> mapp
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= N; ++j)
		{
			mapp[i][j] = tmp_new_mapp[i][j];
		}
	}

	return;
}


void rotate_maze(void)
{
	//1. 회전시킬 정사각형 잡기
	for (int i = 2; i <= N; ++i)		// select_N
	{
		for (int j = 1; j <= ((N - i) + 1); ++j)		// str_row 	
		{
			for (int k = 1; k <= ((N - i) + 1); ++k)	// str_col
			{
				//int end_row = (j + i) - 1;
				//int end_col = (k + i) - 1;

				// 2. 회전 수행
				if (is_possible_rotate(j, k, i))
				{
					// 회전 수행 : mapp[][], visit[][] 업데이트 -> 출구 위치 변수, q_runnersPos 업데이트
					rotate(j, k, i);

					return;
				}
			}
		}

	}

	return;
}


bool is_possible_rotate(const int str_row, const int str_col, const int select_N)
{
	int end_row = (str_row + select_N) - 1;
	int end_col = (str_col + select_N) - 1;

	//1. 출구 포함 확인
	if (exit_row < str_row || exit_row > end_row || exit_col < str_col || exit_col > end_col) return false;

	//2. 한 명 이상의 참가자 포함 확인
	for (int i = str_row; i <= end_row; ++i)
	{
		for (int j = str_col; j <= end_col; ++j)
		{
			//if (visit[i][j] >= 1) return true;
			if (mapp[i][j] > -11 && mapp[i][j] <= -1) return true;
		}
	}

	return false;
}

void rotate(const int str_row, const int str_col, const int select_N)
{
	//0. 셋팅
	int tmp_new_mapp[MAX_N][MAX_N] = { 0, };
	for (int i = 1; i <= N; ++i)
	{
		for (int j = 1; j <= N; ++j)
		{
			tmp_new_mapp[i][j] = mapp[i][j];
		}
	}


	for (int i = 0; i < select_N; ++i)
	{
		for (int j = 0; j < select_N; ++j)
		{
			tmp_new_mapp[str_row + i][str_col + j] = mapp[str_row + select_N - j - 1][str_col + i];			//1. 90도 회전 수행 
			//if(tmp_new_mapp[str_row + i][str_col + j] >= 1) tmp_new_mapp[i][j] -= 1;					//2. 회전된 벽 내구도 1 감소 
			if (tmp_new_mapp[str_row + i][str_col + j] >= 1) tmp_new_mapp[str_row + i][str_col + j] -= 1;
			else if (tmp_new_mapp[str_row + i][str_col + j] == -11)										//3. 출구 위치 변경 반영	
			{
				exit_row = str_row + i;
				exit_col = str_col + j;
			}
		}
	}

	//4. 회전 반영
	for (int i = str_row; i <= (str_row + select_N - 1); ++i)
	{
		for (int j = str_col; j <= (str_col + select_N - 1); ++j)
		{
			mapp[i][j] = tmp_new_mapp[i][j];
		}
	}

	return;
}