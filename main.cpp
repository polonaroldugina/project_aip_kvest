#include <SFML/Graphics.hpp>
#include "main.h"
#include <time.h>
#include <sstream>
#include <iostream>
#include <Math.h>


using namespace sf;
using namespace std;

int ts = 100;

Vector2i offset(181, 126);


int score = 0; //очкив игре
int roadW = 2000;//длина пути
int segL = 200; //длина одного куска площадки
float camD = 0.84; //видимость
int width = 1024;
int height = 768;

struct piece
{
    //кусочек нашего пазла в игре, задаем его возможные параметры
    int x, y, col, row, kind, match, alpha;
    piece() { match = 0; alpha = 255; }
}   grid[10][10];

/**
 * меняем местами в игре клетки(кусочки картинки)
 * @param p1 - 1 кусочек который мы меняем со 2
 * @param p2 - 2 кусочек
 */
void swap(piece p1, piece p2)
{
    //меняем местами в игре клетки(кусочки картинки)
    std::swap(p1.col, p2.col);
    std::swap(p1.row, p2.row);

    grid[p1.row][p1.col] = p1;
    grid[p2.row][p2.col] = p2;
}

/**
 * вычисляем коеффициент отклонения дерева от оси в анимации
 * @param ar - номер этапа нашей анимации(номер кусочка длины трассы)
 */
int ratio(int ar) {
    // вычисляем коеффициент отклонения дерева от оси в анимации
    int d = ar % 30;
    float s = cos(d)/sin(d);
    ar = sin(d)*sin(d) + 2 * cos(d) * cos(d) - 1;
    ar = ar / (s*s);
    ar = asin(sqrt(ar)) + d + 1.6;
    if (ar % 2 == 0) {ar=-ar;};
    return ar;
}

/**
 *
 * @param w - окно рисования
 * @param c - цвет отрисовки
 * @param x1 - 1 координата места для отрисовки
 * @param y1 - 2 координата места для отрисовки
 * @param w1 - 3 координата места для отрисовки
 * @param x2 - 4 координата места для отрисовки
 * @param y2 - 5 координата места для отрисовки
 * @param w2 - 6 координата места для отрисовки
 */
void drawQuad(RenderWindow &w, Color c, int x1,int y1,int w1,int x2,int y2,int w2)
{
    //рисуем поле последнй игры, в зависимости от координаты зрителя
    ConvexShape shape(4);
    shape.setFillColor(c);
    shape.setPoint(0, Vector2f(x1-w1,y1));
    shape.setPoint(1, Vector2f(x2-w2,y2));
    shape.setPoint(2, Vector2f(x2+w2,y2));
    shape.setPoint(3, Vector2f(x1+w1,y1));
    w.draw(shape);
}

struct Line
{
    //задаем
    float x,y,z; //объемный центр видимости
    float X,Y,W; //параметры экрана
    float curve,spriteX,clip,scale;
    Sprite sprite;

    Line()
    {spriteX=curve=x=y=z=0;}

    /**
     *
     * @param camX - положение камеры по X
     * @param camY - положение камеры по Y
     * @param camZ - положение камеры по Z
     */
    void project(int camX,int camY,int camZ)
    {
        //рассчитываем один из параметров исходя из других 2
        scale = camD/(z-camZ);
        X = (1 + scale*(x - camX)) * width/2;
        Y = (1 - scale*(y - camY)) * height/2;
        W = scale * roadW  * width/2;
    }

    /**
     *
     * @param app - окно приложения
     */
    void drawSprite(RenderWindow &app)
    {
        //отрисовываем спрайты на экране
        Sprite s = sprite;
        int w = s.getTextureRect().width;
        int h = s.getTextureRect().height;
        float destX = X + scale * spriteX * width/2;
        float destY = Y + 4;
        float destW  = w * W / 266;
        float destH  = h * W / 266;

        destX += destW * spriteX; //изменение по X
        destY += destH * (-1);    //изменение по Y

        float clipH = destY+destH-clip;
        if (clipH<0) clipH=0;

        if (clipH>=destH) return;
        s.setTextureRect(IntRect(0,0,w,h-h*clipH/destH));
        s.setScale(destW/w,destH/h);
        s.setPosition(destX, destY);
        app.draw(s);
    }
};


