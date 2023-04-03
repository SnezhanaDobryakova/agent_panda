#include <windows.h>
#include <gl/gl.h>
#include "stb-master/stb_easy_font.h"
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

GLuint texture;
GLuint background;

void Background_Init (){

    int twidth, thight, tcnt;    //переменные ширины, высоты,

    unsigned char *data=stbi_load("background3.png",&twidth,&thight,&tcnt,0);

    glGenTextures(1, &background);            //генерация текстуры
    glBindTexture(GL_TEXTURE_2D, background);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                                                            //настройки GL_CLAMP
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, thight,
                 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);       //задание текстуры
    stbi_image_free(data);           //освобождение буфера

}

void Background_Show()
{
    float vertex[]= {0,0, 1080,0, 1024,720, 0,720};        //вектор текстурируемого
                                                                //многоугольника
    float texCoord[] = {0,1,1,1,1,0,0,0};   // текстурные координаты

    glClearColor(0,0,0,0);

    glEnable(GL_TEXTURE_2D); //разрешить двумерное текстурирование
    glBindTexture(GL_TEXTURE_2D, background);
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

void Texture_Init (){


    int twidth, thight, tcnt;    //переменные ширины, высоты,

    unsigned char *data=stbi_load("agent_panda.png",&twidth,&thight,&tcnt,0);

    glGenTextures(1, &texture);            //генерация текстуры
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                                                            //настройки GL_CLAMP
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, thight,
                 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);       //задание текстуры
    stbi_image_free(data);           //освобождение буфера

}

void Texture_Show()
{
    float vertex[]= {0,0, 100,0, 100,240, 0,240};        //вектор текстурируемого
                                                                //многоугольника
    float TexCord[] = {0,1,1,1,1,0,0,0};   // текстурные координаты

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
    static float spriteYsize=80;
    static float charsizey=80;
    static float charsizex=100;
    int n=0, t=0;
    //Проводим нормализацию пиксельных координат для текущего кадра:
    float left=(charsizex*n)/spriteXsize; //вычисление координат кадра
     //на изображении от номера кадра
    float right=left+(charsizex/spriteXsize);
    float top=(charsizey*t)/spriteYsize;
    float bottom=top+(charsizey/spriteYsize);
    //Изменяем текстурные координаты перед выводом текстуры:
    TexCord[1] = TexCord[3] = bottom; // запись в вектор текстурных
                                        //координат
    TexCord[5] = TexCord[7] = top;
    TexCord[2] = TexCord[4] = left;
    TexCord[0] = TexCord[6] = right;

    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_ALPHA_TEST);

}

void AddButton(char *name, int x,int y, int width , int height, float scale){
    float buffer[1000];
    float vertx[8];
    int num_quads;

    float *vert = vertx;
        vert[0]=vert[6]=x;
        vert[2]=vert[4]=x+width;
        vert[1]=vert[3]=y;
        vert[5]=vert[7]=y+height;

    num_quads=stb_easy_font_print(0,0,name, 0, buffer, sizeof(buffer));
    float textPosX = x + (width-stb_easy_font_width(name)*scale)/2.0;
    float textPosY = y + (height-stb_easy_font_height(name)*scale)/2.0;
    textPosY += scale*2;
    scale=scale;

    glVertexPointer(2, GL_FLOAT,0,vert);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor3f(0.5,0,1);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glColor3f(0.0,0.0,0.0); //цвет обводки
    glLineWidth(2); // толщина обводки кнопки
    glDrawArrays(GL_LINE_LOOP,0,4); //отрисовка обводки
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix(); //матрицу в стек
    glColor3f(0.5,0.5,0.5); //цвет текста
    glTranslatef(textPosX,textPosY,0); //перенос матрицы для отрисовки текста
    glScalef(scale,scale,1); //масштабирование текста
    glEnableClientState(GL_VERTEX_ARRAY); // разрешение
    glVertexPointer(2, GL_FLOAT, 16, buffer); //вектор для отрисовки
    glDrawArrays(GL_QUADS, 0, num_quads*4); //отрисовка текста
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
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
                          1024,
                          720,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct; //создание переменной с координатами прямоуголника
    GetClientRect(hwnd, &rct); //получение текущих координат окна
    glOrtho(0, rct.right, 0, rct.bottom, 1, -1); //выставляем их как координаты окна

    Background_Init();
    Texture_Init ();

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

            Background_Show();

            glPushMatrix();
            glLoadIdentity();
            glOrtho(0, rct.right, rct.bottom, 0,  1, -1);
            AddButton("Start", 20, 20, 150, 60, 3);
            AddButton("Test", 20, 90, 150, 60, 3);
            AddButton("Menu", 20, 160, 150, 60, 3);
            AddButton("Exit", 20, 230, 150, 60, 3);

            glPopMatrix();

            Texture_Show();

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
        /*case WM_MOUSEMOVE:            //при событии - перемещении мыши
            Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));
                                                           //пересылка координат мыши в функцию
            break;
*/
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

