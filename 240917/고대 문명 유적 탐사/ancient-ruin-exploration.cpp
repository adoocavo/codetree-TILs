#include<iostream>
#include<queue>
#include<utility>

using namespace std;


#define N_ROW 5    // mapp 행 크기
#define N_COL 5    // mapp 열 크기
#define N_SElECT_ROW 3  // 회전시킬 격자 행 크기
#define N_SElECT_COL 3  // 회전시킬 격자 열 크기


int K;  // 최대 탐사 반복횟수
int M;  // 벽면에 적힌 유물 조각의 개수

int mapp[N_ROW][N_COL];     //탐색 대상이 되는 격자
queue<int> q_nums;          //벽면에 적힌 유물 조각  

void rotate(const int cnt, const int s_row, const int s_col, int(*rotated)[N_COL]);   // 3*3 격자 회전
int BFS(int(*rotated)[N_COL]);         //  회전된 격자가 반영된 mapp에서 점수획득 + 유물 조각 업데이트 
void Fill(int(*rotated)[N_COL]);       // 회전 후, -1로 표시한 곳 채워넣기 

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    //1. input
    cin >> K >> M;
    for(int i = 0; i < N_ROW; ++i)
    {
       for(int j = 0; j < N_COL; ++j) 
       {
            cin >> mapp[i][j];
       }
    }

    int input_M;
    for(int i = 0; i < M; ++i)
    {
        cin >> input_M;
        q_nums.push(input_M);
    }

    //2. 탐색
    while(K--)
    {   
        int getScore;                //  유물획득 점수
        int getSocreMax_ans = 0;        //  1회 턴에서 유물 획득 MAX 점수 
        bool found = false;          // (1턴)탐색에서 유물 획득 여부 확인

        int rotatedMax[N_ROW][N_COL];    //[1]탐사진행, [2]유물획득 과정 후의 변형이 반영된 mapp



        //2-1. 격자중심, 회전 횟수 결정, 1차 획득 계산
        for(int cnt = 1; cnt <= 3; ++cnt) // 90도 회전 1/2/3회 반복
        {
            //for(int s_row = 0; s_row <= N_ROW-N_SElECT_ROW; ++s_row)        //탐색대상 격자의 가장 왼쪽 위 row
            for(int s_col = 0; s_col <= N_COL-N_SElECT_COL; ++s_col)    //탐색대상 격자의 가장 왼쪽 위 col
            {
                //for(int s_col = 0; s_col <= N_COL-N_SElECT_COL; ++s_col)    //탐색대상 격자의 가장 왼쪽 위 col
                for(int s_row = 0; s_row <= N_ROW-N_SElECT_ROW; ++s_row)        //탐색대상 격자의 가장 왼쪽 위 row
                {      
                    int rotated[N_ROW][N_COL];   //회전 반영한 mapp 저장
                    
                    //// 1. 회전 수행 함수
                    rotate(cnt, s_row, s_col, rotated);

                    //// 2. 유물 "1차 획득" 수행 함수(BFS, getScore에 저장)
                    getScore = BFS(rotated);

                    //// 3. 유물 획득 점수 최대값 처리
                    if(getSocreMax_ans < getScore)
                    {
                        found = true;

                        getSocreMax_ans = getScore;

                        // rotated를 연쇄획득을 위해 '2-1 loop' 밖으로 copy
                        for(int i = 0; i < N_ROW; ++i)
                        {
                            for(int j = 0; j < N_COL; ++j)
                            {
                                rotatedMax[i][j] = rotated[i][j];
                            }
                        }
                    }
                }
            }
        }

        //// 5. 탐사 종료 조건
        if(!found) break;

        //2-2. 연쇄획득
        while(1)
        {
            //1. -1 칸 유리조각으로 채워넣기
            Fill(rotatedMax);
            
            //2. 다시 탐색
            int nxtSocore = BFS(rotatedMax);
            
            //3. 종료조건
            if(nxtSocore == 0) break;

            else
            {
                getSocreMax_ans += nxtSocore;
                continue;
            }
            

        }

        // 3-1. getSocreMax_ans 출력
        cout << getSocreMax_ans << " ";

        // 3-2. 다음 턴 위해 mapp에 회전 및 유물획득 반영
        for(int i = 0; i < N_ROW; ++i)
        {
            for(int j = 0; j < N_COL; ++j)
            {
                mapp[i][j] = rotatedMax[i][j];
            }
        }
    }
    
    return 0;
}



