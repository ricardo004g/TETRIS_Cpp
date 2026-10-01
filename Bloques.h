#include "Bloque.h"

class BloqueO : public Bloque
{
public:
    BloqueO(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla)
        : Bloque(_positionOnMatrixX, _positionOnMatrixY, _tabla)
    {
        tipoDeBloque = 1; 
        cargarBloque();   
    }


    void cargarBloque() override {
        int bloque[4][4] = {
            {1, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        };

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                setElemento(i, j, bloque[i][j]);
            }
        }
    }

    void rotarBloque() override {}
};

class BloqueI : public Bloque
{
public:
    BloqueI();
    
    void cargarBloque() override {
        int bloque[4][4] = {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        };

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                setElemento(i, j, bloque[i][j]);
            }
        }
    }
    void rotarBloque() override {
        
        index++;
        int bloqueArriba[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

        int bloqueAcostado[4][4] = {
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}};

        switch (index)
        {
        case 1:
            tipoDeBloque = 3;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueArriba[i][j]);
                }
            }
            break;

        case 2:
            tipoDeBloque = 0;
            index = -1;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueAcostado[i][j]);
                }
            }
            break;
        
        default:
            break;
        }
    }
};

class BloqueL : public Bloque
{
public:
    BloqueL();
    
    void cargarBloque() override {
        int bloque[4][4] = {
            {1, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        };

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                setElemento(i, j, bloque[i][j]);
            }
        }
    }

    void rotarBloque() override {
        
        index++;
        int bloqueArriba[4][4] = {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

        int bloqueDerecha[4][4] ={
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        int bloqueAbajo[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        int bloqueIzquierda[4][4] = {
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        switch (index)
        {
        case 0:

            tipoDeBloque = 1;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueDerecha[i][j]);
                }
            }
            break;
        
        case 1:

            tipoDeBloque = 2;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueAbajo[i][j]);
                }
            }
            break;

        case 2:

            tipoDeBloque = 3;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueIzquierda[i][j]);
                }
            }
            break;

        case 3:

            tipoDeBloque = 0;
            index = -1;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueArriba[i][j]);
                }
            }
            break;
        
        default:
            break;
        }
    }
};

class BloqueT : public Bloque
{
public:
    BloqueT();
    
    void cargarBloque() override {
        int bloque[4][4] = {
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        };

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                setElemento(i, j, bloque[i][j]);
            }
        }
    }
    void rotarBloque() override {
        
        index++;
        int bloqueArriba[4][4] = {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}};

        int bloqueDerecha[4][4] ={
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        int bloqueAbajo[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        int bloqueIzquierda[4][4] = {
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}};

        switch (index)
        {
        case 0:

            tipoDeBloque = 1;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueDerecha[i][j]);
                }
            }
            break;
        
        case 1:

            tipoDeBloque = 2;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueAbajo[i][j]);
                }
            }
            break;

        case 2:

            tipoDeBloque = 3;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueIzquierda[i][j]);
                }
            }
            break;

        case 3:

            tipoDeBloque = 0;
            index = -1;
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    setElemento(i, j, bloqueArriba[i][j]);
                }
            }
            break;
        
        default:
            break;
        }
    }
};

