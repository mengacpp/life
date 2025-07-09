/*  GAME OF LIFE
 *
 *  IMPORTANT: Input handling not supported on Windows
 */

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
include <conio.h>
// TODO implement threads on windows
#else
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#endif

// draw a spaceship
bool lwss = false;
bool ggg = false;
bool pacman = false;

float aspect_ratio = 2.35f / 1.0f, fps = 2.0f;

char live_char = '#', dead_char = ' ';

size_t rows, buf_rows;
size_t cols, buf_cols;

size_t border = 100;
size_t despawn_freq = 20;
size_t despawn_border = 30;

bool running = false;

float delta_t();
void sleep_ms(uint64_t ms);

size_t get_id(size_t col, size_t row);
size_t buf_get_id(size_t col, size_t row);

void setup_buf(char buf[]);
void update_buf(bool const grid[], bool const changed[], char buf[]);
void draw_buf(char const buf[]);
void draw_frame(bool const grid[], bool const changed[], char buf[]);

void clear_screen();
void hide_cursor();
void show_cursor();
int getch();

void start_input_handler();
void *input_handler(void* args);

void set(bool grid[], bool changed[], size_t col, size_t row, bool new_val);
void iterate(bool grid[], bool changed[], uint64_t *iter);
void update_neighbours(uint8_t grid[], size_t col, size_t row); 

void spawn_lwss(bool grid[], bool changed[], size_t col, size_t row);
void spawn_ggg(bool grid[], bool changed[], size_t col, size_t row);

void init();

void parse_args(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    bool grid[cols * rows]; 
    memset(grid, 0, sizeof grid);

    bool changed[cols * rows];
    memset(changed, 0, sizeof changed);

    char scr_buf[buf_cols * buf_rows];
    setup_buf(scr_buf);
    
    if(lwss)
        spawn_lwss(grid, changed, 10, 10);
    else if(ggg)
        spawn_ggg(grid, changed, 10, 10);

    uint64_t iter = 0;
    init();
    draw_frame(grid, changed, scr_buf);
    while(running)
    {
        iterate(grid, changed, &iter);
        draw_frame(grid, changed, scr_buf);
    }


    clear_screen();
    show_cursor();

    return 0;
}

void parse_args(int argc, char *argv[])
{
    size_t scr_cols = 20, scr_rows = 20;

    for (size_t i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "--lwss") == 0)
        {
            lwss = true; 
        }
        if(strcmp(argv[i], "--ggg") == 0)
        {
            ggg = true; 
        }
        if(strcmp(argv[i], "--pacman") == 0)
        {
            pacman = true; 
        }
        else if(strcmp(argv[i], "--rows") == 0)
        {
            if(i >= argc - 1)
                exit(1);

            scr_rows = strtol(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "--cols") == 0)
        {
            if(i >= argc - 1)
                exit(1);

            scr_cols = strtol(argv[++i], NULL, 10);
        }
        else if(strcmp(argv[i], "--fps") == 0)
        {
            if(i >= argc - 1)
                exit(1);

            fps = strtol(argv[++i], NULL, 10);
        }
    }

    rows = scr_rows + border * 2;
    cols = scr_cols + border * 2;

    buf_rows = scr_rows;
    buf_cols = (scr_cols * aspect_ratio) + 1;
}

void init()
{
    running = true;
    start_input_handler();
    hide_cursor();
}

void *input_handler(void* args)
{
    while (running) 
    {
        int c = getch();

        if(!c) continue;

        if(c == 'q')
        {
            running = false;
            break;
        }

        if(c == '+')
        {
            fps += 1.0f;
        }

        if(c == '-')
        {
           if(fps > 1.0f)
               fps -= 1.0f;
        }

    }
    
    return NULL;
}

void iterate(bool grid[], bool changed[], uint64_t *iter)
{
    memset(changed, 0, cols * rows);
    uint8_t alive_neighbours[cols * rows];
    memset(alive_neighbours, 0, sizeof alive_neighbours);

    for (size_t row = 0; row < rows; ++row) {
        for (size_t col= 0; col < cols; ++col) {
            if(!grid[get_id(col, row)]) continue;
            update_neighbours(alive_neighbours, col, row);
        } 
    }

    for (size_t row = 1; row < rows - 1; ++row) {
        for (size_t col = 1; col < cols - 1; ++col) {
            size_t id = get_id(col, row);

            if(*iter % despawn_freq == 0)
            {
                if(row < despawn_border || row > rows - despawn_border)
                {
                    if(col < despawn_border || col > cols - despawn_border)
                    {
                        set(grid, changed, col, row, false);
                        continue;
                    }
                }
            }

            if(grid[id] && (alive_neighbours[id] != 2 && alive_neighbours[id] != 3))
            {
                set(grid, changed, col, row, false);
            }
            else if(!grid[id] && alive_neighbours[id] == 3)
            {
                set(grid, changed, col, row, true);
            }
        } 
    }

    iter++;
}


