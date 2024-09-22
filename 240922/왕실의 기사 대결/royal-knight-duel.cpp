#include<iostream>
#include<utility>
#include<queue>
using namespace std;


#define L_MAX 40+1			//체스판의 크기 max
#define N_MAX 30+1			// 기사의 수 max
#define Q_MAX 100+1			// 왕 명령의 수 max


int L;
int N;
int Q;



int mapp[L_MAX + 1][L_MAX + 1];					// 1-idxed, (빈칸 : 0, 함정 : 1, 벽 : 2)
int knights_pos_mapp[L_MAX + 1][L_MAX + 1];		// 기사의 위치를 저장 (1~30 : 기사, 나머지 : 빈칸 || 함정 || 벽)

pair<int, int> knights_Pos[N_MAX + 1];			// idx번 기사의 좌상단 좌표 r, c
pair<int, int> knights_wid_hei[N_MAX + 1];		// idx번 기사의 h, w
int power[N_MAX + 1];							// idx번 기사의 체력 k

pair<int, int> king_order[Q_MAX + 1];			// 왕의 idx번째 명령 i(기사 번호), d(이동 방향)

bool is_alive[N_MAX];							//idx번째 기사 살아있는지 저장
int ans_sum_of_damage[N_MAX];						//idx번째 기사가 받은 데미지 저장

int off_row[4] = { -1, 0, 1, 0 };
int off_col[4] = { 0, 1, 0, -1 };

void move_knights(const int target_knights_idx, const int move_direction);


int main()
{
	//1. input
	//// 1st: L(체스판 크기) N(기사 명수) Q(왕 명령 횟수)
	cin >> L >> N >> Q;

	//// 2nd : L 개의 줄에 걸쳐서 L×L 크기의 체스판에 대한 정보
	for (int i = 1; i <= L; ++i)
	{
		for (int j = 1; j <= L; ++j)
		{
			cin >> mapp[i][j];

		}
	}

	//// 3rd : N 개의 줄에 걸쳐서 초기 기사들의 정보가 주어짐(r, c, h, w, k)
	////// +) 기사 위치 표시 
	for (int i = 1; i <= N; ++i)
	{
		cin >> knights_Pos[i].first >> knights_Pos[i].second >> knights_wid_hei[i].first >> knights_wid_hei[i].second >> power[i];
		is_alive[i] = true;

		for (int j = knights_Pos[i].first; j <= knights_Pos[i].first + knights_wid_hei[i].first - 1; ++j)
		{
			for (int k = knights_Pos[i].second; k <= knights_Pos[i].second + knights_wid_hei[i].second - 1; ++k)
			{
				knights_pos_mapp[j][k] = i;
			}
		}
	}


	//// 4th Q 개의 줄에 걸쳐 왕의 명령에 대한 정보가 주어짐(i, d)
	for (int i = 1; i <= Q; ++i)
	{
		cin >> king_order[i].first >> king_order[i].second;
	}

	//2. Q번 명령 수행
	for (int i = 1; i <= Q; ++i)
	{
		//// 기사 이동 + 대결 대미지
		if (!is_alive[king_order[i].first]) continue;
		move_knights(king_order[i].first, king_order[i].second);
	}

	//3. 생존한 기사들이 총 받은 대미지의 합을 출력
	int ans = 0;
	for (int i = 1; i <= N; ++i)
	{
		if (is_alive[i])
		{
			ans += ans_sum_of_damage[i];
		}
	}
	cout << ans;
}




