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

//medidas de la pantalla virtual donde se dibuja antes de volcar a la consola
const int PANTALLA_ANCHO = 60;
const int PANTALLA_ALTO = 32;

///pantalla virtual: todo el dibujo del juego va aca dentro y recien al final
///del cuadro se copia entero a la consola de una sola vez. Escribir celda por
///celda con printf era lo que hacia lento el juego
struct Lienzo
{
    char celdas[PANTALLA_ALTO][PANTALLA_ANCHO];
    int colores[PANTALLA_ALTO][PANTALLA_ANCHO];

    int x = 0;
    int y = 0;
    int colorActual = COLOR_TXT;

    ///deja la pantalla en blanco; se llama al empezar cada cuadro
    void limpiar()
    {
        for(int f = 0; f < PANTALLA_ALTO; f++)
        {
            for(int c = 0; c < PANTALLA_ANCHO; c++)
            {
                celdas[f][c] = ' ';
                colores[f][c] = COLOR_TXT;
            }
        }

        x = 0;
        y = 0;
        colorActual = COLOR_TXT;
    }

    void irA(int nuevaX, int nuevaY)
    {
        x = nuevaX;
        y = nuevaY;
    }

    void usarColor(int color)
    {
        colorActual = color;
    }

    ///pone un caracter en la posicion del cursor y avanza; si la posicion se
    ///sale de la pantalla no dibuja nada, asi no hay que	andarse con limites
    void escribir(char c)
    {
        if(x >= 0 && x < PANTALLA_ANCHO && y >= 0 && y < PANTALLA_ALTO)
        {
            celdas[y][x] = c;
            colores[y][x] = colorActual;
        }

        x++;
    }

    void escribir(const char* texto)
    {
        for(int i = 0; texto[i] != '\0'; i++)
        {
            escribir(texto[i]);
        }
    }

    void escribirNumero(int numero)
    {
        char buffer[16];
        sprintf(buffer, "%d", numero);

        escribir(buffer);
    }

    ///copia la pantalla virtual a la consola; es la unica llamada por cuadro
    ///que toca la consola de verdad
    void volcar()
    {
        HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO info;

        if(!GetConsoleScreenBufferInfo(hCon, &info))
        {
            return;
        }

        //si la consola es mas chica que la pantalla virtual se dibuja solo
        //lo que entra, asi no falla la escritura
        int columnas = PANTALLA_ANCHO;
        int filas = PANTALLA_ALTO;

        if(columnas > info.dwSize.X) columnas = info.dwSize.X;
        if(filas > info.dwSize.Y) filas = info.dwSize.Y;

        if(columnas < 1 || filas < 1)
        {
            return;
        }

        COORD tamanho;
        tamanho.X = (SHORT) columnas;
        tamanho.Y = (SHORT) filas;

        COORD origen;
        origen.X = 0;
        origen.Y = 0;

        SMALL_RECT region;
        region.Left = 0;
        region.Top = 0;
        region.Right = (SHORT)(columnas - 1);
        region.Bottom = (SHORT)(filas - 1);

        CHAR_INFO* buffer = new CHAR_INFO[columnas * filas];

        for(int f = 0; f < filas; f++)
        {
            for(int c = 0; c < columnas; c++)
            {
                buffer[f * columnas + c].Char.AsciiChar = celdas[f][c];
                buffer[f * columnas + c].Attributes = (WORD) colores[f][c];
            }
        }

        WriteConsoleOutput(hCon, buffer, tamanho, origen, &region);

        delete[] buffer;
    }
};

extern Lienzo lienzo;

void gotoxy(int, int);
void setColor(int);
void imprimir(char c);
void imprimir(const char* texto);
void imprimirNumero(int numero);

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
                    imprimir(CELDA_VACIA);
                }
                else
                {
                    setColor(tablaMatriz[i][j]);
                    imprimir(CELDA_LLENA);
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
            imprimir(caracter);
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


Lienzo lienzo;

void gotoxy(int x, int y)
{
    lienzo.irA(x, y);
}

void setColor(int color)
{
    lienzo.usarColor(color);
}

void imprimir(char c)
{
    lienzo.escribir(c);
}

void imprimir(const char* texto)
{
    lienzo.escribir(texto);
}

void imprimirNumero(int numero)
{
    lienzo.escribirNumero(numero);
}

#endif
