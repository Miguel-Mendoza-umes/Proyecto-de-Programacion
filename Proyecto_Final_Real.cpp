//Proyecto final de Miguel Abraham Mendoza López
//Carnet No. 202508086
//Curso de programacion 1 primer semestre
//Replica del juego Croassy Road
//Despues de más de 7 archivos fallidos alfin funciono

//librerias
#include <iostream>      // Para entrada/salida estándar
#include <conio.h>       // Para _kbhit() y _getch()
#include <windows.h>     // Para funciones de consola de Windows
#include <vector>        // Para contenedores dinámicos
#include <ctime>         // Para funciones de tiempo
#include <cstdlib>       // Para rand() y srand()
#include <algorithm>     // Para remove_if()

using namespace std;
//variables globales
// Colores para la consola (valores de atributos de texto de Windows)
const int COLOR_VERDE = 10;    // Color verde para árboles y vidas
const int COLOR_ROJO = 12;      // Color rojo para enemigos
const int COLOR_AMARILLO = 14;  // Color amarillo para jugador y monedas
const int COLOR_BLANCO = 15;    // Color blanco predeterminado
const int COLOR_AZUL = 9;       // Color azul para autos amigables
const int COLOR_GRIS = 8;       // Color gris para piedras
const int COLOR_CIAN = 11;      // Color cian para agua y menús
const int COLOR_MORADO = 13;    // Color morado para troncos y poder
const int COLOR_RESET = 7;      // Color por defecto de la consola

// Dimensiones del tablero de juego
const int ALTO = 25;            // Altura del área de juego
const int ANCHO = 60;           // Ancho del área de juego

// Constantes para tipos de obstáculos
const int AGUA = 1;             // Identificador de agua
const char ARBOL = 'T';         // Símbolo para árboles
const char PIEDRA = '^';        // Símbolo para piedras
const char AGUA_SIMBOLO = '~';  // Símbolo para agua
const char TRONCO = '=';        // Símbolo para troncos
const char VIDA = '+';          // Símbolo para vidas extra
const char PODER = '*';         // Símbolo para poder de inmunidad

//estructura de datos

// Estructura para representar posiciones en el tablero
struct Posicion {
    int x, y;  // Coordenadas x (horizontal) e y (vertical)
};

// Estructura para representar obstáculos en el juego
struct Obstaculo {
    Posicion pos;           // Posición en el tablero
    bool esEnemigo;         // Si es peligroso para el jugador
    int direccion;          // Dirección de movimiento (1 derecha, -1 izquierda)
    vector<string> sprite;  // Representación gráfica
    int color;              // Color del obstáculo
    int ancho;              // Ancho del sprite
    bool esInmovil;         // Si no se mueve (árboles, piedras)
    char simbolo;           // Símbolo para obstáculos inamovibles
    bool esTronco = false;  // Indica si es un tronco en el agua
};

// Estructura para representar monedas recolectables
struct Moneda {
    Posicion pos;           // Posición en el tablero
    bool recolectada;       // Si ya fue recolectada
};

// Estructura para el poder de inmunidad
struct Poder {
    Posicion pos;           // Posición en el tablero
    bool activo;            // Si está disponible para recolectar
    time_t tiempoObtencion; // Tiempo cuando fue recolectado
};

// Estructura para vidas extra
struct VidaExtra {
    Posicion pos;           // Posición en el tablero
    bool recolectada;       // Si ya fue recolectada
};

//mas variables 
int ancho = ANCHO;                  // Ancho actual del juego
int alto = ALTO;                    // Alto actual del juego
Posicion jugador;                   // Posición del jugador
vector<Obstaculo> obstaculos;       // Lista de obstáculos
vector<Moneda> monedas;             // Lista de monedas
vector<VidaExtra> vidasExtra;       // Lista de vidas extra
Poder poderInmunidad = {{0, 0}, false, 0}; // Poder de inmunidad
int puntuacion = 0;                 // Puntuación actual
int nivel = 1;                      // Nivel actual
int velocidad = 150;                // Velocidad del juego
bool juegoActivo = false;           // Estado del juego
bool salirJuego = false;            // Control de salida
string causaPerdida;                // Razón de perder
int vidas = 3;                      // Vidas restantes
bool tieneInmunidad = false;        // Estado de inmunidad
time_t tiempoInmunidad = 0;         // Tiempo de obtención de inmunidad