void move_knights(const int target_knights_idx, const int move_direction)
{
	//0. 셋팅
	int tmp_knights_pos_mapp[L_MAX + 1][L_MAX + 1];
	for (int i = 1; i <= L; ++i)
	{
		for (int j = 1; j <= L; ++j)
		{
			tmp_knights_pos_mapp[i][j] = knights_pos_mapp[i][j];

		}
	}

	queue<pair<int, int>> q_cur_info;
	q_cur_info.push({ target_knights_idx, move_direction });

	//1.
	while (!q_cur_info.empty())
	{
		int cur_target_knights_idx = q_cur_info.front().first;
		int cur_move_direction = q_cur_info.front().second;
		q_cur_info.pop();

		int cur_ltop_row = knights_Pos[cur_target_knights_idx].first;	//이동 대상 기사의 좌상단 row 좌표
		int cur_ltop_col = knights_Pos[cur_target_knights_idx].second;	//이동 대상 기사의 좌상단 col 좌표

		int cur_height = knights_wid_hei[cur_target_knights_idx].first;	//이동 대상 기사의 h
		int cur_width = knights_wid_hei[cur_target_knights_idx].second;	//이동 대상 기사의 w

		int cur_rBottom_row = cur_ltop_row + cur_height;		// 이동 대상 기사의 우하단 row 좌표
		int cur_rBottom_col = cur_ltop_col + cur_width;			// 이동 대상 기사의 우하단 row 좌표



		//1-1. 이동하려는 방향의 끝에 벽이 있는지 확인
		//// 체스판 밖도 벽으로 간주 -> 이동하려는 방향의 끝 마지막 mapp[][]이 반드시 빈 칸 혹은 함정 이어야 한다??
		/*
		int end_row, end_col;						// 이동할 방향 끝의 좌상단 좌표
		end_row = cur_ltop_row + (off_row[cur_move_direction] * (L - cur_ltop_row - 1));
		end_col = cur_ltop_col + (off_col[cur_move_direction] * (L - cur_ltop_col - 1));

		//if (mapp[end_row][end_col] == 2) return;
		if (mapp[end_row][end_col] == 2 || knights_pos_mapp[end_row][end_col] > 0) continue;
		*/
		//상 : row 1 고정, 하 : row L 고정, 좌 : col 1 고정, 우 : col L 고정
		int fixed_rc[4] = { 1,L,L,1 };
		if (cur_move_direction == 0 || cur_move_direction == 2)		//row 고정
		{
			for (int i = 1; i <= L; ++i)
			{
				if (mapp[fixed_rc[cur_move_direction]][i] == 2 || knights_pos_mapp[fixed_rc[cur_move_direction]][i] > 0) return; //continue;
			}
		}
		else if (cur_move_direction == 1 || cur_move_direction == 3)	//col 고정
		{
			for (int i = 1; i <= L; ++i)
			{
				if (mapp[i][fixed_rc[cur_move_direction]] == 2 || knights_pos_mapp[i][fixed_rc[cur_move_direction]] > 0) return; //continue;
			}
		}


		//1-2. 범위 초과 여부 확인(mapp 밖도 벽이다)
		//// 좌상단 row, col - 최소 row, col과 비교, 우하단 row, col - 최대 row, col과 비교
		int nxt_ltop_row, nxt_ltop_col;				// 이동할 위치의 좌상단 좌표
		int nxt_rBottom_row, nxt_rBottom_col;		// 이동할 위치의 우하단 좌표

		nxt_ltop_row = cur_ltop_row + off_row[cur_move_direction];
		nxt_ltop_col = cur_ltop_col + off_col[cur_move_direction];

		nxt_rBottom_row = nxt_ltop_row + cur_height;
		nxt_rBottom_col = nxt_ltop_col + cur_width;

		if (nxt_ltop_row < 1 || nxt_rBottom_row > L + 1 || nxt_ltop_col < 1 || nxt_rBottom_col > L + 1) continue;

		//1-3. 이동하려는 위치에 다른 기사가 있는지 확인
		for (int i = nxt_ltop_row; i <= nxt_rBottom_row - 1; ++i)
		{
			for (int j = nxt_ltop_col; j <= nxt_rBottom_col - 1; ++j)
			{
				if (knights_pos_mapp[i][j] > 0 && knights_pos_mapp[i][j] != cur_target_knights_idx)
				{
					q_cur_info.push({ knights_pos_mapp[i][j], move_direction });
				}
			}
		}

		//1-4. 현재 이동한 기사의 이동을 tmp_knights_pos_mapp에 반영

		//// (tmp_knights_pos_mapp에)이동한 기사의 이전 위치 초기화
		for (int i = cur_ltop_row; i <= cur_rBottom_row - 1; ++i)
		{
			for (int j = cur_ltop_col; j <= cur_rBottom_col - 1; ++j)
			{
				tmp_knights_pos_mapp[i][j] = 0;
			}
		}

		//// (tmp_knights_pos_mapp에)기사의 새로운 위치 표시 
		for (int i = nxt_ltop_row; i <= nxt_rBottom_row - 1; ++i)
		{
			for (int j = nxt_ltop_col; j <= nxt_rBottom_col - 1; ++j)
			{
				tmp_knights_pos_mapp[i][j] = cur_target_knights_idx;
			}
		}

		//1-5. 명령을 받은 기사 제외, 체력이 깎는다
		int getDamage = 0;
		if (target_knights_idx != cur_target_knights_idx)
		{
			for (int i = nxt_ltop_row; i <= nxt_rBottom_row - 1; ++i)
			{
				for (int j = nxt_ltop_col; j <= nxt_rBottom_col - 1; ++j)
				{
					if (mapp[i][j] == 1)
					{
						++getDamage;
					}
				}
			}
			ans_sum_of_damage[cur_target_knights_idx] += getDamage;
			power[cur_target_knights_idx] -= getDamage;
			if (power[cur_target_knights_idx] <= 0) is_alive[cur_target_knights_idx] = false;
		}
	}

	//2. 후처리
	//// tmp_knights_pos_mapp -> knights_pos_mapp
	for (int i = 1; i <= L; ++i)
	{
		for (int j = 1; j <= L; ++j)
		{
			knights_pos_mapp[i][j] = tmp_knights_pos_mapp[i][j];

		}
	}
}