void rotate(const int cnt, const int s_row, const int s_col, int(*rotated)[N_COL])
{
    //1. mapp -> rotated 복사본 만들기
    for(int i = 0; i < N_ROW; ++i)
    {
       for(int j = 0; j < N_COL; ++j) 
       {
            *(*(rotated+i)+j) = *(*(mapp+i)+j);  
       }
    }

    //2. rotated를 cnt 횟수만큼 시계방향 회전시키기
    for(int k = 0; k < cnt; ++k)
    {
        ////2-1. 모서리 값들 회전
        int tmp1 = rotated[s_row+0][s_col+0];
        rotated[s_row+0][s_col+0] = rotated[s_row+2][s_col+0];
        rotated[s_row+2][s_col+0] = rotated[s_row+2][s_col+2];
        rotated[s_row+2][s_col+2] = rotated[s_row+0][s_col+2];
        rotated[s_row+0][s_col+2] = tmp1;

        ////2-2. 중앙값 회전
        int tmp2 = rotated[s_row+0][s_col+1];
        rotated[s_row+0][s_col+1] = rotated[s_row+1][s_col+0];
        rotated[s_row+1][s_col+0] = rotated[s_row+2][s_col+1];
        rotated[s_row+2][s_col+1] = rotated[s_row+1][s_col+2];
        rotated[s_row+1][s_col+2] = tmp2;
    }
    
    return;
}

int BFS(int(*rotated)[N_COL])          //회전된 격자가 반영된 mapp(rotated)에서 점수획득(1차 획득만!!) + 유물 조각 업데이트 
{
    //0. 변수 셋팅
    int getSocre = 0;                       // 획득 유물 점수 총합 저장
    int off_row[4] = {0, 1, -1, 0};
    int off_col[4] = {1, 0, 0, -1};

    bool visit[N_ROW][N_COL] = {false, };   //방문 node 표시
    
    
    //1. BFS 시작점 선택
    for(int i = 0; i < N_ROW; ++i)
    {
       for(int j = 0; j < N_COL; ++j) 
       {
            //2. 특정 시작점 -> 탐색 수행
            if(!visit[i][j])
            {
                ////2-1. 초기처리
                queue<pair<int, int>> q_nodes;          // 방문 node 저장
                queue<pair<int, int>> trace;            // 유물 획득(network 형성)하는 node들 저장
                
                visit[i][j] = true;
                q_nodes.push({i,j});
                trace.push({i,j});

                ////2-2. while loop : 선택된 시작점에서부터 BFS 탐색 수행(network 만들기)
                pair<int, int> cur_node;
                while(!q_nodes.empty())
                {
                    ////// 2-2-1. cur_node 저장
                    cur_node = q_nodes.front();
                    q_nodes.pop();

                    ////// 2-2-2. for loop 탐색
                    for(int k = 0; k < 4; ++k)
                    {
                        int nxt_row = cur_node.first + off_row[k];
                        int nxt_col = cur_node.second + off_col[k];

                        //// network 제한조건(nxt node 탐색 가능여부)
                        if(nxt_row < 0 || nxt_row >= N_ROW || nxt_col < 0 || nxt_col >= N_COL) continue;
                        if(visit[nxt_row][nxt_col]) continue;
                        if(rotated[cur_node.first][cur_node.second] != rotated[nxt_row][nxt_col]) continue;

                        //// visit, queue, trace 처리
                        visit[nxt_row][nxt_col] = true;
                        q_nodes.push({nxt_row, nxt_col});
                        trace.push({nxt_row, nxt_col});
                    }
                }
           
            
                ////2-3. network 확인
                ////// => trace.size() >=3인 경우에,
                //////  (1) node 개수만큼 점수 합 구하기 
                //////  (2) 유물이 획득된 위치 -1 저장해서 표시하기
                if(trace.size() >= 3)
                {
                    getSocre += trace.size();
                    /*
                    for(int k = 0; k < trace.size(); ++k)
                    {
                        rotated[trace.front().first][trace.front().second] = -1;
                    }
                    */
                   while(!trace.empty())
                   {
                        rotated[trace.front().first][trace.front().second] = -1;
                        trace.pop();                   
                   }
                }
            }
       }
    }

    return getSocre;
}

void Fill(int(*rotated)[N_COL])
{
    for(int k = 0; k < N_COL; ++k)
    {
        for(int l = N_ROW-1; l >= 0; --l)
        {
            if(rotated[l][k] == -1)
            {
                rotated[l][k] = q_nums.front();
                q_nums.pop();
            }
        }
    }
    
    return;
}