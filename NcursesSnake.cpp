#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define tick 100000 // 틱레이트 마이크로초 단위 1초에 10틱

bool fail; // 게임 실패 여부
int stage;
char dir; // 방향 U D L R
int movetimer; // move 함수용 타이머 시간 저장
vector<int> snakex;
vector<int> snakey; // 뱀 x, y좌표
int g_itemx, g_itemy, g_timer; // growth item x,y 좌표, 타이머
int p_itemx, p_itemy, p_timer; // poison item x,y 좌표, 타이머
int gate1_x, gate1_y; // gate1의 x,y좌표
int gate2_x, gate2_y; // gate2의 x,y좌표
int gatecount; // gate 함수 재호출용
int chkgrowth, chkp, chkgate, maxbody; //item 획득 횟수, gate 통과 횟수
int m_growth, m_poison, m_gate, m_body; // 목표
int winx, winy; // 창 크기
bool clearall; // 모든 스테이지 clear

int map[4][21][21] =
{
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    },
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    },
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    },
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    }
}; // [stage][행][열]) 일단은 스테이지 1만

void reset(); // 게임 시작시 함수 초기화
void input(); // 키 입력
void move(); // 입력 받은 키에 따라 이동
void show(); // 그래픽을 보여주는 함수
void growth_item(); // growth item 만드는 함수
void poison_item(); // poison item 만드는 함수
void gate(); //gate를 만드는 함수
void timer(); // 타이머
void setstage(int n); // stage 초기화 함수
void setgoal();
void chkgoal();

int main()
{
    reset();
    while (!fail)
    {
        timer();
        input();
        show();
        usleep(tick);
    }
    if(clearall == false)
    {
        clear();
        mvprintw(winy/2, (winx-11)/2, "You Failed!");
        refresh();
        usleep(2000000);  
    }
    endwin(); // fail 시 종료
    return 0;
}

void reset()
{
    initscr(); // 터미널 초기화
    start_color(); // 컬러모드 사용
    nodelay(stdscr, true); // 키 입력 기다리지 않음
    noecho(); // 입력받은 키 출력하지 않음
    curs_set(0); // 커서 숨기기
    cbreak(); // 입력받은 키 바로 활용
    keypad(stdscr, TRUE); // 특수 키 사용 가능
    getmaxyx(stdscr, winy, winx); // 창 크기 재기
    
    fail = false;
    clearall = false;
    stage = 0;

    setstage(stage);

    //colors
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(4, COLOR_CYAN, COLOR_CYAN);
    init_pair(5, COLOR_GREEN, COLOR_GREEN);
    init_pair(6, COLOR_RED, COLOR_RED);
    init_pair(7, COLOR_MAGENTA, COLOR_MAGENTA);
}

void input()
{
    int key = getch();

    switch (key)
    {
    case KEY_UP:
        if (dir == 'D')
            fail = true;
        else
            dir = 'U';
        break;
    case KEY_DOWN:
        if (dir == 'U')
            fail = true;
        else
            dir = 'D';
        break;
    case KEY_LEFT:
        if (dir == 'R')
            fail = true;
        else
            dir = 'L';
        break;
    case KEY_RIGHT:
        if (dir == 'L')
            fail = true;
        else
            dir = 'R';
        break;
    }
}

