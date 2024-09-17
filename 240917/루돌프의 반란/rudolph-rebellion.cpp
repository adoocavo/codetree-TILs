#include<iostream>
#include<utility>
#include<cmath>
#include<queue>

using namespace std;

#define MAX_N 50+4
#define MAX_P 30+4          //최대 산타 명수
#define MAX_DIST 4802+1     // 루돌프-산타 간 최대 거리

//DS for Santa
int santaPos[MAX_P][2];     // 산타 위치(row, col) 저장 
int wakeup_turn[MAX_P];     // 기절한 산타가 일어나는 턴 저장, 0 : 기절안함
bool is_alive[MAX_P];       // 산타가 게임에서 탈락했는지 여부 저장
int scores[MAX_P];          // 산타들의 점수 저장

// DS for Rudol
pair<int, int> rudolPos;    //루돌프 위치 저장

/*
// 루돌프 이동 offset
int off_row_R[8] = {-1, 0, 1, 0, -1, -1, 1, 1};
int off_col_R[8] = {0, 1, 0, -1, -1, 1, 1, -1};
*/

// 산타 이동 offset
int off_row_S[4] = { -1, 0, 1, 0 };
int off_col_S[4] = { 0, 1, 0, -1 };

int N;          // mapp size : N x N
int M;          // 게임 진행 턴 횟수
int P;          // 산타 명수
int C;          // 루돌프의 힘(루돌프가 움직여서 충돌 -> 산타가 C 칸 만큼 밀려남)
int D;          // 산타의 힘(산타가 움직여서 충돌 -> 산타가 D 칸 만큼 밀려남)

int mapp[MAX_N][MAX_N];     // 게임판, 1-idxed, -1 : R / 1~P : S / 0 : 빈칸

void R_move(const int turnNum);             // 루돌프 이동 처리
void Santa_move(const int turnNum);         // 산타 이동 처리

// 산타가 루돌프와 충돌해서 밀려나는 동작 처리
//// santaIdx : 이동할 산타의 idx(번호)
//// move_row, move_col : 현재 산타의 위치에서 어느 방향으로 이동할지 정보
//// move_num : 몇 칸 이동할지 정보
void Santa_bye(const int santaIdx, const int move_row, const int move_col, const int move_num);

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);

    //1. input
    cin >> N >> M >> P >> C >> D;
    cin >> rudolPos.first >> rudolPos.second;
    mapp[rudolPos.first][rudolPos.second] = -1;

    int input_S_idx, input_r, input_c;
    for (int i = 0; i < P; ++i)
    {
        cin >> input_S_idx >> input_r >> input_c;
        mapp[input_r][input_c] = input_S_idx;

        santaPos[input_S_idx][0] = input_r;
        santaPos[input_S_idx][1] = input_c;
        is_alive[input_S_idx] = true;
    }


    int K = 1;
    bool shutdown = true;
    while (K <= M)
    {
        //2. 루돌프 이동
        R_move(K);

        //3. 산타 이동
        Santa_move(K);

        //4. 탈락X 산타들에게 +1점씩 주기
        shutdown = true;
        for (int i = 1; i <= P; ++i)
        {
            if (is_alive[i])
            {
                scores[i] += 1;
                shutdown = false;
            }
        }

        //5. 종료 조건 확인
        if (shutdown) break;

        ++K;
    }

    //6. output
    for (int i = 1; i <= P; ++i)
    {
        cout << scores[i] << " ";
    }

}


