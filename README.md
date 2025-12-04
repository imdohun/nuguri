# C 언어 기반 크로스 플랫폼 터미널 게임 **“Nuguri”** 실행 보고서 (README)

## 1. 기본 정보

- **학번 / 이름**: `조장:20223122 / 윤도훈` ` 20203042 / 구근환` ` 20243100 / 박성환` ` 20213120 / 이승호`

---

## 2. GitHub 정보

- **Repository URL**: `https://github.com/imdohun/nuguri`  

---

## 3. 개발 및 실행 환경

- **개발 언어**: C 
- **필수 파일**
  - `nuguri.c` : 메인 게임 로직 및 OS별 분기 처리
  - `map.txt` : 스테이지 맵 데이터 (스테이지 사이를 빈 줄로 구분, 동적 크기 로딩)
- **테스트 환경**
  - Windows 11 + WSL(Ubuntu) 터미널
  - Windows Terminal / PowerShell
  - Linux(Ubuntu) 터미널  
  - macOS는 코드 상 지원(전처리 및 사운드)만 구현, 실제 테스트는 Linux/Windows 위주

---

## 4. OS별 컴파일 및 실행 방법 가이드

### 4-1. 공통 사항

1. `nuguri.c`와 `map.txt`를 **같은 디렉터리**에 둔다.  
2. 터미널 인코딩을 **UTF-8**로 설정하면 한글 메시지가 정상 출력된다.
3. 실행 후 화살표 키와 스페이스바, `q` 키로 게임을 조작한다.

---

### 4-2. Windows (MinGW-w64, WSL 외 일반 Windows용)

#### (1) MinGW-w64 설치 후 컴파일

```bash
gcc -o nuguri.exe nuguri.c
```

- `windows.h`, `conio.h`를 사용하며, `Beep`, `_kbhit`, `_getch` 등 Windows 콘솔 API를 통해 입력과 사운드를 처리한다.  

#### (2) 실행

```bash
./nuguri.exe
```

- Windows 콘솔에서 한글 출력 및 UTF-8 처리를 위해 `main()` 시작 부분에서  
  `setlocale(LC_ALL, ".UTF8");` 와 `system("chcp 65001 > nul");` 을 호출한다.  

---

### 4-3. Linux / WSL(Ubuntu)

#### (1) 컴파일

```bash
gcc -o nuguri nuguri.c
```

- `unistd.h`, `termios.h`, `fcntl.h`를 사용하여 **Raw 모드 + 비동기 입력(kbhit)**을 직접 구현한다.  

#### (2) 실행

```bash
./nuguri
```

- `printf("\033[2J\033[1;1H")` 및 `printf("\033[H")`를 사용한 ANSI 이스케이프 코드로 화면을 갱신한다.  

---

### 4-4. macOS (터미널)

#### (1) 컴파일

```bash
gcc -o nuguri nuguri.c
```

- Linux와 동일하게 `termios` 기반 Raw 모드와 `kbhit()` 구현을 사용한다.

#### (2) 실행

```bash
./nuguri
```

- 사운드: `sound()` 함수에서 `afplay /System/Library/Sounds/Glass.aiff &`를 호출하여 점프 시 간단한 효과음을 재생하도록 구현했다.  

---

## 5. 게임 조작 및 규칙

### 5-1. 조작법

실행 중 화면 상단에 다음과 같이 조작법이 표시된다.  

- `←`, `→` : 좌우 이동 (`a`, `d`와 매핑)
- `↑`, `↓` : 사다리 위/아래 이동 (`w`, `s`와 매핑)
- `Space` : 점프
- `q` : 게임 종료

### 5-2. 주요 오브젝트

- `#` : 발판 / 벽  
- `H` : 사다리  
- `C` : 코인 (획득 시 점수 +20)  
- `X` : 적 (충돌 시 생명 감소 및 점수 감소)  
- `S` : 각 스테이지의 시작 위치  
- `E` : 스테이지 종료 지점

---

## 6. 구현 기능 리스트

### 6-1. 필수 기능

1. **크로스 플랫폼 지원**
   - `#ifdef _WIN32` 분기를 통해  
     - Windows: `<windows.h>`, `<conio.h>` + `_kbhit`, `_getch`, `Beep` 사용  
     - Linux/macOS: `<termios.h>`, `<unistd.h>`, `<fcntl.h>` + 직접 구현한 `kbhit()` 사용  
   - `clrscr()` 함수에서
     - Windows: `system("cls")`
     - Linux/macOS: ANSI 이스케이프 코드 (`\033[2J\033[1;1H`) 로 화면 정리  

