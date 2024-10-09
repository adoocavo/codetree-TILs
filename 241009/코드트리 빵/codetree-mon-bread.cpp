#include<iostream>
#include<set>
#include<map>
#include<queue>
#include<deque>
#include<utility>
using namespace std;

#define N_MAX (15+1)


int n, m;


//사람 관련
//// key : 사람idx
map<int, pair<int, int>> man_pos;		//참가자들의 위치 저장
//map<int, bool> is_arrive;				//참가자들의 도착여부 저장
//set<pair<int, int>> is_arrived;		//도착한 참가자들 저장
set<int> is_arrived;					//도착한 참가자들 번호 정보 저장

// 편의점 관련
//// key :  사람idx
map<int, pair<int, int>> store_pos;		//참가자에 해당되는 편의점 위치 저장

// 베이스캠프 관련
//// => 탐색시 mapp ==-1여부 확인
set<pair<int, int>> baseCamps_pos;		// 베이스캠프 위치 저장

// 
int mapp[N_MAX][N_MAX];				//  0: 빈 공간, 1: 베이스캠프, -1 : 이동불가(<-편의점, 베이스캠프)

// 1턴(1분)의 결과로, -1로 변해야 할 위치 저장
//// => 턴 시작마다 초기화
//// => 수정 : 위치가 아닌, -1로 변해야 할 사람의 idx를 저장
//set<pair<int, int>> be_wall;		
//set<int> be_wall;
set<int> have_to_erase_idx;				// 편의점에 도착한 사람 번호 저장 -> man_pos, store_pos erase
set<pair<int, int>> be_wall;			// 편의점/base 위치 중, 벽이 되어야 하는 위치 저장 ->  mapp = -1, baseCamps_pos.erase

// 격자에 있는 사람들 모두가 본인이 가고 싶은 편의점 방향을 향해서 1 칸 움직
void man_move();
// t번 사람을 베이스캠프로 이동
void move_to_base(const int tarMan_idx);
// 격자 내 모든 사람 1칸 이동
void moveAll(const int tarPos_idx);


// 상,좌,우,하 
int off_r[4] = { -1, 0, 0, 1 };
int off_c[4] = { 0, -1, 1, 0 };

//정답 출력 대상
int ans;

int main()
{
	//=========================================
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n >> m;

	int in_info;
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> in_info;
			mapp[i][j] = in_info;
			if (in_info == 1)
			{
				baseCamps_pos.insert({ i,j });
			}
		}
	}

	int in_r, in_c;
	for (int i = 1; i <= m; ++i)
	{
		cin >> in_r >> in_c;

		store_pos[i] = { in_r, in_c };
		//is_arrive[i] = false;
	}
	//=========================================

	//2. 게임 수행 
	int t = 1;
	while (1)
	{
		//// 0. 초기화
		be_wall.clear();
		have_to_erase_idx.clear();

		//// 1-1. 격자에 있는 사람들 이동
		////// => 격자에 사람이 존재해야 수행
		////// => (is_arrived) ,man_pos, be_wall 업데이트
		if (man_pos.size() > 0)
		{
			for (auto it_move : man_pos)
			{
				moveAll(it_move.first);
			}
		}

		//// 1-2. 격자 사람들 이동 후, be_wall 기반으로 업데이트 수행
		////// => be_wall,  -> 'mapp = -1'
		////// => have_to_erase_idx -> store_pos.erase, man_pos.erase 업데이트
		for (auto tar_it : be_wall)
		{
			int tr = tar_it.first;
			int tc = tar_it.second;
			mapp[tr][tc] = -1;

		}
		for (auto tar_it : have_to_erase_idx)
		{
			int tr = man_pos[tar_it].first;
			int tc = man_pos[tar_it].second;

			store_pos.erase(tar_it);
			man_pos.erase(tar_it);

		}
		be_wall.clear();
		//have_to_erase_idx.clear();

		//// 2. t번 사람을 베이스캠프로 이동
		////// => t ≤ m 확인
		////// => man_pos, be_wall 업데이트
		if (t <= m)
		{
			move_to_base(t);
		}

		////3. t번 사람이 베이스캠프로 이동 후, 업데이트 수행
		////// => be_wall ->  baseCamps_pos, mapp 업데이트
		for (auto tar_it : be_wall)
		{
			int tr = tar_it.first;
			int tc = tar_it.second;

			mapp[tr][tc] = -1;
			baseCamps_pos.erase({ tr , tc });
		}

		////4. 종료조건 확인 -> break
		//if (man_pos.empty())	break;
		if (is_arrived.size() == m)
		{
			ans = t;
			break;
		}

		++t;
	}


	//3. (게임 종료 후)총 몇 분 후에 모두 편의점에 도착하는지 출력
	cout << ans;

}


