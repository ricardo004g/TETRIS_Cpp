#ifndef TABLA_H
#define TABLA_H

#include <iostream>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <windows.h>

const int ANCHO = 10;
const int ALTO = 20;

//posicion de la tabla en la pantalla; la celda (i, j) se dibuja en
//((X_TABLERA + j) * 2, Y_TABLERA + i) porque cada celda ocupa 2 caracteres
const int X_TABLERA = 3;
const int Y_TABLERA = 2;

//columna donde empieza el panel de la derecha (puntos, nivel, siguiente...)
const int X_PANEL = 30;

//colores y caracteres usados para dibujar la pila
const int COLOR_VACIO = 8;    //gris oscuro: celda libre
const int COLOR_TITULO = 15;
const int COLOR_TXT = 7;
const char CELDA_VACIA = 176; //░
const char CELDA_LLENA = 254; //█

void gotoxy(int, int);
void setColor(int);

class Tabla
{
private:
    //cada celda guard 0 si esta libre o el color del bloque que la ocupa
    int tablaMatriz[ALTO][ANCHO] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0}};

public:
    //filas que quedaron completas y hay que eliminar
    std::list<int> indicesDelete;
    /// "beginIndex" es el indice de la primera fila eliminada; sirve para
    ///  saber cuantas filas se encogieron hacia abajo
    int beginIndex = 0;

    void dibujarTabla()
    {
        for(int i = 0; i < ALTO; i++)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                gotoxy((X_TABLERA + j) * 2, Y_TABLERA + i);

                if(tablaMatriz[i][j] == 0)
                {
                    setColor(COLOR_VACIO);
                    printf("%c", CELDA_VACIA);
                }
                else
                {
                    setColor(tablaMatriz[i][j]);
                    printf("%c", CELDA_LLENA);
                }
            }
        }
    }

    ///dibuja una fila completa con un caracter y color; se usa en la animacion
    ///de las lineas que se completan
    void dibujarFila(int fila, char caracter, int color)
    {
        for(int j = 0; j < ANCHO; j++)
        {
            gotoxy((X_TABLERA + j) * 2, Y_TABLERA + fila);
            setColor(color);
            printf("%c", caracter);
        }
    }

    void setElemento(int row, int col, int elemento)
    {
        if(row >= 0 && row < ALTO && col >= 0 && col < ANCHO)
        {
            tablaMatriz[row][col] = elemento;
        }
    }

    int getElemento(int row, int col) const
    {
        if(row >= 0 && row < ALTO && col >= 0 && col < ANCHO)
        {
            return tablaMatriz[row][col];
        }
        return -1;
    }

    ///busca las filas completas y las deja en "indicesDelete";
    ///devuelve true si hay al menos una
    bool verfificarFilasCompletas()
    {
        indicesDelete.clear();

        for(int i = 0; i < ALTO; i++)
        {
            int numOnes = 0;

            for(int j = 0; j < ANCHO; j++)
            {
                if(tablaMatriz[i][j] != 0)
                {
                    numOnes++;
                }
            }

            if(numOnes == ANCHO)
            {
                indicesDelete.push_back(i);
            }
        }

        if(indicesDelete.empty())
        {
            return false;
        }

        beginIndex = indicesDelete.front();

        return true;
    }

    ///encoge la tabla: borra las filas completas de "indicesDelete" y baja
    ///todo lo que quedo arriba, rellenando con filas vacias
    void bajarElementosSuperiores()
    {
        int destino = ALTO;

        //de abajo hacia arriba, toda fila que no se elimina se copia mas abajo,
        //asi las filas de arriba terminan cayendo sobre las de abajo
        for(int origen = ALTO - 1; origen >= 0; origen--)
        {
            if(std::find(indicesDelete.begin(), indicesDelete.end(), origen) != indicesDelete.end())
            {
                continue;
            }

            destino--;

            for(int j = 0; j < ANCHO; j++)
            {
                tablaMatriz[destino][j] = tablaMatriz[origen][j];
            }
        }

        //las primeras filas que quedaron sin origen van vacias
        for(int i = destino - 1; i >= 0; i--)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                tablaMatriz[i][j] = 0;
            }
        }

        indicesDelete.clear();
    }

    ///vacia la tabla para empezar una partida nueva
    void reiniciar()
    {
        indicesDelete.clear();
        beginIndex = 0;

        for(int i = 0; i < ALTO; i++)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                tablaMatriz[i][j] = 0;
            }
        }
    }

    ///borra las filas de "indicesDelete" sin avisar; se usa cuando el jugador
    ///pierde y ya no interesa ver como se encoge la pila
    void limpiarFilasCompletas()
    {
        for(int fila : indicesDelete)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                tablaMatriz[fila][j] = 0;
            }
        }
    }
};


void gotoxy(int x, int y)
{
    HANDLE hCon;

    hCon = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD dwPos;

    dwPos.X = x;
    dwPos.Y = y;

    SetConsoleCursorPosition(hCon, dwPos);
}

void setColor(int color)
{
    HANDLE hCon;

    hCon = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hCon, color);
}

#endif