2. **생명력(라이프) 시스템**
   - `MAX_LIVES`를 3으로 정의하고 `lives` 전역 변수로 관리.  
   - 적(`X`)과 충돌 시
     - `lives--`
     - 점수에서 최대 50점 감점 (`score = max(score - 50, 0)`)
     - 남은 목숨이 있으면 현재 스테이지를 다시 초기화(`init_stage`) 후 시작 위치로 이동
     - 목숨이 0이면 `game_over = 1`로 설정, GAME OVER 화면 진입  

3. **타이틀 및 엔딩 화면**
   - **타이틀 화면**:  
     - 큰 ASCII 아트로 `NUGURI` 로고 출력 후  
       - `1. START_GAME`  
       - `2. END_GAME`  
       를 선택하는 메뉴 (`title_menu()` 함수) 구현. 입력 검증 실패 시 재입력 요구.  
   - **클리어 화면**:  
     - 모든 스테이지를 통과하면 `CLEAR` ASCII 아트와 함께  
       - “축하합니다! 모든 스테이지를 클리어했습니다!”  
       - 최종 점수 출력 (`clear()` 함수).  
   - **게임 오버 화면**:  
     - 목숨이 0이 되면 "GAME OVER!" 화면 출력과 함께 최종점수 출력  
     - 플레이 도중 q를 입력 하면 즉시 게임 종료  

4. **맵 로딩 및 다중 스테이지**
   - `map.txt`를 한 줄씩 읽어들여, 빈 줄을 기준으로 스테이지를 구분한다.  
   - 각 스테이지를 `Stage` 구조체(높이/너비/맵 포인터/다음 스테이지 포인터)로 관리하는 단일 연결 리스트로 구성. (`append`, `load_maps`)  
   - `init_stage()`에서 `S`, `X`, `C`를 스캔하여 플레이어 시작 위치, 적 배열, 코인 배열을 초기화.  

5. **점프 및 중력 시스템**
   - `is_jumping`, `velocity_y`를 이용한 간단한 물리 기반 점프 구현.  
   - 사다리 위에서는 점프와 상하 이동 로직을 분리하여 자연스러운 움직임 제공.  

6. **적 이동 및 충돌 처리**
   - `Enemy` 구조체에 `dir` 필드를 두어 좌/우 이동 방향을 저장.  
   - 발판 끝이나 벽에 닿거나 아래 블록이 비어 있을 경우 방향을 반전시켜 좌우 순찰.  
   - 플레이어와 좌표가 겹치면 생명력 감소 및 스테이지 초기화/게임 오버 처리.  

7. **사운드 효과**
   - 점프 입력 시 `sound()` 함수 호출.  
     - Windows: `Beep(800, 150)`
     - macOS: `afplay`로 시스템 사운드 재생
     - Linux: `\a` (벨 문자) 출력 후 `fflush(stdout)`  

---

## 7. 실행 화면 (스크린샷)

> 실제 제출 시, 아래 예시 경로에 맞춰 스크린샷 파일을 추가한다.

1. **게임 타이틀 화면**

   - 파일 : <img width="1122" height="596" alt="Image" src="https://github.com/user-attachments/assets/32a5670f-5adb-4497-b857-363050cf663d" />
   - 내용: `NUGURI` ASCII 로고와 `1. START_GAME`, `2. END_GAME` 메뉴, 입력 프롬프트 표시.

2. **Stage 1 플레이 화면**

   - 파일 : <img width="919" height="738" alt="Image" src="https://github.com/user-attachments/assets/368ec474-20d4-4095-8fef-687f1639f67f" />
   - 상단에 `Stage: 1 | Score: 60 | Lives: 3` 표시.  
   - 사다리(`H`), 적(`X`), 코인(`C`), 출구(`E`)가 배치된 맵과 플레이어(`P`) 위치 확인 가능.

3. **Stage 2 플레이 화면**

   - 파일 : <img width="828" height="754" alt="Image" src="https://github.com/user-attachments/assets/5cc02a8d-f19d-419e-9102-03dc6224f15f" />
   - 상단에 `Stage: 2 | Score: 220 | Lives: 3`.  
   - 하단에 많은 적(`X`)이 일렬로 배치되어 있어 난이도가 증가한 모습.

4. **게임 클리어 화면**

   - 파일 : <img width="706" height="388" alt="Image" src="https://github.com/user-attachments/assets/8d54d851-c937-4d8c-bb87-a8814c2b3841" />
   - 큰 `CLEAR` ASCII 아트 및 “축하합니다! 모든 스테이지를 클리어했습니다! 최종 점수: 290” 출력.

5. **게임 오버 화면**

   - 파일 : <img width="513" height="221" alt="Image" src="https://github.com/user-attachments/assets/a5be7438-2df9-4e8a-bb30-f13749a3ac42" />
   - “GAME OVER!” 와 “최종 점수: 0” 또는 해당 시점 점수 출력.

