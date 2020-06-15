#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define tick 10000 // 틱레이트 마이크로초 단위 1초에 100틱

bool fail; // 게임 실패 여부
int stage;
char dir; // 방향 U D L R
int movetimer; // move 함수용 타이머 시간 저장
vector<int> snakex;
vector<int> snakey; // 뱀 x, y좌표
int g_itemx, g_itemy; // growth item x,y 좌표
int p_itemx, p_itemy; // poison item x,y 좌표
int gate1_x, gate1_y; // gate1의 x,y좌표
int gate2_x, gate2_y; // gate2의 x,y좌표
int gatecount; // gate 함수 재호출용

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
void growth_item(); // growth item 만드는 함수
void poison_item(); // poison item 만드는 함수
void gate(); //gate를 만드는 함수

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
    while (!fail)
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
    growth_item();
    poison_item();
    gate();
    gatecount = 0;
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
    movetimer += 1;
    if (movetimer > 50)
    {
        if (gatecount > 0)
        {
            if (gatecount == 1)
            {
                map[stage][gate1_y][gate1_x] = 1;
                map[stage][gate2_y][gate2_x] = 1;
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

        if (map[stage][snakey[0]][snakex[0]] == 4 || map[stage][snakey[0]][snakex[0]] == 1) // Game Rule #1 실패 조건
        {
            fail = true;
        }
        else if (map[stage][snakey[0]][snakex[0]] == 5) // growth item 획득시
        {
            snakex.push_back(lastx); // 몸통 뒤에 좌표 추가
            snakey.push_back(lasty);
            growth_item();
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
                poison_item();
            }
        }
        else if (map[stage][snakey[0]][snakex[0]] == 7) //Gate 통과시
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
            if(snakex[0] == gate1_x)
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
            }/*
            else // Gate 진출 방향 2
            {   
                snakex[0] = exitx;
                snakey[0] = exity;
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
                if (map[stage][snakey[0]][snakex[0]] != 7 || map[stage][snakey[0]][snakex[0]] != 1 || map[stage][snakey[0]][snakex[0]] != 2)
                {

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

                    if (map[stage][snakey[0]][snakex[0]] != 7 || map[stage][snakey[0]][snakex[0]] != 1 || map[stage][snakey[0]][snakex[0]] != 2)
                    {
                        switch (dir) // 진입 방향의 반시계방향
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
                        if (map[stage][snakey[0]][snakex[0]] != 7 || map[stage][snakey[0]][snakex[0]] != 1 || map[stage][snakey[0]][snakex[0]] != 2)
                        {
                            switch (dir) // 진입 방향과 반대 방향
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
            } */           
        }

        map[stage][snakey[0]][snakex[0]] = 3; // map 에 수정된 snake의 좌표 전달
        for (int i = 1; i < snakex.size(); i++)
        {
            map[stage][snakey[i]][snakex[i]] = 4;
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
                    else if (map[i][j][z] == 5) {
                        printw("F"); //food
                    }
                    else if (map[i][j][z] == 6) {
                        printw("P"); //poison
                    }
                    else if (map[i][j][z] == 7) {
                        printw("G"); //gate
                    }
                }
            }
        }
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