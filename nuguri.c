#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

#define MAX_ENEMIES 15 // 최대 적 개수 증가
#define MAX_COINS 30   // 최대 코인 개수 증가
#define MAX_LIVES 3 // 최대 목숨 지정

// 구조체 정의
typedef struct {
    int x, y;
    int dir; // 1: right, -1: left
} Enemy;

typedef struct {
    int x, y;
    int collected;
} Coin;

typedef struct{
    int height;
    int width;
    char **map;
    struct Stage *next;
}Stage;

// 전역 변수
int player_x, player_y;
int stage = 0;
int score = 0;
int lives = MAX_LIVES;
int game_over = 0;

int MAX_STAGES;


// 플레이어 상태
int is_jumping = 0;
int velocity_y = 0;
int on_ladder = 0;

// 게임 객체
Enemy enemies[MAX_ENEMIES];
int enemy_count = 0;
Coin coins[MAX_COINS];
int coin_count = 0;

#ifndef _WIN32
// 터미널 설정 (리눅스/WSL 전용)
struct termios orig_termios;
#endif

// 함수 선언
void disable_raw_mode();
void enable_raw_mode();

void init_stage(int, int, char**);
void draw_game(int, int, char**);
void update_game(char input, int, int, char**);
void move_player(char input, int, int, char**);
void move_enemies(int, int, char**);
void check_collisions(int, int, char**);
int kbhit();
void title_menu();
void clear();
void sound();

Stage* load_maps();
Stage* append(Stage* , char** , int , int );


#ifdef _WIN32
void clrscr(void)
{
  system("cls"); 
}
#else
void clrscr()
{                             
  printf("\033[2J\033[1;1H"); 
  fflush(stdout);             
}
#endif

int main() {
#ifdef _WIN32
    setlocale(LC_ALL, ".UTF8");               
    system("chcp 65001 > nul");                 
   
#endif

    title_menu();

    Stage* head = load_maps();

    Stage *cur = head;

    Stage *temp = head;
    while(temp != NULL) {
        MAX_STAGES++;
        temp = temp -> next;
    }
    
    stage = 0;

    srand(time(NULL));
    enable_raw_mode();
    init_stage(head -> height, head-> width, head ->map);

    lives = MAX_LIVES;
    game_over = 0;


    char c = '\0';

    while (!game_over && stage < MAX_STAGES) {
        
#ifdef _WIN32
        if (kbhit()) {
            c = (char)_getch();

            if (c == 'q' || c == 'Q') {
                game_over = 1;
                continue;
            }

            // 방향키 처리 (0 또는 0xE0 후에 코드 들어옴)
            if (c == 0 || c == (char)0xE0) {
                int code = _getch();
                switch (code) {
                    case 72: c = 'w'; break; // Up
                    case 80: c = 's'; break; // Down
                    case 77: c = 'd'; break; // Right
                    case 75: c = 'a'; break; // Left
                    default: c = '\0'; break;
                }
            }

            // 버퍼에 남아 있는 추가 키 버리기
            while (kbhit()) _getch();

        } else {
            c = '\0';
        }
#else

        if (kbhit()) {
            c = getchar();
            if (c == 'q') {
                game_over = 1;
                continue;
            }
            if (c == '\x1b') {
                getchar(); // '['
                switch (getchar()) {
                    case 'A': c = 'w'; break; // Up
                    case 'B': c = 's'; break; // Down
                    case 'C': c = 'd'; break; // Right
                    case 'D': c = 'a'; break; // Left
                }
            }
            while (kbhit()) getchar();
        } else {
            c = '\0';
        }
    #endif
        update_game(c, cur->height, cur->width, cur-> map);
        draw_game(cur -> height, cur-> width, cur-> map);
        #ifdef _WIN32
            Sleep(90);    
        #else
            usleep(90000);
        #endif
        if (cur->map[player_y][player_x] == 'E') {
            stage++;
            score += 100;

            cur = cur -> next;

            if (stage < MAX_STAGES) {
                init_stage(cur->height, cur->width, cur-> map);
            } else {
                game_over = 1;
    
                clrscr();
    
                clear();
                printf("축하합니다! 모든 스테이지를 클리어했습니다!\n");
                printf("최종 점수: %d\n", score);
            }
        }
    }

    if(lives <= 0 && stage<MAX_STAGES){

        clrscr();

        printf("GAME OVER!\n");
        printf("최종 점수: %d\n", score);
    }

    disable_raw_mode();
    return 0;
}