---

## 8. 개발 중 발생한 OS 호환성 문제와 해결 과정

### 8-1. Linux/WSL 전용 코드 → Windows 컴파일 불가 문제

- **문제**  
  - 초기 제공 코드는 `termios.h`, `unistd.h`, `tcsetattr`, `tcgetattr`, `usleep` 등에 의존하고 있어 Windows에서 컴파일이 되지 않았다.  
- **해결**  
  - 전처리기 분기 사용:
    ```c
    #ifdef _WIN32
        #include <windows.h>
        #include <conio.h>
    #else
        #include <unistd.h>
        #include <termios.h>
        #include <fcntl.h>
    #endif
    ```
  - Windows에서는 콘솔 모드를 변경하지 않고 `_kbhit()`, `_getch()`를 사용하도록 `kbhit()`와 입력 루프를 분리 구현했다.  

### 8-2. 화면 깜빡임 및 커서 위치 문제

- **문제**  
  - Windows에서 `system("cls")`를 매 프레임 호출하면 화면 깜빡임이 심하고, 커서가 화면 하단으로 밀리는 현상이 발생.  
- **해결**
  - 전체 화면 지우기는 스테이지 초기화 시에만 `clrscr()`로 수행하고, 매 프레임에서는 `printf("\033[H")`로 커서를 (1,1) 위치로만 이동시켜 덮어 그리는 방식으로 변경해 깜빡임을 줄였다.  

### 8-3. 방향키 입력 처리 차이

- **문제**
  - Linux/WSL에서는 방향키가 `ESC [ A/B/C/D` 시퀀스로 들어오고, Windows에서는 0 또는 0xE0 + 스캔 코드(72, 80, 75, 77) 형태로 들어와 동일한 로직으로 처리할 수 없었다.
- **해결**
  - Linux/WSL:
    ```c
    if (c == '\x1b') {
        getchar(); // '[' 소비
        switch (getchar()) {
            case 'A': c = 'w'; break;
            case 'B': c = 's'; break;
            case 'C': c = 'd'; break;
            case 'D': c = 'a'; break;
        }
    }
    ```
  - Windows:
    ```c
    if (c == 0 || c == (char)0xE0) {
        int code = _getch();
        switch (code) {
            case 72: c = 'w'; break;
            case 80: c = 's'; break;
            case 77: c = 'd'; break;
            case 75: c = 'a'; break;
        }
    }
    ```
  - 두 플랫폼 모두에서 최종적으로 내부 입력은 `w/a/s/d/Space/q`로 통일되도록 설계했다.  

### 8-4. 키 입력 버퍼에 남은 값으로 인한 연속 입력 문제

- **문제**
  - 방향키를 길게 누른 뒤 떼면, 버퍼에 남아 있던 입력이 한 번에 처리되어 의도치 않은 여러 번의 이동이 발생했다.
- **해결**
  - 한 프레임에 필요한 입력을 처리한 뒤 `while (kbhit()) _getch();` 혹은 `while (kbhit()) getchar();` 루프로 나머지 버퍼를 비워 연속 입력을 방지했다.  

### 8-5. 사운드 출력 차이

- **문제**
  - `printf("\a")` 방식은 일부 환경에서 소리가 작게 나거나 들리지 않는 문제가 있었고, Windows에서는 더 명확한 효과음을 원했다.
- **해결**
  - OS별로 다른 구현을 둔 `sound()` 함수를 도입:
    - Windows: `Beep(800, 150)`
    - macOS: `afplay`로 시스템 사운드 재생
    - Linux: `\a` 출력  

---

## 9. 소스 코드 구조 요약

- `main()`  
  - 타이틀 출력 → `load_maps()`로 맵 로딩 → `init_stage()`로 첫 스테이지 초기화 → 게임 루프(`update_game` + `draw_game`) 실행  
- `load_maps()` / `append()`  
  - `map.txt`에서 스테이지를 읽어 동적으로 2차원 맵 배열 할당 후 `Stage` 연결 리스트 생성.  
- `init_stage()`  
  - 플레이어 시작 위치, 적/코인 정보 초기화.  
- `move_player()` / `move_enemies()` / `check_collisions()`  
  - 입력, 점프/중력, 적 이동, 충돌/점수/라이프 처리.  
- `draw_game()`  
  - 현재 맵, 플레이어, 적, 코인을 하나의 화면으로 합성 후 출력.  
- `kbhit()`, `enable_raw_mode()`, `disable_raw_mode()`  
  - OS별 비동기 입력 및 터미널 모드 제어.  


