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
#include <thread>
#include <mutex>
#include "Bloques.h"

auto lastFall = std::chrono::steady_clock::now();
int fallDelay = 800;

//puntos por linea al completar 1, 2, 3 o 4 lineas (la 4 es el "tetris")
const int PUNTOS_POR_LINEA[5] = {0, 100, 300, 500, 800};

const int LINEAS_POR_NIVEL = 10;
const int DELAY_INICIAL = 800;
const int DELAY_MINIMO = 90;

//puntos por cada fila que baja la pieza al soltarla con la flecha de abajo
const int PUNTOS_POR_FILA_BAJADA = 1;

//----------------------------------------------------------------------------
//Sonidos
//
//Se usa la funcion Beep() de windows.h, que ya esta en kernel32: no hace
//falta linking extra ni ningun archivo de audio en el repo.
//Se pueden apagar con la tecla M durante la partida.
//----------------------------------------------------------------------------
namespace Sonido
{
    bool activo = true;

    //Beep es un recurso unico de Windows: si dos sonidos suenan al mismo tiempo
    //se superponen y se tapan entre si (medido: cinco pitidos de 100 ms al
    //mismo tiempo tardan 109 ms en total, o sea que ninguno espera al otro y
    //se mezclan). Con este cerrojo el segundo sonido espera a que termine el
    //primero, asi cada uno se escucha entero y no sale una mezcla
    std::mutex cerrojoSonido;

    ///toca una lista de tonos de a uno. Cada par de la lista es una frecuencia
    ///en herz y abajo los milisegundos que dura, asi que la lista de una linea
    ///es {frecuencia, duracion, frecuencia, duracion, ...}
    ///
    ///La lista tiene que ser static: el hilo se la lleva copiada por puntero y
    ///si fuera una variable de la funcion, al salir de ella quedaria apuntando
    ///a memoria liberada
    void tocar(const int* lista, int pares)
    {
        if(!activo)
        {
            return;
        }

        //Beep es bloqueante, o sea que si lo llamamos desde el hilo del juego
        //el juego se queda congelado mientras dura el pitido. Por eso va en un
        //hilo aparte
        std::thread([lista, pares]()
        {
            std::lock_guard<std::mutex> cerrojo(cerrojoSonido);

            for(int i = 0; i < pares; i++)
            {
                Beep(lista[i * 2], lista[i * 2 + 1]);
            }
        }).detach();
    }

    ///sonido de linea completada: un golpe corto, como la pieza que encaja
    ///contra el piso, y despues un arpegio que crece con la cantidad de lineas
    ///cerradas de una vez
    void lineaCompletada(int lineas)
    {
        if(lineas < 1)
        {
            lineas = 1;
        }

        if(lineas > 4)
        {
            lineas = 4;
        }

        //las duraciones no bajan de 75 ms porque cada llamada a Beep gasta
        //unos 20 ms de encima antes de arrancar el tono (medido), y con
        //duraciones mas cortas el pitido sale entrecortado
        static const int UNA_LINEA[4] =
            {130, 80, 784, 110};                                 //golpe + sol

        static const int DOS_LINEAS[6] =
            {130, 80, 784, 100, 1046, 100};                      //golpe + sol, do

        static const int TRES_LINEAS[8] =
            {130, 80, 784, 90, 1046, 90, 1318, 90};               //golpe + sol, do, mi

        static const int CUATRO_LINEAS[10] =
            {130, 80, 784, 85, 1046, 85, 1318, 85, 1567, 85};    //golpe + sol, do, mi, sol

        //indice 0 es para "sobran lineas", que no deberia pasar pero queda
        //por las dudas
        static const int* LISTAS[5] =
            {UNA_LINEA, UNA_LINEA, DOS_LINEAS, TRES_LINEAS, CUATRO_LINEAS};

        static const int PARES[5] = {4, 4, 6, 8, 10};

        tocar(LISTAS[lineas], PARES[lineas]);
    }