void move()
{
    if (gatecount > 0)
    {
        if (gatecount == 1)
        {
            map[stage][gate1_y][gate1_x] = 1;
            map[stage][gate2_y][gate2_x] = 1;
            chkgate += 1; //획득개수 체크
            gate();
        }
        gatecount --;
    }
    int last = snakex.size();

    int lastx = snakex[last - 1]; //몸통 마지막 좌표 저장
    int lasty = snakey[last - 1];

    map[stage][snakey[last - 1]][snakex[last - 1]] = 0; // 몸통 마지막 좌표 지우기
    for (int i = last - 1; i > 0; i--) // 몸통 앞으로 한 칸 씩 복제
    {
        snakex[i] = snakex[i - 1];
        snakey[i] = snakey[i - 1];
    }

    switch (dir) // 머리 좌표 이동
    {
    case 'U':
        snakey[0]--;
        break;
    case 'D':
        snakey[0]++;
        break;
    case 'L':
        snakex[0]--;
        break;
    case 'R':
        snakex[0]++;
        break;
    }

    if (map[stage][snakey[0]][snakex[0]] == 7) //Gate 통과시
    {
        gatecount = last;

        int exitx, exity; // 출구 좌표 저장
        /*
        for (int i = 0; i < 21; i++)
        {
            for (int j = 0; j < 21; j++)
            {
                if (map[stage][j][i] == 7)
                {
                    if (snakex[0] != i)
                    {
                        exitx = i;
                        exity = j;
                    }
                }
            }
        }*/
        if(snakex[0] == gate1_x && snakey[0] == gate1_y)
        {
            exitx = gate2_x;
            exity = gate2_y;
        }
        else
        {
            exitx = gate1_x;
            exity = gate1_y;
        }

        if(exitx == 0) // Gate 진출 방향 1
        {
            snakex[0] = exitx + 1;
            snakey[0] = exity;
            dir = 'R';
        }
        else if(exitx == 20)
        {
            snakex[0] = exitx - 1;
            snakey[0] = exity;
            dir = 'L';
        }
        else if(exity == 0)
        {
            snakex[0] = exitx;
            snakey[0] = exity + 1;
            dir = 'D';
        }
        else if(exity == 20)
        {
            snakex[0] = exitx;
            snakey[0] = exity - 1;
            dir = 'U';
        }
        else // Gate 진출 방향 2
        {   
            snakex[0] = exitx;
            snakey[0] = exity;
            char tempdir = dir;
            switch (dir) // 진입 방향과 일치하는 방향
            {
            case 'U':
                snakey[0]--;
                break;
            case 'D':
                snakey[0]++;
                break;
            case 'L':
                snakex[0]--;
                break;
            case 'R':
                snakex[0]++;
                break;
            }
            if (map[stage][snakey[0]][snakex[0]] == 7 || map[stage][snakey[0]][snakex[0]] == 1 || map[stage][snakey[0]][snakex[0]] == 2)
            {   
                snakex[0] = exitx;
                snakey[0] = exity;
                switch (dir) // 진입 방향의 시계방향
                {
                case 'U':
                    dir = 'R';
                    break;
                case 'D':
                    dir = 'L';
                    break;
                case 'L':
                    dir = 'U';
                    break;
                case 'R':
                    dir = 'D';
                    break;
                }

                switch (dir)
                {
                case 'U':
                    snakey[0]--;
                    break;
                case 'D':
                    snakey[0]++;
                    break;
                case 'L':
                    snakex[0]--;
                    break;
                case 'R':
                    snakex[0]++;
                    break;
                }

                if (map[stage][snakey[0]][snakex[0]] == 7 || map[stage][snakey[0]][snakex[0]] == 1 || map[stage][snakey[0]][snakex[0]] == 2)
                {
                    snakex[0] = exitx;
                    snakey[0] = exity;
                    switch (tempdir) // 진입 방향의 반시계방향
                    {
                    case 'U':
                        dir = 'L';
                        break;
                    case 'D':
                        dir = 'R';
                        break;
                    case 'L':
                        dir = 'D';
                        break;
                    case 'R':
                        dir = 'U';
                        break;
                    }

                    switch (dir)
                    {
                    case 'U':
                        snakey[0]--;
                        break;
                    case 'D':
                        snakey[0]++;
                        break;
                    case 'L':
                        snakex[0]--;
                        break;
                    case 'R':
                        snakex[0]++;
                        break;
                    }
                    if (map[stage][snakey[0]][snakex[0]] == 7 || map[stage][snakey[0]][snakex[0]] == 1 || map[stage][snakey[0]][snakex[0]] == 2)
                    {
                        snakex[0] = exitx;
                        snakey[0] = exity;
                        switch (tempdir) // 진입 방향과 반대 방향
                        {
                        case 'U':
                            dir = 'D';
                            break;
                        case 'D':
                            dir = 'U';
                            break;
                        case 'L':
                            dir = 'R';
                            break;
                        case 'R':
                            dir = 'L';
                            break;
                        }

                        switch (dir)
                        {
                        case 'U':
                            snakey[0]--;
                            break;
                        case 'D':
                            snakey[0]++;
                            break;
                        case 'L':
                            snakex[0]--;
                            break;
                        case 'R':
                            snakex[0]++;
                            break;
                        }
                    }
                }
            }    
        }           
    }

    int grown = 0, poisoned = 0; // 뱀 이동 후 아이템 생성 함수 호출하기 위한 변수
    if (map[stage][snakey[0]][snakex[0]] == 4 || map[stage][snakey[0]][snakex[0]] == 1) // Game Rule #1 실패 조건
    {
        fail = true;
    }
    else if (map[stage][snakey[0]][snakex[0]] == 5) // growth item 획득시
    {
        snakex.push_back(lastx); // 몸통 뒤에 좌표 추가
        snakey.push_back(lasty);
        grown = 1;
        chkgrowth += 1; //획득개수 체크

        if(maxbody < snakex.size())
        {
            maxbody = snakex.size();
        }
    }
    else if (map[stage][snakey[0]][snakex[0]] == 6) // poison item 획득시
    {
        if(last == 3) // 길이가 3이면 실패
        {
            fail = true;
        }
        else
        {
            map[stage][snakey[last - 1]][snakex[last - 1]] = 0; // 몸통 마지막 좌표 지우기
            snakex.pop_back(); // 좌표값 삭제
            snakey.pop_back();
            poisoned = 1;
            chkp += 1; //획득개수 체크
        }
    }

    map[stage][snakey[0]][snakex[0]] = 3; // map 에 수정된 snake의 좌표 전달
    for (int i = 1; i < snakex.size(); i++)
    {
        map[stage][snakey[i]][snakex[i]] = 4;
    }
    if(poisoned == 1)
    {
        poison_item();
        p_timer = 100;
    }
    if(grown == 1)
    {
        growth_item();
        g_timer = 100;
    }
    chkgoal();
}

