//modificado 11/10/2025
//rehecho 01/10/2026
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <conio.h>
#include <chrono>
#include <list>
#include <algorithm>
#include <vector>
#include <random>
#include "Bloques.h"

auto lastFall = std::chrono::steady_clock::now();
int fallDelay = 800;

//puntos por linea al completar 1, 2, 3 o 4 lineas (la 4 es el "tetris")
const int PUNTOS_POR_LINEA[5] = {0, 100, 300, 500, 800};

const int LINEAS_POR_NIVEL = 10;
const int DELAY_INICIAL = 800;
const int DELAY_MINIMO = 90;

//puntos por cada fila que se baja a mano o con el bloque fantasma
const int PUNTOS_POR_FILA_BAJADA = 1;

void borrarCursor();

void dibujarLimites();
void dibujarPanel(int puntos, int nivel, int lineasTotales, int tipoSiguiente, int tipoReserva);
void borrarRectangulo(int x, int y, int ancho, int alto);
void mostrarPausa();
void mostrarFin(int puntos, int nivel, int lineasTotales);

///crea un bloque del tipo pedido; los tipos van de 0 a 6 en el mismo orden
///que las clases de Bloques.h
Bloque* crearBloque(int tipo, Tabla& tabla);

///saca un tipo de pieza al azar usando la regla de la bolsa: se llena una bolsa
///con las 7 piezas, se mezclan y se van sacando de a una; cuando la bolsa se
///vacia se vuelve a llenar. Asi no salen cinco seguidas la misma
int siguienteTipo(std::vector<int>& bolsa, std::mt19937& generador);

///guarda los datos de una partida para poder reiniciar sin salir del programa
struct Partida
{
    int puntos = 0;
    int nivel = 1;
    int lineasTotales = 0;
};

