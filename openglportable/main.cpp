#include "iGraphics.h"
#include "Game.h"

Game game;

void iDraw()
{
    game.draw();
}

void iMouseMove(int mx, int my)
{
    game.handleMouseMove(mx, my);
}

void iMouseDrag(int mx, int my)
{
    game.handleMouseMove(mx, my);
}

void iMouse(int button, int state, int mx, int my)
{
    game.handleMouseClick(button, state, mx, my);
}

void iMouseWheel(int dir, int mx, int my)
{
    // Not used
}

void iKeyboard(unsigned char key, int state)
{
    if(state == GLUT_DOWN)
        game.handleInput(key);
}

void iSpecialKeyboard(unsigned char key, int state)
{
    if(state == GLUT_DOWN)
        game.handleSpecialKey(key);
}

void updateGameCallback()
{
    game.update();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    srand(time(0));
    game.init();

    // Timer for game update (approx 60 FPS)
    iSetTimer(17, updateGameCallback);

    iOpenWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Shooter Game");
    return 0;
}