void show()
{
    clear();
    int i = stage;
    for (int j = 0; j < 21; j++) {
        printw("\n");
        for (int z = 0; z < 21; z++) {
            if (map[i][j][z] == 2) {
                attron(COLOR_PAIR(2));
                printw("+"); //lmmune wall
                attroff(COLOR_PAIR(2));
            }
            else if (map[i][j][z] == 1) {
                attron(COLOR_PAIR(1));
                printw("-"); //wall
                attroff(COLOR_PAIR(1));
            }
            else if (map[i][j][z] == 0) {
                printw(" ");
            }
            else if (map[i][j][z] == 3) {
                attron(COLOR_PAIR(3));
                printw("0"); //head
                attroff(COLOR_PAIR(3));
            }
            else if (map[i][j][z] == 4) {
                attron(COLOR_PAIR(4));
                printw("o"); //body
                attroff(COLOR_PAIR(4));
            }
            else if (map[i][j][z] == 5) {
                attron(COLOR_PAIR(5));
                printw("F"); //food
                attroff(COLOR_PAIR(5));
            }
            else if (map[i][j][z] == 6) {
                attron(COLOR_PAIR(6));
                printw("P"); //poison
                attroff(COLOR_PAIR(6));
            }
            else if (map[i][j][z] == 7) {
                attron(COLOR_PAIR(7));
                printw("G"); //gate
                attroff(COLOR_PAIR(7));
            }
        }
    }
    mvprintw(1, 30, "Score Board");             //점수표
    mvprintw(2, 30, "B: %d / %d", snakex.size(), maxbody);    //뱀의 몸길이
    mvprintw(3, 30, "+: %d", chkgrowth);        //growth item 획득 개수
    mvprintw(4, 30, "-: %d", chkp);             //poison item 획득 개수
    mvprintw(5, 30, "G: %d", chkgate);          //gate 통과 횟수
    mvprintw(8, 30, "Mission Board ");      //미션표
    mvprintw(9, 30, "B: %d", m_body);
    if(maxbody >= m_body)
    {
        printw(" (V)");
    }
    else
    {
        printw(" ( )");
    }
    mvprintw(10, 30, "+: %d", m_growth);
    if(chkgrowth >= m_growth)
    {
        printw(" (V)");
    }
    else
    {
        printw(" ( )");
    }
    mvprintw(11, 30, "-: %d", m_poison);
    if(chkp >= m_poison)
    {
        printw(" (V)");
    }
    else
    {
        printw(" ( )");
    }
    mvprintw(12, 30, "G: %d", m_gate);
    if(chkgate >= m_gate)
    {
        printw(" (V)");
    }
    else
    {
        printw(" ( )");
    }
    refresh();
}
void growth_item() {
    int r = 0; //난수 생성 변수
    int food_crush_on = 0; //item이 뱀과 충돌했을 경우 1
    while (1) {
        food_crush_on = 0;
        srand((unsigned)time(NULL) + r); //난수표생성
        g_itemx = (rand() % 18) + 1; //난수를 좌표값에
        g_itemy = (rand() % 18) + 1;

        if (map[stage][g_itemy][g_itemx] != 0) { //item이 맵에서 0이 아닌 부분과 만나면
            food_crush_on = 1; //on
            r++;
        }
        if (food_crush_on == 1) continue; //부딪히면 while문 다시 시작

        map[stage][g_itemy][g_itemx] = 5; //안부딪히면 좌표에 item을 찍음
        break;
    }

}
void poison_item() {
    int r = 3;
    int food_crush_on = 0;
    while (1) {
        food_crush_on = 0;
        srand((unsigned)time(NULL) + r);
        p_itemx = (rand() % 18) + 1;
        p_itemy = (rand() % 18) + 1;

        if (map[stage][p_itemy][p_itemx] != 0) {
            food_crush_on = 1;
            r++;
        }
        if (food_crush_on == 1) continue;

        map[stage][p_itemy][p_itemx] = 6;
        break;

    }
}
void gate() {
    srand((unsigned)time(NULL));
    while (1) {
        gate1_x = (rand() % 21); //랜덤으로 gate1의 x,y좌표 설정
        gate1_y = (rand() % 21);
        if (map[stage][gate1_y][gate1_x] == 1) {  //난수가 wall이 될때까지 실행함
            map[stage][gate1_y][gate1_x] = 7;       // wall이면 gate로 변경
            break;
        }
    }
    srand((unsigned)time(NULL));
    while (1) {
        gate2_x = (rand() % 21); //랜덤으로 gate2의 x,y좌표 설정
        gate2_y = (rand() % 21);
        if (map[stage][gate2_y][gate2_x] == 1) {
            map[stage][gate2_y][gate2_x] = 7;
            break;
        }
    }
}

