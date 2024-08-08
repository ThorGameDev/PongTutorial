#include <iostream>
#include <ostream>
#include <time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <chrono>
using namespace std;

bool kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}

char getch()
{
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

enum eDir
{
    STOP = 0,
    LEFT = 1,
    UPLEFT = 2,
    DOWNLEFT = 3,
    RIGHT = 4,
    UPRIGHT = 5,
    DOWNRIGHT = 6
};

class cBall
{
    private:
        int x, y;
        int originalX, originalY;
        eDir direction;
        chrono::time_point<chrono::steady_clock> time;
    public:
        cBall(int posX, int posY)
        {
            originalX = posX;
            originalY = posY;
            x = posX;
            y = posY;
            direction = STOP;
            time=chrono::steady_clock::now();
        }
        void Reset()
        {
            x = originalX;
            y = originalY;
            direction = STOP;
        }
        void changeDirection(eDir d)
        {
            direction = d;
        }
        void randomDirection()
        {
            direction = (eDir)((rand() % 6) + 1); //FIX THE RAND LATTER!!! USE mt19937
        }
        inline int getX() {return x;}
        inline int getY() {return y;}
        inline eDir getDirection() {return direction;}
        bool Move()
        {
            int movetime = 50000;
            int diagonal = 70711;
            bool move_diagonal = true;
            int movex, movey;
            movex = movey = 0;

            switch(direction)
            {
                case STOP:
                    break;
                case UPLEFT:
                    movex = -1;
                    movey = -1;
                    break;
                case LEFT:
                    movex = -1;
                    move_diagonal = false;
                    break;
                case DOWNLEFT:
                    movex = -1;
                    movey = +1;
                    break;
                case UPRIGHT:
                    movex = 1;
                    movey = -1;
                    break;
                case RIGHT:
                    movex = 1;
                    move_diagonal = false;
                    break;
                case DOWNRIGHT:
                    movex = 1;
                    movey = +1;
                    break;
                default:
                    break;
            }

            chrono::time_point<chrono::steady_clock> now = chrono::steady_clock::now();
            int time_elapsed = chrono::duration_cast<chrono::microseconds>(now - time).count();
            if (time_elapsed >= diagonal || (time_elapsed >= movetime && !move_diagonal))
            {
                x += movex;
                y += movey;
                time=now;
                return true;
            }
            else
            {
                return false;
            }
        }
};

class cPaddle
{
    private:
        int x, y;
        int originalX, originalY;
    public:
        cPaddle()
        {
            x = y = 0;
        }
        cPaddle(int posX, int posY) : cPaddle()
        {
            originalX = posX;
            originalY = posY;
            x = posX;
            y = posY;
        }
        inline void Reset() {x = originalX; y = originalY; }
        inline int getX() {return x;}
        inline int getY() {return y;}
        inline void moveUp() {y--;}
        inline void moveDown() {y++;}
};

class cGameManager
{
    private:
        int width, height;
        int score1, score2;
        char up1, down1, up2, down2;
        bool quit;
        cBall* ball;
        cPaddle* player1;
        cPaddle* player2;
    public:
        cGameManager(int w, int h)
        {
            srand(time(NULL));
            quit = false;
            up1 = 'w'; up2 = 'i';
            down1 = 's'; down2 = 'k';
            score1 = score2 = 0;
            width = w; height = h;
            ball = new cBall(w / 2, h / 2);
            player1 = new cPaddle(1, h/2 - 3);
            player2 = new cPaddle(w - 2, h/2 - 3);
        }
        ~cGameManager()
        {
            delete ball;
            delete player1;
            delete player2;
        }
        void ScoreUp(cPaddle* player)
        {
            if(player == player1)
                score1 ++;
            else if (player == player2)
                score2++;

            ball->Reset();
            player1->Reset();
            player2->Reset();
        }
        void Draw()
        {
            int ballx = ball->getX();
            int bally = ball->getY();
            int player1x = player1->getX();
            int player2x = player2->getX();
            int player1y = player1->getY();
            int player2y = player2->getY();

            system("clear");
            cout << "┌";
            for(int i = 0; i < width; i++)
            {
                cout << "─";
            }
            cout << "┐\n";
            for(int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    if( j == 0)
                        cout << "│";

                    if(ballx == j && bally == i)
                        cout << "⏺";
                    else if (player1x == j && player1y == i)
                        cout << "┃";
                    else if (player2x == j && player2y == i)
                        cout << "┃";
                    else if (player1x == j && player1y + 1 == i)
                        cout << "┃";
                    else if (player2x == j && player2y + 1 == i)
                        cout << "┃";
                    else if (player1x == j && player1y + 2 == i)
                        cout << "┃";
                    else if (player2x == j && player2y + 2 == i)
                        cout << "┃";
                    else if (player1x == j && player1y + 3 == i)
                        cout << "┃";
                    else if (player2x == j && player2y + 3 == i)
                        cout << "┃";
                    else
                        cout << " ";

                    if ( j == width - 1)
                        cout << "│";
                }
                cout << "\n";
            }
            cout << "└";
            for(int i = 0; i < width; i++)
            {
                cout << "─";
            }
            cout << "┘\n";

            cout << "Score1: " << score1 << "\nScore2: " << score2 << "\n";
        }
        void Input()
        {
            int player1x = player1->getX();
            int player2x = player2->getX();
            int player1y = player1->getY();
            int player2y = player2->getY();

            if (kbhit())
            {
                char current = getch();
                if (current == up1)
                    if (player1y > 0)
                        player1->moveUp();
                if (current == up2)
                    if (player2y > 0)
                        player2->moveUp();
                if (current == down1)
                    if (player1y + 4 < height)
                        player1->moveDown();
                if (current == down2)
                    if (player2y + 4 < height)
                        player2->moveDown();

                if(ball->getDirection() == STOP)
                {
                    ball->randomDirection();
                }

                if (current == 'q')
                    quit=true;

            }
        }
        void Logic()
        {
            int ballx = ball->getX();
            int bally = ball->getY();
            int player1x = player1->getX();
            int player2x = player2->getX();
            int player1y = player1->getY();
            int player2y = player2->getY();

            //Left Paddle
            for (int i = 0; i < 4; i++)
                if (ballx == player1x + 1)
                    if (bally == player1y + i)
                        ball->changeDirection((eDir)((rand() % 3) + 4));

            //Right Paddle
            for (int i = 0; i < 4; i++)
                if (ballx == player2x - 1)
                    if (bally == player2y + i)
                        ball->changeDirection((eDir)((rand() % 3) + 1));
            
            //bottom wall
            if(bally == height - 1)
                ball->changeDirection(ball->getDirection() == DOWNRIGHT ? UPRIGHT : UPLEFT);
            
            //top wall
            if(bally == 0)
                ball->changeDirection(ball->getDirection() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);

            if( ballx == width -1)
                ScoreUp(player1);

            if( ballx == 0)
                ScoreUp(player2);
        }
        void Run()
        {
            while(!quit)
            {
                Draw();
                bool ball_moved = ball->Move();
                Input();
                if (ball_moved)
                    Logic();
            }
        }

};

int main()
{
    cGameManager c(40, 10);
    c.Run();
    return 0;    
}