//dibujos

// Sprite del jugador (3 líneas)
vector<string> spriteJugador = {
    " O ",      // Cabeza
    "/|\\",     // Torso y brazos
    "/ \\"      // Piernas
};


// Sprite de auto (4 líneas)
vector<string> spriteAuto = {
    "  ______       ",
    " /|_||_\\`.__  ",
    "(   _    _ _\\ ",
    "=`-(_)--(_)-'  "
};

// Sprite de camión (4 líneas)
vector<string> spriteCamion = {
    "      ____        ",
    " ____//_]|________",
    "(o _ |  -|   _  o|",
    " `(_)-------(_)--'"
};

// Sprite de tronco (1 línea)
vector<string> spriteTronco = {
    "==========="
};

// Sprite de bomba (4 líneas)
vector<string> spriteBomba = {
    "  ( ) ",
    " /   \\",
    "|     |",
    " \\___/"
};

// Sprite de moneda (1 línea)
vector<string> spriteMoneda = {
    " $ "
};

// Sprite de agua (1 línea)
vector<string> spriteAgua = {
    "~~~~~~~~~~"
};

// Sprite de vida extra (1 línea)
vector<string> spriteVida = {
    " + "
};

// Sprite de poder (1 línea)
vector<string> spritePoder = {
    " * "
};


// Estructura para configurar niveles de dificultad
struct Dificultad {
    string nombre;          // Nombre de la dificultad
    int velocidadBase;      // Velocidad base de los obstáculos
    int enemigosPorNivel;   // Cantidad de enemigos por nivel
    int obstaculosPorNivel; // Cantidad de obstáculos por nivel
    bool tieneBombas;       // Si incluye bombas como obstáculos
};

//dificultad
vector<Dificultad> dificultades = {
    {"Fácil", 300, 2, 3, false},    // Dificultad fácil
    {"Normal", 150, 3, 4, false},   // Dificultad normal
    {"Difícil", 50, 4, 5, true}     // Dificultad difícil
};

int dificultadSeleccionada = 1;      // Dificultad predeterminada (Normal)

// funciones en consola
/**
 * Mueve el cursor de la consola a la posición especificada
 * @param x Coordenada horizontal (columna)
 * @param y Coordenada vertical (fila)
 */
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


/**
 * Cambia el color del texto en la consola
 * @param color Código de color a aplicar
 */
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/**
 * Oculta el cursor en la consola para mejor experiencia visual
 */
void ocultarCursor() {
    CONSOLE_CURSOR_INFO cursor;
    cursor.dwSize = 100;
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}


/**
 * Limpia la pantalla de la consola
 */
void limpiarPantalla() {
    system("cls");
}

//funciones de dibujado de objetos
//menu principal
int menuInicio()
{
    int opcion = 1;
    do
    //seleccion de opciones
    {
        system("cls");
        setColor(COLOR_CIAN);
        cout << "===== CROSSY CONSOLE ROAD DE TEMU=====\n";
        setColor(COLOR_BLANCO);
        cout << "[1] Comenzar Juego\n";
        cout << "[2] Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;
    } while (opcion != 1 && opcion != 2);
    return opcion;
}

void dibujarBorde()
{
    setColor(COLOR_GRIS);
    // Dibuja bordes horizontales
    for (int i = 0; i < ancho; i++)
    {
        gotoxy(i, 0);
        cout << char(205);// Línea superior
        gotoxy(i, alto - 1);
        cout << char(205);// Línea inferior
    }
    // Dibuja bordes verticales
    for (int i = 0; i < alto; i++)
    {
        gotoxy(0, i);
        cout << char(186); // Línea izquierda
        gotoxy(ancho - 1, i);
        cout << char(186);// Línea derecha
    }
    // Dibuja esquinas
    gotoxy(0, 0);
    cout << char(201);  // Esquina superior izquierda
    gotoxy(ancho - 1, 0);
    cout << char(187);  // Esquina superior derecha
    gotoxy(0, alto - 1);
    cout << char(200);  // Esquina inferior izquierda
    gotoxy(ancho - 1, alto - 1);
    cout << char(188);  // Esquina inferior derecha
}

