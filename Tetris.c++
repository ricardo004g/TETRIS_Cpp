//modificado 11/10/2025
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <conio.h>
#include <chrono>
#include <list>
#include <algorithm>
#include <vector>
#include "Bloques.h"

auto lastFall = std::chrono::steady_clock::now();
int fallDelay = 500;

void borrarCursor();

void dibujarLimites();

int main()
{
    borrarCursor();
    system("cls");

    Tabla tabla;

    BloqueO *bloqueInGame = new BloqueO(0, 0, tabla);

    bool siguienteBloque = false;

    bool game_over = false;

    while(!game_over)
    {
        if(tabla.verfificarFilasCompletas())
        {
            tabla.bajarElementosSuperiores();
        }

        tabla.dibujarTabla();
        bloqueInGame->borrarBloque();

        bloqueInGame->moverBloque();

        auto now = std::chrono::steady_clock::now();

        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFall).count() > fallDelay)
        {
            bloqueInGame->cayendo();
            lastFall = now;
        }

        bloqueInGame->calcularColisionInferior();

        if(bloqueInGame->isColision() || bloqueInGame->getColisionInferior() > ALTO - 2)
        {
            bloqueInGame->copyToMatrix();
            delete bloqueInGame;
            siguienteBloque = true;
        }

        if(siguienteBloque)
        {   
            bloqueInGame = new BloqueO(0,0, tabla);
            siguienteBloque = false;
        }

        bloqueInGame->dibujarBloque();

        Sleep(30);
    }
    return 0;
}

void borrarCursor()
{
    HANDLE hCon;

    hCon = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cInfo;

    cInfo.dwSize = 100;
    cInfo.bVisible = false;

    SetConsoleCursorInfo(hCon, &cInfo);
}

void dibujarLimites()
{
    for(int i = 4; i <= 16*2; i++)
    {
        gotoxy(i, 1); printf("%c", 196);
        gotoxy(i,22); printf("%c", 196);
    }

    for(int j = 2; j <= 22; j++)
    {
        gotoxy(3, j); printf("%c", 179);
        gotoxy(16*2, j); printf("%c", 179);
    }

    gotoxy(3, 1); printf("%c", 218);
    gotoxy(32, 22); printf("%c", 191);
}