void Santa_bye(const int santaIdx, const int move_row, const int move_col, const int move_num)
{
    //queue<tuple<int, int, int, int>> nxt_Santa;         //연쇄충돌 대상 정보 저장
    //nxt_Santa.push({santaIdx, move_row, move_col, move_num});

    queue<pair<int, int>> nxt_Santa_info;       // 상호작용 대상 Santa의 idx, move_num
    queue<pair<int, int>> move_direction;       // 상호작용 대상 Santa의 이동 row, col

    //1. 초기 셋팅
    nxt_Santa_info.push({ santaIdx, move_num });
    move_direction.push({ move_row, move_col });

    //2. BFS 
    pair<int, int> cur_Santa;                   // 현재 밀려나는 동작 처리 대상 Santa의 idx, move_num
    pair<int, int> cur_direction;               // 현재 밀려나는 동작 처리 대상 Santa의 이동방향(row, col)
    while (!nxt_Santa_info.empty() || !move_direction.empty())
    {
        //// 2-1. 현재 밀림 동작 처리 대상 정보 저장
        cur_Santa = nxt_Santa_info.front();
        nxt_Santa_info.pop();

        cur_direction = move_direction.front();
        move_direction.pop();

        //// 2-2.
        int nxt_row = santaPos[cur_Santa.first][0] + (cur_Santa.second * cur_direction.first);
        int nxt_col = santaPos[cur_Santa.first][1] + (cur_Santa.second * cur_direction.second);

        //// 2-3. 산타 탈락 처리
        //if (nxt_row < 1 || nxt_row > MAX_N || nxt_col < 1 || nxt_col > MAX_N)
        if (nxt_row < 1 || nxt_row > N || nxt_col < 1 || nxt_col > N)
        {
            is_alive[cur_Santa.first] = false;
            continue;
        }

        //// 2-4. 착지한 지점에 산타 존재 
        if (mapp[nxt_row][nxt_col] > 0)
        {
            nxt_Santa_info.push({ mapp[nxt_row][nxt_col], 1 });
            move_direction.push({ cur_direction.first, cur_direction.second });

            //mapp[nxt_row][nxt_col] = cur_Santa.first;                       // 착지한 지점에 새로운 산타 idx 저장

            //continue;
        }

        //// 2-5. 산타 위치변경 정보 수정
        mapp[nxt_row][nxt_col] = cur_Santa.first;
        santaPos[cur_Santa.first][0] = nxt_row;
        santaPos[cur_Santa.first][1] = nxt_col;


    }

    return;
}

void R_move(const int turnNum)
{
    //1. '거리'가 가장 가까운 산타 선택
    int dist;
    int minDist = MAX_DIST;
    int tar_r = 0, tar_c = 0;               //루돌프가 박치기 타겟으로 하는 산타 위치
    int tar_santa_idx = 0;                  //루돌프가 박치기 타겟으로 하는 산타 번호

    for (int i = 1; i <= P; ++i)
    {
        if (!is_alive[i]) continue;

        //// 거리 계산
        dist = pow(santaPos[i][0] - rudolPos.first, 2) + pow(santaPos[i][1] - rudolPos.second, 2);

        //// 최소 거리인 산타 수정
        if (minDist > dist)
        {
            tar_r = santaPos[i][0];
            tar_c = santaPos[i][1];
            tar_santa_idx = i;

            minDist = dist;
        }
        else if (minDist == dist && tar_santa_idx != 0)
        {
            if (tar_r < santaPos[i][0])
            {
                tar_r = santaPos[i][0];
                tar_c = santaPos[i][1];
                tar_santa_idx = i;
            }
            else if (tar_r == santaPos[i][0])
            {
                if (tar_c < santaPos[i][1])
                {
                    tar_r = santaPos[i][0];
                    tar_c = santaPos[i][1];
                    tar_santa_idx = i;
                }
            }
        }
    }


    //2. 이동 가능한 8 방향중, 선택된 산타(tar_santa_idx, tar_r, tar_c)와 가장 가까워자는 방향 선택
    //// => 8 방향으로 이동했을때의 거리를 모두 구하지 않으면서 방향 설정 가능
    //int prev_r;      // 선택된 방향으로 이동 전의 루돌프 row
    //int prev_c;      // 선택된 방향으로 이동 전의 루돌프 col
    int off_r = 0, off_c = 0;

    //// 2-1. nxt_r 구하기
    if (tar_r > rudolPos.first) off_r = 1;
    else if (tar_r < rudolPos.first) off_r = -1;

    //// 2-2. nxt_c 구하기
    if (tar_c > rudolPos.second) off_c = 1;
    else if (tar_c < rudolPos.second) off_c = -1;

    //// 2-3. mapp에 변경 반영, 루돌프 위치 업데이트
    //prev_r = rudolPos.first; 
    //prev_c = rudolPos.second;
    mapp[rudolPos.first][rudolPos.second] = 0;
    rudolPos.first += off_r;
    rudolPos.second += off_c;

    mapp[rudolPos.first][rudolPos.second] = -1;

    //3. 충돌 처리
    if (rudolPos.first == tar_r && rudolPos.second == tar_c)
    {
        //// 3-1. 산타 점수 획득
        scores[tar_santa_idx] += C;

        //// 3-2. C만큼 산타 밀려나기(루돌프가 이동해온 방향으로)
        Santa_bye(tar_santa_idx, off_r, off_c, C);
        //Santa_bye(tar_santa_idx, off_r*-1, off_c*-1, C);

        //// 3-3. 기절 처리
        wakeup_turn[tar_santa_idx] = turnNum + 2;

    }


    return;
}