/**
 * Dibuja un sprite en la posición especificada
 * @param pos Posición donde dibujar
 * @param sprite Sprite a dibujar
 * @param color Color a usar
 */
void dibujarSprite(Posicion pos, const vector<string> &sprite, int color) {
    setColor(color);
    for (size_t i = 0; i < sprite.size(); i++) {
        gotoxy(pos.x, pos.y + i);
        cout << sprite[i];
    }
}

/**
 * Dibuja al jugador con el color apropiado según su estado
 */
void dibujarJugador() {
    if (tieneInmunidad) {
        // Efecto de parpadeo cuando tiene inmunidad
        time_t ahora = time(NULL);
        if ((ahora - tiempoInmunidad) % 2 == 0) 
            dibujarSprite(jugador, spriteJugador, COLOR_MORADO);
        else
            dibujarSprite(jugador, spriteJugador, COLOR_AMARILLO);
    } else {
        dibujarSprite(jugador, spriteJugador, COLOR_AMARILLO);
    }
}

// funciones de elementos
/**
 * Genera obstáculos inamovibles (árboles y piedras)
 */
void generarObstaculosInmoviles() {
    // Genera árboles
    for (int i = 0; i < 3 + nivel; i++) {
        Obstaculo arbol;
        arbol.pos.x = 1 + rand() % (ancho - 2);
        arbol.pos.y = 2 + rand() % (alto - 4);
        arbol.esEnemigo = false;
        arbol.esInmovil = true;
        arbol.simbolo = ARBOL;
        arbol.color = COLOR_VERDE;
        obstaculos.push_back(arbol);
    }

    // Genera piedras
    for (int i = 0; i < 2 + nivel; i++) {
        Obstaculo piedra;
        piedra.pos.x = 1 + rand() % (ancho - 2);
        piedra.pos.y = 2 + rand() % (alto - 4);
        piedra.esEnemigo = false;
        piedra.esInmovil = true;
        piedra.simbolo = PIEDRA;
        piedra.color = COLOR_GRIS;
        obstaculos.push_back(piedra);
    }
}


/**
 * Genera vidas extra en posiciones aleatorias
 */
void generarVidasExtra() {
    for (int i = 0; i < 1 + nivel / 2; i++) {
        VidaExtra vida;
        vida.pos.x = 1 + rand() % (ancho - 2);
        vida.pos.y = 2 + rand() % (alto - 4);
        vida.recolectada = false;
        vidasExtra.push_back(vida);
    }
}


/**
 * Genera el poder de inmunidad con cierta probabilidad
 */
void generarPoderInmunidad() {
    if (rand() % 5 == 0) {  // 20% de probabilidad de aparecer
        poderInmunidad.pos.x = 1 + rand() % (ancho - 2);
        poderInmunidad.pos.y = 2 + rand() % (alto - 4);
        poderInmunidad.activo = true;
    }
}

//funciones de inicializacion
/**
 * Inicializa el juego con los valores por defecto
 */
