#ifndef BLOQUE_H
#define BLOQUE_H

#include <vector>
#include "Tabla.h"

//cada pieza tiene su propio color, que ademas se guarda dentro de la tabla
//para poder dibujar la pila con los mismos colores
const int COLOR_BLOQUE_I = 11; //cian
const int COLOR_BLOQUE_O = 14; //amarillo
const int COLOR_BLOQUE_T = 13; //magenta
const int COLOR_BLOQUE_S = 10; //verde
const int COLOR_BLOQUE_Z = 12; //rojo
const int COLOR_BLOQUE_J = 9;  //azul
const int COLOR_BLOQUE_L = 6;  //naranja

class Bloque
{
protected:
    //posicion del bloque dentro de la tabla
    int positionOnMatrixX;
    int positionOnMatrixY;

    //posicion equivalente en pantalla; se usa solo para dibujar
    int x = X_TABLERA;
    int y = Y_TABLERA;

    Tabla& tabla;

    //los "sprites" de los bloques estan representados por indices dentro de una
    //matriz de 4x4, asi todos los bloques comparten el mismo tipo de dato;
    //las piezas de 3x3 solo usan la esquina superior izquierda
    int bloque[4][4];

    //lado del sprite; 2 para la O, 4 para la I y 3 para el resto
    int ladoSprite = 4;

    int color = 7;

public:
    virtual ~Bloque() {}

    Bloque(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla): tabla(_tabla)
    {
        positionOnMatrixX = _positionOnMatrixX;
        positionOnMatrixY = _positionOnMatrixY;
    }

    virtual void cargarBloque() = 0;

    ///numero de la pieza: se usa para el previsualizado y para la reserva
    virtual int getTipo() = 0;

    ///deja anotados el tamano del sprite y el color; las clases hijas la
    ///llaman desde su propio cargarBloque()
    void configurarSprite(int lado, int _color)
    {
        ladoSprite = lado;
        color = _color;
    }