int main()
{
    //основной код выполнения программы, начало - запуск - меню
    RenderWindow window(sf::VideoMode(800, 800), "MENU"); //размер окна

    Sprite m_BackgroundSprite;
    Texture m_BackgroundTexture;

    // Загружаем фон в текстуру
    m_BackgroundTexture.loadFromFile("../img/t1.jpg");

    // Связываем спрайт и текстуру
    m_BackgroundSprite.setTexture(m_BackgroundTexture);


    Sprite bat_fly;
    Texture bat_fly_1;

    // Загружаем героя в текстуру
    bat_fly_1.loadFromFile("../img/hero1.png");

    // Связываем спрайт и текстуру
    bat_fly.setTexture(bat_fly_1);

    bat_fly.setPosition(260, 40);


    Sprite text0;
    Texture text1;

    // Загружаем текст в текстуру
    text1.loadFromFile("../img/text.png");

    // Связываем спрайт и текстуру
    text0.setTexture(text1);

    text0.setPosition(250, 454);


    // Аналогично создаем кнопки
    Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
    menuTexture1.loadFromFile("../img/111.png");
    menuTexture2.loadFromFile("../img/222.png");
    menuTexture3.loadFromFile("../img/333.png");

    Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3);

    // Параметры меню и нажатия копки
    bool isMenu = 1;
    int menuNum = 0;

    // Координаты кнопок
    menu1.setPosition(250, 300);
    menu2.setPosition(250, 454);
    menu3.setPosition(250, 608);

    // пока открыто окно выполняем его отрисовку
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            //если пользователь закрыл окно - закрываем программу
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Отрисовываем элементы

        window.clear(sf::Color::Blue);
        window.draw(m_BackgroundSprite);
        window.draw(bat_fly);

        window.draw(menu1);
        window.draw(menu2);
        window.draw(menu3);

        // Задаем пути передвижения бабочки(героя)

        if (Keyboard::isKeyPressed(Keyboard::Left)) {bat_fly.move(-0.1, 0); } //первая координата Х отрицательна =>идём влево
        if (Keyboard::isKeyPressed(Keyboard::Right)) {bat_fly.move(0.1, 0); } //первая координата Х положительна =>идём вправо
        if (Keyboard::isKeyPressed(Keyboard::Up)) {bat_fly.move(0, -0.1); } //вторая координата (У) отрицательна =>идём вверх (вспоминаем из предыдущих уроков почему именно вверх, а не вниз)
        if (Keyboard::isKeyPressed(Keyboard::Down)) {bat_fly.move(0, 0.1); } //вторая координата (У) положительна =>идём вниз (если не понятно почему именно вниз - смотрим предыдущие уроки)

        // Связываем параметр кнопки и положение мыши
        if (IntRect(250, 300, 300, 94).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Green); menuNum = 1; }
        if (IntRect(250, 454, 300, 94).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Green); menuNum = 2; }
        if (IntRect(250, 608, 300, 94).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Green); menuNum = 3; }

        // Если нажали мышку, то выполняем функции кнопок
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (menuNum == 1) {window.close();
                //закрываем предыдущее окно, отрисовываем новое
                srand(time(0));

                RenderWindow app(VideoMode(750, 750), "HOME FIRST");
                app.setFramerateLimit(60);
                //текстуры игры
                Texture t1, t2;
                t1.loadFromFile("../img/background.png");
                t2.loadFromFile("../img/t3.jpg");

                Sprite background(t1), gems(t2);

                //задаем положение и параметры кнопки
                Texture menuTexture4;
                menuTexture4.loadFromFile("../img/444.png");
                Sprite menu4(menuTexture4);
                menu4.setPosition(225, 600);

                //задаем поле игры
                for (int i = 1; i <= 4; i++)
                    for (int j = 1; j <= 4; j++)
                    {
                        grid[i][j].kind = rand() % 3;
                        grid[i][j].col = j;
                        grid[i][j].row = i;
                        grid[i][j].x = j * ts;
                        grid[i][j].y = i * ts;
                    }
                int x0, y0, x, y; int click = 0; Vector2i pos;
                bool isSwap = false, isMoving = false;

                while (app.isOpen())
                {
                    Event e;
                    // пока открыто окно выполняем его отрисовку
                    while (app.pollEvent(e))
                    {
                        if (e.type == Event::Closed)
                            app.close();

                        if (e.type == Event::MouseButtonPressed)
                            if (e.key.code == Mouse::Left)
                            {
                                if (!isSwap && !isMoving) click++;
                                pos = Mouse::getPosition(app) - offset;
                            }
                    }

                    // Управление мышью
                    if (click == 1)
                    {
                        x0 = pos.x / ts + 1;
                        y0 = pos.y / ts + 1;
                    }
                    if (click == 2)
                    {
                        x = pos.x / ts + 1;
                        y = pos.y / ts + 1;
                        if (abs(x - x0) + abs(y - y0) == 1)
                        {
                            swap(grid[y0][x0], grid[y][x]); isSwap = 1; click = 0;
                        }
                        else click = 1;
                    }

                    //Ищем "Три в ряд"
                    for (int i = 1; i <= 4; i++)
                        for (int j = 1; j <= 4; j++)
                        {
                            if (grid[i][j].kind == grid[i + 1][j].kind)
                                if (grid[i][j].kind == grid[i - 1][j].kind)
                                    for (int n = -1; n <= 1; n++) grid[i + n][j].match++;

                            if (grid[i][j].kind == grid[i][j + 1].kind)
                                if (grid[i][j].kind == grid[i][j - 1].kind)
                                    for (int n = -1; n <= 1; n++) grid[i][j + n].match++;
                        }

                    //Анимируем переходы
                    isMoving = false;
                    for (int i = 1; i <= 4; i++)
                        for (int j = 1; j <= 4; j++)
                        {
                            piece& p = grid[i][j];
                            int dx, dy;
                            for (int n = 0; n < 5; n++)   //Скорость
                            {
                                dx = p.x - p.col * ts;
                                dy = p.y - p.row * ts;
                                if (dx) p.x -= dx / abs(dx);
                                if (dy) p.y -= dy / abs(dy);
                            }
                            if (dx || dy) isMoving = 1;
                        }

                    //Удаление анимации
                    if (!isMoving)
                        for (int i = 1; i <= 4; i++)
                            for (int j = 1; j <= 4; j++)
                                if (grid[i][j].match) if (grid[i][j].alpha > 10) { grid[i][j].alpha -= 10; isMoving = true; }

                    //Получить счет

                    for (int i = 1; i <= 4; i++)
                        for (int j = 1; j <= 4; j++)
                            score += 0.04 * grid[i][j].match;


                    Font fnt; //переменная шрифта
                    fnt.loadFromFile("../Fonts/CENTAUR.ttf");
                    auto s = std::to_string(score);
                    Text TStore(s, fnt, 50);
                    TStore.setPosition(10, 10);//позиция на экране




                    //Второй обмен, если нет совпадения
                    if (isSwap && !isMoving)
                    {
                        if (!score) swap(grid[y0][x0], grid[y][x]); isSwap = 0;
                    }

                    //Обновить сетку
                    if (!isMoving)
                    {
                        for (int i = 4; i > 0; i--)
                            for (int j = 1; j <= 4; j++)
                                if (grid[i][j].match)
                                    for (int n = i; n > 0; n--)
                                        if (!grid[n][j].match) { swap(grid[n][j], grid[i][j]); break; };

                        for (int j = 1; j <= 4; j++)
                            for (int i = 4, n = 0; i > 0; i--)
                                if (grid[i][j].match)
                                {
                                    grid[i][j].kind = rand() % 4;
                                    grid[i][j].y = -ts * n++;
                                    grid[i][j].match = 0;
                                    grid[i][j].alpha = 255;
                                }
                    }


                    app.draw(background);
                    app.draw(menu4);
                    if (IntRect(225, 600, 300, 94).contains(Mouse::getPosition(app))) {menu4.setColor(Color::Green); menuNum = 4; }

                    //элементы отрисовки
                    for (int i = 1; i <= 4; i++)
                        for (int j = 1; j <= 4; j++)
                        {
                            piece p = grid[i][j];
                            gems.setTextureRect(IntRect(p.kind * 100, 0, 100, 100));
                            gems.setColor(Color(255, 255, 255, p.alpha));
                            gems.setPosition(p.x, p.y);
                            gems.move(offset.x - ts, offset.y - ts);
                            app.draw(gems);

                        }
                    //вывод баллов
                    app.draw(TStore);
                    if (Mouse::isButtonPressed(Mouse::Left))
                    {
                        //нажатие кнопки - переход в финал квеста
                    if (menuNum == 4) {
                        //закрываем предыдущее окно
                        //прорисовываем новое
                        app.close();
                        RenderWindow app(VideoMode(width, height), "GO HOME");
                        app.setFramerateLimit(60);

                        Texture t[50];
                        Sprite object[50];
                        //добавляем все наши деревья в спрайты
                        for(int i=1;i<=7;i++)
                        {
                            t[i].loadFromFile("../images/"+std::to_string(i)+".png");
                            t[i].setSmooth(true);
                            object[i].setTexture(t[i]);
                        }


                        //фон игры также задаем через спрайты
                        Texture bg;
                        bg.loadFromFile("../images/bg.png");
                        bg.setRepeated(true);
                        Sprite sBackground(bg);
                        sBackground.setTextureRect(IntRect(0,0,1280,720));
                        sBackground.setPosition(0,0);

                        std::vector<Line> lines;

                        // прорисовываем каждое дерево
                        for(int i=0;i<2800;i++)
                        {
                            Line line;
                            line.z = i*segL;

                            int k = ratio(i);

                            if (i>300 && i<700) line.curve=0.5;
                            if (i>1100) line.curve=-0.7;

                            if (i%30==0) {line.spriteX=-k; line.sprite=object[4];}
                            if (i%30==1){line.spriteX=-k; line.sprite=object[6];}
                            if (i%30==2) {line.spriteX=-k; line.sprite=object[4];}
                            if (i%30==3) {line.spriteX=-k; line.sprite=object[1];}

                            if (i==400)  {line.spriteX=0; line.sprite=object[7];}
                            if (i==800)  {line.spriteX=1.2; line.sprite=object[7];}


                            if (i%30==4) {line.spriteX=k; line.sprite=object[2];}
                            if (i%30==5) {line.spriteX=k; line.sprite=object[5];}
                            if (i%30==6) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==7) {line.spriteX=k; line.sprite=object[5];}
                            if (i%30==8){line.spriteX=k; line.sprite=object[6];}
                            if (i%30==9) {line.spriteX=k; line.sprite=object[4];}

                            if (i%30==10) {line.spriteX=k; line.sprite=object[3];}
                            if (i%30==11) {line.spriteX=k; line.sprite=object[5];}
                            if (i%30==12) {line.spriteX=k; line.sprite=object[4];}
                            if (i%30==13){line.spriteX=k; line.sprite=object[6];}
                            if (i%30==14) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==15) {line.spriteX=k; line.sprite=object[3];}
                            if (i%30==16) {line.spriteX=k; line.sprite=object[6];}

                            if (i%30==17) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==18){line.spriteX=k; line.sprite=object[6];}
                            if (i%30==19) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==20) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==21) {line.spriteX=k; line.sprite=object[5];}
                            if (i%30==22) {line.spriteX=k; line.sprite=object[6];}
                            if (i%30==23){line.spriteX=k; line.sprite=object[4];}

                            if (i%30==24) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==25) {line.spriteX=k; line.sprite=object[6];}
                            if (i%30==26) {line.spriteX=k; line.sprite=object[6];}
                            if (i%30==27) {line.spriteX=k; line.sprite=object[1];}
                            if (i%30==28){line.spriteX=k; line.sprite=object[6];}
                            if (i%30==29) {line.spriteX=k; line.sprite=object[1];}

                            if (i>750) line.y = sin(i/30.0)*1500;

                            //вовзращаем в вектор одно дерево соответсвующее нашей точке игры

                            lines.push_back(line);
                        }
                        //задаем количество деревьем и парметры игрока
                        int N = lines.size();
                        float playerX = 0;
                        int pos = 0;
                        int H = 1500;

                        while (app.isOpen())
                        {
                            // пока открыто окно выполняем его отрисовку
                            Event e;
                            while (app.pollEvent(e))
                            {
                                // закрываем окно по необходимости
                                if (e.type == Event::Closed)
                                    app.close();
                            }
                            int speed=0;
                            // возможности управления с клавиатуры
                            if (Keyboard::isKeyPressed(Keyboard::Right)) playerX+=0.05;
                            if (Keyboard::isKeyPressed(Keyboard::Left)) playerX-=0.08;
                            if (Keyboard::isKeyPressed(Keyboard::Up)) speed=200;
                            if (Keyboard::isKeyPressed(Keyboard::Down)) speed=-100;
                            if (Keyboard::isKeyPressed(Keyboard::Tab)) speed*=2;
                            if (Keyboard::isKeyPressed(Keyboard::W)) H+=100;
                            if (Keyboard::isKeyPressed(Keyboard::S)) H-=100;
                            pos+=speed;
                            while (pos >= N*segL) pos-=N*segL;
                            while (pos < 0) pos += N*segL;

                            //прорисовка фона
                            app.clear(Color(105,205,4));
                            app.draw(sBackground);
                            int startPos = pos/segL;
                            int camH = lines[startPos].y + H;
                            if (speed>0) sBackground.move(-lines[startPos].curve*2,0);
                            if (speed<0) sBackground.move( lines[startPos].curve*2,0);
                            int maxy = height;
                            float x=0,dx=0;

                            //Отрисовываем зону видимости
                            for(int n = startPos; n<startPos+500; n++)
                            {
                                // отрисоываем зону видимости исходя из координат
                                Line &l = lines[n%N];
                                l.project(playerX*roadW-x, camH, startPos*segL - (n>=N?N*segL:0));
                                x+=dx;
                                dx+=l.curve;
                                l.clip=maxy;
                                if (l.Y>=maxy) continue;
                                maxy = l.Y;
                                Color grass  = (n/3)%2?Color(37,100,6):Color(37,91,6);
                                Line p = lines[(n-1)%N]; //previous line
                                drawQuad(app, grass, 0, p.Y, width, 0, l.Y, width);

                            }

                            //Рисуем лес
                            for(int n=startPos+500; n>startPos; n--){
                                lines[n%N].drawSprite(app);}
                            app.display();}
                    };};
                    app.display();
                }
            };
            if (menuNum == 2) {
                //выводим подсказку на экран
                window.draw(text0);
            };
            if (menuNum == 3)  {
                //если нажали 3 кнопку, то выходим из меню
                window.close();
                isMenu = false;}
        }
        window.display();
    }
    return 0;
}