void Santa_move(const int turnNum)
{
    for (int i = 1; i <= P; ++i)
    {
        //1. 제한조건 고려해서 움직일 산타 선택 
        //if(!is_alive[i] || (wakeup_turn[i] != 0 && wakeup_turn[i] != turnNum))    continue; 
        if (!is_alive[i] || wakeup_turn[i] > turnNum)    continue;

        //2. 제한조건 고려해서 이동할 방향 선택 
        int dist;               //현재 선택된 산타(방향 이동 반영) 위치 - 루돌프 위치 간의 거리
        int minDist = MAX_DIST;
        int nxt_r, nxt_c;                                    // 선택된 방향으로 이동 후의 산타 위치
        int tar_r = -1, tar_c = -1, nxt_direction = -1;      // 이동할 방향 확정값 
        int prev_dist = pow(santaPos[i][0] - rudolPos.first, 2) + pow(santaPos[i][1] - rudolPos.second, 2);
        for (int j = 0; j < 4; ++j)
        {
            nxt_r = santaPos[i][0] + off_row_S[j];
            nxt_c = santaPos[i][1] + off_col_S[j];

            //// 2-1. 제한1, 제한2           
            //if (nxt_r < 1 || nxt_r > MAX_N || nxt_c < 1 || nxt_c > MAX_N) continue;
            if (nxt_r < 1 || nxt_r > N || nxt_c < 1 || nxt_c > N) continue;
            if (mapp[nxt_r][nxt_c] > 0) continue;

            dist = pow(nxt_r - rudolPos.first, 2) + pow(nxt_c - rudolPos.second, 2);

            //// 2-2. 제한3
            if (prev_dist <= dist) continue;

            //// 2-3. 이동할 방향 확정
            if (minDist > dist)
            {
                tar_r = nxt_r;
                tar_c = nxt_c;
                nxt_direction = j;

                minDist = dist;

                if (minDist == 0) break;
            }
        }

        //// 3. 산타 움직임 처리
        if (nxt_direction != -1)
        {
            mapp[santaPos[i][0]][santaPos[i][1]] = 0;

            santaPos[i][0] = tar_r;
            santaPos[i][1] = tar_c;
            
            if (mapp[santaPos[i][0]][santaPos[i][1]] == -1)
            {
                // 충돌 처리
                //// 산타 점수 획득
                scores[i] += D;

                //// D만큼 산타 밀려나기(산타가 이동해온 반대 방향으로)
                //Santa_bye(i, off_row_S[nxt_direction], off_col_S[nxt_direction], D);
                Santa_bye(i, off_row_S[nxt_direction]*-1, off_col_S[nxt_direction]*-1, D);

                //// 기절 처리, 사망처리
                // wakeup_turn[i] = turnNum + 2;
                if(is_alive[i]) wakeup_turn[i] = turnNum + 2;
            }
            else
            {
                mapp[santaPos[i][0]][santaPos[i][1]] = i;
            }
        }

    }

    return;

}