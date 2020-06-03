#include <ncurses.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define tick 10000 // 틱레이트 마이크로초 단위 1초에 100틱

bool fail; // 게임 실패 여부
int stage;
char dir; // 방향 U D L R
int movetimer; // move 함수용 타이머 시간 저장
vector<int> snakex;
vector<int> snakey; // 뱀 x, y좌표
int map[4][21][21] = {
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
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1},// width: 10~12, heigth:10
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
    }
 }; // [stage][행][열]) 일단은 스테이지 1만

void reset(); // 게임 시작시 함수 초기화
void input(); // 키 입력
void move(); // 입력 받은 키에 따라 이동
void show(); // 그래픽을 보여주는 함수


int main()
{   
    initscr(); // 터미널 초기화
    start_color(); // 컬러모드 사용
    nodelay(stdscr, true); // 키 입력 기다리지 않음
    noecho(); // 입력받은 키 출력하지 않음
    curs_set(0); // 커서 숨기기
    cbreak(); // 입력받은 키 바로 활용
    keypad(stdscr, TRUE); // 특수 키 사용 가능
    reset();
    while(!fail)
    {
        input();
        move();
        show();
        usleep(tick);
    }
    endwin(); // fail 시 종료
    return 0;
}

void reset()
{
    fail = false;
    stage = 0;
    movetimer = 0;

    snakex.push_back(9);
    snakey.push_back(9);
    snakex.push_back(10);
    snakey.push_back(9);
    snakex.push_back(11);
    snakey.push_back(9);
    dir = 'L';
}

void input()
{
    int key = getch();

    switch (key)
    {
        case KEY_UP:
            if(dir == 'D')
                fail = true;
            else
                dir = 'U';
            break;
        case KEY_DOWN:
            if(dir == 'U')
                fail = true;
            else
                dir = 'D';
            break;
        case KEY_LEFT:
            if(dir == 'R')
                fail = true;
            else
                dir = 'L';
            break;
        case KEY_RIGHT:
            if(dir == 'L')
                fail = true;
            else
                dir = 'R';
            break;
    }
}

void move()
{
    movetimer += 1;
    if(movetimer > 50)
    {
        int last = snakex.size();

        map[stage][snakey[last-1]][snakex[last-1]] = 0; // 몸통 마지막 좌표 지우기
        for(int i = last-1; i > 0; i--) // 몸통 앞으로 한 칸 씩 복제
        {
            snakex[i] = snakex[i-1];
            snakey[i] = snakey[i-1];
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

        if(map[stage][snakey[0]][snakex[0]] == 4 || map[stage][snakey[0]][snakex[0]] == 1) // Game Rule #1 실패 조건
        {
            fail = true;
        }
        else
        {
            map[stage][snakey[0]][snakex[0]] = 3; // map 에 수정된 snake의 좌표 전달
            for(int i = 1; i < last; i++)
            {
                map[stage][snakey[i]][snakex[i]] = 4;
            }
        }
        movetimer = 0;
    }
}

void show()
{
    clear();
    // stage1
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 21; j++) {
            printw("\n");
            for (int z = 0; z < 21; z++) {
                if (i == 0) {
                    if (map[i][j][z] == 2) {
                        printw("+"); //lmmune wall
                    }
                    else if (map[i][j][z] == 1) {
                        printw("-"); //wall
                    }
                    else if (map[i][j][z] == 0) {
                        printw(" ");
                    }
                    else if (map[i][j][z] == 3) {
                        printw("0"); //head
                    }
                    else if (map[i][j][z] == 4) {
                        printw("o"); //body
                    }
                }
            }
        }
    }
    refresh();
}