void timer()
{
    g_timer --;
    p_timer --;
    movetimer --;

    if(g_timer == 0)
    {
        map[stage][g_itemy][g_itemx] = 0;
        growth_item();
        g_timer = 100;
    }
    if(p_timer == 0)
    {
        map[stage][p_itemy][p_itemx] = 0;
        poison_item();
        p_timer = 100;
    }
    if(movetimer == 0)
    {
        move();
        movetimer = 5;
    }
}
void setstage(int n)
{
    clear();
    mvprintw(winy/2, (winx-7)/2, "Stage %d", stage+1);
    refresh();
    usleep(2000000);
    
    movetimer = 5;
    g_timer = 100;
    p_timer = 100;
    gatecount = 0;

    chkgate = 0;
    chkgrowth = 0;
    chkp = 0;
    maxbody = 3;

    vector<int>().swap(snakex);
    vector<int>().swap(snakey);

    switch (stage)
    {
    case 0:
        snakex.push_back(9);
        snakey.push_back(9); // head (9, 9)
        snakex.push_back(10); 
        snakey.push_back(9); // body (10, 9)
        snakex.push_back(11);
        snakey.push_back(9); // body (11, 9)
        dir = 'L';
        break;
    case 1:
        snakex.push_back(10);
        snakey.push_back(9); // head (9, 9)
        snakex.push_back(10); 
        snakey.push_back(10); // body (10, 9)
        snakex.push_back(10);
        snakey.push_back(11); // body (11, 9)
        dir = 'U';
        break;
    case 2:
        snakex.push_back(9);
        snakey.push_back(9); // head (9, 9)
        snakex.push_back(10); 
        snakey.push_back(9); // body (10, 9)
        snakex.push_back(11);
        snakey.push_back(9); // body (11, 9)
        dir = 'L';
        break;
    case 3:
        snakex.push_back(9);
        snakey.push_back(17); // head (9, 9)
        snakex.push_back(10); 
        snakey.push_back(17); // body (10, 9)
        snakex.push_back(11);
        snakey.push_back(17); // body (11, 9)
        dir = 'L';
        break;
    }

    growth_item();
    poison_item();
    gate();
    setgoal();
}

void setgoal()
{
    srand((unsigned)time(NULL) + 111);
    m_body = (rand() % 4) + 5; // 몸 길이 5 - 8
    m_growth = (rand() % 5) + 3;
    m_poison = (rand() % 3) + 1;
    m_gate = (rand() % 5) + 1;
}

void chkgoal()
{
    if(chkgate >= m_gate && chkgrowth >= m_growth && chkp >= m_poison && maxbody >= m_body)
    {
        if(stage < 3)
        {
            clear();
            mvprintw(winy/2, (winx-20)/2, "You Cleared Stage %d!", stage+1);
            mvprintw(winy/2+1, (winx-31)/2, "Let's move on to the Next Stage");
            refresh();
            usleep(2000000);
            stage ++;
            setstage(stage);
        }
        else
        {
            clear();
            mvprintw(winy/2, (winx-22)/2, "You Cleared All Stages");
            mvprintw(winy/2+1, (winx-10)/2, "Great Job!");
            refresh();
            usleep(2000000);
            fail = true;
            clearall = true;
        }
    }
}
