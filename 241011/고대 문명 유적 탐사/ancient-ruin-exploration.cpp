//1.  "~ 같다면 -> ~ " 제한조건 처리방식 : 3중 for
//// => 

#include<queue>
#include<iostream>
#include<utility>
#include<set>
using namespace std;

#define MAPP_SIZE (5+1)

int mapp[MAPP_SIZE][MAPP_SIZE];

int K, M;						//
queue<int> wall_info;
//int getPoint;					// 각 턴마다 출력, 갱신

void rotate(const int sr, const int sc, const int len, const int num_fo_rotate, int(&tmp)[MAPP_SIZE][MAPP_SIZE]);
const int BFS(int(&tmp)[MAPP_SIZE][MAPP_SIZE], const int flag);			// 유물 획득
void mapp_update(int(&tmp)[MAPP_SIZE][MAPP_SIZE]);		// 유물 획득한 자리 업데이트
// copy -> copied
void mapp_copy(int(&copy)[MAPP_SIZE][MAPP_SIZE], int(&copied)[MAPP_SIZE][MAPP_SIZE]);

// 상 우 하 좌
int off_r[4] = { -1, 0, 1, 0 };
int off_c[4] = { 0, 1, 0, -1 };


int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. 
	cin >> K >> M;
	for (int i = 1; i <= 5; ++i)
	{
		for (int j = 1; j <= 5; ++j)
		{
			cin >> mapp[i][j];
		}
	}
	int in_info;
	for (int i = 1; i <= M; ++i)
	{
		cin >> in_info;
		wall_info.push(in_info);
	}


	// 2. 게임 수행
	//int max_score;
	for (int i = 1; i <= K; ++i)
	{
		//[1] 탐사 진행 (1) '3×3 격자 선택 + 회전' case 중, 최적 회전 수행 (2) 최적 유물 획득 
		int tmp_mapp_for_select[MAPP_SIZE][MAPP_SIZE];
		int max_score = 0;
		////1. 5×5 격자 내에서 '3×3 격자 선택 + 회전' -> 3가지 제한조건 고려한 최적의 회전 수행
		for (int rotate_cnt = 1; rotate_cnt <= 3; ++rotate_cnt)
		{
			//// 3*3 정사각형 좌상단 col 작은 순
			for (int lc = 1; lc <= 3; ++lc)
			{
				//// 3*3 정사각형 좌상단 row 큰 순
				for (int lr = 1; lr <= 3; ++lr)
				{
					////0. mapp -> tmp
					int tmp[MAPP_SIZE][MAPP_SIZE];
					for (int j = 1; j <= 5; ++j)
					{
						for (int k = 1; k <= 5; ++k)
						{
							tmp[j][k] = mapp[j][k];
						}
					}
					
					////2. rotate
					//rotate(lr, lc, 3, i, tmp);
					rotate(lr, lc, 3, rotate_cnt, tmp);

					////3. 초기 유물 획득 
					int score_per_case = BFS(tmp, 0);

					////5. max인지 확인하기 -> 최적의 회전결과, 획득 유물 수 저장
					if (score_per_case > max_score)
					{
						max_score = score_per_case;
						mapp_copy(tmp, tmp_mapp_for_select);
					}

				}
			}

		}

		// 탐사 진행 과정에서 어떠한 방법을 사용하더라도 유물을 획득할 수 없었다면 모든 탐사는 그 즉시 종료 
		if (max_score == 0) return 0;

		// 

		//[2] 유물 획득
		////0. tmp_mapp_for_select -> mapp
		for (int j = 1; j <= 5; ++j)
		{
			for (int k = 1; k <= 5; ++k)
			{
				mapp[j][k] = tmp_mapp_for_select[j][k];
			}
		}

		//// 1. 유물 획득 구현 : while loop 내에서 연쇄획득구현
		int tmp_ans = 0;
		int ans = 0;
		while (1)
		{
			//// 획득
			tmp_ans = BFS(mapp, 1);
		
			//// 업데이트
			mapp_update(mapp);

			//// 연쇄 획득
			if (tmp_ans == 0) break;
			else ans += tmp_ans;
		}


		/*
		//// 1. 1차 획득
		getPoint += BFS(mapp, 1);
		
		//// 2. 연쇄획득
		////// 1. 업데이트
		mapp_update(mapp);

		////// 2. 추가 획득
		while (1)
		{
			int again_get = 0;

			again_get = BFS(mapp, 1);
			mapp_update(mapp);

			if (again_get == 0) { break; }
			else { max_score += again_get; }
			
		}
		*/
		////3. max_score 출력
		//cout << max_score << ' ';
		cout << ans << ' ';
	}
	return 0;

}


// copy -> copied
void mapp_copy(int(&copy)[MAPP_SIZE][MAPP_SIZE], int(&copied)[MAPP_SIZE][MAPP_SIZE])
{
	for (int i = 1; i <= 5; ++i)
	{
		for (int j = 1; j <= 5; ++j)
		{
			copied[i][j] = copy[i][j];
		}
	}
}


