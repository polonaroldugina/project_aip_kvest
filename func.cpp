#include <SFML/Graphics.hpp>
#include <time.h>
#include <sstream>
#include <iostream>
#include <Math.h>
#include "func.h"


using namespace sf;
using namespace std;

int ratio_t(int ar) {
    // вычисляем коеффициент отклонения дерева от оси в анимации
    int d = ar % 30;
    float s = cos(d)/sin(d);
    ar = sin(d)*sin(d) + 2 * cos(d) * cos(d) - 1;
    ar = ar / (s*s);
    ar = asin(sqrt(ar)) + d + 1.6;
    if (ar % 2 == 0) {ar=-ar;};
    return ar;
}


float project_t(int camX,int camY,int camZ)
{
    int roadW = 2000;
    float camD = 0.84; //camera depth
    int width1 = 1024;
    int height1 = 768;
    float x1,y1,z1; //3d center of line
    float X1,Y1,W1; //screen coord
    float scale1;
    scale1 = camD/(z1-camZ);
    X1 = (1 + scale1*(x1 - camX)) * width1/2;
    Y1 = (1 - scale1*(y1 - camY)) * height1/2;
    W1 = scale1 * roadW  * width1/2;
    return W1;
}

////


