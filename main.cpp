#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>
#include <time.h>
#include <thread>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "console.h"
#include "image.h"
#define SCREEN_WIDTH 1100
#define SCREEN_HEIGHT 450
#define LEFT_MAP 10
#define TOP_MAP 1
#define MAP_HEIGHT 15
#define MAP_WIDTH 80
#define LEFT_SCORE 100
#define TOP_SCORE 1
#define SCORE_WIDTH 25
#define SCORE_HEIGHT 8
#define MAX_JUMP 5
#define SPEED_PLAYER 15
#define MOVE_PLAYER 3
#define SPEED_AMO 5
#define SPEED_AMO_BOSS 3
#define SPEED_ENEMY 20
#define SPEED_BOSS 5
#define HP_PLAYER 1000
#define HP_BOSS 2000
#define HP_ENEMY 10
#define DAME_PLAYER 20
#define DAME_BOSS 100
#define DAME_ENEMY 30
#define DAME_AMO_BOSS 30
#define COLOR_DEFAULT ColorCode_Green
#define COLOR_SELECT 30
#define COLOR_HELP 8

using namespace std;
int cheat;
int playerScore;
int arrAlive[MAP_HEIGHT+10][MAP_WIDTH+10];
bool sound = 1;
int level = 0; ///0: enemy, 1: boss
int chooseMenu = 1;
int highScore = 0;

/// All function
void aboutMe();
void printMenu();
void menuGame();
void printLogo();
void Init();
void printXY(string Text, int x, int y);
void soundBackGround();
void soundBattle();
void soundBoss();
void soundShot();
void soundLose();
void soundWin();
void soundClick();
void designMap();
class baseObject
{
protected:
    int x, y;
    string image;
    int color;
public:
    baseObject() {x = 0, y = 0, image = "", color = COLOR_DEFAULT;}
    baseObject(int x, int y, string image, int color)
    {
        this->x = x;
        this->y = y;
        this->image = image;
        this->color = color;
    }
    void show()
    {
        TextColor(color);
        int _y = y;
        int _x = x;
        for(int i = 0; i < (int)image.size(); i++)
        {
            gotoXY(_x, _y);
            if(image[i] == '\n') {_x = x; _y++;continue;}
            cout << image[i];
            _x++;
        }
    }
    void hideImage()
    {
        int _y = y;
        int _x = x;
        for(int i = 0; i < (int)image.size(); i++)
        {
            gotoXY(_x, _y);
            if(image[i] == '\n') {_x = x; _y++;continue;}
            cout << " ";
            _x++;
        }
    }
    void setXY(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    void setImage(string image)
    {
        this->image = image;
    }
    void setColor(int color)
    {
        this->color = color;
    }
    int getX()
    {
        return this->x;
    }
    int getY()
    {
        return this->y;
    }
    string getImage()
    {
        return this->image;
    }
    int getHeight()
    {
        int objHeight = 0;
        for(int i = 0; i < (int)image.size(); i++)
        {
            if(image[i] == '\n') objHeight++;
        }
        objHeight++;
        return objHeight;
    }
    int getWidth()
    {
        int objWidth = 0;
        int w = 0;
        for(int i = 0; i < (int)image.size(); i++)
        {
            w++;
            if(image[i] == '\n')
            {
                objWidth = max(objWidth, w);
                w = 0;
            }
        }
        objWidth = max(objWidth, w);
        return objWidth;
    }
};
class gameObject : public baseObject
{
private:
    int speed;
    int hp; /// Health point
    int dame;
public:
    gameObject()
    {
        x = 0, y = 0, speed = 0, hp = 0,dame = 0, image = "", color = COLOR_DEFAULT;
    }
    gameObject(int x, int y, int speed, int hp,int dame, string image, int color)
    {
        this->x = x;
        this->y = y;
        this->image = image;
        this->speed = speed;
        this->color = color;
        this->hp = hp;
        this->dame = dame;
    }

    void setSpeed(int speed)
    {
        this->speed = speed;
    }
    void setHP(int hp)
    {
        this->hp = hp;
    }
    void setDame(int dame)
    {
        this->dame = dame;
    }
    int getDame()
    {
        return this->dame;
    }
    int getHP()
    {
        return this->hp;
    }

