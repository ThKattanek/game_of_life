#include <iostream>
#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

void CalcNextGeneration(bool *playfield, int row, int column);
void DrawCells(RenderWindow &window, bool *playfield, int w, int h, int row, int column);
void DrawRaster(RenderWindow &window, int w, int h, int raster_w, int raster_h);
int GetCountNeighbor(bool *playfield, int pitch, int x, int y);
void SetCellLife(bool *playfield, int pitch, int x, int y, bool life);
bool GetCellLife(bool *playfield, int pitch, int x, int y);
void Set_R_Pentomino(bool *playfield, int pitch, int x, int y);

#define WINDOW_XW 640
#define WINDOW_YW 640
#define ROW 128  // ZEILEN
#define COLUMN 128 // SPALTEN

int main()
{
    sf::Font font;
    sf::Text text;
    sf::Clock clock;
    sf::Time time1,time2;

    srand(time(NULL));

    bool playfield[(ROW+2) * (COLUMN+2)];
    for(int i=0; i<((ROW+2) * (COLUMN+2)); i++) playfield[i] = false;

    float fps_buffer[32];
    float fps = 0;
    unsigned char fps_buffer_pointer = 0;

    bool calc_enable = false;
    bool draw_raster_enable = false;

    int generation = 0;



    RenderWindow window(VideoMode(WINDOW_XW,WINDOW_YW),"Game Of Life by Th.Kattanek (C)2016");

    window.setVerticalSyncEnabled(true);

    if(!font.loadFromFile("arial.ttf"))
    {
        cout << "Fehler beim laden des Fonts. [arial.ttf]" << endl;
        return EXIT_FAILURE;
    }

    clock.restart();
    time1 = time2 = clock.getElapsedTime();

    text.setColor(Color(255,0,0,255));
    text.setFont(font);
    text.setPosition(0,0);
    text.setCharacterSize(16);
    text.setStyle(sf::Text::Bold);

    while(window.isOpen())
    {
        Event event;
        while(window.pollEvent(event))
        {
            if(event.type == Event::Closed)
                window.close();

            if(event.type == Event::KeyPressed)
            {
                if(Keyboard::isKeyPressed(Keyboard::Space))
                {
                    for(int i=0; i<5; i++)
                    {
                       int x = rand() % COLUMN;
                       int y = rand() % ROW;

                       Set_R_Pentomino(playfield, COLUMN+2, x,y);
                    }

                    calc_enable = true;

                }

                if(Keyboard::isKeyPressed((Keyboard::Escape)))
                {
                    calc_enable = false;

                    generation = 0;
                    for(int i=0; i<((ROW+2) * (COLUMN+2)); i++) playfield[i] = false;
                }

                if(Keyboard::isKeyPressed((Keyboard::R)))
                {
                    if(draw_raster_enable)
                        draw_raster_enable = false;
                    else draw_raster_enable = true;
                }
            }
        }

        window.clear(Color(255,255,255,255));

        if(calc_enable)
        {
            CalcNextGeneration(playfield,ROW,COLUMN);
            generation++;
        }

        DrawCells(window,playfield,WINDOW_XW,WINDOW_YW,ROW,COLUMN);
        if(draw_raster_enable) DrawRaster(window,WINDOW_XW,WINDOW_YW,WINDOW_XW / COLUMN, WINDOW_YW / ROW);

        time1 = clock.getElapsedTime();
        sf::Time frame_time = time1 - time2;
        time2 = time1;

        fps_buffer[fps_buffer_pointer & 0x1f] = 1.0 / frame_time.asSeconds();
        fps_buffer_pointer++;

        fps = 0;
        for(int i=0; i<8; i++)
            fps += fps_buffer[i];
        fps /= 8;

        char str1[100];
        sprintf(str1,"Space = Start, Esc = Stop, R = Raster, Gen: %d",generation);
        text.setString(str1);

        window.draw(text);
        window.display();
    }
    return 0;
}

void DrawRaster(RenderWindow &window, int w, int h, int raster_w, int raster_h)
{
    sf::VertexArray lines(sf::LinesStrip, 2);
    lines[0].color = Color(150,150,150,255);
    lines[1].color = Color(150,150,150,255);

    for(int x=0; x<w; x+=raster_w)
    {
         lines[0].position = sf::Vector2f(x, 0);
         lines[1].position = sf::Vector2f(x, h-1);
         window.draw(lines);
    }

    for(int y=0; y<h; y+=raster_h)
    {
         lines[0].position = sf::Vector2f(0, y);
         lines[1].position = sf::Vector2f(w-1, y);
         window.draw(lines);
    }
}

void DrawCells(RenderWindow &window, bool *playfield, int w, int h, int row, int column)
{
    int raster_w = w / column;
    int raster_h = h / row;

    sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(raster_w, raster_h));
    rectangle.setFillColor(Color(30,30,30,255));
    rectangle.setOutlineThickness(0);

    int pitch = column+2;

    for(int y=1; y<row+1; y++)
    {
        int start_y = pitch * y;
        for(int x=1; x<column+1; x++)
        {
            if(playfield[start_y++] == true)
            {
                rectangle.setPosition((x-1)*raster_w, (y-1)*raster_h);
                window.draw(rectangle);
            }
        }
    }
}

void CalcNextGeneration(bool *playfield, int row, int column)
{
    bool new_playfield[(row+2) * (column+2)];
    for(int i=0; i<((row+2) * (column+2)); i++ ) new_playfield[i] = false;

    for(int y=1; y<row+1; y++)
        for(int x=1; x<column+1; x++)
        {
            int neighbor = GetCountNeighbor(playfield,column+2,x,y);

            if(GetCellLife(playfield,column+2,x,y))
            {
                /// Zelle lebt
                if(neighbor < 2) SetCellLife(new_playfield,column+2,x,y,false);
                if((neighbor == 2) || (neighbor == 3)) SetCellLife(new_playfield,column+2,x,y,true);
                if(neighbor > 3) SetCellLife(new_playfield,column+2,x,y,false);
            }
            else
            {
                /// Zelle ist tod
                if(neighbor == 3) SetCellLife(new_playfield,column+2,x,y,true);
                else SetCellLife(new_playfield,column+2,x,y,false);
            }
        }

    for(int i=0; i<((row+2) * (column+2)); i++)
    {
        playfield[i] = new_playfield[i];
    }

}

int GetCountNeighbor(bool *playfield, int pitch, int x, int y)
{
    int pos = y * pitch + x;
    int count = 0;

    if(playfield[pos - 1]) count++;
    if(playfield[pos + 1]) count++;
    if(playfield[pos + pitch]) count++;
    if(playfield[pos + pitch - 1]) count++;
    if(playfield[pos + pitch + 1]) count++;
    if(playfield[pos - pitch]) count++;
    if(playfield[pos - pitch - 1]) count++;
    if(playfield[pos - pitch + 1]) count++;

    return count;
}

void SetCellLife(bool *playfield, int pitch, int x, int y, bool life)
{
    int pos = y * pitch + x;
    playfield[pos] = life;
}

bool GetCellLife(bool *playfield, int pitch, int x, int y)
{
    int pos = y * pitch + x;
    return playfield[pos];
}

void Set_R_Pentomino(bool *playfield, int pitch, int x, int y)
{
    SetCellLife(playfield,pitch,x,y+1,true);
    SetCellLife(playfield,pitch,x+1,y,true);
    SetCellLife(playfield,pitch,x+1,y+1,true);
    SetCellLife(playfield,pitch,x+1,y+2,true);
    SetCellLife(playfield,pitch,x+2,y,true);
}