void inicializarJuego() {
    // Posición inicial del jugador
    jugador.x = ancho / 2 - spriteJugador[0].length() / 2;
    jugador.y = alto - 5;
    
    // Limpiar contenedores
    obstaculos.clear();
    monedas.clear();
    vidasExtra.clear();
    
    // Resetear estados
    poderInmunidad.activo = false;
    puntuacion = 0;
    nivel = 1;
    vidas = 3;
    tieneInmunidad = false;
    velocidad = dificultades[dificultadSeleccionada].velocidadBase;
    juegoActivo = true;

    //Generar vehiculos 
    for (int i = 0; i < nivel * dificultades[dificultadSeleccionada].obstaculosPorNivel; i++) {
        Obstaculo o;
        o.pos.y = 2 + rand() % (alto - 6 - 5); 
        o.pos.x = rand() % (ancho - 10);

        // Aleatoriamente decide si es auto (amigable) o camión (enemigo)
        if (rand() % 2 == 0) {
            o.sprite = spriteAuto;
            o.color = COLOR_AZUL;
            o.esEnemigo = false;
        }
        else
        {
            o.sprite = spriteCamion;
            o.color = COLOR_ROJO;
            o.esEnemigo = true;
        }

        o.ancho = o.sprite[0].length();
        o.direccion = (rand() % 2 == 0) ? 1 : -1; // Dirección aleatoria
        o.esInmovil = false;

        obstaculos.push_back(o);
    }

    // Generar filas de agua con troncos
    int filasDeAgua = (alto - 1 - 3) / 5 + 1;
    for (int i = 0; i < filasDeAgua; i++) {
        int y_pos = 3 + i * 5;
        int numTroncosPorFila = 2 + rand() % 3;  // 2-4 troncos por fila
        
        for (int j = 0; j < numTroncosPorFila; j++) {
            Obstaculo tronco;
            tronco.pos.y = y_pos;
            tronco.esTronco = true;
            tronco.pos.x = rand() % (ancho - spriteTronco[0].length());
            tronco.sprite = spriteTronco;
            tronco.color = COLOR_MORADO;
            tronco.esEnemigo = false;
            tronco.ancho = spriteTronco[0].length();
            tronco.direccion = (rand() % 2 == 0) ? 1 : -1;
            tronco.esInmovil = false;
            obstaculos.push_back(tronco);
        }
    }

    // Generar otros elementos del juego
    generarObstaculosInmoviles();

    // Generar monedas
    for (int i = 0; i < 5 + nivel * 2; i++) {
        Moneda m;
        m.pos.x = rand() % (ancho - spriteMoneda[0].length());
        m.pos.y = rand() % (alto - 3) + 1;
        m.recolectada = false;
        monedas.push_back(m);
    }

    generarVidasExtra();
    generarPoderInmunidad();

    // Mensaje especial para dificultad difícil
    if (dificultadSeleccionada == 2)
    {
        limpiarPantalla();
        setColor(COLOR_ROJO);
        gotoxy(ancho / 2 - 15, alto / 2);
        cout << "ESCONDETE DONDE PUEDAS";
        setColor(COLOR_BLANCO);
        gotoxy(ancho / 2 - 10, alto / 2 + 2);
        cout << "Preparate...";
        Sleep(2000);
    }
}

void dibujarObstaculos() {
    for (const auto &o : obstaculos)
    {
        if (o.esInmovil)
        {
            gotoxy(o.pos.x, o.pos.y);
            setColor(o.color);
            cout << o.simbolo;
        }
        else
        {
            dibujarSprite(o.pos, o.sprite, o.color);
        }
    }
}

void dibujarMonedas() {
    for (const auto &m : monedas)
    {
        if (!m.recolectada)
        {
            dibujarSprite(m.pos, spriteMoneda, COLOR_AMARILLO);
        }
    }
}

void dibujarVidasExtra() {
    for (const auto &v : vidasExtra)
    {
        if (!v.recolectada)
        {
            dibujarSprite(v.pos, spriteVida, COLOR_VERDE);
        }
    }
}

void dibujarPoderInmunidad() {
    if (poderInmunidad.activo)
    {
        dibujarSprite(poderInmunidad.pos, spritePoder, COLOR_MORADO);
    }
}

void dibujarAgua() {
    setColor(COLOR_CIAN);
    for (int y = 3; y < alto - 1; y += 5)
    {
        for (int x = 1; x < ancho - 1; x += spriteAgua[0].length())
        {

            gotoxy(x, y);
            cout << string(min((int)spriteAgua[0].length(), ancho - 1 - x), '~');
        }
    }
}

