#ifndef BLOQUES_H
#define BLOQUES_H

#include "Bloque.h"

///ayuda comun para todos los bloques: escribe el sprite dentro del 4x4 del
///bloque y deja anotados el tamano y el color de la pieza
void cargarSprite(Bloque& bloque, const int* sprite, int lado, int _color);

class BloqueO : public Bloque
{
public:
    BloqueO(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 1; }

    void cargarBloque() override {
        int sprite[2][2] = {
            {1, 1},
            {1, 1}
        };

        cargarSprite(*this, &sprite[0][0], 2, COLOR_BLOQUE_O);
    }
};

class BloqueI : public Bloque
{
public:
    BloqueI(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 0; }

    void cargarBloque() override {
        int sprite[4][4] = {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        };

        cargarSprite(*this, &sprite[0][0], 4, COLOR_BLOQUE_I);
        normalizarSprite();
    }
};

class BloqueL : public Bloque
{
public:
    BloqueL(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 6; }

    void cargarBloque() override {
        int sprite[3][3] = {
            {1, 0, 0},
            {1, 0, 0},
            {1, 1, 0}
        };

        cargarSprite(*this, &sprite[0][0], 3, COLOR_BLOQUE_L);
        normalizarSprite();
    }
};

class BloqueT : public Bloque
{
public:
    BloqueT(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 2; }

    void cargarBloque() override {
        int sprite[3][3] = {
            {0, 1, 0},
            {1, 1, 1},
            {0, 0, 0}
        };

        cargarSprite(*this, &sprite[0][0], 3, COLOR_BLOQUE_T);
        normalizarSprite();
    }
};

class BloqueS : public Bloque
{
public:
    BloqueS(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 3; }

    void cargarBloque() override {
        int sprite[3][3] = {
            {0, 1, 1},
            {1, 1, 0},
            {0, 0, 0}
        };

        cargarSprite(*this, &sprite[0][0], 3, COLOR_BLOQUE_S);
        normalizarSprite();
    }
};

class BloqueZ : public Bloque
{
public:
    BloqueZ(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 4; }

    void cargarBloque() override {
        int sprite[3][3] = {
            {1, 1, 0},
            {0, 1, 1},
            {0, 0, 0}
        };

        cargarSprite(*this, &sprite[0][0], 3, COLOR_BLOQUE_Z);
        normalizarSprite();
    }
};

class BloqueJ : public Bloque
{
public:
    BloqueJ(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        cargarBloque();
        aparecerEnTabla();
    }

    int getTipo() override { return 5; }

    void cargarBloque() override {
        int sprite[3][3] = {
            {0, 0, 1},
            {1, 1, 1},
            {0, 0, 0}
        };

        cargarSprite(*this, &sprite[0][0], 3, COLOR_BLOQUE_J);
        normalizarSprite();
    }
};

void cargarSprite(Bloque& bloque, const int* sprite, int lado, int _color)
{
    bloque.configurarSprite(lado, _color);

    //se limpia todo el 4x4 para que no queden restos de una rotacion anterior
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            bloque.setElemento(i, j, 0);
        }
    }

    //"sprite" viene como una lista plana de celdas, asi que se indexa con
    //i * lado + j
    for(int i = 0; i < lado; i++)
    {
        for(int j = 0; j < lado; j++)
        {
            bloque.setElemento(i, j, sprite[i * lado + j]);
        }
    }
}

#endif
