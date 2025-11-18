#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

const int MAX_ROWS = 20;
const int MAX_COLUMNS = 10;
int deltaX = 0;
bool rotate = 0;
int colorNum = 1;
float timer = 0;
float delay = 0.3;

int field[MAX_ROWS][MAX_COLUMNS] = {0};

struct Point
{int x,y;} coordinates[4], backup_coordinates[4];

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

bool check()
{
    for (int i = 0;i < 4;i++)
    {
        if (coordinates[i].x < 0 || coordinates[i].x >= MAX_COLUMNS || coordinates[i].y >= MAX_ROWS)
        {
            return false;
        }
        else if (field[coordinates[i].y][coordinates[i].x])
        {
            return false;
        }
    }
   return true;
}

void move()
{
    for (int i = 0;i < 4;i++)
    {
        backup_coordinates[i] = coordinates[i];
        coordinates[i].x += deltaX;
    }
    if (!check())
    {
        for (int i = 0;i < 4;i++)
        {
            coordinates[i] = backup_coordinates[i];
        }
    }
}

void rotation()
{
    if (rotate)
    {
        Point rotationCenter = coordinates[1]; //center of rotation
        for (int i = 0;i < 4;i++)
        {
            int x = coordinates[i].y - rotationCenter.y;
            int y = coordinates[i].x - rotationCenter.x;
            coordinates[i].x = rotationCenter.x - x;
            coordinates[i].y = rotationCenter.y + y;
        }
        if (!check())
        {
            for (int i = 0;i < 4;i++)
            {
                coordinates[i] = backup_coordinates[i];
            }
        }
    }
}

void ticking()
{
    if (timer > delay)
    {
        for (int i = 0;i < 4;i++)
        {
            backup_coordinates[i] = coordinates[i];
            coordinates[i].y += 1;
        }

        if (!check())
        {
            for (int i = 0;i < 4;i++)
            {
                field[backup_coordinates[i].y][backup_coordinates[i].x] = colorNum;
            }

            colorNum = 1 + rand() % 7;
            int nextTetrominoShape = rand() % 7;
            for (int i = 0;i < 4;i++)
            {
                coordinates[i].x = figures[nextTetrominoShape][i] % 2;
                coordinates[i].y = figures[nextTetrominoShape][i] / 2;
            }
        }

        timer = 0;
    }
}

void lineCheck()
{
	int destRow = MAX_ROWS - 1;
	for (int i = MAX_ROWS - 1;i > 0;i--)
	{
		int count = 0;
		for (int j = 0;j < MAX_COLUMNS;j++)
		{
			if (field[i][j])
			{
				count++;
			}
			field[destRow][j] = field[i][j];
		}
		if (count < MAX_COLUMNS)
		{
			destRow--;
		}
	}
}

void reset() 
{
    deltaX = 0;
    rotate = 0;
    delay = 0.3;
}


int tetris()
{
    srand(time(0));     

    RenderWindow window(VideoMode(320, 480), "The Game!");

    Texture m_tilesTexture;
    Texture m_backgroundTexture;
    Texture m_frameTexture;
    m_tilesTexture.loadFromFile("images/tetris/tiles.png");
    m_backgroundTexture.loadFromFile("images/tetris/background.png");
    m_frameTexture.loadFromFile("images/tetris/frame.png");


    Sprite m_tilesSprite(m_tilesTexture);
    Sprite m_backgroundSprite(m_backgroundTexture);
    Sprite m_frameSprite(m_frameTexture);

    Clock clock;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event m_event;
        while (window.pollEvent(m_event))
        {
            if (m_event.type == Event::Closed)
            {
                window.close();
            }

            if (m_event.type == Event::KeyPressed)
            {
                if (m_event.key.code == Keyboard::Up)
                {
                    rotate = true;
                }
                else if (m_event.key.code == Keyboard::Left)
                {
                    deltaX = -1;
                }
                else if (m_event.key.code == Keyboard::Right)
                {
                    deltaX = 1;
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            delay = 0.05;
        }

    //// <- Move -> ///
        move();

    //////Rotate//////
        rotation();

    ///////Tick//////
        ticking();

    ///////check lines//////////
		lineCheck();

        reset();

    /////////draw//////////
    window.clear(Color::White);    
    window.draw(m_backgroundSprite);
          
    for (int i = 0;i < MAX_ROWS;i++)
    {
        for (int column_index = 0;column_index < MAX_COLUMNS;column_index++)
        {
            if (field[i][column_index] == 0)
            {
                continue;
            }
            m_tilesSprite.setTextureRect(IntRect(field[i][column_index] * 18, 0, 18, 18));
            m_tilesSprite.setPosition(column_index * 18, i * 18);
            m_tilesSprite.move(28, 31); //offset
            window.draw(m_tilesSprite);
        }
    }

    for (int i=0;i<4;i++)
    {
       m_tilesSprite.setTextureRect(IntRect(colorNum*18,0,18,18));
       m_tilesSprite.setPosition(coordinates[i].x*18,coordinates[i].y*18);
       m_tilesSprite.move(28,31); //offset
       window.draw(m_tilesSprite);
    }

    window.draw(m_frameSprite);
    window.display();
    }

    return 0;
}

