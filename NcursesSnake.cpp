#include <ncurses.h>
#include <unistd.h>

#define tick 10000 // 틱레이트 마이크로초 단위 1초에 100틱

bool fail;
int stage;
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