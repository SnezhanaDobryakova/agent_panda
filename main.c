#include <windows.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include <stdbool.h>

#include "menu.h"


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int width = 1080;
int height = 720;
float gravity = 0.1;    // скорость изменения координаты Y
                            //(для реализации падения)
float speed = 5;     //скорость изменения координат
float currentframe=2;
float chSiX;
float chSiY;
bool directional;
bool State=0;

GLuint texture;
GLuint background;
GLuint sprite;

typedef struct
{
    float left, right, top, bottom, textX, textY;
} sritCoord;

typedef struct
{
    float x, y, dx, dy;     //обе координаты и скорость изменения
} Hero;

Hero pers;
sritCoord vrtcoord;

//Для отрисовывания объекта в определенном месте необходимо провести
//инициализацию:

void Hero_Init(Hero *obj, float x1, float y1, float dx1, float dy1)
{
 obj->x=x1;
 obj->y=y1;
 obj->dx=dx1;
 obj->dy=dy1;
}

//Функция обработки перемещения:
void Hero_Move(Hero *obj)
{
    obj->y+=obj->dy; //обновление координат
    obj->x+=obj->dx;
    Reflect(&obj->dy, &obj->y, obj->y<0,0);
    Reflect(&obj->dx,&obj->x, obj->x<0, 0);
    Reflect(&obj->dx,&obj->x, obj->x>width-chSiX-0, width-chSiX-0);
    obj->dy-=gravity; // влияние примера гравитации
    if (GetKeyState(VK_LEFT)<0 && State==1) // проверка состояния
                                //программы и нажатия клавиши влево
    {
        currentframe+=0.15; // подсчет для смены кадра по спрайт
                                //листу (не оптимально)
        obj->dx-=speed; // пересылка изменения координаты
        obj->x+=obj->dx; //вычисление конечной координаты
        obj->dx=0; // обнуление для исключение пересчета
                    // при обновлении координаты в начале
        if (currentframe>8) currentframe-=7; //обнуление номера кадра для цикличности
        directional=1;

         //cout << currentframe <<';'<< directional <<';' << obj->x << ';' << obj->y << endl;
        //фрагмент для проверки текущей координаты
    }
         if (GetKeyState(VK_RIGHT)<0 && State==1) // повторение предыдущего условия
         {
             currentframe+=0.15;
             obj->dx+=speed;
             obj->x+=obj->dx;
             obj->dx=0;
             if (currentframe>8) currentframe-=7;
             directional=0;
             //cout << currentframe <<';'<< directional <<';' << obj->x << ';' << obj->y << endl;
     }
     if (GetKeyState(VK_UP)<0 && (obj->y<20) && State==1)
     //повторение, с условием проверки
     // нахождения на определенном уровне
     {
         obj->dy =speed*1.2; // дает импульс для имитации прыжка
         obj->y+=obj->dy; //расчет координаты Y
     }
}

void Reflect (float *da, float *a, BOOL cond, float wall)
{
     if (!cond) return; //условие проверки пересечения
                            //координаты и препятствия
     *da*=-0,1; // пересчет изменения координаты
                    //в начале функции перемещения
     *a=wall; //перемещение в координату стены
}

void Hero_Show(Hero *obj)
{
    glPushMatrix();
    glTranslatef(obj->x,obj->y,0);
    Pers_Show(sprite, currentframe, &pers);
    glPopMatrix();

}

void Init_Button()
{
    AddButton("Start", 20, 20, 150, 60, 3);
    AddButton("?", 20, 90, 150, 60, 3);
    AddButton("Exit", 20, 160, 150, 60, 3);
}

void Init()
{
    Hero_Init(&pers, width/2.0,height/2.0,0,0);
    Init_Textures("agent_panda.png", &sprite, GL_CLAMP, GL_CLAMP, GL_NEAREST);
    Init_Textures("background3.png", &background, GL_REPEAT, GL_CLAMP, GL_NEAREST);
}

void MouseDown()
{
    int buttonId = Menu_MouseDown();
    if(buttonId<0) return;
    char *name = Menu_GetButtonName(buttonId);
    printf("%s\n",name, &buttonId);

    switch (buttonId)
    {
    case 0:
        if (State==0) State=1;
        break;
    case 1:
        //if (State==1) State=0;
        break;
    case 2:
        PostQuitMessage(0);
        break;

    }

}