    int getSpeed()
    {
        return this->speed;
    }

};

class amoObject : public baseObject
{
private:
    int type; /// 0 Ban ngang, 1 ban doc
    int speed;
    int dame;
public:
    amoObject(){type = 0;speed = 0;dame = 0;}
    amoObject(int x, int y, int type, int speed, int dame, string image, int color)
    {
        this->x = x;
        this->y = y;
        this->type = type;
        this->image = image;
        this->color = color;
        this->speed = speed;
        this->dame = dame;
    }
    int getType(){return this->type;}
    int getSpeed() {return this->speed;}
    int getDame() {return this->dame;}
    void setType(int type) {this->type = type;}
    void setSpeed(int speed) {this->speed = speed;}
    void setDame(int dame) {this->dame = dame;}
};

int Rand(int left, int right);
gameObject randEnemy();

gameObject player(LEFT_MAP + 5, TOP_MAP + MAP_HEIGHT - 4, SPEED_PLAYER, HP_PLAYER, DAME_PLAYER, playerImage[IDLE_IMAGE], ColorCode_White);
gameObject enemy = randEnemy();
gameObject boss(LEFT_MAP + MAP_WIDTH/2, TOP_MAP+1, SPEED_BOSS, HP_BOSS, DAME_BOSS, bossImage, ColorCode_DarkCyan);
vector<amoObject> amo_list, amo_boss_list;
void playerAction(gameObject &gameObj);
void playerIdle(gameObject &gameObj); ///Dung yen
void checkVaCham(gameObject &attacker, gameObject &defender);
void playerJump(gameObject &gameObj);
void playerIdle(gameObject &gameObj);
void playerDown(gameObject &gameObj);
void moveRight(gameObject &gameObj, int lengthRace);
void moveLeft(gameObject &gameObj, int lengthRace);
void moveAmo(vector<amoObject> &amo_list, gameObject &victim); /// Xu li dan di chuyen
void shotPlayer(gameObject &gameObj);
void shotBoss(gameObject &gameObj);
void playerAction(gameObject &gameObj);
void enemyAction(gameObject &enemy);
void bossAction(gameObject &boss);
void displayScore(int key);
void printLoadGame();
void printGameOver();
void resetAll();
void playGame();

int main()
{
    srand(time(NULL));
    Init();
    menuGame();
   // _getch();
    return 0;
}
void aboutMe()
{
    printLogo();
    printXY("Full Name: Chu Van Hung", LEFT_MAP + 30, TOP_MAP + 12);
    printXY("ID: 18020020", LEFT_MAP + 30, TOP_MAP + 13);
    printXY("Email: tuoitre2k@gmail.com", LEFT_MAP + 30, TOP_MAP + 14);
    int chooseMenuAbout = 1;
    while(1)
    {
        if(_kbhit())
        {
            char key = _getch();
            if(key)
                soundClick();
            if(key == key_Down || key == 's' || key == 'S')
            {
                chooseMenuAbout++;
                chooseMenuAbout = min(chooseMenuAbout, 2);
            }
            else if(key == key_Up || key == 'w' || key == 'W')
            {
                chooseMenuAbout--;
                chooseMenuAbout = max(chooseMenuAbout, 1);
            }
            else if(key == 13)
            {
                if(chooseMenuAbout == 1)
                {
                    system("cls");
                    return;
                }
                else if(chooseMenuAbout == 2)
                {
                    exit(1);
                }
            }
        }
        else
        {
            gotoXY(LEFT_MAP + 40, TOP_MAP + 17);
            if(chooseMenuAbout == 1)
                TextColor(COLOR_SELECT);
            else
                TextColor(COLOR_DEFAULT);
            cout << "     Back     ";
            gotoXY(LEFT_MAP + 40, TOP_MAP + 18);
            if(chooseMenuAbout == 2)
                TextColor(COLOR_SELECT);
            else
                TextColor(COLOR_DEFAULT);
            cout << "     Quit     ";
        }
    }
}
void printMenu()
{
    gotoXY(LEFT_MAP + 40, TOP_MAP + 15);
    if(chooseMenu == 1)
        TextColor(COLOR_SELECT);
    else
        TextColor(COLOR_DEFAULT);
    cout << "     Play        ";
    gotoXY(LEFT_MAP + 40, TOP_MAP + 16);
    if(chooseMenu == 2)
        TextColor(COLOR_SELECT);
    else
        TextColor(COLOR_DEFAULT);
    cout << "     Sound (" << ( (sound) ? "On) " : "Off)" )<< " ";
    gotoXY(LEFT_MAP + 40, TOP_MAP + 17);
    if(chooseMenu == 3)
        TextColor(COLOR_SELECT);
    else
        TextColor(COLOR_DEFAULT);
    cout << "     About       ";
    gotoXY(LEFT_MAP + 40, TOP_MAP + 18);
    if(chooseMenu == 4)
        TextColor(COLOR_SELECT);
    else
        TextColor(COLOR_DEFAULT);
    cout << "     Quit        ";

    TextColor(COLOR_HELP);
    gotoXY(LEFT_MAP + 80, TOP_MAP + 16);
    cout << "UP/W - MOVE UP";
    gotoXY(LEFT_MAP + 80, TOP_MAP + 17);
    cout << "DOWN/S - MOVE DOWN";
    gotoXY(LEFT_MAP + 80, TOP_MAP + 18);
    cout << "ENTER - SELECT";
}
void Init()
{
    TextColor(ColorCode_Green);
    ResizeConsole(SCREEN_WIDTH, SCREEN_HEIGHT);
    HideCursor();
    SetConsoleTitle("Stickman");
    ShowCur(false);
}
void printXY(string Text, int x, int y)
{
    gotoXY(x, y);
    cout << Text;
}
void menuGame()
{

    soundBackGround();
    printLogo();
    printLoadGame();
    while(1)
    {
        if(_kbhit())
        {
            char key = _getch();
            if(key)
                soundClick();
            if(key == key_Down || key == 's' || key == 'S')
            {
                chooseMenu++;
                chooseMenu = min(chooseMenu, 4);
            }
            else if(key == key_Up || key == 'w' || key == 'W')
            {
                chooseMenu--;
                chooseMenu = max(chooseMenu, 1);
            }
            else if(key == 13)
            {
                if(chooseMenu == 1)
                {
                    system("cls");
                    playGame();
                }
                else if(chooseMenu == 2)
                {
                    system("cls");
                    sound = !sound;
                    if(sound)
                    {
                        mciSendString("resume background", NULL, 0, NULL);
                    }
                    else
                        mciSendString("pause background", NULL, 0, NULL);
                }
                else if(chooseMenu == 3)
                {
                    system("cls");
                    aboutMe();
                }
                else if(chooseMenu == 4)
                    exit(1);
            }
            else if(key == '1') cheat++;
            else cheat = 0;
        }
        else
        {
            printLogo();
            printMenu();
        }
    }
}
void designMap()
{
    TextColor(ColorCode_GreenGreen);
    for(int y = TOP_MAP; y <= TOP_MAP + MAP_HEIGHT - 1; y++)
    {
        if(y == TOP_MAP || y == TOP_MAP + MAP_HEIGHT - 1)
            for(int x = LEFT_MAP; x <= LEFT_MAP + MAP_WIDTH - 1; x++)
                printXY(" ", x, y);
        printXY("  ", LEFT_MAP - 1, y);
        printXY("  ", LEFT_MAP + 1 + MAP_WIDTH - 1, y);
    }
    TextColor(COLOR_DEFAULT);
}

void soundBackGround()
{
    if(!sound)
    {
        mciSendString("close background", NULL, 0, NULL);
        return;
    }
    mciSendString("open \"background.mp3\" type mpegvideo alias background", NULL, 0, NULL);
    mciSendString("play background repeat", NULL, 0, NULL);
}
void soundBattle()
{
    mciSendString("pause background", NULL, 0, NULL);
    if(!sound)
        return;
    mciSendString("open \"battle.mp3\" type mpegvideo alias battle", NULL, 0, NULL);
    mciSendString("play battle repeat", NULL, 0, NULL);
}
void soundBoss()
{
    mciSendString("close battle", NULL, 0, NULL);
    if(!sound)
        return;
    mciSendString("open \"boss.wav\" type mpegvideo alias boss", NULL, 0, NULL);
    mciSendString("play boss repeat", NULL, 0, NULL);
}
void soundShot()
{
    if(!sound)
        return;
    mciSendString("open \"shot.mp3\" type mpegvideo alias shot", NULL, 0, NULL);
    mciSendString("play shot from 0", NULL, 0, NULL);
}
void soundClick()
{
    if(!sound)
        return;
    mciSendString("open \"click.wav\" type mpegvideo alias click", NULL, 0, NULL);
    mciSendString("play click from 0", NULL, 0, NULL);
}
void soundLose()
{
    if(!sound)
        return;
    mciSendString("close battle", NULL, 0, NULL);
    mciSendString("close boss", NULL, 0, NULL);
    mciSendString("open \"lose.wav\" type mpegvideo alias lose", NULL, 0, NULL);
    mciSendString("play lose from 0 wait", NULL, 0, NULL);
    mciSendString("close lose", NULL, 0, NULL);
}
void soundWin()
{
    if(!sound)
        return;
    mciSendString("close boss", NULL, 0, NULL);
    mciSendString("open \"win.mp3\" type mpegvideo alias win", NULL, 0, NULL);
    mciSendString("play win from 0 wait", NULL, 0, NULL);
    mciSendString("close win", NULL, 0, NULL);
}

int Rand(int left, int right)
{
    return rand()%(right - left + 1) + left;
}
gameObject randEnemy()
{
    gameObject enemy(LEFT_MAP + MAP_WIDTH - 6, Rand(TOP_MAP + 1, TOP_MAP + MAP_HEIGHT - 4), SPEED_ENEMY, HP_ENEMY, DAME_ENEMY,  enemyImage, ColorCode_Blue);
    return enemy;
}
void playerJump(gameObject &gameObj)
{
    int _x = gameObj.getX();
    int _y = TOP_MAP + MAP_HEIGHT - 4;
    int _speed = gameObj.getSpeed();
    for(int i = 1; i < MAX_JUMP; i++)
    {
        if(_y - 1 <= TOP_MAP)
            break;

        if(level == 0) {
            enemyAction(enemy);
            moveAmo(amo_list, enemy);
            checkVaCham(enemy, player);
        }
        if(level == 1){
            bossAction(boss);
            moveAmo(amo_list, boss);
            moveAmo(amo_boss_list, player);
            checkVaCham(boss, player);
        }
        _y--;
        gameObj.hideImage();
        gameObj.setImage(playerImage[JUMP_IMAGE]);
        gameObj.setXY(_x, _y);
        gameObj.show();
        Sleep(_speed);
    }
    int _height = gameObj.getHeight();
    for(int i = 1; i < MAX_JUMP; i++)
    {
        if(_y + _height - 1 >= TOP_MAP + MAP_HEIGHT - 2)
            break;
        if(level == 0) {
            enemyAction(enemy);
            moveAmo(amo_list, enemy);
            checkVaCham(enemy, player);
        }
       if(level == 1){
            bossAction(boss);
            moveAmo(amo_list, boss);
            moveAmo(amo_boss_list, player);
            checkVaCham(boss, player);
        }
        _y++;
        gameObj.hideImage();
        gameObj.setXY(_x, _y);
        gameObj.show();
        Sleep(_speed);
    }
    playerIdle(gameObj);
}
void playerIdle(gameObject &gameObj) ///Dung yen
{
    if(gameObj.getImage() != playerImage[SIT_IMAGE] && gameObj.getImage() != playerImage[IDLE_IMAGE]) {
        gameObj.hideImage();
        gameObj.setImage(playerImage[IDLE_IMAGE]);
    }
    gameObj.show();
}
void playerDown(gameObject &gameObj)
{
    gameObj.hideImage();
    gameObj.setImage(playerImage[SIT_IMAGE]);
    gameObj.setXY(gameObj.getX(), TOP_MAP + MAP_HEIGHT - 3);
}
void moveRight(gameObject &gameObj, int lengthRace) ///Di chuyen sang phai
{
    int _x = gameObj.getX();
    int _y = TOP_MAP + MAP_HEIGHT - 4;
    int _speed = gameObj.getSpeed();
    gameObj.hideImage();
    gameObj.setImage(playerImage[RIGHT_IMAGE]);
    gameObj.setXY(_x+lengthRace, _y);
    gameObj.show();
    Sleep(_speed);
}
void moveLeft(gameObject &gameObj, int lengthRace)
{
    int _x = gameObj.getX();
    int _y = TOP_MAP + MAP_HEIGHT - 4;
    int _speed = gameObj.getSpeed();
    gameObj.hideImage();
    gameObj.setImage(playerImage[LEFT_IMAGE]);
    gameObj.setXY(_x - lengthRace, _y);
    gameObj.show();
    Sleep(_speed);
}
void moveAmo(vector<amoObject> &amo_list, gameObject &victim)
{
    int xVictim = victim.getX();
    int yVictim = victim.getY();
    int wVictim = victim.getWidth();
    int hVictim = victim.getHeight();

    for(int i = 0; i < (int)amo_list.size(); i++)
    {
        int xAmo = amo_list[i].getX();
        int yAmo = amo_list[i].getY();
        int wAmo = amo_list[i].getWidth();
        int hAmo = amo_list[i].getHeight();
        int speedAmo = amo_list[i].getSpeed();
        int dameAmo = amo_list[i].getDame();
        int type = amo_list[i].getType();
        /// Update score , enemy khi ban trung
        if(xAmo + wAmo - 1 >= xVictim && xAmo <= xVictim + wVictim - 1 && yAmo >= yVictim && yAmo <= yVictim + hVictim - 1)
        {
            amo_list[i].hideImage();
            if(victim.getImage() == enemy.getImage()){
                playerScore += 5;
                victim.hideImage();
                int _speed = victim.getSpeed();
                victim = randEnemy();
                victim.setSpeed(max(0, _speed-1));
            }
            else if(victim.getImage() == boss.getImage())
            {
                playerScore += 5;
                int bossHP = boss.getHP();
                bossHP -= dameAmo;
                bossHP = max(0, bossHP);
                boss.setHP(bossHP);

            }
            else if(victim.getImage() == player.getImage())
            {
                int playerHP = player.getHP();
                playerHP -= dameAmo;
                playerHP = max(0, playerHP);
                player.setHP(playerHP);
            }
            amo_list.erase(amo_list.begin() + i);
            continue;
        }
        else if(type == 0 && xAmo + wAmo + speedAmo < LEFT_MAP + MAP_WIDTH && xAmo + wAmo + speedAmo > LEFT_MAP + 1)
        {
            amo_list[i].hideImage();
            amo_list[i].setXY(xAmo + speedAmo, yAmo);
            amo_list[i].show();
        }
        else if(type == 1 && yAmo + hAmo + speedAmo < TOP_MAP + MAP_HEIGHT && yAmo + hAmo + speedAmo > TOP_MAP + 1)
        {
            amo_list[i].hideImage();
            amo_list[i].setXY(xAmo, yAmo + speedAmo);
            amo_list[i].show();
        }
        else
        {
            amo_list[i].hideImage();
            amo_list.erase(amo_list.begin() + i);
        }
    }
}
void shotPlayer(gameObject &gameObj, int type)
{
    ///type 0 ban ngang, type 1 ban doc
    int _x = gameObj.getX();
    int _y = gameObj.getY();
    int _width = gameObj.getWidth();
    /*int _speed = gameObj.getSpeed();
    int tmp = 1; /// Trang thai dung, ngoi 1:dung, 0:ngoi
    if(gameObj.getImage() == playerImage[SIT_IMAGE]) tmp = 0;
    else tmp = 1;
    gameObj.hideImage();
    if(tmp == 1) gameObj.setImage(playerImage[SHOT_IDLE_IMAGE]);
    else gameObj.setImage(playerImage[SHOT_SIT_IMAGE]);
    gameObj.show();
    Sleep(_speed);*/
    soundShot();
    if(type == 0) {
        amoObject amo(_x + _width + 1, _y + 1, type, SPEED_AMO, DAME_PLAYER, amoImage, ColorCode_Yellow);
        amo_list.push_back(amo);
    }
    else {
        amoObject amo(_x + 1, _y - 1, type, -SPEED_AMO/3, DAME_PLAYER, amoHeadImage, ColorCode_Yellow);
        amo_list.push_back(amo);
    }
   /* gameObj.hideImage();
    if(tmp == 1) gameObj.setImage(playerImage[IDLE_IMAGE]);
    else gameObj.setImage(playerImage[SIT_IMAGE]);*/

}
void shotBoss(gameObject &gameObj)
{
    int tmp = Rand(1, 6);
    if(tmp == 1) {

        int _x = gameObj.getX();
        int _y = gameObj.getY();
        int _h = gameObj.getHeight();
        int _w = gameObj.getWidth();
        if(_y + _h < TOP_MAP + MAP_HEIGHT - 3){ /// Ban doc
            amoObject amo_boss( Rand(0, _w - 1) + _x, _y + _h, 1, SPEED_AMO_BOSS/3, DAME_AMO_BOSS, amoBossImage, ColorCode_Red);
            amo_boss_list.push_back(amo_boss);
        }
        else {
            amoObject amo_boss(_x - 1, _y + 1, 0, -SPEED_AMO_BOSS, DAME_AMO_BOSS, amoBossImage, ColorCode_Red);
            amo_boss_list.push_back(amo_boss);
            amo_boss.setXY(_x + _w, _y + 1);
            amo_boss.setSpeed(SPEED_AMO_BOSS);
            amo_boss_list.push_back(amo_boss);
        }
    }
}
void checkVaCham(gameObject &attacker, gameObject &defender)
{
    int xAttacker = attacker.getX();
    int yAttacker = attacker.getY();
    int widthAttacker = attacker.getWidth();
    int heightAttacker = attacker.getHeight();
    int dameAttack = attacker.getDame();

    int xDefender = defender.getX();
    int yDefender = defender.getY();
    int widthDefender = defender.getWidth();
    int heightDefender = defender.getHeight();
    int hpDefender = defender.getHP();

    for(int i = yAttacker; i < yAttacker + heightAttacker; i++)
    {
        for(int j = xAttacker; j < xAttacker + widthAttacker; j++)
            arrAlive[i][j] = 1;
    }
    bool vaCham = 0;
    for(int i = yDefender; i < yDefender + heightDefender; i++)
    {
        if(vaCham == 1)
            break;
        for(int j = xDefender; j < xDefender + widthDefender; j++)
            if(arrAlive[i][j] == 1)
            {
                vaCham = 1;
                break;
            }
    }
    for(int i = yAttacker; i < yAttacker + heightAttacker; i++)
    {
        for(int j = xAttacker; j < xAttacker + widthAttacker; j++)
            arrAlive[i][j] = 0;
    }
    if(vaCham == 1)  /// Neu va cham
    {
        hpDefender -= dameAttack;
        hpDefender = max(hpDefender, 0);
        defender.setHP(hpDefender);
    }
}
void playerAction(gameObject &gameObj)
{
    if(_kbhit())
    {
        char key = _getch();
        int _x = gameObj.getX();
        int _y = gameObj.getY();
        int _width = gameObj.getWidth();

        if((key == key_Left || key == 'a' || key == 'A') && _x > LEFT_MAP + MOVE_PLAYER - 1)
            moveLeft(gameObj, MOVE_PLAYER);
        else if((key == key_Right || key == 'd' || key == 'D') && _x + _width < LEFT_MAP + MAP_WIDTH - MOVE_PLAYER + 1)
            moveRight(gameObj, MOVE_PLAYER);
        else if(key == key_Down || key == 's' || key == 'S')
            playerDown(gameObj);
        else if((key == key_Up || key == 'w' || key == 'W') && _y > TOP_MAP + 1)
            playerJump(gameObj);
        else if(key == 'q' || key == 'Q')
            shotPlayer(gameObj, 0);
        else if(key == 'e' || key == 'E')
            shotPlayer(gameObj, 1);
    }
    playerIdle(gameObj);
}

void enemyAction(gameObject &enemy)
{
    int _x = enemy.getX();
    int _y = enemy.getY();
    int _height = enemy.getHeight();
    int _speed = enemy.getSpeed();
    enemy.hideImage();
    if(_x - 1 > LEFT_MAP)
    {
        int tmp = _y + Rand(-1, 1);
        if(tmp <= TOP_MAP)
            tmp = TOP_MAP + 1;
        if(tmp + _height >= TOP_MAP + MAP_HEIGHT - 1)
            tmp = TOP_MAP + MAP_HEIGHT - 2 - _height + 1;
        enemy.setXY(_x-1, tmp);
        enemy.show();
    }
    else
    {
        playerScore++;
        enemy = randEnemy();
        enemy.setSpeed(_speed-1);
    }
    Sleep(_speed);
}
void bossAction(gameObject &boss)
{
    int _x = boss.getX();
    int _y = boss.getY();
    int _height = boss.getHeight();
    int _width = boss.getWidth();
    //int _speed = boss.getSpeed();
    if(Rand(1, 6) == 1)
    {
        int tmpY = _y + Rand(-3, 3);
        int tmpX = _x + Rand(-3, 3);
        if(tmpY <= TOP_MAP)
            tmpY = TOP_MAP + 1;
        if(tmpY + _height >= TOP_MAP + MAP_HEIGHT - 1)
            tmpY = TOP_MAP + MAP_HEIGHT - 2 - _height + 1;
        if(tmpX <= LEFT_MAP) tmpX = LEFT_MAP + 1;
        if(tmpX + _width >= LEFT_MAP + MAP_WIDTH - 1)
            tmpX = LEFT_MAP + MAP_WIDTH - 2 - _width + 1;
        if(tmpX != _x && tmpY != _y) {
            boss.hideImage();
            boss.setXY(tmpX, tmpY);
        }
    }
    boss.show();
    shotBoss(boss);
    //Sleep(_speed);
}
void displayScore(int key)
{
    TextColor(ColorCode_BlueBlue);
    for(int i = TOP_SCORE; i < TOP_SCORE + SCORE_HEIGHT; i++)
    {
        if(i == TOP_SCORE || i == TOP_SCORE + SCORE_HEIGHT - 1)
            for(int j = LEFT_SCORE; j < LEFT_SCORE + SCORE_WIDTH; j++)
                printXY(" ", j, i);

        printXY("  ", LEFT_SCORE-1, i);
        printXY("  ", LEFT_SCORE + SCORE_WIDTH, i);
    }
    TextColor(ColorCode_White);
    if(key == 1)
    {
        printGameOver();
        printXY("YOU WIN !", LEFT_SCORE + SCORE_WIDTH/5, TOP_SCORE + 1);
        highScore = max(highScore, playerScore);
        printXY("High Score : " + to_string(highScore),LEFT_SCORE + SCORE_WIDTH/5, TOP_SCORE + 5);
        soundWin();
    }
    if(key == -1)
    {
        printGameOver();
        printXY("YOU LOSS !", LEFT_SCORE + SCORE_WIDTH/5, TOP_SCORE + 1);
        highScore = max(highScore, playerScore);
        printXY("High Score : " + to_string(highScore),LEFT_SCORE + SCORE_WIDTH/5, TOP_SCORE + 5);
        soundLose();

    }
    printXY("Score : " + to_string(playerScore),LEFT_SCORE + SCORE_WIDTH/5, TOP_SCORE + 3);
    TextColor(COLOR_HELP);
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 3);
    cout << "UP/W - Jump";
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 4);
    cout << "DOWN/S - Sit";
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 5);
    cout << "LEFT/A - Move Left";
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 6);
    cout << "RIGHT/D - Move Right";
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 7);
    cout << "Q & E - Shot";
    gotoXY(LEFT_SCORE + 3, TOP_SCORE + SCORE_HEIGHT + 8);
    cout << "[Mission] : Alive";
}
void printLogo()
{

    TextColor(rand()%15+1);
    printXY("   _____   _     _          _                                ", LEFT_MAP + 30, TOP_MAP + 4);
    TextColor(rand()%15+1);
    printXY("  / ____| | |   (_)        | |                               ", LEFT_MAP + 30, TOP_MAP + 5);
    TextColor(rand()%15+1);
    printXY(" | (___   | |_   _    ___  | | __  _ __ ___     __ _   _ __  ", LEFT_MAP + 30, TOP_MAP + 6);
    TextColor(rand()%15+1);
    printXY("  \\___ \\  | __| | |  / __| | |/ / | '_ ` _ \\   / _` | | '_ \\ ", LEFT_MAP + 30, TOP_MAP + 7);
    TextColor(rand()%15+1);
    printXY("  ____) | | |_  | | | (__  |   <  | | | | | | | (_| | | | | |", LEFT_MAP + 30, TOP_MAP + 8);
    TextColor(rand()%15+1);
    printXY(" |_____/   \\__| |_|  \\___| |_|\\_\\ |_| |_| |_|  \\__,_| |_| |_|", LEFT_MAP + 30, TOP_MAP + 9);
    TextColor(rand()%15+1);
    printXY("                                                             ", LEFT_MAP + 30, TOP_MAP + 10);
    TextColor(COLOR_DEFAULT);
}
void printLoadGame()
{
    for(int i = 0; i <= 45; i++)
    {
        TextColor(ColorCode_White);
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 15);
        cout << "  o ";
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 16);
        cout << " /|\\";
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 17);
        cout << " / \\";
        gotoXY(LEFT_MAP + 33 + i, TOP_MAP + 18);
        TextColor(ColorCode_BlueBlue);
        cout << " ";
        TextColor(ColorCode_White);
        Sleep(i*2);
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 15);
        cout << "    ";
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 16);
        cout << "    ";
        gotoXY(LEFT_MAP + 30 + i, TOP_MAP + 17);
        cout << "    ";
    }
    system("cls");
}
void printGameOver()
{
    printXY(" __              ___     __        ___  __  ", LEFT_MAP+15, TOP_MAP+1);
    printXY("/ _`  /\\   |\\/| |__     /  \\ \\  / |__  |__)", LEFT_MAP+15, TOP_MAP+2);
    printXY("\\__> /~~\\  |  | |___    \\__/  \\/  |___ |  \\ ", LEFT_MAP+15, TOP_MAP+3);

}
void resetAll()
{
    playerScore = 0;
    level = 0;
    player.hideImage();
    enemy.hideImage();
    printXY("                                            ", LEFT_MAP+15, TOP_MAP+1);
    printXY("                                            ", LEFT_MAP+15, TOP_MAP+2);
    printXY("                                            ", LEFT_MAP+15, TOP_MAP+3);
    player.setXY(LEFT_MAP + 5, TOP_MAP + MAP_HEIGHT - 4);
    player.setImage(playerImage[IDLE_IMAGE]);
    player.setHP(HP_PLAYER);
    boss.setHP(HP_BOSS);
    enemy = randEnemy();
    amo_list.clear();
    amo_boss_list.clear();
}
void playGame()
{
    designMap();
    resetAll();
    player.show();
    soundBattle();
    ///Print HP
    int playerHP = player.getHP();
    gotoXY(LEFT_MAP - 5, TOP_MAP + MAP_HEIGHT);
    TextColor(ColorCode_DarkWhite);
    cout << "[Stickman]";
    TextColor(ColorCode_RedRed);
    for(int i = TOP_MAP + MAP_HEIGHT - int(double(playerHP)/HP_PLAYER * MAP_HEIGHT); i < TOP_MAP + MAP_HEIGHT; i++){
        gotoXY(LEFT_MAP - 3, i);
        cout << "  ";
    }
    TextColor(COLOR_DEFAULT);

    while(1)
    {

        /// Update HP
        playerHP = player.getHP();
        TextColor(COLOR_DEFAULT);
        for(int i = TOP_MAP; i < TOP_MAP + MAP_HEIGHT - int(double(playerHP)/HP_PLAYER * MAP_HEIGHT); i++)
        {
            gotoXY(LEFT_MAP - 3, i);
            cout << "  ";
        }
        if(playerHP > 0) {
            TextColor(ColorCode_RedRed);
            gotoXY(LEFT_MAP - 3, TOP_MAP + MAP_HEIGHT - 1);
            cout << "  ";
            TextColor(COLOR_DEFAULT);
        }


        if(playerHP <= 0)
        {
            if(cheat < 9) {
                displayScore(-1);
                break;
            }
        }
        if(enemy.getSpeed() <= 0)
        {
            soundBoss();
            level = 1;
            int bossHP = boss.getHP();
            TextColor(COLOR_DEFAULT);
            for(int i = TOP_MAP; i < TOP_MAP + MAP_HEIGHT - int(double(bossHP)/HP_BOSS * MAP_HEIGHT); i++)
            {
                gotoXY(LEFT_MAP + MAP_WIDTH + 2, i);
                cout << "  ";
            }
            TextColor(ColorCode_Blue);
            gotoXY(LEFT_MAP + MAP_WIDTH - 1, TOP_MAP + MAP_HEIGHT);
            cout << "[BOSS]";

            TextColor(ColorCode_RedRed);
            for(int i = TOP_MAP + MAP_HEIGHT - int(double(bossHP)/HP_BOSS * MAP_HEIGHT); i < TOP_MAP + MAP_HEIGHT; i++) {
                gotoXY(LEFT_MAP + MAP_WIDTH + 2, i);
                cout << "  ";
            }
            if(bossHP > 0) {gotoXY(LEFT_MAP + MAP_WIDTH + 2, TOP_MAP + MAP_HEIGHT - 1);cout << "  ";}
            TextColor(COLOR_DEFAULT);
            if(bossHP <= 0)
            {
                displayScore(1);
                break;
            }
            bossAction(boss);
            playerAction(player);
            moveAmo(amo_list, boss);
            moveAmo(amo_boss_list, player);
            checkVaCham(boss, player);
        }
        else
        {
            enemyAction(enemy);
            playerAction(player);
            moveAmo(amo_list, enemy);
            checkVaCham(enemy, player);
        }
        displayScore(0);
    }

    int choosePlay = 1;
    while(1)
    {
        if(_kbhit())
        {
            char key = _getch();
            if(key)
                soundClick();
            if(key == key_Down || key == 's' || key == 'S')
            {
                choosePlay++;
                choosePlay = min(choosePlay, 3);
            }
            else if(key == key_Up || key == 'w' || key == 'W')
            {
                choosePlay--;
                choosePlay = max(choosePlay, 1);
            }
            else if(key == 13)
            {
                if(choosePlay == 1)
                {
                    system("cls");
                    playGame();
                    return;
                }
                else if(choosePlay == 2)
                {
                    if(sound)
                        mciSendString("resume background", NULL, 0, NULL);
                    system("cls");
                    return;
                }
                else if(choosePlay == 3)
                    exit(1);
            }
        }
        else
        {
            gotoXY(LEFT_MAP + 40, TOP_MAP + 17);
            if(choosePlay == 1)
                TextColor(COLOR_SELECT);
            else
                TextColor(COLOR_DEFAULT);
            cout << "  Play again  ";
            gotoXY(LEFT_MAP + 40, TOP_MAP + 18);
            if(choosePlay == 2)
                TextColor(COLOR_SELECT);
            else
                TextColor(COLOR_DEFAULT);
            cout << "    Back      ";
            gotoXY(LEFT_MAP + 40, TOP_MAP + 19);
            if(choosePlay == 3)
                TextColor(COLOR_SELECT);
            else
                TextColor(COLOR_DEFAULT);
            cout << "    Quit      ";
        }
    }
}