void move_to_base(const int tarMan_idx)
{
	// store_pos[tarMan_idx].second -> basecamp 최단거리
	// str_row : store_pos[tarMan_idx].second.first, str_col : store_pos[tarMan_idx].second.second
	//// 

	//0. 
	bool visit[N_MAX][N_MAX] = { false, };
	deque<pair<int, int>> q_nodes;
	set<pair<int, int>> cands;							//최단경로 내의 base 후보군들 위치 저장

	//queue<pair<int, int>> tmp_queue;

	//// 출발 : store_pos, 도착 : base
	const int str_r = store_pos[tarMan_idx].first;
	const int str_c = store_pos[tarMan_idx].second;

	q_nodes.push_back({ str_r ,  str_c });
	visit[str_r][str_c] = true;

	while (!q_nodes.empty())
	{
		//// 0. 
		deque<pair<int, int>> q_tmp;				// 출발지로부터 동일한 범위 내의 모든 격자들에 대해, 4방향 이동 좌표 저장 
		pair<int, int> cur_node;

		//// 1. 출발지로부터 동일한 범위 내의 모든 격자들에 대해, 4방향 이동좌표 찾기 -> 도착지가 존재하는지 확인
		for (auto it_nodes : q_nodes)
		{
			cur_node = it_nodes;

			// case1 : 도착!
			if (baseCamps_pos.find({ cur_node.first, cur_node.second }) != baseCamps_pos.end())			//도착!
			{
				cands.insert({ cur_node.first, cur_node.second });
			}

			// case2 : 현재 위치 기반 4방향 찾기
			else
			{
				for (int i = 0; i < 4; ++i)
				{
					int nxt_r = cur_node.first + off_r[i];
					int nxt_c = cur_node.second + off_c[i];

					//제한0 : 범위초과
					if (nxt_r < 1 || nxt_r > n || nxt_c < 1 || nxt_c > n)	continue;
					//제한1
					if (visit[nxt_r][nxt_c]) continue;
					//제한2
					if (mapp[nxt_r][nxt_c] == -1) continue;

					q_tmp.push_back({ nxt_r, nxt_c });
					visit[nxt_r][nxt_c] = true;
				}
			}
		
		}

		////2. 정답처리
		if (cands.size() > 0)
		{
			auto it = cands.begin();

			//// man_pos 업데이트
			man_pos[tarMan_idx].first = (*it).first;
			man_pos[tarMan_idx].second = (*it).second;

			//// be_wall 업데이트
			be_wall.insert({ man_pos[tarMan_idx].first, man_pos[tarMan_idx].second });

			return;
		}
		

		////3.
		q_nodes = q_tmp;
	
	}

}