void Init_Textures( char *filename, GLuint *textureID, int swarp, int twarp, int filter)
{
    int twidth, thight, tcnt;
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,0);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,swarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,twarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,twidth,thight,
                 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void Background_Show(GLuint texture)
{
    float vertex[]= {0,0, 1080,0, 1080,720, 0,720};        //вектор текстурируемого
                                                                //многоугольника
    float texCoord[] = {0,1,1,1,1,0,0,0};   // текстурные координаты

    glClearColor(0,0,0,0);

    glEnable(GL_TEXTURE_2D); //разрешить двумерное текстурирование
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.7);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertex); //вектор для отрисовки
    glTexCoordPointer(2,GL_FLOAT, 0, texCoord); //адрес массива откуда брать текстурные координаты
    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_ALPHA_TEST);

}

void Pers_Show(GLuint texture, int n, Hero *obj)
{
    static float vertex[]= {0,0, 200,0, 200,160, 0,160};        //вектор текстурируемого
                                                                //многоугольника
    static float TexCord[] = {0,0, 0.12,0, 0.12,1, 0,1};   // текстурные координаты
    static float line[]= {0,0, 200,0, 200,160, 0,160};      //контур текстурируемого многоугольника

    glEnable(GL_TEXTURE_2D); //разрешить двумерное текстурирование
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);        // проверка на элементы α-канала
    glAlphaFunc(GL_GREATER, 0.7); // задается тип уровня и его
                                                //числовая граница
    glEnableClientState(GL_VERTEX_ARRAY);   //использование вектора
                                            //координат
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); //использование
                                                    //вектора текстурных координат
    glVertexPointer(2, GL_FLOAT, 0, vertex); //используем вектор координат
    glTexCoordPointer(2,GL_FLOAT, 0, TexCord); //используем вектор
                                                //текстурных координат

    static float spriteXsize=800; //переменные с размерами
                                    //текстуры и отдельного кадра
    static float spriteYsize=240;
    static float charsizey=80;
    static float charsizex=100;
    //Проводим нормализацию пиксельных координат для текущего кадра:
    vrtcoord.left=(charsizex*n)/spriteXsize; //вычисление координат кадра
                                                //на изображении от номера кадра
    vrtcoord.right=vrtcoord.left+(charsizex/spriteXsize);
    vrtcoord.top=(charsizey*0)/spriteYsize;
    vrtcoord.bottom=vrtcoord.top+(charsizey/spriteYsize);
    //Изменяем текстурные координаты перед выводом текстуры:
    TexCord[1] = TexCord[3] = vrtcoord.bottom; // запись в вектор текстурных координат
    TexCord[5] = TexCord[7] = vrtcoord.top;
    TexCord[2] = TexCord[4] = vrtcoord.left;
    TexCord[0] = TexCord[6] = vrtcoord.right;

    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDisable(GL_ALPHA_TEST);

    glLineWidth(3);
    glVertexPointer(2,GL_FLOAT,0,line);
    glDrawArrays(GL_LINE_LOOP, 0,4);



    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();

    chSiX=charsizex;
    chSiY=charsizey;

}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1080,
                          720,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    ///////////////////////////////

    RECT rct; //создание переменной с координатами прямоуголника
    GetClientRect(hwnd, &rct); //получение текущих координат окна
    glOrtho(0, rct.right, 0, rct.bottom, 1, -1); //выставляем их как координаты окна

    //////////////////////////

    Init_Button();
    Init();

    ///////////////////////////

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Background_Show(background);

            if (State==1)
            {
                Hero_Move(&pers);

                Hero_Show(&pers);

            };

            glPushMatrix();
                glLoadIdentity();
                glOrtho(0, rct.right, rct.bottom, 0,  1, -1);
                Menu_SlowMenu();
            glPopMatrix();

            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:            //при событии - перемещении мыши
            Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));//пересылка координат мыши в функцию

            break;

        case WM_LBUTTONDOWN:
            MouseDown();
        break;

        case WM_LBUTTONUP:
            Menu_MouseUp();
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