void update_neighbours(uint8_t grid[], size_t col, size_t row)
{
    size_t left_nb = 0, right_nb = 0, top_nb = 0, bottom_nb = 0;

    if(col == 0)
        left_nb = cols - 1;
    else 
        left_nb = col - 1;

    if(col == cols - 1)
        right_nb = 0;
    else
        right_nb = col + 1;

    if(row == 0)
        top_nb = rows - 1;
    else 
        top_nb = row -1;

    if(row == rows - 1)
        bottom_nb = 0;
    else
        bottom_nb = row + 1;

    grid[get_id(left_nb, top_nb)]++;
    grid[get_id(left_nb, row)]++;
    grid[get_id(left_nb, bottom_nb)]++;
    grid[get_id(right_nb, top_nb)]++;
    grid[get_id(right_nb, row)]++;
    grid[get_id(right_nb, bottom_nb)]++;
    grid[get_id(col, top_nb)]++;
    grid[get_id(col, bottom_nb)]++;
}

void set(bool grid[], bool changed[], size_t col, size_t row, bool new_val)
{
    size_t id = get_id(col, row);

    if(grid[id] == new_val) return;

    grid[id] = new_val;
    changed[id] = true;
}

void start_input_handler()
{
#ifdef _WIN32
#else
    pthread_t tid;
    int err;

    err = pthread_create(&tid,
                         NULL,
                         input_handler,      
                         NULL  
                        );
    if (err) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
#endif
}

 size_t get_id(size_t col, size_t row)
{
    return cols * row + col;
}

size_t buf_get_id(size_t col, size_t row)
{
    return buf_cols * (row - border) + ((col - border) * aspect_ratio);
}

void setup_buf(char buf[])
{
    memset(buf, dead_char, buf_cols * buf_rows);

    for (size_t row = 0; row < buf_rows; ++row) {
        buf[buf_cols * row + buf_cols - 1] = '\n'; 
    }
}

void update_buf(bool const grid[], bool const changed[], char buf[])
{
    for (size_t row = border; row < rows - border; ++row) {
        for (size_t col = border; col < cols - border; ++col) {
            int id = get_id(col, row);
            if(!changed[id]) continue;

            int buf_col_end = buf_get_id(col + 1, row);

            // dont print over \n
            if(col == cols - 1) 
                buf_col_end--;

            int buf_id = buf_get_id(col, row);
            int to_fill = buf_col_end - buf_id;

            char c = grid[id] ? live_char : dead_char;
            
            memset(buf + buf_id, c, to_fill * sizeof(char));

        }
    }
}

void draw_buf(char const buf[])
{
    clear_screen();
    printf("%s", buf);
}

void draw_frame(bool const grid[], bool const changed[], char buf[])
{
    update_buf(grid, changed, buf);
    draw_buf(buf);
    sleep_ms(delta_t());
}

void clear_screen()
{
    printf("\033[2J\033[H");
}
void hide_cursor()
{
    printf("\33[?25l");
}
void show_cursor()
{
    printf("\33[?25h");
}


int getch()
{ 
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= (unsigned)(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
#endif
}

float delta_t()
{
    return 1000.0f / fps;
}

void sleep_ms(uint64_t ms)
{
#ifdef _WIN32
    Sleep(ms) 
#else 
    usleep(ms * 1000);
#endif
}

#define ANIMATE(x, y) set(grid, changed, border + col + x, border + col + y, true)
void spawn_lwss(bool grid[], bool changed[], size_t col, size_t row)
{
    ANIMATE(0, 2);
    ANIMATE(0, 3);
    ANIMATE(1, 0);
    ANIMATE(1, 1);
    ANIMATE(1, 3);
    ANIMATE(1, 4);
    ANIMATE(2, 0);
    ANIMATE(2, 1);
    ANIMATE(2, 2);
    ANIMATE(2, 3);
    ANIMATE(3, 1);
    ANIMATE(3, 2);
}

void spawn_ggg(bool grid[], bool changed[], size_t col, size_t row)
{
    // left square
    ANIMATE(1, 5);
    ANIMATE(1, 6);
    ANIMATE(2, 5);
    ANIMATE(2, 6);

    // right square
    ANIMATE(35, 3);
    ANIMATE(35, 4);
    ANIMATE(36, 3);
    ANIMATE(36, 4);

    // 'circle'
    ANIMATE(11, 5);
    ANIMATE(11, 6);
    ANIMATE(11, 7);

    ANIMATE(12, 4);
    ANIMATE(12, 8);

    ANIMATE(13, 9);
    ANIMATE(13, 3);
    
    ANIMATE(14, 9);
    ANIMATE(14, 3);

    ANIMATE(15, 6);

    ANIMATE(16, 4);
    ANIMATE(16, 8);

    ANIMATE(17, 5);
    ANIMATE(17, 6);
    ANIMATE(17, 7);

    ANIMATE(18, 6);

    // the other thing

    ANIMATE(21, 3);
    ANIMATE(21, 4);
    ANIMATE(21, 5);

    ANIMATE(22, 3);
    ANIMATE(22, 4);
    ANIMATE(22, 5);

    ANIMATE(23, 2);
    ANIMATE(23, 6);

    ANIMATE(25, 2);
    ANIMATE(25, 1);
    ANIMATE(25, 6);
    ANIMATE(25, 7);
}
#undef ANIMATE