void title_menu() {
    int a;
    while(1) {
        clrscr();

        printf("\n\n\n\n\n");
        printf("           NN   NN  UU     UU   GGGGGG   UU     UU  RRRRRRR    IIIII         \n");
        printf("           NNN  NN  UU     UU  GG        UU     UU  RR    RR    III          \n");
        printf("           NN N NN  UU     UU  GG   GGG  UU     UU  RRRRRRR     III          \n");
        printf("           NN  NNN  UUU   UUU  GG    GG  UUU   UUU  RR   RR     III          \n");
        printf("           NN   NN   UUUUUUU    GGGGGG    UUUUUUU   RR    RR   IIIII         \n\n");
        printf("                                 1. START_GAME                               \n\n");
        printf("                                  2. END_GAME                                \n\n");
        printf("            Press Input Number : ");
        if (scanf("%d", &a) != 1)  {
            while (getchar() != '\n');
            printf("\n            Input A Number");
            getchar();
            continue;
        }

        if (a == 1) {
            break;
        } else if (a == 2) {
            exit(0);
        } else {
            while (getchar() != '\n');           
            printf("\n            Input '1' or '2'");            
            getchar();
            continue;
        }
    }
    return;
}


#ifndef _WIN32
// 리눅스/WSL: 터미널 Raw 모드 활성화/비활성화
void disable_raw_mode(){ 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
}

void enable_raw_mode(){
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#else
void disable_raw_mode(){ }
void enable_raw_mode(){ }
#endif

// 현재 스테이지 초기화
void init_stage(int height, int width, char** map) {
    enemy_count = 0;
    coin_count = 0;
    is_jumping = 0;
    velocity_y = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char cell = map[y][x];
            if (cell == 'S') {
                player_x = x;
                player_y = y;
            } else if (cell == 'X' && enemy_count < MAX_ENEMIES) {
                enemies[enemy_count] = (Enemy){x, y, (rand() % 2) * 2 - 1};
                enemy_count++;
            } else if (cell == 'C' && coin_count < MAX_COINS) {
                coins[coin_count++] = (Coin){x, y, 0};
            }
        }
    }
}

// 게임 화면 그리기
void draw_game(int height, int width, char** map) {

    clrscr();

    printf("Stage: %d | Score: %d | Lives: %d\n", stage + 1, score, lives);
    printf("조작: ← → (이동), ↑ ↓ (사다리), Space (점프), q (종료)\n");

    char** display_map = malloc(sizeof(char*) * height);

    for(int y=0; y < height; y++) {
        display_map[y] = malloc(sizeof(char) * (width + 1));
        for(int x=0; x < width; x++) {
            char cell = map[y][x];
            if (cell == 'S' || cell == 'X' || cell == 'C') {
                display_map[y][x] = ' ';
            } else {
                display_map[y][x] = cell;
            }
        }
    }
    
    for (int i = 0; i < coin_count; i++) {
        if (!coins[i].collected) {
            display_map[coins[i].y][coins[i].x] = 'C';
        }
    }

    for (int i = 0; i < enemy_count; i++) {
        display_map[enemies[i].y][enemies[i].x] = 'X';
    }

    display_map[player_y][player_x] = 'P';

    for (int y = 0; y < height; y++) {
        for(int x=0; x< width; x++){
            printf("%c", display_map[y][x]);
        }
        printf("\n");
    }
}

// 게임 상태 업데이트
void update_game(char input, int height, int width, char** map) {
    move_player(input, height, width, map);
    move_enemies(height, width, map);
    check_collisions(height, width, map);
}
// 플레이어 이동 로직
void move_player(char input, int height, int width, char** map) {
    int next_x = player_x, next_y = player_y;
    char floor_tile = (player_y + 1 < height) ? map[player_y + 1][player_x] : ' ';
    char current_tile = map[player_y][player_x];

    on_ladder = (current_tile == 'H');

    switch (input) {
        case 'a': next_x--; break;
        case 'd': next_x++; break;
        case 'w': if (on_ladder) next_y--; break;
        case 's': if (on_ladder && (player_y + 1 < height) && map[player_y + 1][player_x] != '#') next_y++; break;
        case ' ':
            if (!is_jumping && (floor_tile == '#' || on_ladder)) {
                is_jumping = 1;
                velocity_y = -3;
            }
            sound();
            if(on_ladder && map[player_y-1][player_x]=='#') player_y-=1;
            break;
    }

    if (next_x >= 0 && next_x < width && map[player_y][next_x] != '#') player_x = next_x;
    
    if (on_ladder && (input == 'w' || input == 's')) {
        if(next_y >= 0 && next_y < height && map[next_y][player_x] != '#') {
            player_y = next_y;
            is_jumping = 0;
            velocity_y = 0;
        }
    } 
    else {
        if (is_jumping) {
            if(velocity_y<0){
            	next_y = player_y -1;
            }
	        else if(velocity_y == 0){
                next_y = player_y;
            }
            else{
			    next_y=player_y+1;}

            if(next_y < 0) next_y = 0;
            velocity_y++;

            if (velocity_y <= 0 && next_y < height && map[next_y][player_x] == '#') {
                velocity_y = 0;

            } 
            else if (velocity_y>0&&map[next_y][player_x]=='#'){
                player_y;
            } 
            else if (next_y < height) {

                player_y = next_y;
            }
            
            if ((player_y + 1 < height) && map[player_y + 1][player_x] == '#') {
                is_jumping = 0;
                velocity_y = 0;
            }

            if (player_y + 1 >= height) player_y++;

        } else {
            if (floor_tile != '#' && floor_tile != 'H') {
                 if (player_y + 1 < height) player_y++;
                 else  player_y++;
            }
        }
        
    }
    if (player_y >= height) {
        init_stage(height, width, map);
        lives--;
    }
}

