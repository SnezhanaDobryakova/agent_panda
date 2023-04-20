#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

int AddButton(char *name, float x, float y, float width, float height, float textS);
void Menu_SlowMenu();
void Menu_Clear();

char *Menu_GetButtonName(int buttonID);

int Menu_MouseMove (float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();

#endif // MENU_H_INCLUDED
