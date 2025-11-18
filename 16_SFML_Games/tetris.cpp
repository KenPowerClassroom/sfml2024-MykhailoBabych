#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int MAX_ROWS = 20;
const int MAX_COLUMNS = 10;

struct Point { int x, y; };

struct GameState
{
    int deltaX = 0;
    bool rotate = false;
    int colorNum = 1;
    float timer = 0;
    float delay = 0.3;

    int field[MAX_ROWS][MAX_COLUMNS] = { 0 };

    Point coordinates[4];
    Point backup_coordinates[4];
};

int figures[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};

bool check(GameState& state)
{
    for (int i = 0; i < 4; i++)
    {
        if (state.coordinates[i].x < 0 || state.coordinates[i].x >= MAX_COLUMNS ||
            state.coordinates[i].y >= MAX_ROWS)
            return false;

        if (state.field[state.coordinates[i].y][state.coordinates[i].x])
            return false;
    }
    return true;
}

void move(GameState& state)
{
    for (int i = 0; i < 4; i++)
    {
        state.backup_coordinates[i] = state.coordinates[i];
        state.coordinates[i].x += state.deltaX;
    }
    if (!check(state))
    {
        for (int i = 0; i < 4; i++)
            state.coordinates[i] = state.backup_coordinates[i];
    }
}

void rotation(GameState& state)
{
    if (state.rotate)
    {
        Point center = state.coordinates[1];
        for (int i = 0; i < 4; i++)
        {
            int x = state.coordinates[i].y - center.y;
            int y = state.coordinates[i].x - center.x;
            state.coordinates[i].x = center.x - x;
            state.coordinates[i].y = center.y + y;
        }
        if (!check(state))
        {
            for (int i = 0; i < 4; i++)
                state.coordinates[i] = state.backup_coordinates[i];
        }
    }
}

void ticking(GameState& state)
{
    if (state.timer > state.delay)
    {
        for (int i = 0; i < 4; i++)
        {
            state.backup_coordinates[i] = state.coordinates[i];
            state.coordinates[i].y += 1;
        }

        if (!check(state))
        {
            for (int i = 0; i < 4; i++)
                state.field[state.backup_coordinates[i].y][state.backup_coordinates[i].x] = state.colorNum;

            state.colorNum = 1 + rand() % 7;
            int nextShape = rand() % 7;

            for (int i = 0; i < 4; i++)
            {
                state.coordinates[i].x = figures[nextShape][i] % 2;
                state.coordinates[i].y = figures[nextShape][i] / 2;
            }
        }

        state.timer = 0;
    }
}

void lineCheck(GameState& state)
{
    int destRow = MAX_ROWS - 1;
    for (int i = MAX_ROWS - 1; i >= 0; i--)
    {
        int count = 0;
        for (int j = 0; j < MAX_COLUMNS; j++)
        {
            if (state.field[i][j]) count++;
            state.field[destRow][j] = state.field[i][j];
        }
        if (count < MAX_COLUMNS)
            destRow--;
    }
}

void reset(GameState& state)
{
    state.deltaX = 0;
    state.rotate = false;
    state.delay = 0.3;
}

int tetris()
{
    srand(time(0));

    GameState state;

    // ------------------------------------
    // INITIALIZE FIRST TETROMINO
    // ------------------------------------
    int shape = rand() % 7;
    for (int i = 0; i < 4; i++)
    {
        state.coordinates[i].x = figures[shape][i] % 2;
        state.coordinates[i].y = figures[shape][i] / 2;
    }
    // ------------------------------------

    RenderWindow window(VideoMode(320, 480), "The Game!");

    Texture tilesTexture, backgroundTexture, frameTexture;
    tilesTexture.loadFromFile("images/tetris/tiles.png");
    backgroundTexture.loadFromFile("images/tetris/background.png");
    frameTexture.loadFromFile("images/tetris/frame.png");

    Sprite tilesSprite(tilesTexture);
    Sprite backgroundSprite(backgroundTexture);
    Sprite frameSprite(frameTexture);

    Clock clock;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        state.timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up) state.rotate = true;
                else if (e.key.code == Keyboard::Left) state.deltaX = -1;
                else if (e.key.code == Keyboard::Right) state.deltaX = 1;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Down))
            state.delay = 0.05;

        move(state);
        rotation(state);
        ticking(state);
        lineCheck(state);
        reset(state);

        window.clear(Color::White);
        window.draw(backgroundSprite);

        // draw field
        for (int i = 0; i < MAX_ROWS; i++)
        {
            for (int j = 0; j < MAX_COLUMNS; j++)
            {
                if (!state.field[i][j]) continue;
                tilesSprite.setTextureRect(IntRect(state.field[i][j] * 18, 0, 18, 18));
                tilesSprite.setPosition(j * 18, i * 18);
                tilesSprite.move(28, 31);
                window.draw(tilesSprite);
            }
        }

        // draw current tetromino
        for (int i = 0; i < 4; i++)
        {
            tilesSprite.setTextureRect(IntRect(state.colorNum * 18, 0, 18, 18));
            tilesSprite.setPosition(state.coordinates[i].x * 18, state.coordinates[i].y * 18);
            tilesSprite.move(28, 31);
            window.draw(tilesSprite);
        }

        window.draw(frameSprite);
        window.display();
    }

    return 0;
}
