#include<iostream>
#include<map>
#include<vector>
#include<queue>
#include<set>
using namespace std;

#define L_MAX (40+1)
#define N_MAX (30+1)		//기사 최대 수

int mapp[L_MAX][L_MAX];		// 빈칸(0), 함정(1), 또는 벽(2)

// 이동방향 :  0, 1, 2, 3 (위쪽, 오른쪽, 아래쪽, 왼쪽)
int off_row[4] = { -1, 0, 1, 0 };
int off_col[4] = { 0, 1, 0, -1 };

// 기사 관련 ds(idx : 기사 번호)
//// => 탈락 -> erase 처리!
map<int, pair<int, int>> knights_pos;			// 기사들 위치 저장
map<int, pair<int, int>> knights_shiled;		// 기사들 방패 크기 저장
vector<int> knights_life(N_MAX);				// 기사들 체력 저장 


// 명령받은 기사 관련 ds
map<int, int> knights_ordered;					// 명령받은 기사 저장(idx : direc)
vector<int> init_life(N_MAX);					// 기사들 초기 체력(for ans)

// 왕 명령 저장
queue<pair<int, int>> orders;					//왕 명령 저장(기사번호, 방향)

//기사 생존여부 관련
bool is_alive[N_MAX];

int L;
int N;
int Q;

// 기사 이동
void BFS_move(const int ordered_idx, const int ordered_direc);


int main()
{
	//1. input
	cin >> L >> N >> Q;

	for (int i = 1; i <= L; ++i)
	{
		for (int j = 1; j <= L; ++j)
		{
			cin >> mapp[i][j];
		}

	}

	int r, c, h, w, k;
	for (int i = 1; i <= N; ++i)
	{
		cin >> r >> c >> h >> w >> k;

		knights_pos[i].first = r;
		knights_pos[i].second = c;

		knights_shiled[i].first = h;
		knights_shiled[i].second = w;

		knights_life.at(i) = k;
		init_life.at(i) = k;

		is_alive[i] = true;
	}

	int input_i, input_d;
	for (int i = 1; i <= Q; ++i)
	{
		cin >> input_i >> input_d;
		orders.push({ input_i , input_d });
	}

	//2. 게임 Q번 수행
	pair<int, int> cur_order;
	while (!orders.empty())
	{
		////1. 현재 명령 처리 대상 정보 저장
		cur_order = orders.front();
		orders.pop();

		int ordered_idx = cur_order.first;
		int ordered_direc = cur_order.second;

		////2. 기사 이동 + 데미지 처리 수행
		if (!is_alive[ordered_idx]) continue;
		BFS_move(ordered_idx, ordered_direc);
	}

	//3. ans 처리
	int ans_sum = 0;
	for (int i = 1; i <= N; ++i)
	{
		if (is_alive[i])
		{
			ans_sum += (init_life.at(i) - knights_life.at(i));
		}
	}

	cout << ans_sum;

	return 0;

}


void BFS_move(const int ordered_idx, const int ordered_direc)
{
	// 주의 : 명령 받은 기사, (연쇄 이동으로 인해)현재 이동 대상 기사는 다를 수 있음!

	//0. 
	queue<int> q_nodes;							// 이동 대상 idx 저장
	set<int> qset;								// 연쇄처리시 중복으로 연쇄처리 여부 확인
	int getDamaged[N_MAX] = { 0, };				// 해당 명령에서, 이동/연쇄이동으로 인한 기사들이 받는 데미지 저장
	vector<pair<int, int>> nxt_poses(N_MAX);	// 해당 명령에서, 이동/연쇄이동으로 인해 변경되는 기사 위치 저장

	q_nodes.push(ordered_idx);
	qset.insert(ordered_idx);

	//1. 기사 이동처리
	//// => 연쇄이동시 벽 만나면 여기서 그냥 끝난다
	while (!q_nodes.empty())
	{
		int cur_idx = q_nodes.front();		// 현재 이동 대상 idx(명령 받은 기사와 다를 수 있음)
		q_nodes.pop();

		//// 1. 이동할 칸 찾기
		nxt_poses[cur_idx].first = knights_pos[cur_idx].first + off_row[ordered_direc];
		nxt_poses[cur_idx].second = knights_pos[cur_idx].second + off_col[ordered_direc];
		int nxt_row = nxt_poses[cur_idx].first;
		int nxt_col = nxt_poses[cur_idx].second;
		//int nxt_row = knights_pos[cur_idx].first + off_row[ordered_direc];
		//int nxt_col = knights_pos[cur_idx].second + off_col[ordered_direc];

		//// 2. 제한조건 확인
		for (int i = nxt_row; i <= (nxt_row + knights_shiled[cur_idx].first) - 1; ++i)
		{
			for (int j = nxt_col; j <= (nxt_col + knights_shiled[cur_idx].second) - 1; ++j)
			{
				if (i <1 || i > L || j < 1 || j > L) return;
				if (mapp[i][j] == 2) return;
				if (mapp[i][j] == 1) ++getDamaged[cur_idx];
			}
		}

		//// 3. 기사 겹치는지 확인 
		////// target_idx : 겹치는거 확인할 기사의 idx 
		for (auto it : knights_pos)
		{
			// 겹치는거 확인할 기사의 정보 저장
			int target_idx = it.first;
			int t_row = it.second.first;
			int t_col = it.second.second;
			int t_h = knights_shiled[target_idx].first;
			int t_w = knights_shiled[target_idx].second;

			// 이미 이동 대상에 존재
			//if (qset.find(target_idx) == qset.end()) continue;
			if (qset.find(target_idx) != qset.end()) continue;

			// 이미 사망
			if (!is_alive[target_idx]) continue;

			// 겹치지 않는경우 처리
			if ((nxt_row + knights_shiled[cur_idx].first) - 1 < t_row ||
				(t_row + t_h) - 1 < nxt_row) continue;
			if ((nxt_col + knights_shiled[cur_idx].second) - 1 < t_col ||
				(t_col + t_w) - 1 < nxt_col) continue;

			// q_nodes, qset update
			q_nodes.push(target_idx);
			qset.insert(target_idx);
		}

	}

	getDamaged[ordered_idx] = 0;
	//2. 기사의 위치정보 업데이트 -> 기사 데미지 처리()
	//// => qset 기반으로, knights_pos, knights_life, is_alive 업데이트
	for (auto it : qset)
	{
		int tar_update = it;
		knights_pos[tar_update].first = nxt_poses[tar_update].first;
		knights_pos[tar_update].second = nxt_poses[tar_update].second;

		knights_life[tar_update] -= getDamaged[tar_update];

		if (knights_life[tar_update] <= 0) is_alive[tar_update] = false;
	}

	return;

}