// 적 이동 로직
void move_enemies(int height, int width, char** map) {
    for (int i = 0; i < enemy_count; i++) {
        int next_x = enemies[i].x + enemies[i].dir;
        if (next_x < 0 || next_x >= width || map[enemies[i].y][next_x] == '#' || (enemies[i].y + 1 < height && map[enemies[i].y + 1][next_x] == ' ')) {
            enemies[i].dir *= -1;
        } else {
            enemies[i].x = next_x;
        }
    }
}

// 충돌 감지 로직
void check_collisions(int height, int width, char** map) {
    for (int i = 0; i < enemy_count; i++) {
        if (player_x == enemies[i].x && player_y == enemies[i].y) {
            lives--;
            score = (score > 50) ? score - 50 : 0;
            
            if(lives > 0){
                init_stage(height, width, map);
            }else if(lives <=0){
                game_over = 1;
            }
            return;
        }
    }
    for (int i = 0; i < coin_count; i++) {
        if (!coins[i].collected && player_x == coins[i].x && player_y == coins[i].y) {
            coins[i].collected = 1;
            score += 20;
        }
    }
}

// 비동기 키보드 입력 확인
int kbhit() {
#ifdef _WIN32
    return _kbhit();
#else
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
#endif
}

void clear(){
    printf(" ██████╗██╗     ███████╗ █████╗ ██████╗ \n");
    printf("██╔════╝██║     ██╔════╝██╔══██╗██╔══██╗\n");
    printf("██║     ██║     █████╗  ███████║██████╔╝\n");
    printf("██║     ██║     ██╔══╝  ██╔══██║██╔══██╗\n");
    printf("╚██████╗███████╗███████╗██║  ██║██║  ██║\n");
    printf(" ╚═════╝╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝\n");
}

void sound() {
#ifdef _WIN32
    Beep(800, 150);
#elif defined(__APPLE__)
    system("afplay /System/Library/Sounds/Glass.aiff &");
#elif defined(__linux__)
    printf("\a");
    fflush(stdout);
#endif
}
Stage* append(Stage *head, char **map, int height, int width) {
    Stage* newnode = malloc(sizeof(Stage));
    newnode -> map = map;
    newnode -> width = width;
    newnode -> height = height;
    newnode -> next = NULL;

    if(head == NULL) return newnode;

    Stage* temp = head;
    while(temp -> next != NULL) temp = temp ->next;
    temp -> next = newnode;

    return head;
}

Stage* load_maps() {
    FILE *file = fopen("map.txt", "r");
    if (!file) {
        perror("map.txt 파일을 열 수 없습니다.");
        exit(1);
    }

    Stage* head = NULL;
    char** map = malloc(sizeof(char*));
    int height = 0;
    int width = 0;
    int stage = 0;
    
    char line[2048];

    while (fgets(line, sizeof(line), file)) {
        if ((line[0] == '\n' || line[0] == '\r') && height > 0) {
            
            head = append(head, map, height, width);

            map = malloc(sizeof(char*));
            height = 0;
            width = 0;

            stage++;
            continue;
        }

        int len = strcspn(line, "\n\r");
        if (len > width) width = len;

        map = realloc(map, sizeof(char*) * (height + 1));
        map[height] = malloc(sizeof(char) * (len + 1));
        strncpy(map[height], line, len);
        map[height][len] = 0;

        height++;  
    }
    if (height > 0) {
        head = append(head, map, height, width);    
    }

    fclose(file);
    return head;
}