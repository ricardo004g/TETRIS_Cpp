#include <iostream>
#include <list>
#include <stdio.h>
#include <windows.h>

const int ANCHO = 10;
const int ALTO = 20;

void gotoxy(int, int);

class Tabla
{
private:
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
    std::list<int> indicesDelete;
    /// "beginIndex" es el indice final de la fila a borrar; tambien sirve para 
    ///  la longitud de fila que necesita la submatriz
    int beginIndex = 0;

    void dibujarTabla()
    {
        for(int i =0; i<ALTO; i++)
        {
            for(int j = 0; j<ANCHO; j++)
            {
                if(tablaMatriz[i][j] == 0)
                {
                    gotoxy((3+j)*2,3+i); printf("-");
                }else
                {
                    gotoxy((3+j)*2,3+i); printf("0");
                }
            }
            std::cout<<"\n";
        }
    }

    void setElemento(int row, int col, int elemento)
    {
        if(row >= 0 && row < 20 && col >= 0 && col < 10)
        {
            tablaMatriz[row][col] = elemento;
        }
    }

    int getElemento(int row, int col)
    {
        if(row >= 0 && row < 20 && col >= 0 && col < 10)
        {
            return tablaMatriz[row][col];
        }
        return -1;
    }

    bool verfificarFilasCompletas()
    {
        bool bajar = false;

        int puntero = 0; 
        int numOnes = 0;

        for(int i = 0; i < ALTO; i++)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                puntero = tablaMatriz[i][j];
                if(puntero == 1)
                {
                    numOnes++;
                }

                if(numOnes == 10)
                {
                    bajar = true;
                    indicesDelete.push_back(i);

                    if(indicesDelete.size() == 1)
                    {
                        beginIndex = i;
                    }
                }
            }
            puntero = 0; 
            numOnes = 0;
        }
        return bajar;
    }

    void bajarElementosSuperiores()
    {
        int zeros[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int longitud = indicesDelete.size();

        //borrar filas "completas"
        for(int i = 0; i < longitud; i++)
        {
            int index = indicesDelete.front();

            std::copy(zeros, zeros + ANCHO,tablaMatriz[index]);

            indicesDelete.pop_front();
        }

        //copiar elementos superiores a la submatriz
        int subMatriz[beginIndex][ANCHO];
        for(int i = 0; i < beginIndex; i++)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                subMatriz[i][j] = tablaMatriz[i][j];
                tablaMatriz[i][j] = 0;
            }
        }

        //bajar elementos
        for(int i = 0; i < beginIndex; i++)
        {
            for(int j = 0; j < ANCHO; j++)
            {
                tablaMatriz[i+longitud][j] = subMatriz[i][j];
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