    ///rota el sprite 90 grados en sentido horario; la logica vive aca para
    ///que ninguna pieza tenga que escribirla a mano
    void rotarBloque()
    {
        if(ladoSprite <= 2)
        {
            return; //la O es un cuadrado, girar no cambia nada
        }

        int rotado[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                rotado[j][ladoSprite - 1 - i] = bloque[i][j];
            }
        }

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                bloque[i][j] = rotado[i][j];
            }
        }

        normalizarSprite();
    }

    ///intenta girar el bloque; si al girar no cabe (por ejemplo contra una
    ///pared) prueba a empujarlo un poco hacia los lados o hacia arriba antes
    ///deojo la rotacion. Devuelve true si el bloque quedo girado.
    bool intentarRotar()
    {
        int spriteAntes[4][4];
        int ladoAntes = ladoSprite;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                spriteAntes[i][j] = bloque[i][j];
            }
        }

        rotarBloque();

        if(cabeEn(0, 0))
        {
            return true;
        }

        //empujones: izquierda, derecha, arriba y un paso mas a cada lado
        const int empujones[5][2] = {{-1,0},{1,0},{0,-1},{-2,0},{2,0}};

        for(int e = 0; e < 5; e++)
        {
            if(cabeEn(empujones[e][0], empujones[e][1]))
            {
                positionOnMatrixX += empujones[e][0];
                positionOnMatrixY += empujones[e][1];
                x += empujones[e][0];
                y += empujones[e][1];

                return true;
            }
        }

        //no hubo forma de girar, se deja el bloque como estaba
        ladoSprite = ladoAntes;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                bloque[i][j] = spriteAntes[i][j];
            }
        }

        return false;
    }

    ///desplaza las celdas ocupadas del sprite hasta pegarlas a la esquina
    ///superior izquierda del 4x4; asi la posicion del bloque siempre apunta a
    ///la esquina real de la pieza y las rotaciones no hacen saltos de una
    ///celda (pasaria con la I, que al girar queda descentrada)
    void normalizarSprite()
    {
        //aca van los limites REALES del sprite: el menor y el mayor indice
        //que tengan una celda ocupada, por filas y por columnas
        int minFila = 4, minCol = 4;
        int maxFila = -1, maxCol = -1;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(bloque[i][j] != 1)
                {
                    continue;
                }

                if(i < minFila) { minFila = i; }
                if(i > maxFila) { maxFila = i; }
                if(j < minCol)   { minCol = j; }
                if(j > maxCol)   { maxCol = j; }
            }
        }

        //sprite vacio: no hay nada que mover
        if(maxFila == -1)
        {
            ladoSprite = 0;
            return;
        }

        int alto = maxFila - minFila + 1;
        int ancho = maxCol - minCol + 1;

        int movido[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};

        for(int i = minFila; i <= maxFila; i++)
        {
            for(int j = minCol; j <= maxCol; j++)
            {
                movido[i - minFila][j - minCol] = bloque[i][j];
            }
        }

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                bloque[i][j] = movido[i][j];
            }
        }

        ladoSprite = (alto > ancho) ? alto : ancho;
    }

    ///dice si el bloque, movido (dx, dy), cabe dentro de la tabla sin pisar
    ///ni salirse; es la unica funcion que decide si un movimiento es legal
    bool cabeEn(int dx, int dy)
    {
        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] != 1)
                {
                    continue;
                }

                int xAbs = positionOnMatrixX + j + dx;
                int yAbs = positionOnMatrixY + i + dy;

                //las paredes y el piso
                if(xAbs < 0 || xAbs >= ANCHO || yAbs < 0 || yAbs >= ALTO)
                {
                    return false;
                }

                //otro bloque ya guardado en la tabla
                if(tabla.getElemento(yAbs, xAbs) != 0)
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool isColision()          { return !cabeEn(0, 0); }
    bool isColisionInferior()  { return !cabeEn(0, 1); }
    bool isColisionIzquierda() { return !cabeEn(-1, 0); }
    bool isColisionDerecha()   { return !cabeEn(1, 0); }

    void moverIzquierda()
    {
        if(cabeEn(-1, 0))
        {
            x--;
            positionOnMatrixX--;
        }
    }

    void moverDerecha()
    {
        if(cabeEn(1, 0))
        {
            x++;
            positionOnMatrixX++;
        }
    }

    void caerUnPaso()
    {
        if(cabeEn(0, 1))
        {
            y++;
            positionOnMatrixY++;
        }
    }

    ///deja caer el bloque hasta el piso; devuelve cuantas filas bajo
    int caerAlPiso()
    {
        int filas = 0;

        while(cabeEn(0, 1))
        {
            y++;
            positionOnMatrixY++;
            filas++;
        }

        return filas;
    }

    ///deja el bloque fijo en la tabla; su color queda guardado para poder
    ///dibujarlo despues
    void copyToMatrix()
    {
        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] == 1)
                {
                    tabla.setElemento(positionOnMatrixY + i, positionOnMatrixX + j, color);
                }
            }
        }
    }

    ///dibuja el bloque en su posicion de la tabla
    void dibujarBloque()
    {
        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] == 1)
                {
                    gotoxy((X_TABLERA + positionOnMatrixX + j) * 2, Y_TABLERA + positionOnMatrixY + i);
                    setColor(color);
                    printf("%c", CELDA_LLENA);
                }
            }
        }
    }

    ///dibuja la pieza "fantasma": donde caeria el bloque si se soltara ahora
    void dibujarFantasma()
    {
        int distancia = 0;

        while(cabeEn(0, distancia + 1))
        {
            distancia++;
        }

        if(distancia == 0)
        {
            return;
        }

        //mismo color de la pieza pero apagado (bit 8 = fondo intenso), para
        //que se vea de que color es la pieza que va a caer
        int colorApagado = color | 8;

        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] == 1)
                {
                    gotoxy((X_TABLERA + positionOnMatrixX + j) * 2, Y_TABLERA + positionOnMatrixY + i + distancia);
                    setColor(colorApagado);
                    printf("%c", CELDA_LLENA);
                }
            }
        }
    }

    void borrarBloque()
    {
        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] == 1)
                {
                    gotoxy((X_TABLERA + positionOnMatrixX + j) * 2, Y_TABLERA + positionOnMatrixY + i);
                    setColor(COLOR_VACIO);
                    printf("%c", CELDA_VACIA);
                }
            }
        }
    }

    ///dibuja el bloque en cualquier posicion de la pantalla; se usa para el
    ///previsualizado de la siguiente pieza y de la reserva
    void dibujarEn(int origenX, int origenY)
    {
        for(int i = 0; i < ladoSprite; i++)
        {
            for(int j = 0; j < ladoSprite; j++)
            {
                if(bloque[i][j] == 1)
                {
                    gotoxy(origenX + j * 2, origenY + i);
                    setColor(color);
                    printf("%c", CELDA_LLENA);
                }
            }
        }
    }

    ///ancho real de la pieza en celdas, contando solo las celdas ocupadas
    int getAncho()
    {
        int ancho = 0;

        for(int j = 0; j < ladoSprite; j++)
        {
            for(int i = 0; i < ladoSprite; i++)
            {
                if(bloque[i][j] == 1)
                {
                    ancho++;
                    break;
                }
            }
        }

        return ancho;
    }

    ///deja el bloque centrado y en la primera fila de la tabla
    void aparecerEnTabla()
    {
        positionOnMatrixX = (ANCHO - getAncho()) / 2;
        positionOnMatrixY = 0;

        x = X_TABLERA;
        y = Y_TABLERA;
    }

    //Getter y Setters

    int getColor(){return color;}
    int getPOMatrixX(){return positionOnMatrixX;}
    int getPOMatrixY(){return positionOnMatrixY;}

    void setElemento(int row, int col, int elemento)
    {
        if(row >= 0 && row < 4 && col >= 0 && col < 4) {
            bloque[row][col] = elemento;
        }
    }

    int getElemento(int row, int col)
    {
        if(row >= 0 && row < 4 && col >= 0 && col < 4)
        {
            return bloque[row][col];
        }
        return -1;
    }
};

#endif