int main()
{
    system("cls");
    borrarCursor();

    Partida partida;

    std::mt19937 generador(
        (unsigned) std::chrono::steady_clock::now().time_since_epoch().count());

    bool salir = false;

    while(!salir)
    {
        Tabla tabla;
        tabla.reiniciar();

        std::vector<int> bolsa;

        partida = Partida();
        fallDelay = DELAY_INICIAL;
        lastFall = std::chrono::steady_clock::now();

        int tipoSiguiente = siguienteTipo(bolsa, generador);

        Bloque* bloqueInGame = crearBloque(tipoSiguiente, tabla);
        tipoSiguiente = siguienteTipo(bolsa, generador);

        //la reserva guarda el tipo, no el bloque, asi el intercambio es facile
        int tipoReserva = -1;   //-1 = todavia no se guardo ninguna pieza
        bool reservaUsada = false;

        bool game_over = false;
        bool pausado = false;

        while(!game_over)
        {
            if(pausado)
            {
                //se dibuja la tabla una vez para que la pausa no la borre
                tabla.dibujarTabla();
                bloqueInGame->dibujarBloque();
                mostrarPausa();

                Sleep(30);

                if(_kbhit())
                {
                    char tecla = _getch();

                    if(tecla == 'P' || tecla == 'p' || tecla == 27)
                    {
                        pausado = false;
                    }
                    else if(tecla == 'Q' || tecla == 'q')
                    {
                        salir = true;
                        break;
                    }
                }
                continue;
            }

            if(_kbhit())
            {
                char tecla = _getch();

                if(tecla == 'P' || tecla == 'p')
                {
                    pausado = true;
                    continue;
                }

                if(tecla == 'Q' || tecla == 'q' || tecla == 27)
                {
                    salir = true;
                    break;
                }

                //cambiar la pieza por la de la reserva
                if(tecla == 'Z' || tecla == 'z')
                {
                    if(!reservaUsada)
                    {
                        int tipoAux = tipoReserva;
                        tipoReserva = bloqueInGame->getTipo();

                        delete bloqueInGame;

                        if(tipoAux == -1)
                        {
                            bloqueInGame = crearBloque(tipoSiguiente, tabla);
                            tipoSiguiente = siguienteTipo(bolsa, generador);
                        }
                        else
                        {
                            bloqueInGame = crearBloque(tipoAux, tabla);
                        }

                        reservaUsada = true;

                        //la pieza que entra podria no tener lugar
                        if(bloqueInGame->isColision())
                        {
                            game_over = true;
                            continue;
                        }
                    }
                    continue;
                }
            }

            tabla.dibujarTabla();
            bloqueInGame->borrarBloque();

            //movimiento con las flechas
            if(GetAsyncKeyState(VK_LEFT) & 0x1)  { bloqueInGame->moverIzquierda(); }
            if(GetAsyncKeyState(VK_RIGHT) & 0x1) { bloqueInGame->moverDerecha(); }
            if(GetAsyncKeyState(VK_UP) & 0x1)    { bloqueInGame->intentarRotar(); }

            auto now = std::chrono::steady_clock::now();

            //bajar rapido con la flecha de abajo: cada fila suma un punto
            if(GetAsyncKeyState(VK_DOWN) & 0x1 && bloqueInGame->cabeEn(0, 1))
            {
                bloqueInGame->caerUnPaso();
                partida.puntos += PUNTOS_POR_FILA_BAJADA;
                lastFall = now;
            }

            if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFall).count() > fallDelay)
            {
                bloqueInGame->caerUnPaso();
                lastFall = now;
            }

            //el bloque se queda fijo cuando ya no puede bajar mas
            if(bloqueInGame->isColisionInferior())
            {
                bloqueInGame->copyToMatrix();
                delete bloqueInGame;

                if(tabla.verfificarFilasCompletas())
                {
                    int lineasBorradas = (int) tabla.indicesDelete.size();

                    if(lineasBorradas > 0 && lineasBorradas <= 4)
                    {
                        partida.puntos += PUNTOS_POR_LINEA[lineasBorradas] * partida.nivel;
                    }

                    tabla.bajarElementosSuperiores();
                    partida.lineasTotales += lineasBorradas;
                }

                //aparece la pieza siguiente; si el jugador tiene algo guardado,
                //sale esa primero
                if(tipoReserva != -1)
                {
                    int tipoAux = tipoReserva;

                    //la guardada pasa a la reserva y se saca otra de la bolsa,
                    //asi el ciclo de piezas sigue advancing sin repetirse
                    tipoReserva = tipoSiguiente;
                    tipoSiguiente = siguienteTipo(bolsa, generador);

                    bloqueInGame = crearBloque(tipoAux, tabla);
                }
                else
                {
                    bloqueInGame = crearBloque(tipoSiguiente, tabla);
                    tipoSiguiente = siguienteTipo(bolsa, generador);
                }

                reservaUsada = false;

                //el nivel sube cada LINEAS_POR_NIVEL lineas y la caida acelera
                int nivelNuevo = 1 + (partida.lineasTotales / LINEAS_POR_NIVEL);

                if(nivelNuevo != partida.nivel)
                {
                    partida.nivel = nivelNuevo;
                    fallDelay = DELAY_INICIAL - ((partida.nivel - 1) * 50);

                    if(fallDelay < DELAY_MINIMO)
                    {
                        fallDelay = DELAY_MINIMO;
                    }
                }

                //si la pieza nueva no entra, se termino la partida
                if(bloqueInGame->isColision())
                {
                    game_over = true;
                }
                continue;
            }

            bloqueInGame->dibujarFantasma();
            bloqueInGame->dibujarBloque();

            dibujarLimites();
            dibujarPanel(partida.puntos, partida.nivel, partida.lineasTotales,
                         tipoSiguiente, tipoReserva);

            Sleep(30);
        }

        if(salir)
        {
            delete bloqueInGame;
            break;
        }

        delete bloqueInGame;
        bloqueInGame = nullptr;

        mostrarFin(partida.puntos, partida.nivel, partida.lineasTotales);
    }

    return 0;
}

Bloque* crearBloque(int tipo, Tabla& tabla)
{
    switch(tipo)
    {
    case 0: return new BloqueI(0, 0, tabla);
    case 1: return new BloqueO(0, 0, tabla);
    case 2: return new BloqueT(0, 0, tabla);
    case 3: return new BloqueS(0, 0, tabla);
    case 4: return new BloqueZ(0, 0, tabla);
    case 5: return new BloqueJ(0, 0, tabla);
    case 6: return new BloqueL(0, 0, tabla);
    }
    return new BloqueO(0, 0, tabla);
}

int siguienteTipo(std::vector<int>& bolsa, std::mt19937& generador)
{
    if(bolsa.empty())
    {
        for(int i = 0; i < 7; i++)
        {
            bolsa.push_back(i);
        }

        std::shuffle(bolsa.begin(), bolsa.end(), generador);
    }

    int tipo = bolsa.back();
    bolsa.pop_back();

    return tipo;
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
    int izquierda = (X_TABLERA - 1) * 2;
    int derecha = (X_TABLERA + ANCHO) * 2;
    int arriba = Y_TABLERA - 1;
    int abajo = Y_TABLERA + ALTO;

    for(int x = izquierda + 2; x < derecha; x += 2)
    {
        gotoxy(x, arriba); setColor(8); printf("%c", 196);
        gotoxy(x, abajo); setColor(8); printf("%c", 196);
    }

    for(int y = arriba + 1; y < abajo; y++)
    {
        gotoxy(izquierda, y); setColor(8); printf("%c", 179);
        gotoxy(derecha, y); setColor(8); printf("%c", 179);
    }

    gotoxy(izquierda, arriba); setColor(8); printf("%c", 218);
    gotoxy(derecha, arriba);   setColor(8); printf("%c", 191);
    gotoxy(izquierda, abajo);   setColor(8); printf("%c", 192);
    gotoxy(derecha, abajo);     setColor(8); printf("%c", 217);
}

