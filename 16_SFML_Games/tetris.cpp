#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int MAX_ROWS = 20;
const int MAX_COLUMNS = 10;
const int NUM_FIGURES = 7;
const int FIGURE_BLOCKS = 4;
const int TILE_SIZE = 18;
const int TILE_OFFSET_X = 28;
const int TILE_OFFSET_Y = 31;
const float DEFAULT_DELAY = 0.3f;
const float FAST_DROP_DELAY = 0.05f;
const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 480;

struct Point { int x, y; };

struct GameState
{
    int deltaX = 0;
    bool rotate = false;
    int colorNum = 1;
    float timer = 0;
    float delay = DEFAULT_DELAY;

    int field[MAX_ROWS][MAX_COLUMNS] = { 0 };

    Point coordinates[FIGURE_BLOCKS];
    Point backup_coordinates[FIGURE_BLOCKS];
};

int figures[NUM_FIGURES][FIGURE_BLOCKS] =
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
    for (int i = 0; i < FIGURE_BLOCKS; i++)
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
    for (int i = 0; i < FIGURE_BLOCKS; i++)
    {
        state.backup_coordinates[i] = state.coordinates[i];
        state.coordinates[i].x += state.deltaX;
    }
    if (!check(state))
    {
        for (int i = 0; i < FIGURE_BLOCKS; i++)
            state.coordinates[i] = state.backup_coordinates[i];
    }
}

void rotation(GameState& state)
{
    if (state.rotate)
    {
        Point center = state.coordinates[1];
        for (int i = 0; i < FIGURE_BLOCKS; i++)
        {
            int x = state.coordinates[i].y - center.y;
            int y = state.coordinates[i].x - center.x;
            state.coordinates[i].x = center.x - x;
            state.coordinates[i].y = center.y + y;
        }
        if (!check(state))
        {
            for (int i = 0; i < FIGURE_BLOCKS; i++)
                state.coordinates[i] = state.backup_coordinates[i];
        }
    }
}

void ticking(GameState& state)
{
    if (state.timer > state.delay)
    {
        for (int i = 0; i < FIGURE_BLOCKS; i++)
        {
            state.backup_coordinates[i] = state.coordinates[i];
            state.coordinates[i].y += 1;
        }

        if (!check(state))
        {
            for (int i = 0; i < FIGURE_BLOCKS; i++)
                state.field[state.backup_coordinates[i].y][state.backup_coordinates[i].x] = state.colorNum;

            state.colorNum = 1 + rand() % NUM_FIGURES;
            int nextShape = rand() % NUM_FIGURES;

            for (int i = 0; i < FIGURE_BLOCKS; i++)
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
    state.delay = DEFAULT_DELAY;
}

int tetris()
{
    srand(time(0));

    GameState state;

    int shape = rand() % NUM_FIGURES;
    for (int i = 0; i < FIGURE_BLOCKS; i++)
    {
        state.coordinates[i].x = figures[shape][i] % 2;
        state.coordinates[i].y = figures[shape][i] / 2;
    }

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The Game!");

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
            state.delay = FAST_DROP_DELAY;

        move(state);
        rotation(state);
        ticking(state);
        lineCheck(state);
        reset(state);

        window.clear(Color::White);
        window.draw(backgroundSprite);

        for (int i = 0; i < MAX_ROWS; i++)
        {
            for (int j = 0; j < MAX_COLUMNS; j++)
            {
                if (!state.field[i][j]) continue;
                tilesSprite.setTextureRect(IntRect(state.field[i][j] * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
                tilesSprite.setPosition(j * TILE_SIZE, i * TILE_SIZE);
                tilesSprite.move(TILE_OFFSET_X, TILE_OFFSET_Y);
                window.draw(tilesSprite);
            }
        }

        for (int i = 0; i < FIGURE_BLOCKS; i++)
        {
            tilesSprite.setTextureRect(IntRect(state.colorNum * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
            tilesSprite.setPosition(state.coordinates[i].x * TILE_SIZE, state.coordinates[i].y * TILE_SIZE);
            tilesSprite.move(TILE_OFFSET_X, TILE_OFFSET_Y);
            window.draw(tilesSprite);
        }

        window.draw(frameSprite);
        window.display();
    }

    return 0;
}
