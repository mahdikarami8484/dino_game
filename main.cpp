#include <thread>
#include <string>
#include <map>
#include <atomic>
#include <vector>
#include <ncurses.h>
#include <iostream>


const int update_time = 50;

struct vector_2d
{
    int y, x;
};

enum Color {
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  // ... add more colors as needed
};

std::map<Color, int> colorCodes = {
  { BLACK , 0},
  { WHITE , 1},
  { GREEN , 2},
  { YELLOW, 3}
};

// for split
std::vector<std::string> split(std::string str, std::string delimiter)
{
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        tokens.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }

    return tokens; 
}

class Ground
{
    public:
    Ground()
    {
        start_pos.y, start_pos.x, end_pos.y, end_pos.x = 0;
    }

    void set_pos(int start_pos_y, int start_pos_x, int end_pos_y, int end_pos_x)
    {
        for(int i = 0; i <= end_pos.y-start_pos.y; i++)
        {
            for(int j = 0; j <= end_pos.x-start_pos.x; j++){
                mvprintw(end_pos.y - i, end_pos.x - j, " ");
            }
        }
        
        start_pos.y = start_pos_y;
        start_pos.x = start_pos_x;
        end_pos.y = end_pos_y;
        end_pos.x = end_pos_x;
        draw();
    }

    std::map<std::string, vector_2d> get_pos()
    {
        std::map<std::string, vector_2d> pos;
        pos["start_pos"] = start_pos;
        pos["end_pos"] = end_pos;

        return pos; 
    }

    void draw()
    {
        for(int i = 0; i <= end_pos.y-start_pos.y; i++)
        {
            for(int j = 0; j <= end_pos.x-start_pos.x; j++){
                attron(COLOR_PAIR(colorCodes[YELLOW]));
                mvprintw(end_pos.y - i, end_pos.x - j, "%s", sprite.data());
            }
        }
        refresh();
    
    }

    private:
    std::string sprite = "*";
    Color background = RED;
    vector_2d start_pos;
    vector_2d end_pos;   
};
Ground ground;

class Player
{
    public:
    Player(Ground* ground_ptr) : ground_ptr(ground_ptr)
    {
        pos.y, pos.x = 0;

        floor = split(sprite, "\n");
        sprite_height = floor.size();

        for(int i = 0; i < sprite_height; ++i)
        {
            for (int j = 0; j < floor[i].size(); ++j) {
                spaces += " ";
            }
            spaces += "\n";
        }
    }

    void set_pos(int pos_y, int pos_x)
    {
        mvprintw(pos.y, pos.x, "%s",  spaces.data());
        pos.y = pos_y;
        pos.x = pos_x;
        draw();
    }

    int get_sprite_height()
    {
        return sprite_height;
    }

    vector_2d get_pos()
    {
        return pos;
    }

    void draw()
    {
        attron(COLOR_PAIR(colorCodes[WHITE]));
        for(int i = 0; i < sprite_height; ++i){
            mvprintw(pos.y+i, pos.x, "%s", floor[i].data());
        }
        refresh();
    }

    void jump()
    {
        Ground ground = *ground_ptr;
        if(ground.get_pos()["start_pos"].y-sprite_height-2 >= pos.y) return;
        int i = 0;
        while(i < jump_force){
            mvprintw(pos.y, pos.x, "%s", spaces.data());
            pos.y--;
            draw();
            i++;
            std::this_thread::sleep_for(std::chrono::milliseconds(update_time));
        }
        
    }

    void gravity()
    {
        Ground ground = *ground_ptr;
        if(ground.get_pos()["start_pos"].y - sprite_height > pos.y)
        {
            
            mvprintw(pos.y, pos.x, "%s", spaces.data());
            pos.y += 1;
            draw();
        }
    }

    void right()
    {
        mvprintw(pos.y, pos.x, "%s", spaces.data());
        pos.x++;
        draw();
    }

    void left()
    {
        mvprintw(pos.y, pos.x, "%s", spaces.data());
        pos.x--;
        draw();
    }

    private:
    std::string sprite = "OO_OO\nOO-OO\n";
    std::string spaces = "";
    std::vector<std::string> floor;
    int sprite_height = 1;
    int jump_force = 5;
    Ground* ground_ptr;
    vector_2d pos;    
};
Player player(&ground);

class Barrier
{
    public:
    Barrier(Ground* ground_ptr) : ground_ptr(ground_ptr)
    {
        pos.y, pos.x = 0;

        floor = split(sprite, "\n");
        sprite_height = floor.size();

        std::cout << sprite.size() << std::endl;
        
        for(int i = 0; i < sprite_height; ++i)
        {
            for (int j = 0; j < floor[i].size(); ++j) {
                spaces += ' ';
            }
            
            spaces += "\n";
        }
    }

    void set_pos(int pos_y, int pos_x)
    {
        mvprintw(pos.y, pos.x, "%s",  spaces.data());
        pos.y = pos_y;
        pos.x = pos_x;
        draw();
    }

    int get_sprite_height()
    {
        return sprite_height;
    }

    vector_2d get_pos()
    {
        return pos;
    }

    void draw()
    {
        attron(COLOR_PAIR(colorCodes[GREEN]));
        for(int i = 0; i < sprite_height; ++i){
            mvprintw(pos.y+i, pos.x, "%s", floor[i].data());
        }
        refresh();
    }

    // void right()
    // {
    //     mvprintw(pos.y, pos.x, "%s", spaces.data());
    //     pos.x++;
    //     draw();
    // }

    // void left()
    // {
    //     mvprintw(pos.y, pos.x, "%s", spaces.data());
    //     pos.x--;
    //     draw();
    // }

    private:
    std::string sprite = "    OO\nOO  OO  OO\nOOOOOOOOOO\n    OO\n    OO\n    OO\n";
    std::string spaces = "";
    std::vector<std::string> floor;
    int sprite_height = 1;
    int jump_force = 5;
    Ground* ground_ptr;
    vector_2d pos;    
};
Barrier barrier(&ground);

vector_2d window_max;

std::atomic<int> key(0);


// for draw objects
void draw()
{
    ground.draw();
    player.draw();
    barrier.draw();
    refresh();
}


void handle_key()
{
    if (key == ' ') {
        player.jump();
        key.store(0);
    }
    if(key == 'D' || key == 'd'){
        player.right();
        key.store(0);
    }
    if(key == 'A' || key == 'a'){
        player.left();
        key.store(0);
    }
}

void get_key()
{
    int ch;
    while (true) {
        ch = getch();
        key.store(ch); 
    }
}

void init_colors()
{
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

}

void start()
{
    initscr();
    start_color(); // Initialize color support
    init_colors(); // for create colors
    noecho();
    cbreak();
    keypad(stdscr, TRUE); // Enable keypad mode to capture function keys
    getmaxyx(stdscr, window_max.y, window_max.x);
    ground.set_pos(window_max.y - 2, 0, window_max.y, window_max.x);
    player.set_pos(ground.get_pos()["start_pos"].y - player.get_sprite_height() - 1, window_max.x/2 - 6);
    barrier.set_pos(ground.get_pos()["start_pos"].y - barrier.get_sprite_height() , window_max.x/2 + 4);
}

void update()
{
    player.gravity();
    handle_key();
    std::this_thread::sleep_for(std::chrono::milliseconds(update_time));
}

// root function
int main()
{
    start();
    draw();
    std::thread get_key_thread(get_key);

    // game loop ( game is a loop :D ) 
    while(true)
    {
        update();
    }

    endwin();

    get_key_thread.join();
    return 0;
}