    ///sonido de derrota: una melodycita que va bajando de tono y termina con
    ///una nota larga y grave
    void derrota()
    {
        static const int MELODIA[10] =
            {392, 150, 330, 150, 262, 160, 196, 180, 147, 500};
        //                         sol     mi     do     sol bajo  re

        tocar(MELODIA, 5);
    }
}

void borrarCursor();

void dibujarLimites();
void dibujarPanel(int puntos, int nivel, int lineasTotales, int tipoSiguiente, int tipoReserva);
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

        while(!game_over)
        {
            if(_kbhit())
            {
                char tecla = _getch();

                if(tecla == 'Q' || tecla == 'q' || tecla == 27)
                {
                    salir = true;
                    break;
                }

                //prender y apagar los sonidos en el momento
                if(tecla == 'M' || tecla == 'm')
                {
                    Sonido::activo = !Sonido::activo;
                    continue;
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

            //la pantalla se arma entera en memoria y recien al final del
            //cuadro se vuelca a la consola; por eso no hace falta ir
            //borrando la pieza de a poco
            lienzo.limpiar();
            tabla.dibujarTabla();

            //movimiento con las flechas
            if(GetAsyncKeyState(VK_LEFT) & 0x1)  { bloqueInGame->moverIzquierda(); }
            if(GetAsyncKeyState(VK_RIGHT) & 0x1) { bloqueInGame->moverDerecha(); }
            if(GetAsyncKeyState(VK_UP) & 0x1)    { bloqueInGame->intentarRotar(); }

            auto now = std::chrono::steady_clock::now();

            //flecha de abajo: la pieza cae de golpe hasta el piso; cada fila
            //recorrida suma un punto
            if(GetAsyncKeyState(VK_DOWN) & 0x1)
            {
                partida.puntos += bloqueInGame->caerAlPiso() * PUNTOS_POR_FILA_BAJADA;
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

                        Sonido::lineaCompletada(lineasBorradas);
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

            bloqueInGame->dibujarBloque();

            dibujarLimites();
            dibujarPanel(partida.puntos, partida.nivel, partida.lineasTotales,
                         tipoSiguiente, tipoReserva);

            lienzo.volcar();

            Sleep(16);
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

    setColor(8);

    for(int x = izquierda + 2; x < derecha; x += 2)
    {
        gotoxy(x, arriba); imprimir((char) 196);
        gotoxy(x, abajo);  imprimir((char) 196);
    }

    for(int y = arriba + 1; y < abajo; y++)
    {
        gotoxy(izquierda, y); imprimir((char) 179);
        gotoxy(derecha, y);   imprimir((char) 179);
    }

    gotoxy(izquierda, arriba); imprimir((char) 218);
    gotoxy(derecha, arriba);   imprimir((char) 191);
    gotoxy(izquierda, abajo);   imprimir((char) 192);
    gotoxy(derecha, abajo);     imprimir((char) 217);
}

///dibuja una pieza del panel; el bloque se crea una sola vez por tipo y se
///guarda para no estar reservando memoria en cada cuadro. "cual" sirve para
///que la vista siguiente y la de la reserva tengan su propia copia: si
///compartieran una, sepondrian pisando todo el tiempo
void dibujarVista(int tipo, int x, int y, int cual)
{
    static int tipos[2] = {-2, -2};
    static Bloque* vistas[2] = {nullptr, nullptr};
    static Tabla tablaDeLasVistas;

    if(tipo != tipos[cual])
    {
        delete vistas[cual];

        vistas[cual] = crearBloque(tipo, tablaDeLasVistas);
        tipos[cual] = tipo;
    }

    vistas[cual]->dibujarEn(x, y);
}

void dibujarPanel(int puntos, int nivel, int lineasTotales, int tipoSiguiente, int tipoReserva)
{
    int y = Y_TABLERA;
    int x = X_PANEL;

    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("PUNTOS");
    gotoxy(x + 10, y); setColor(14); imprimirNumero(puntos);

    y += 2;
    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("NIVEL");
    gotoxy(x + 10, y); setColor(14); imprimirNumero(nivel);

    y += 2;
    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("LINEAS");
    gotoxy(x + 10, y); setColor(14); imprimirNumero(lineasTotales);

    y += 3;
    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("SIGUIENTE");

    y += 1;
    dibujarVista(tipoSiguiente, x, y, 0);

    y += 6;
    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("RESERVA");

    y += 1;
    if(tipoReserva == -1)
    {
        setColor(COLOR_TXT);
        gotoxy(x, y + 1); imprimir("vacia");
    }
    else
    {
        dibujarVista(tipoReserva, x, y, 1);
    }

    y += 6;
    setColor(COLOR_TITULO);
    gotoxy(x, y); imprimir("CONTROLES");

    y += 2;
    setColor(COLOR_TXT);
    gotoxy(x, y);     imprimir("izq / der   mover");
    gotoxy(x, y + 1); imprimir("arriba      rotar");
    gotoxy(x, y + 2); imprimir("abajo       soltar");
    gotoxy(x, y + 3); imprimir("Z           reservar");

    //la ultima linea avisa si el sonido esta prendido o apagado
    gotoxy(x, y + 4);

    if(Sonido::activo)
    {
        imprimir("M           sonido [on]");
    }
    else
    {
        imprimir("M           sonido [off]");
    }

    gotoxy(x, y + 5); imprimir("Q           salir");
}

void mostrarFin(int puntos, int nivel, int lineasTotales)
{
    //aca se llega solo cuando la pila se lleno, no cuando se sale con la Q
    Sonido::derrota();

    int centroX = ((X_TABLERA * 2) + (ANCHO * 2)) / 2;
    //se arma el cartel sobre una pantalla limpia, asi el texto se lee bien
    //aunque la pila este llena
    lienzo.limpiar();

    int y = Y_TABLERA + 3;
    char buffer[32];

    //recuadro del cartel
    setColor(COLOR_TITULO);

    for(int x = centroX - 15; x < centroX + 15; x += 2)
    {
        gotoxy(x, y); imprimir((char) 196);
        gotoxy(x, y + 12); imprimir((char) 196);
    }

    for(int f = y + 1; f < y + 12; f++)
    {
        gotoxy(centroX - 15, f); imprimir((char) 179);
        gotoxy(centroX + 14, f); imprimir((char) 179);
    }

    gotoxy(centroX - 15, y);   imprimir((char) 218);
    gotoxy(centroX + 14, y);   imprimir((char) 191);
    gotoxy(centroX - 15, y+12); imprimir((char) 192);
    gotoxy(centroX + 14, y+12); imprimir((char) 217);

    gotoxy(centroX - 5, y + 2); setColor(12); imprimir("GAME OVER");

    gotoxy(centroX - 6, y + 4); setColor(COLOR_TITULO); imprimir("PUNTOS");

    sprintf(buffer, "%d", puntos);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 5); setColor(14); imprimir(buffer);

    sprintf(buffer, "NIVEL %d", nivel);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 7); setColor(COLOR_TXT); imprimir(buffer);

    sprintf(buffer, "LINEAS %d", lineasTotales);
    gotoxy(centroX - (int) strlen(buffer) / 2, y + 8); setColor(COLOR_TXT); imprimir(buffer);

    gotoxy(centroX - 8, y + 10); setColor(COLOR_TXT); imprimir("ENTER  reiniciar");
    gotoxy(centroX - 5, y + 11); setColor(COLOR_TXT); imprimir("Q  salir");

    lienzo.volcar();

    char tecla = _getch();

    if(tecla != 'Q' && tecla != 'q' && tecla != 27)
    {
        system("cls");
    }
}
