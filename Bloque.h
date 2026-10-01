#include <vector>
#include "Tabla.h"

class Bloque{
protected:
    int x = 3;
    int y = 3;
    Tabla& tabla;
    std::vector<std::vector<int>> colisionVector;
    std::vector<std::vector<int>> leftCollisionVector;
    std::vector<std::vector<int>> rightCollisionVector;
    //IMPORTANTE
    //longitudBLoque toma la longitud maxima de #1 horizontalmente
    int longitudBloque;

    //los "sprites" de los bloque estan representados por indices
    int index = -1;

    int bloque[4][4];
    
    int positionOnMatrixX;
    int positionOnMatrixY;

    int extremoAbsIzquierdo;
    int extremoAbsDerecho;

    int colisionInferior;
    int tipoDeBloque;

public:
    virtual ~Bloque() {} 
    
    Bloque(int _positionOnMatrixX, int _positionOnMatrixY, Tabla& _tabla): tabla(_tabla){
        positionOnMatrixX = _positionOnMatrixX;
        positionOnMatrixY = _positionOnMatrixY; 
        tipoDeBloque = 0;
        bloqueSize();
        calcularExtremos();
    } 

    virtual void cargarBloque() = 0;
    virtual void rotarBloque() = 0;

    //CALCULA EL TAMAÑO DEL BLOQUE
    void bloqueSize()
    {
        int longitud = -1; 
        int countIndex = 0;

        for(int i = 0; i < 4; i++)
        {   
            for(int j = 0; j < 4; j++)
            {   
                if(bloque[i][j] == 1)
                {
                    countIndex++;
                }
            }
            if(countIndex > longitud)
            {   
                longitud = countIndex;
            }
            countIndex = 0;
        }
        longitudBloque = longitud;
    }

    //CALCULAR INDICES COLUMNA DE LOS EXTREMOS DEL BLOQUE
    void calcularExtremos()
    {
        int index = 0;
        
        int extremoDerecho = -1;
        int extremoIzquierdo = 0;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(bloque[i][j] == 1)
                {
                    index = j;
                    if(index > extremoDerecho)
                    {
                    extremoDerecho = index;
                    }
                }
            }
        }
        extremoIzquierdo = extremoDerecho - (longitudBloque - 1);

        extremoAbsIzquierdo = positionOnMatrixX + extremoIzquierdo;
        extremoAbsDerecho = positionOnMatrixX + extremoDerecho;
    }

    void calcularColisionInferior()
    {
        int index = 0;
            
        int pisoBloque = -1;

        for(int i = 0; i < 4; i++)
        {   
            for(int j = 0; j < 4; j++)
            {   
                if(bloque[j][i] == 1)
                {
                    index = j;
                    if(index > pisoBloque)
                    {
                    pisoBloque = index; 
                    }
                }    
            }
        }
        colisionInferior = positionOnMatrixY + pisoBloque;
    }

    void dibujarBloque()
    {
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j ++)
            {
                if(bloque[i][j]){
                    //gotoxy((x + j) * 2, y + i); printf("%c", 254);
                    gotoxy((x + j)*2, y + i); printf("0");
                }
            }
        }
    }

    void borrarBloque()
    {
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j ++)
            {
                if(bloque[i][j])
                {
                    gotoxy((x + j) * 2, y + i); printf("  ");
                }
            }
        }
    }

    void moverBloque()
    {
        if (GetAsyncKeyState(VK_LEFT) & 0x1 && extremoAbsIzquierdo > 0 && isLeftSideColision())
        {
            x--;
            positionOnMatrixX--;
            extremoAbsIzquierdo--;
            extremoAbsDerecho--;
        } 

        if (GetAsyncKeyState(VK_RIGHT) & 0x1 && extremoAbsDerecho < 9 && isRightSideColision())
        {
            x++;
            positionOnMatrixX++;
            extremoAbsDerecho++;
            extremoAbsIzquierdo++;
        }

        //rotar bloque
        if ((GetAsyncKeyState(VK_UP) & 0x1 && extremoAbsIzquierdo >= 0 && extremoAbsDerecho <= 9 &&
            (extremoAbsIzquierdo != 0 || tipoDeBloque != 1) && 
            (extremoAbsDerecho != 9 || tipoDeBloque != 3)))
        {
            rotarBloque();
            bloqueSize();
            calcularExtremos();
        } 
    }

    void cayendo()
    {
        if(positionOnMatrixY < 20)
        {
            y++;
            positionOnMatrixY++;
        }
    }

    //CORREGIR
    void copyToMatrix()
    {
        //verifica la posicion del bloque en la matriz, si la posicion es igual a la esperada entonces copia 
        //el bloque a la matriz(tabla)
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(bloque[i][j] == 1)
                {                       
                    int xAbs = positionOnMatrixX + j;
                    int yAbs = positionOnMatrixY + i;

                    if (xAbs >= 0 && xAbs < ANCHO && yAbs >= 0 && yAbs < ALTO)
                    {
                    tabla.setElemento(yAbs, xAbs, 1);
                    }
                }
            }                
        }   
    }

    void cargarColision()
    {
        colisionVector.clear();

        int xAbs = 0; 
        int yAbs = 0;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                //indice j equivale a las filas e indice i equivale a las columnas
                if(bloque[j][i] == 1)
                {
                    xAbs = positionOnMatrixX + i;
                    yAbs = positionOnMatrixY + j + 1; // posición debajo del bloque
                }
            }
            colisionVector.push_back({yAbs, xAbs});
        }
    }

    bool isColision()
    {
        cargarColision();
        bool colision = false;
        for(int i = 0; i < 4; i++)
        {
            if(tabla.getElemento(colisionVector[i][0],colisionVector[i][1]) == 1)
            {
                colision = true;
            }
        }
        return colision;
    }

    void leftSideCollision()
    {
        leftCollisionVector.clear();
        int xAbs = 0; 
        int yAbs = 0;

        //calcular colision del latera izquierdo
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if(bloque[i][j] == 1)
                {
                    yAbs = positionOnMatrixY + i;
                    xAbs = positionOnMatrixX + j - 1;
                    break;
                } 
            }
            leftCollisionVector.push_back({yAbs, xAbs});
        }
    }

    void rightSideCollision()
    {
        rightCollisionVector.clear();
        int xAbs = 0;
        int yAbs = 0;

        //calcular colision del latera derecho
        for (int i = 0; i < 4; i++)
        {
            for (int j = 3; j > 0; j--)
            {
                if(bloque[i][j] == 1)
                {
                    yAbs = positionOnMatrixY + i;
                    xAbs = positionOnMatrixX + j + 1;
                    break;
                }
            }
            rightCollisionVector.push_back({yAbs, xAbs});
        }
    }
    
    bool isRightSideColision()
    {
        rightSideCollision();
        for(int i = 0; i < (int) rightCollisionVector.size(); i++)
        {
            if(tabla.getElemento(rightCollisionVector[i][0], rightCollisionVector[i][1]) == 1)
            {
                return false;
            }
        }
        return true;
    }

    bool isLeftSideColision()
    {
        leftSideCollision();
        for(int i = 0; i < (int) leftCollisionVector.size(); i++)
        {
            if(tabla.getElemento(leftCollisionVector[i][0], leftCollisionVector[i][1]) == 1)
            {
                return false;
            }
        }
        return true;
    }

    //Getter y Setters

    int getX() {return x;}
    int getY() {return y;}

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

    int getColisionInferior(){return colisionInferior;}
};