void moveAll(const int tarPos_idx)
{
	//0. 
	bool visit[N_MAX][N_MAX] = { false, };
	deque<pair<int, int>> q_nodes;
	set<pair<int, int>> cands;			// 이동할 위치 후보군들 저장
	set<pair<int, int>> tars;			// 사람 현재 위치 기준 4방향 저장

	//// 목적지 좌표들 구하기 : tars에 저장
	for (int i = 0; i < 4; ++i)
	{
		int tr = man_pos[tarPos_idx].first + off_r[i];
		int tc = man_pos[tarPos_idx].second + off_c[i];

		//// 제한 고려
		if (tr < 1 || tr > n || tc < 1 || tc > n)	continue;
		if (mapp[tr][tc] == -1) continue;

		//// tars에 저장
		tars.insert({ tr, tc });
	}

	//// 출발 : store_pos, 도착 : tars
	const int str_r = store_pos[tarPos_idx].first;
	const int str_c = store_pos[tarPos_idx].second;
	q_nodes.push_back({ str_r , str_c });
	visit[str_r][str_c] = true;

	/*
	//// 출발 - 도착이 붙어있는 경우 처리
	for (auto it : tars)
	{
		if (it.first == str_r && it.second == str_c)
		{
			//// man_pos 업데이트
			man_pos[tarPos_idx].first = str_r;
			man_pos[tarPos_idx].second = str_c;

			//// 편의점 도착 처리
			have_to_erase_idx.insert(tarPos_idx);
			be_wall.insert({ man_pos[tarPos_idx].first, man_pos[tarPos_idx].second });
			is_arrived.insert(tarPos_idx);

			return;
		}
	}
	*/
	//1.
	while (!q_nodes.empty())
	{
		////0. 
		pair<int, int> cur_node;
		deque<pair<int, int>> q_tmp;

		////1.
		for (auto it_nodes : q_nodes)
		{
			cur_node = it_nodes;
		
			// case1 : 도착!
			if (tars.find({ cur_node.first, cur_node.second }) != tars.end())			//도착!
			{
				cands.insert({ cur_node.first, cur_node.second });
			}
		

			// case2 : 현재 위치 기반 4방향 찾기
			else
			{
				for (int i = 0; i < 4; ++i)
				{
					int nxt_r = cur_node.first + off_r[i];
					int nxt_c = cur_node.second + off_c[i];

					//제한0 : 범위초과
					if (nxt_r < 1 || nxt_r > n || nxt_c < 1 || nxt_c > n)	continue;
					//제한1
					if (visit[nxt_r][nxt_c]) continue;
					//제한2
					if (mapp[nxt_r][nxt_c] == -1) continue;

					q_tmp.push_back({ nxt_r, nxt_c });
					visit[nxt_r][nxt_c] = true;
				}
			}
		
		}

		////2. 정답처리
		if (cands.size() > 0)
		{
			auto it = cands.begin();

			//// man_pos 업데이트
			man_pos[tarPos_idx].first = (*it).first;
			man_pos[tarPos_idx].second = (*it).second;


			//// 편의점 도착여부 확인 -> have_to_erase_idx, be_wall, is_arrived
			//// be_wall 업데이트
			if (man_pos[tarPos_idx].first == store_pos[tarPos_idx].first &&
				man_pos[tarPos_idx].second == store_pos[tarPos_idx].second)
			{
				have_to_erase_idx.insert(tarPos_idx);
				be_wall.insert({ man_pos[tarPos_idx].first, man_pos[tarPos_idx].second });
				is_arrived.insert(tarPos_idx);
			}

			return;
		}

		////3.
		q_nodes = q_tmp;
	}
	/*
	//1. 
	pair<int, int> cur_node;
	while (!q_nodes.empty())
	{
		//// 1.
		cur_node = q_nodes.front();
		q_nodes.pop();

		//// 정답처리
		if (cands.size() > 0)
		{
			auto it = cands.begin();

			//// man_pos 업데이트
			man_pos[tarPos_idx].first = (*it).first;
			man_pos[tarPos_idx].second = (*it).second;


			//// 편의점 도착여부 확인 -> have_to_erase_idx, be_wall, is_arrived
			//// be_wall 업데이트
			if (man_pos[tarPos_idx].first == store_pos[tarPos_idx].first &&
				man_pos[tarPos_idx].second == store_pos[tarPos_idx].second)
			{
				have_to_erase_idx.insert(tarPos_idx);
				be_wall.insert({ man_pos[tarPos_idx].first, man_pos[tarPos_idx].second });
				is_arrived.insert(tarPos_idx);
			}

			return;

		}

		////2. 
		for (int i = 0; i < 4; ++i)
		{
			int nxt_r = cur_node.first + off_r[i];
			int nxt_c = cur_node.second + off_c[i];

			//제한0 : 범위초과
			if (nxt_r < 1 || nxt_r > n || nxt_c < 1 || nxt_c > n)	continue;
			//
			if (visit[nxt_r][nxt_c]) continue;
			if (mapp[nxt_r][nxt_c] == -1) continue;

			q_nodes.push({ nxt_r, nxt_c });
			visit[nxt_r][nxt_c] = true;

			if (tars.find({ nxt_r, nxt_c }) != tars.end())	// tars 다!
			{
				cands.insert({ nxt_r, nxt_c });
			}

		}
	}
	*/
}