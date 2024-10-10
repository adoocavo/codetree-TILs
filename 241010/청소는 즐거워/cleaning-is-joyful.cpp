#include<iostream>
#include<map>
#include<utility>
using namespace std;

#define N_MAX (499+2)

int mapp[N_MAX][N_MAX];					//  0~1000(먼지의 양)
int mapp_tornado[N_MAX][N_MAX];			// 이동방향 저장 배열(0 좌, 1 하, 2 우, 3 상)

int n;		//격자 크기
int ans;	// 격자 밖으로 떨어진 먼지 양의 총합 저장

// 빗자루 이동 방향 
//// (0 좌, 1 하, 2 우, 3 상)
int off_r[4] = { 0, 1, 0, -1 };
int off_c[4] = { -1, 0, 1, 0 };

// 먼지 퍼지는거 관련
//// 마지막 정보는 a관련 정보
int dx[4][10] = { { -2, -1,-1,-1,0,1,1,1,2,0 }, {0,1,0,-1,2,1,0,-1,0,1},{2, 1,1,1,0,-1,-1,-1,-2,0},{0,-1,0,1,-2,-1,0,1,0,-1} };
int dy[4][10] = { { 0,-1,0,1,-2,-1,0,1,0,-1 },{-2,-1,-1,-1,0,1,1,1,2,0},{ 0,1,0,-1,2,1,0,-1,0,1},{2,1,1,1,0,-1,-1,-1,-2,0} };

// percent[{dx[i], dy[i]}] : 현재 빗자루 위치 기준으로 {dx[i], dy[i]} 떨어진 위치에 더헤질 퍼센트
//map<pair<int, int>, int> percent;
//// => idx : dx, dy의 두 번째 인덱스, value :  dx, dy의 두 번째 인덱스에 따라 정해진 퍼센트
int percent[10] = { 2,10,7,1,5,10,7,1,2,0 };


// 토네이도 만들기
void make_tornado(void);
// 빗자루 이동 이후 먼지 관련 처리
void splash(const int curr_r, const int curr_c, const int curr_direc);


int main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);

	//1. input
	cin >> n;

	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			cin >> mapp[i][j];
		}
	}
	/*
	//1-2. map 초기화
	percent[{dx[0], dy[0]}] = 2;
	percent[{dx[1], dy[1]}] = 10;
	percent[{dx[2], dy[2]}] = 7;

	percent[{dx[3], dy[3]}] = 1;
	percent[{dx[4], dy[4]}] = 5;
	percent[{dx[5], dy[5]}] = 10;

	percent[{dx[6], dy[6]}] = 7;
	percent[{dx[7], dy[7]}] = 1;
	percent[{dx[8], dy[8]}] = 2;

	percent[{dx[9], dy[9]}] = 0;
	*/
	//2. 토네이도 배열 만들기
	make_tornado();

	//3. 
	/*
	int cur_r = n % 2 + 1;
	int cur_c = n % 2 + 1;
	*/
	int cur_r = n / 2 + 1;
	int cur_c = n / 2 + 1;
	while (!(cur_r == 1 && cur_c == 1))
	{
		////1. 빗자루 이동
		int nxt_r = cur_r + off_r[mapp_tornado[cur_r][cur_c]];
		int nxt_c = cur_c + off_c[mapp_tornado[cur_r][cur_c]];
		int curr_direc = mapp_tornado[cur_r][cur_c];						//빗자루가 이동하기 전 위치에서, 이동한 위치를 바라보는 방향

		////2. 빗자루가 이동한 위치의 격자(Curr)에 있는 먼지가 함께 이동 처리
		////// => ans, mapp 업데이트
		splash(nxt_r, nxt_c, curr_direc);

		//// 3. nxt -> cur
		cur_r = nxt_r;
		cur_c = nxt_c;
	}


	//4. 좌측 최상단까지 도달하는데까지 격자 밖으로 떨어진 먼지 양의 총합을 계산
	cout << ans;

	return 0;
}

void splash(const int curr_r, const int curr_c, const int curr_direc)
{
	//0.
	int splash_r;		// 먼지가 날라가는 격자 r
	int splash_c;		// 먼지가 날라가는 격자 c
	int splash_size;	// 먼지가 날라갈 격자에 날라가는 먼지 양
	int curr_size = mapp[curr_r][curr_c];				//현재빗자루가 위치한 격자의 먼지 양
	//int curr_direc = mapp_tornado[curr_r][curr_c]		//현재 빗자루가 이동하기 전 위치에서의 방향

	int tmp_ans = 0;	// 격자를 벗어나는 먼지 양 합 저장
	int for_a = 0;		// 격자를 벗어나지 않는 먼지 양 합 저장 

	for (int i = 0; i < 10; ++i)
	{
		//1. 먼지가 날라가는 격자 위치, 먼지 양 구하기
		/*
		splash_r = curr_r + dx[i];
		splash_c = curr_c + dy[i];
		splash_size = (curr_size * ((percent[{dx[i], dy[i]}]) / 100.0));
		*/
		splash_r = curr_r + dx[curr_direc][i];
		splash_c = curr_c + dy[curr_direc][i];
		splash_size = (curr_size * ((percent[i]) / 100.0));

		// a 위치 처리
		if (i == 9)
		{
			splash_size = (curr_size - for_a);
		}

		//2-1 먼지가 날라가는 위치가 격자를 초과
		if (splash_r < 1 || splash_r > n || splash_c < 1 || splash_c > n)
		{
			tmp_ans += splash_size;
		}

		//2-2 다른 격자에 먼지가 날림
		else
		{
			mapp[splash_r][splash_c] += splash_size;
			//for_a += splash_size;
		}
		for_a += splash_size;
	}

	// tmp_ans -> ans
	ans += tmp_ans;
	return;
}



void make_tornado(void)
{
	//0. 필요 var, ds 정의 

	int cur_r = n / 2 + 1;
	int cur_c = n / 2 + 1;

	int move_num = 1;		// 1회 이동당 몇 칸 이동
	int move_direc = 0;		// 1회 이동당 어느 방향으로 이동
	int flag_cnt = 0;		// 1회 이동을 몇번 수행?? ('2' -> move_num update)

	while (1)
	{
		//1. 현재 좌표에서 1회 이동 수행
		//// => flag_cnt 업데이트
		for (int i = 0; i < move_num; ++i)
		{
			////0. 현재 좌표가 (1,1)인지 확인 => 마무리
			if (cur_r == 1 && cur_c == 1)
			{
				return;
			}

			////1.현재 좌표에, 현재 방향을 저장
			mapp_tornado[cur_r][cur_c] = move_direc;

			////2. 1회 이동 내에서, 다음 좌표 정보 저장
			int nxt_r = cur_r + off_r[move_direc];
			int nxt_c = cur_c + off_c[move_direc];

			////3. 다음 좌표 -> 현재 좌표로 업데이트
			cur_r = nxt_r;
			cur_c = nxt_c;
		}
		flag_cnt++;

		//2. 1회 이동 수행 후 -> move_direc 업데이트
		move_direc = (move_direc + 1) % 4;

		//3. 2회 이동? -> move_num 업데이트
		if (flag_cnt == 2)
		{
			move_num++;
			flag_cnt = 0;
		}
	}
}