// funciones de logica
/**
 * Mueve los obstáculos según su dirección
 */
void moverObstaculos() {
    for (auto &o : obstaculos) {
        if (!o.esInmovil) {
            o.pos.x += o.direccion;

            // Rebotar en los bordes
            if (o.pos.x <= 0) {
                o.pos.x = 0;
                o.direccion = 1;
            } else if (o.pos.x + o.ancho >= ancho - 1) {
                o.pos.x = ancho - 1 - o.ancho;
                o.direccion = -1;
            }
        }
    }

    // En dificultad difícil, las bombas pueden reaparecer en nuevas posiciones
    if (dificultades[dificultadSeleccionada].tieneBombas && (rand() % 100) < 5) {
        for (auto &o : obstaculos) {
            if (o.sprite == spriteBomba) {
                o.pos.x = rand() % (ancho - o.ancho);
                o.pos.y = rand() % (alto - o.sprite.size());
            }
        }
    }
}


/**
 * Verifica si dos sprites están colisionando
 * @param pos1 Posición del primer sprite
 * @param sprite1 Sprite del primer objeto
 * @param pos2 Posición del segundo sprite
 * @param sprite2 Sprite del segundo objeto
 * @return true si hay colisión, false en caso contrario
 */
bool colision(const Posicion &pos1, const vector<string> &sprite1, 
              const Posicion &pos2, const vector<string> &sprite2) {
    int ancho1 = sprite1.empty() ? 0 : sprite1[0].length();
    int alto1 = sprite1.size();
    int ancho2 = sprite2.empty() ? 0 : sprite2[0].length();
    int alto2 = sprite2.size();

    return (pos1.x < pos2.x + ancho2 &&
            pos1.x + ancho1 > pos2.x &&
            pos1.y < pos2.y + alto2 &&
            pos1.y + alto1 > pos2.y);
}

/**
 * Verifica colisión simple (misma posición)
 * @param pos1 Primera posición
 * @param pos2 Segunda posición
 * @return true si están en la misma posición
 */
bool colisionSimple(const Posicion &pos1, const Posicion &pos2) {
    return (pos1.x == pos2.x && pos1.y == pos2.y);
}


/**
 * Verifica si el jugador está sobre un tronco
 * @param jugador Posición del jugador
 * @param obstaculos Lista de obstáculos
 * @return true si está sobre un tronco
 */
