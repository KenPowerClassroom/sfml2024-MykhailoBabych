#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int arkanoid()
{
    srand(time(0));

    RenderWindow app(VideoMode(520, 450), "Arkanoid!");
    app.setFramerateLimit(60);

    Texture blockTexture,backGroundTexture,ballTexture,paddleTexture;
    blockTexture.loadFromFile("images/arkanoid/block01.png");
    backGroundTexture.loadFromFile("images/arkanoid/background.jpg");
    ballTexture.loadFromFile("images/arkanoid/ball.png");
    paddleTexture.loadFromFile("images/arkanoid/paddle.png");

    Sprite sBackground(backGroundTexture), sBall(ballTexture), sPaddle(paddleTexture);
    sPaddle.setPosition(300,440);

    Sprite block[1000];

    int blockIndex=0;
    for (int index=1;index<=10;index++)
    for (int j=1;j<=10;j++)
      {
         block[blockIndex].setTexture(blockTexture);
         block[blockIndex].setPosition(index*43,j*20);
         blockIndex++;
      }

    float horizontalMovement=6, verticalMovement=5;
    float xPos=300, yPos=300;

    while (app.isOpen())
    {
       Event event;
       while (app.pollEvent(event))
       {
         if (event.type == Event::Closed)
             app.close();
       }

    xPos+=horizontalMovement;
    for (int i=0;i<blockIndex;i++)
        if ( FloatRect(xPos+3,yPos+3,6,6).intersects(block[i].getGlobalBounds()) ) 
             {block[i].setPosition(-100,0); horizontalMovement=-horizontalMovement;}

    yPos+=verticalMovement;
    for (int i=0;i<blockIndex;i++)
        if ( FloatRect(xPos+3,yPos+3,6,6).intersects(block[i].getGlobalBounds()) ) 
             {block[i].setPosition(-100,0); verticalMovement=-verticalMovement;}

    if (xPos<0 || xPos>520)  horizontalMovement=-horizontalMovement;
    if (yPos<0 || yPos>450)  verticalMovement=-verticalMovement;

    if (Keyboard::isKeyPressed(Keyboard::Right)) sPaddle.move(6,0);
    if (Keyboard::isKeyPressed(Keyboard::Left)) sPaddle.move(-6,0);

    if ( FloatRect(xPos,yPos,12,12).intersects(sPaddle.getGlobalBounds()) ) verticalMovement=-(rand()%5+2);

    sBall.setPosition(xPos,yPos);

    app.clear();
    app.draw(sBackground);
    app.draw(sBall);
    app.draw(sPaddle);

    for (int i=0;i<blockIndex;i++)
     app.draw(block[i]);

    app.display();
    }

  return 0;
}