void borrarRectangulo(int x, int y, int ancho, int alto)
{
    for(int i = 0; i < alto; i++)
    {
        gotoxy(x, y + i);
        setColor(COLOR_VACIO);

        for(int j = 0; j < ancho; j++)
        {
            printf(" ");
        }
    }
}

void dibujarPanel(int puntos, int nivel, int lineasTotales, int tipoSiguiente, int tipoReserva)
{
    int y = Y_TABLERA;
    int x = X_PANEL;

    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("PUNTOS");
    gotoxy(x + 10, y); setColor(14); printf("%d", puntos);

    y += 2;
    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("NIVEL");
    gotoxy(x + 10, y); setColor(14); printf("%d", nivel);

    y += 2;
    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("LINEAS");
    gotoxy(x + 10, y); setColor(14); printf("%d", lineasTotales);

    y += 3;
    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("SIGUIENTE");

    //el previsualizado se dibuja con un bloque real, recien creado y
    //soltado en el heap solo para pintar sus celdas
    y += 1;
    {
        Tabla tablaVacia;
        Bloque* pieza = crearBloque(tipoSiguiente, tablaVacia);
        borrarRectangulo(x, y, 8, 4);
        pieza->dibujarEn(x, y);
        delete pieza;
    }

    y += 6;
    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("RESERVA");

    y += 1;
    if(tipoReserva == -1)
    {
        borrarRectangulo(x, y, 8, 4);
        setColor(COLOR_TXT);
        gotoxy(x, y + 1); printf("vacia");
    }
    else
    {
        Tabla tablaVacia;
        Bloque* pieza = crearBloque(tipoReserva, tablaVacia);
        borrarRectangulo(x, y, 8, 4);
        pieza->dibujarEn(x, y);
        delete pieza;
    }

    y += 6;
    setColor(COLOR_TITULO);
    gotoxy(x, y); printf("CONTROLES");

    y += 2;
    setColor(COLOR_TXT);
    gotoxy(x, y);     printf("izq / der   mover");
    gotoxy(x, y + 1); printf("arriba      rotar");
    gotoxy(x, y + 2); printf("abajo       bajar");
    gotoxy(x, y + 3); printf("Z           reservar");
    gotoxy(x, y + 4); printf("P           pausa");
    gotoxy(x, y + 5); printf("Q           salir");
}

void mostrarPausa()
{
    int centroX = ((X_TABLERA * 2) + (ANCHO * 2)) / 2;

    borrarRectangulo(centroX - 12, Y_TABLERA + 8, 24, 5);

    gotoxy(centroX - 3, Y_TABLERA + 9); setColor(14); printf("PAUSA");
    gotoxy(centroX - 6, Y_TABLERA + 11); setColor(COLOR_TXT); printf("P para seguir");
}

void mostrarFin(int puntos, int nivel, int lineasTotales)
{
    int centroX = ((X_TABLERA * 2) + (ANCHO * 2)) / 2;
    int y = Y_TABLERA + 6;

    borrarRectangulo(centroX - 16, y, 32, 11);

    gotoxy(centroX - 5, y + 1); setColor(12); printf("GAME OVER");

    gotoxy(centroX - 6, y + 3); setColor(COLOR_TITULO); printf("PUNTOS");
    char buffer[32];
    sprintf(buffer, "%d", puntos);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 4); setColor(14); printf("%s", buffer);

    sprintf(buffer, "NIVEL %d", nivel);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 6); setColor(COLOR_TXT); printf("%s", buffer);

    sprintf(buffer, "LINEAS %d", lineasTotales);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 7); setColor(COLOR_TXT); printf("%s", buffer);

    gotoxy(centroX - 8, y + 9); setColor(COLOR_TXT); printf("ENTER  reiniciar");
    gotoxy(centroX - 5, y + 10); setColor(COLOR_TXT); printf("Q  salir");

    char tecla = _getch();

    if(tecla != 'Q' && tecla != 'q' && tecla != 27)
    {
        system("cls");
    }
}