bool estaSobreTronco(const Posicion& jugador, const vector<Obstaculo>& obstaculos) {
    int piesJugador = jugador.y + spriteJugador.size() - 1;

    for (const auto& obstaculo : obstaculos) {
        if (obstaculo.esTronco) {
            if (piesJugador == obstaculo.pos.y) {
                if (jugador.x + spriteJugador[0].length() > obstaculo.pos.x && 
                    jugador.x < obstaculo.pos.x + obstaculo.sprite[0].length()) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Verifica si el jugador está en el agua
 * @param jugador Posición del jugador
 * @return true si está en el agua
 */
bool estaEnAgua(const Posicion& jugador) {
    for (int y_agua = 3; y_agua < ALTO - 1; y_agua += 5) {
        if (jugador.y <= y_agua && jugador.y + spriteJugador.size() > y_agua) {
            return true;
        }
    }
    return false;
}


void verificarInmunidad()
{
    if (tieneInmunidad)
    {
        time_t ahora = time(NULL);
        if (difftime(ahora, tiempoInmunidad) >= 15)
        {
            tieneInmunidad = false;
        }
    }
}

void actualizarJuego() {
    verificarInmunidad();
    
    Posicion posAnterior = jugador;
    if (!tieneInmunidad)
    {
        for (const auto &o : obstaculos)
        {
            if (o.esInmovil)
            {
                    if (jugador.x < o.pos.x + 1 && 
                    jugador.x + spriteJugador[0].length() > o.pos.x &&
                    jugador.y < o.pos.y + 1 && 
                    jugador.y + spriteJugador.size() > o.pos.y)
                {
                    jugador = posAnterior;
                    break;
                }
            }
            else if (o.esTronco) {
    continue;
    }
    else if (colision(jugador, spriteJugador, o.pos, o.sprite))

            {
                if (o.sprite == spriteBomba)
                {
                    obstaculos.erase(remove_if(obstaculos.begin(), obstaculos.end(),
                                              [&o](const Obstaculo &obs)
                                              { return obs.pos.x == o.pos.x && obs.pos.y == o.pos.y; }),
                                    obstaculos.end());
                    puntuacion += 20; 
                }
                else
                {
                    vidas--;
                    if (vidas <= 0)
                    {
                        juegoActivo = false;
                        causaPerdida = o.esEnemigo ? "¡Un vehiculo te atropello!" : "¡Chocaste con un obstaculo!";
                    }
                    else
                    {
                        jugador.x = ancho / 2 - spriteJugador[0].length() / 2;
                        jugador.y = alto - spriteJugador.size() - 1;
                    }
                    return;
                }
            }
        }

        bool enTronco = false;
        for (const auto& o : obstaculos) {
            if (o.esTronco) {
                if (jugador.y == o.pos.y && 
                    jugador.x >= o.pos.x && 
                    jugador.x < o.pos.x + o.ancho) {
                    enTronco = true;
                    jugador.x += o.direccion;
                    break;
                }
            }
        }
        bool enAgua = false;
        for (int y = 3; y < alto - 1; y += 5) {
            if (jugador.y == y) {
            enAgua = true;
            break;
        }
    }

       if (enAgua && !enTronco) {
            vidas--;
            if (vidas <= 0) {
                juegoActivo = false;
                causaPerdida = "¡Te caiste al agua!";
        } else {
            
            jugador.x = ancho / 2 - spriteJugador[0].length() / 2;
            jugador.y = alto - spriteJugador.size() - 1;
        }
        return;
    }

    for (auto &m : monedas)
    {
        if (!m.recolectada && colision(jugador, spriteJugador, m.pos, spriteMoneda))
        {
            m.recolectada = true;
            puntuacion += 10 * nivel;

            if (dificultades[dificultadSeleccionada].tieneBombas && (rand() % 100 < 90))
            {
                Obstaculo bomba;
                bomba.pos.x = rand() % (ancho - spriteBomba[0].length());
                bomba.pos.y = rand() % (alto - spriteBomba.size());
                bomba.esEnemigo = true;
                bomba.direccion = 0;
                bomba.sprite = spriteBomba;
                bomba.color = COLOR_ROJO;
                bomba.ancho = spriteBomba[0].length();
                bomba.esInmovil = false;
                obstaculos.push_back(bomba);
            }
        }
    }

    for (auto &v : vidasExtra)
    {
        if (!v.recolectada && colision(jugador, spriteJugador, v.pos, spriteVida))
        {
            v.recolectada = true;
            vidas++;
            puntuacion += 15;
        }
    }

    if (poderInmunidad.activo && colision(jugador, spriteJugador, poderInmunidad.pos, spritePoder))
    {
        poderInmunidad.activo = false;
        tieneInmunidad = true;
        tiempoInmunidad = time(NULL);
        puntuacion += 30;
    }

    monedas.erase(remove_if(monedas.begin(), monedas.end(),
                          [](const Moneda &m)
                          { return m.recolectada; }),
                  monedas.end());

    vidasExtra.erase(remove_if(vidasExtra.begin(), vidasExtra.end(),
                      [](const VidaExtra &v)
                      { return v.recolectada; }),
                    vidasExtra.end());

    if (jugador.y <= 1)
    {
        nivel++;
        jugador.y = alto - spriteJugador.size() - 1;
        velocidad = max(50, dificultades[dificultadSeleccionada].velocidadBase - (nivel * 10));

        for (int i = 0; i < nivel * dificultades[dificultadSeleccionada].enemigosPorNivel; i++)
        {
            Obstaculo o;
            o.pos.y = 2 + rand() % (alto - 6);
            o.pos.x = rand() % (ancho - 10);

            if (rand() % 2 == 0)
            {
                o.sprite = spriteAuto;
                o.color = COLOR_AZUL;
                o.esEnemigo = false;
            }
            else
            {
                o.sprite = spriteCamion;
                o.color = COLOR_ROJO;
                o.esEnemigo = true;
            }

            o.ancho = o.sprite[0].length();
            o.direccion = (rand() % 2 == 0) ? 1 : -1;
            o.esInmovil = false;
            obstaculos.push_back(o);
        }

        generarObstaculosInmoviles();

        for (int i = 0; i < 3 + nivel; i++)
        {
            Moneda m;
            m.pos.x = rand() % (ancho - spriteMoneda[0].length());
            m.pos.y = rand() % (alto - 3) + 1;
            m.recolectada = false;
            monedas.push_back(m);
        }

        generarVidasExtra();

        if (rand() % 3 == 0) 
        {
            generarPoderInmunidad();
        }
    }
    }
}

void mostrarMenuPrincipal() {
    limpiarPantalla();
    setColor(COLOR_AMARILLO);
    gotoxy(ancho / 2 - 5, 3);
    cout << "CROSSY ROAD DE TEMU";

    setColor(COLOR_BLANCO);
    gotoxy(ancho / 2 - 8, 6);
    cout << "1. Iniciar Juego";
    gotoxy(ancho / 2 - 8, 8);
    cout << "2. Seleccionar Dificultad";
    gotoxy(ancho / 2 - 8, 10);
    cout << "3. Salir";

    gotoxy(ancho / 2 - 15, alto - 2);
    cout << "Usa W, A, S, D o flechas para moverte";
}

void mostrarMenuDificultad()
{
    limpiarPantalla();
    setColor(COLOR_AMARILLO);
    gotoxy(ancho / 2 - 10, 3);
    cout << "SELECCIONA DIFICULTAD";

    setColor(COLOR_BLANCO);
    for (int i = 0; i < dificultades.size(); i++)
    {
        gotoxy(ancho / 2 - 8, 6 + i * 2);
        if (i == dificultadSeleccionada)
        {
            setColor(COLOR_VERDE);
            cout << "> " << dificultades[i].nombre;
            setColor(COLOR_BLANCO);
        }
        else
        {
            cout << "  " << dificultades[i].nombre;
        }
    }

    gotoxy(ancho / 2 - 15, alto - 2);
    cout << "Presiona ESC para volver al menu";
}

void mostrarPantallaPerdida()
{
    limpiarPantalla();

    setColor(COLOR_ROJO);
    gotoxy(ancho / 2 - 5, 3);
    cout << "  _____  _____  _____  _____  _____  _____  _____  _____ ";
    gotoxy(ancho / 2 - 5, 4);
    cout << " |_____||_____||_____||_____||_____||_____||_____||_____|";
    gotoxy(ancho / 2 - 5, 5);
    cout << " |     ||     ||     ||     ||     ||     ||     ||     |";
    gotoxy(ancho / 2 - 5, 6);
    cout << " |  P  ||  E  ||  R  ||  D  ||  I  ||  S  ||  T  ||  E  |";
    gotoxy(ancho / 2 - 5, 7);
    cout << " |_____||_____||_____||_____||_____||_____||_____||_____|";

    setColor(COLOR_BLANCO);
    gotoxy(ancho / 2 - causaPerdida.length() / 2, 9);
    cout << causaPerdida;

    gotoxy(ancho / 2 - 10, 11);
    cout << "Puntuacion: " << puntuacion;

    gotoxy(ancho / 2 - 10, 13);
    cout << "Nivel alcanzado: " << nivel;

    gotoxy(ancho / 2 - 10, 15);
    cout << "Vidas restantes: " << vidas;

    setColor(COLOR_AMARILLO);
    gotoxy(ancho / 2 - 10, 17);
    cout << "¿Quieres jugar de nuevo?";

    setColor(COLOR_BLANCO);
    gotoxy(ancho / 2 - 8, 19);
    cout << "1. Si";
    gotoxy(ancho / 2 - 8, 20);
    cout << "2. No";
}

void mostrarInfoJuego()
{
    gotoxy(2, 0);
    setColor(COLOR_BLANCO);
    cout << "Puntos: " << puntuacion;

    gotoxy(ancho / 2 - 5, 0);
    cout << "Nivel: " << nivel;

    gotoxy(ancho - 20, 0);
    cout << "Vidas: " << vidas;

    if (tieneInmunidad)
    {
        time_t ahora = time(NULL);
        int segundosRestantes = 15 - difftime(ahora, tiempoInmunidad);
        gotoxy(ancho / 2 - 10, alto - 1);
        setColor(COLOR_MORADO);
        cout << "Inmunidad: " << segundosRestantes << "s";
    }
}

void manejarEntrada()
{
    if (_kbhit())
    {
        char tecla = _getch();

        if (!juegoActivo)
            return;

        Posicion posAnterior = jugador;

        if (tecla == 'w' || tecla == 'W' || tecla == 72)
        { 
            jugador.y--;
        }
        else if (tecla == 's' || tecla == 'S' || tecla == 80)
        { 
            jugador.y++;
        }
        else if (tecla == 'a' || tecla == 'A' || tecla == 75)
        { 
            jugador.x = max(1, jugador.x - 2);
        }
        else if (tecla == 'd' || tecla == 'D' || tecla == 77)
        { 
            jugador.x = min(ancho - (int)spriteJugador[0].length() - 1, jugador.x + 2);
        }

        jugador.x = max(1, min(jugador.x, ancho - (int)spriteJugador[0].length() - 1));

       
        for (const auto &o : obstaculos)
        {
            if (o.esInmovil)
            {
             
                if (jugador.x == o.pos.x && jugador.y == o.pos.y)
                {
                    jugador = posAnterior;
                    break;
                }
            }
        }
    }
}

int main()
{
    srand(time(NULL));
    ocultarCursor();

    int opcionMenu = 0;
    bool enMenuDificultad = false;

    while (!salirJuego)
    {
        if (!juegoActivo)
        {
            if (opcionMenu == 0)
            { 
                mostrarMenuPrincipal();

                if (_kbhit())
                {
                    char tecla = _getch();
                    if (tecla == '1')
                    {
                        inicializarJuego();
                    }
                    else if (tecla == '2')
                    {
                        enMenuDificultad = true;
                        opcionMenu = 1;
                    }
                    else if (tecla == '3')
                    {
                        salirJuego = true;
                    }
                }
            }
            else if (opcionMenu == 1)
            { 
                mostrarMenuDificultad();

                if (_kbhit())
                {
                    char tecla = _getch();
                    if (tecla == 27)
                    { 
                        enMenuDificultad = false;
                        opcionMenu = 0;
                    }
                    else if (tecla == 72)
                    { 
                        dificultadSeleccionada = max(0, dificultadSeleccionada - 1);
                    }
                    else if (tecla == 80)
                    { 
                        dificultadSeleccionada = min(2, dificultadSeleccionada + 1);
                    }
                    else if (tecla == 13)
                    { 
                        enMenuDificultad = false;
                        opcionMenu = 0;
                    }
                }
            }
            else
            { 
                mostrarPantallaPerdida();

                if (_kbhit())
                {
                    char tecla = _getch();
                    if (tecla == '1')
                    {
                        opcionMenu = 0; 
                    }
                    else if (tecla == '2')
                    {
                        salirJuego = true;
                    }
                }
            }
        }
        else
        {
            limpiarPantalla();

            manejarEntrada();
            moverObstaculos();
            actualizarJuego();

            dibujarBorde();
            dibujarAgua();
            dibujarObstaculos();
            dibujarMonedas();
            dibujarVidasExtra();
            dibujarPoderInmunidad();
            dibujarJugador();
            mostrarInfoJuego();

            Sleep(velocidad);
        }
    }

    limpiarPantalla();
    gotoxy(ancho / 2 - 5, alto / 2);
    cout << "¡Gracias por jugar!";
    Sleep(2000);

    return 0;
}