void mapp_update(int(&tmp)[MAPP_SIZE][MAPP_SIZE])
{
	//1. 
	for (int tc = 1; tc <= 5; ++tc)
	{
		for (int tr = 5; tr >= 1; --tr)
		{
			if (tmp[tr][tc] == -1)
			{
				tmp[tr][tc] = wall_info.front();
				wall_info.pop();
			}
		}
	}
}


const int BFS(int(&tmp)[MAPP_SIZE][MAPP_SIZE], const int flag)
{
	//0. 
	int score_this = 0;								// 이번 BFS로 얻은 유물 합
	bool v[MAPP_SIZE][MAPP_SIZE] = { false, };
	//queue<pair<int, int>> qset;					//네트워크 마다의 좌표정보 저장 => 네트워크 종료 전에 (0) mapp에 반영(1)score_this 에 누적 (2) 초기화 
	set<pair<int, int>> qset;						//네트워크 마다의 좌표정보 저장 => 네트워크 종료 전에 (1)score_this 에 누적 (2) 초기화 
	queue<pair<int, int>> q_nodes;

	//1. 시작 좌표 설정
	for (int i = 1; i <= 5; ++i)
	{
		for (int j = 1; j <= 5; ++j)
		{
			if (v[i][j]) continue;

			//초기 설정
			v[i][j] = true;
			q_nodes.push({ i, j });
			qset.insert({ i,j });

			pair<int, int>c_node;
			while (!q_nodes.empty())
			{
				//1. 
				c_node = q_nodes.front();
				q_nodes.pop();

				for (int k = 0; k < 4; ++k)
				{
					int n_r = c_node.first + off_r[k];
					int n_c = c_node.second + off_c[k];

					////제한
					if ((n_r < 1 || n_r > 5 || n_c < 1 || n_c > 5) || v[n_r][n_c]) continue;
					if (tmp[n_r][n_c] != tmp[c_node.first][c_node.second]) continue;
					//if (tmp[i][j] != tmp[c_node.first][c_node.second]) continue;
					//if (tmp[i][j] != tmp[n_r][n_c]) continue;

					//// 네트워크 만드는 처리
					v[n_r][n_c] = true;
					q_nodes.push({ n_r, n_c });
					qset.insert({ n_r,n_c });
				}

			}

			if (qset.size() >= 3)
			{
				score_this += qset.size();
				if (flag == 1)
				{
					for (auto it : qset)
					{
						tmp[it.first][it.second] = -1;
					}
				}
			}
			qset.clear();
		}
		/*
		if (qset.size() >= 3)
		{
			score_this += qset.size();
			for (auto it : qset)
			{
				tmp[it.first][it.second] = -1;
			}
		}
		qset.clear();
		*/
	}

	return score_this;

}


void rotate(const int sr, const int sc, const int len, const int num_fo_rotate, int(&tmp)[MAPP_SIZE][MAPP_SIZE])
{
	/*
	//1. 
	for (int i = 1; i <= 5; ++i)
	{
		for (int j = 1; j <= 5; ++j)
		{
			tmp[i][j] = mapp[i][j];
		}
	}
	*/
	//1. 
	int tmp_tmp_mapp[MAPP_SIZE][MAPP_SIZE];
	for (int i = 0; i <= (len - 1); ++i)
	{
		for (int j = 0; j <= (len - 1); ++j)
		{
			tmp_tmp_mapp[sr+i][sc+j] = tmp[sr+i][sc+j];
		}
	}


	//2. 
	if (num_fo_rotate == 1)
	{
		//2-1. 90 회전
		for (int i = 0; i <= (len - 1); ++i)
		{
			for (int j = 0; j <= (len - 1); ++j)
			{
				//tmp[sr + j][sc + (len - 1) - i] = mapp[sr + i][sc + j];
				tmp_tmp_mapp[sr + j][sc + (len - 1) - i] = tmp[sr + i][sc + j];
			}
		}
	}

	else if (num_fo_rotate == 2)
	{
		//2-3. 180 회전
		for (int i = 0; i <= (len - 1); ++i)
		{
			for (int j = 0; j <= (len - 1); ++j)
			{
				//tmp[sr + (len - 1) - i][sc + (len - 1) - j] = mapp[sr + i][sc + j];
				tmp_tmp_mapp[sr + (len - 1) - i][sc + (len - 1) - j] = tmp[sr + i][sc + j];
			}
		}
	}

	else
	{
		//2-3. 270 회전
		for (int i = 0; i <= (len - 1); ++i)
		{
			for (int j = 0; j <= (len - 1); ++j)
			{
				//tmp[sr + (len - 1) - j][sc + i] = mapp[sr + i][sc + j];
				tmp_tmp_mapp[sr + (len - 1) - j][sc + i] = tmp[sr + i][sc + j];
			}
		}

	}

	
	//3. 반영
	for (int i = 0; i <= (len - 1); ++i)
	{
		for (int j = 0; j <= (len - 1); ++j)
		{
			tmp[sr + i][sc + j] = tmp_tmp_mapp[sr + i][sc + j];
		}
	}
	
}