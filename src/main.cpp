#include "Arduino.h"
#include <FastLED.h>

#define AIMANT 12

//Tailles de l'écran
#define WIDTH 36
#define HEIGHT 8
#define WH8 (WIDTH*HEIGHT/8)
#define NBLEDS 288

//Tron
//4 3 1 2
#define NO_RESPONSE 0
#define RIGHT 54
#define LEFT 52
#define UP 56
#define DOWN 50

#define COLOR_BLACK 0
#define COLOR_BLUE 0x2000FF
#define COLOR_ORANGE 0x60FF00
#define COLOR_GREEN 0xFF0000

#define UP_ACTION 0
#define DOWN_ACTION 1
#define RIGHT_ACTION 2
#define LEFT_ACTION 3

#define MAX_PLAYERS 3

#define PILE_SIZE 40

//Tetris
#define WIDTH_T 8
#define HEIGHT_T 36

#define EXIT 2
#define KEY_F 1
#define KEY_LEFT 3
#define KEY_RIGHT 4

#define FILE 4
#define PMAX 0

//Life
//#define LIFE_WARP


// Define launch types
#define LAUNCH_TRON      1
#define LAUNCH_TETRIS    2
#define LAUNCH_LIFE      3
#define LAUNCH_ENDING    4

#define Xmax 36
#define nbLettre 6
#define XLettremax 6
#define YLettremax 8
#define nbLed 288

/**
 * Déclarationd des variables et structures globales
 */

CRGBArray<NBLEDS> leds;
unsigned long colors[] = {0xFF0000, 0x0000FF, 0x00FF00, 0x00FFFF, 0xFFFF00, 0xFF00FF, 0xFFFFFF};
char dx[] = {0, 0, 1, -1, -1, 1, -1, 1};
char dy[] = {-1, 1, 0, 0, -1, -1, 1, 1};

//Tron
unsigned char walls = 1;
unsigned char start_x[] = {0, 0};
unsigned char start_y[] = {0, 0};

typedef struct Point Point;

struct Point
{
	char x, y;
	unsigned char id;
};

Point pile[PILE_SIZE];
char pile_debut, pile_fin;

typedef struct Player Player;
struct Player
{
	char x, y;
	int score;
};

typedef struct Game Game;
struct Game
{
	char nbrPlayer;
	Player players[MAX_PLAYERS];
	unsigned char grid[WIDTH];
};
Game g;

unsigned char screen[WIDTH];

typedef struct TronPlayer TronPlayer;
struct TronPlayer
{
	char x, y;
	char dx, dy;
	char state;
	unsigned long color;
};

struct Tron
{
	TronPlayer p1, p2;
};
typedef struct Tron Tron;

//Eirbot
static uint8_t hue;
static uint8_t eirbot;
uint8_t lumi_fond = 0;
uint8_t lumi_eirbot = 0;

//Tetris
unsigned char form_file[FILE];
const unsigned int forms2[] = {
		0b0101100101101010,
		0b0001010110011101,
		0b0001010110010110,

		0b0000010010000001,
		0b0000010010001001,

		0b0001010101101010,
		0b0010010101101001
};
unsigned char grid[PMAX + 1][HEIGHT_T];
int tetrisLoop = 0;

//Life
unsigned char *grid_life = screen;

//Dictionnaire pour defilement
const PROGMEM int dico[27][6] = {
		{0b01111110, 0b10010000, 0b10010000, 0b10010000, 0b01111110, 0b00000000},
		{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01111100, 0b00000000},
		{0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100, 0b00000000},
		{0b11111110, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0b00000000},
		{0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0b00000000},
		{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b10000000, 0b00000000},
		{0b01111100, 0b10000010, 0b10000010, 0b10001010, 0b10001110, 0b00000000},
		{0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0b00000000},
		{0b00000000, 0b10000010, 0b11111110, 0b10000010, 0b00000000, 0b00000000},
		{0b00000100, 0b00000010, 0b10000010, 0b11111100, 0b10000000, 0b00000000},
		{0b11111110, 0b00010000, 0b00101000, 0b01000100, 0b10000010, 0b00000000},
		{0b11111110, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000000},
		{0b11111110, 0b01100000, 0b00110000, 0b01100000, 0b11111110, 0b00000000},
		{0b11111110, 0b01100000, 0b00110000, 0b00011000, 0b11111110, 0b00000000},
		{0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0b00000000},
		{0b11111110, 0b10010000, 0b10010000, 0b10010000, 0b01100000, 0b00000000},
		{0b01111100, 0b10000010, 0b10001010, 0b10000100, 0b01111010, 0b00000000},
		{0b11111110, 0b10010000, 0b10010000, 0b10011000, 0b01100110, 0b00000000},
		{0b01100100, 0b10010010, 0b10010010, 0b10010010, 0b01001100, 0b00000000},
		{0b10000000, 0b10000000, 0b11111110, 0b10000000, 0b10000000, 0b00000000},
		{0b11111100, 0b00000010, 0b00000010, 0b00000010, 0b11111100, 0b00000000},
		{0b11100000, 0b00111000, 0b00001110, 0b00111000, 0b11100000, 0b00000000},
		{0b11111000, 0b00001110, 0b00111000, 0b00001110, 0b11111000, 0b00000000},
		{0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110, 0b00000000},
		{0b11100000, 0b00010000, 0b00011110, 0b00010000, 0b11100000, 0b00000000},
		{0b10000110, 0b10001010, 0b10010010, 0b10100010, 0b11000010, 0b00000000},
		{0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000}
};
int temp;





/**
 * FONCTIONS GLOBALES
 */

void setLed(int x, int y, long color);
void fillScreen(long color);
unsigned long getLed(int x, int y);
unsigned char coordOK(int x, int y);

/**
 * FONCTIONS DEFILEMENT
 */

void setLedDefil(int x, int y, long color);
void setText(String text, long color);
void defilement(String txt, long color, int dt, int pause);

/**
 * FONCTIONS TRON
 */

void clearPile();
unsigned char isPileVide();
struct Point pop();
void push(Point p);
void setGameGrid(struct Game *game, unsigned char x, unsigned char y, unsigned char v);
unsigned char getGameGrid(Game *game, unsigned char x, unsigned char y);
unsigned char isGameDefined(Game *game);
void playAction(Game *game, unsigned char action, unsigned char id);
void calculateVoronoi(Game *g);
char getBestAction(Game *game, unsigned char id);
void initGame2(Game *game, int n, unsigned char x[], unsigned char y[]);
unsigned char getNextCommandPlayer2();
unsigned char getNextCommandPlayer1();
void setLedTron(int x, int y, long color);
int getLedTron(int x, int y);
void clearScreenTron();
struct Tron newTron();
void moveTronPlayer(struct TronPlayer *p);
void moveTron(Tron *t);
void checkForEndTronPlayer(TronPlayer *p);
void checkForEndTron(Tron *t);
void paintScreenTronPlayer(TronPlayer *p);
void paintScreenTron(Tron *t);
void getCommandsTronPlayer(TronPlayer *p, unsigned char (*getCommand)(void));
void getCommandsTron(Tron *t);
unsigned char tronFinished(Tron *t);
void tickTron(Tron *t);
void launchTron();

/**
 * FONCTIONS EIRBOT
 */

void affichage_eirbot();
void loop_eirbot();
void affichage_cest();
void affichage_fromage();

/**
 * FONCTIONS TETRIS
 */

unsigned char outOfScreen(unsigned char x, unsigned char y);
void setGrid(unsigned char g, unsigned char x, unsigned char y, unsigned long v);
unsigned long getGrid(unsigned char g, unsigned char x, unsigned char y);
void clearGrid(unsigned char g);
void copyGrid(unsigned char g0, unsigned char g1);
void getForm(unsigned char id, unsigned char o, char x[4], char y[4]);
unsigned char formOk(unsigned char g, unsigned char id, unsigned char o, unsigned char x, unsigned char y);
long printForm(unsigned char g, int id, int o, int x, int y, unsigned long value, unsigned char explode);
long evaluate2(unsigned char g);
long brutForce(int prof, unsigned char *xp, unsigned char *yp, unsigned char *op);
void startTetris();


/**
 * FONCTIONS LIFE
 */

void setGridLife(char x, char y, unsigned char v);
unsigned char getGridLife(char x, char y);
void clearLife();
unsigned char step_life();
void initLife();
void startLife();

/**
 * SETUP
 */

void setup() {
	Serial.begin(9600);
	FastLED.addLeds<WS2812B, 3/*datapin*/, RGB>(leds, NBLEDS);
	randomSeed(analogRead(0));
	hue = 0;
	eirbot = 125;
	pinMode(AIMANT, INPUT_PULLUP);
}

/**
 * LOOP
 */

char launchType = LAUNCH_TRON; // Démarrage

void loop() {

	if (digitalRead(AIMANT) == 0) {

		// On efface l'écran et on éteint les leds
		leds.fadeToBlackBy(40);
		leds = CHSV(0, 255, 0);

		// On retest l'état de l'aimant toutes les 2 secondes
		FastLED.delay(2000);
		launchType = LAUNCH_TRON; // Démarrage
	} else {

		switch (launchType) {
			case LAUNCH_TRON:
				launchTron();
				loop_eirbot();
				launchType++;
				break;
			case LAUNCH_TETRIS:
				startTetris();
				loop_eirbot();
				launchType++;
				break;
			case LAUNCH_LIFE:
				startLife();
				loop_eirbot();
				launchType++;
				break;
			default:
				launchType = LAUNCH_TRON; // On reset le launch type une fois arrivé au bout.
		}

	}
}


/**
 * FONCTIONS
 */

// FONCTIONS GLOBALES
void setLed(int x, int y, long color) { //
	leds[(y + (y % 2)) * WIDTH + (2 * ((y + 1) % 2) - 1) * (x + (y % 2))] = color;
}
void fillScreen(long color) { //
	for (int x = 0; x < 288; x++) {
		leds[x] = color;
	}
	FastLED.show();
}
unsigned long getLed(int x, int y) {
	CRGB v = leds[(y + (y % 2)) * WIDTH + (2 * ((y + 1) % 2) - 1) * (x + (y % 2))];
	return ((unsigned long) (v.r) << 16) | ((unsigned long) (v.g) << 8) | (unsigned long) (v.b);
}
unsigned char coordOK(int x, int y) { //
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}


// FONCTIONS TRON
void clearPile() { //
	pile_debut = pile_fin = 0;
}
unsigned char isPileVide() { //
	return pile_debut == pile_fin;
}
struct Point pop() { //
	Point r = pile[pile_debut++];
	if (pile_debut == PILE_SIZE)pile_debut = 0;
	return r;
}
void push(Point p) { //
	pile[pile_fin++] = p;
	if (pile_fin == PILE_SIZE)pile_fin = 0;
}
void setGameGrid(struct Game *game, unsigned char x, unsigned char y, unsigned char v) { //
	game->grid[x] = ((game->grid[x]) & (~(1 << y))) | (v << y);
}
unsigned char getGameGrid(Game *game, unsigned char x, unsigned char y) { //
	return (game->grid[x] >> y) & 1;
}
unsigned char isGameDefined(Game *game) {//
	return game->nbrPlayer > 0;
}
void playAction(Game *game, unsigned char action, unsigned char id) {//
	if (!walls) {
		game->players[id].x = (game->players[id].x + WIDTH + dx[action]) % WIDTH;
		game->players[id].y = (game->players[id].y + HEIGHT + dy[action]) % HEIGHT;
	} else {
		game->players[id].x += dx[action];
		game->players[id].y += dy[action];
	}
	if (!coordOK(game->players[id].x, game->players[id].y) || getGameGrid(game, game->players[id].x, game->players[id].y)) {
		game->players[id].score = -1;
	} else {
		setGameGrid(game, game->players[id].x, game->players[id].y, 1);
	}
}
void calculateVoronoi(Game *g) { //
	clearPile();
	for (int i = 0; i < g->nbrPlayer; i++) {
		if (g->players[i].x != -1) {
			Point p = {g->players[i].x, g->players[i].y, i};
			push(p);
			g->players[i].score = 0;
		}
	}

	while (!isPileVide()) {
		Point p = pop();
		Point q = {0, 0, p.id};
		for (int sens = 0; sens < 4; sens++) {
			if (!walls) {
				q.x = (p.x + dx[sens] + WIDTH) % WIDTH;
				q.y = (p.y + dy[sens] + HEIGHT) % HEIGHT;
				if (getGameGrid(g, q.x, q.y) == 0) {
					push(q);
					g->players[p.id].score++;
					setGameGrid(g, q.x, q.y, 1);
				}
			} else {
				q.x = p.x + dx[sens];
				q.y = p.y + dy[sens];
				if (coordOK(q.x, q.y) && getGameGrid(g, q.x, q.y) == 0) {
					push(q);
					g->players[p.id].score++;
					setGameGrid(g, q.x, q.y, 1);
				}
			}

		}
	}
}
char getBestAction(Game *game, unsigned char id) { //
	int bestScore = 0;
	char r = -1;
	for (int action = 0; action < 4; action++) {
		Game g2 = *game;
		playAction(&g2, action, id);
		if (g2.players[id].score != -1) {
			calculateVoronoi(&g2);
			if (g2.players[id].score > bestScore) {
				bestScore = g2.players[id].score;
				r = action;
			}
		}
	}
	return r;
}
void initGame2(Game *game, int n, unsigned char x[], unsigned char y[]) { //
	for (int x = 0; x < WIDTH; x++) {
		(game->grid)[x] = 0;
	}
	game->nbrPlayer = n;
	for (unsigned char i = 0; i < n; i++) {
		game->players[i].x = x[i];
		game->players[i].y = y[i];
		game->players[i].score = 0;
		setGameGrid(game, x[i], y[i], 1);
	}
}
unsigned char getNextCommandPlayer2() { //
	if (Serial.available() > 0) {
		return Serial.read();
	}
	return NO_RESPONSE;
}
unsigned char getNextCommandPlayer1() { //
//  if (player2.available() > 0) {
//    return player2.read();
//  }
	return NO_RESPONSE;
}
void setLedTron(int x, int y, long color) { //
	screen[x] = (screen[x] & (~(1 << y))) | ((color == COLOR_BLACK ? 0 : 1) << y);
	setLed(x, y, color);
}
int getLedTron(int x, int y) { //
	return (screen[x] >> y) & 1;
}
void clearScreenTron() { //
	for (int i = 0; i < WIDTH; i++) {
		screen[i] = 0;
	}
	fillScreen(COLOR_BLACK);
}
struct Tron newTron() { //
	Tron t = {{start_x[0], start_y[0], 1,  0, 0},
			  {start_x[1], start_y[1], -1, 0, 0}};
	t.p1.color = COLOR_BLUE;
	t.p2.color = COLOR_ORANGE;
	return t;
}
void moveTronPlayer(struct TronPlayer *p) { //
	if (!walls) {
		p->x = (p->x + p->dx + WIDTH) % WIDTH;
		p->y = (p->y + p->dy + HEIGHT) % HEIGHT;
	} else {
		p->x += p->dx;
		p->y += p->dy;
	}
}
void moveTron(Tron *t) {  //
	unsigned char action;
	//moveTronPlayer(&(t->p1));

	action = getBestAction(&g, 0);
	(t->p1).dx = dx[action];
	(t->p1).dy = dy[action];
	moveTronPlayer(&(t->p1));

	action = getBestAction(&g, 1);
	(t->p2).dx = dx[action];
	(t->p2).dy = dy[action];
	moveTronPlayer(&(t->p2));

	if ((t->p1).dx == 1) {
		action = RIGHT_ACTION;
	} else if ((t->p1).dx == -1) {
		action = LEFT_ACTION;
	} else if ((t->p1).dy == 1) {
		action = DOWN_ACTION;
	} else if ((t->p1).dy == -1) {
		action = UP_ACTION;
	}
	playAction(&g, action, 0);

	if ((t->p2).dx == 1) {
		action = RIGHT_ACTION;
	} else if ((t->p2).dx == -1) {
		action = LEFT_ACTION;
	} else if ((t->p2).dy == 1) {
		action = DOWN_ACTION;
	} else if ((t->p2).dy == -1) {
		action = UP_ACTION;
	}
	playAction(&g, action, 1);
}
void checkForEndTronPlayer(TronPlayer *p) { //
	if (p->x < 0 || p->y < 0 || p->x >= WIDTH || p->y >= HEIGHT || getLedTron(p->x, p->y) != 0) {
		p->state = -1;
	}
}
void checkForEndTron(Tron *t) { //
	checkForEndTronPlayer(&(t->p1));
	checkForEndTronPlayer(&(t->p2));
	if ((t->p1).x == (t->p2).x && (t->p1).y == (t->p2).y) {
		(t->p1).state = -1;
		(t->p2).state = -1;
	}
}
void paintScreenTronPlayer(TronPlayer *p) { //
	setLedTron(p->x, p->y, p->color);
}
void paintScreenTron(Tron *t) { //
	paintScreenTronPlayer(&(t->p1));
	paintScreenTronPlayer(&(t->p2));
}
void getCommandsTronPlayer(TronPlayer *p, unsigned char (*getCommand)(void)) { //
	unsigned char c = 1;
	while (c) {
		c = (*getCommand)();
		if (c == RIGHT && !(p->dx == -1 && p->dy == 0)) {
			p->dx = 1;
			p->dy = 0;
		}
		if (c == LEFT && !(p->dx == 1 && p->dy == 0)) {
			p->dx = -1;
			p->dy = 0;
		}
		if (c == UP && !(p->dx == 0 && p->dy == 1)) {
			p->dx = 0;
			p->dy = -1;
		}
		if (c == DOWN && !(p->dx == 0 && p->dy == -1)) {
			p->dx = 0;
			p->dy = 1;
		}
	}
}
void getCommandsTron(Tron *t) { //
	getCommandsTronPlayer(&(t->p1), &getNextCommandPlayer1);
	getCommandsTronPlayer(&(t->p2), &getNextCommandPlayer2);
}
unsigned char tronFinished(Tron *t) { //
	return (t->p1).state != 0 || (t->p2).state != 0;
}//*/
void tickTron(Tron *t) { //
	getCommandsTron(t);
	moveTron(t);
	checkForEndTron(t);
	if (tronFinished(t)) {
		if (t->p1.state == -1 && t->p2.state == -1)
			fillScreen(COLOR_GREEN);
		else if (t->p1.state == -1)
			fillScreen(COLOR_ORANGE);
		else
			fillScreen(COLOR_BLUE);
		delay(500);
		fillScreen(COLOR_BLACK);
		return;
	}
	paintScreenTron(t);

	FastLED.show();
	//delay(200);
}
void launchTron() { //
	start_x[0] = random(0, WIDTH);
	start_x[1] = random(0, WIDTH);
	start_y[0] = random(0, HEIGHT);
	start_y[1] = random(0, HEIGHT);
	clearScreenTron();
	Tron tron = newTron();
	paintScreenTron(&tron);

	initGame2(&g, 2, start_x, start_y);
	while (!tronFinished(&tron)) {
		tickTron(&tron);
		delay(50);
	}
	Serial.println("End");
}

// FONCTION EIRBOT
void loop_eirbot() {

	long int lol = random(100);

	if (lol > 90) {
		defilement("EIRBOT[SUPERIEUR[A[EIRSPACE", 0x505050, 2, 0);
	} else if (lol > 80) {
		for (int i = 0; i < 40; i++)
			affichage_fromage();
		for (int i = 0; i < 20; i++)
			affichage_cest();
		for (int i = 0; i < 40; i++)
			affichage_fromage();
	} else {
		for (int i = 0; i < 100; i++)
			affichage_eirbot();
	}
}
void affichage_eirbot() { //

	leds.fadeToBlackBy(40);
	lumi_fond = 125;
	lumi_eirbot = 255;
	eirbot++;

	//Arrière plan
	leds = CHSV(hue++, 255, lumi_fond);


	// Lettre E
	leds[34] = CHSV(eirbot, 255, lumi_eirbot);
	leds[33] = CHSV(eirbot, 255, lumi_eirbot);
	leds[32] = CHSV(eirbot, 255, lumi_eirbot);
	leds[31] = CHSV(eirbot, 255, lumi_eirbot);
	leds[37] = CHSV(eirbot, 255, lumi_eirbot);
	leds[106] = CHSV(eirbot, 255, lumi_eirbot);
	leds[109] = CHSV(eirbot, 255, lumi_eirbot);
	leds[110] = CHSV(eirbot, 255, lumi_eirbot);
	leds[111] = CHSV(eirbot, 255, lumi_eirbot);
	leds[112] = CHSV(eirbot, 255, lumi_eirbot);
	leds[178] = CHSV(eirbot, 255, lumi_eirbot);
	leds[181] = CHSV(eirbot, 255, lumi_eirbot);
	leds[250] = CHSV(eirbot, 255, lumi_eirbot);
	leds[249] = CHSV(eirbot, 255, lumi_eirbot);
	leds[248] = CHSV(eirbot, 255, lumi_eirbot);
	leds[247] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre I
	leds[258] = CHSV(eirbot, 255, lumi_eirbot);
	leds[259] = CHSV(eirbot, 255, lumi_eirbot);
	leds[260] = CHSV(eirbot, 255, lumi_eirbot);
	leds[261] = CHSV(eirbot, 255, lumi_eirbot);
	leds[262] = CHSV(eirbot, 255, lumi_eirbot);
	leds[243] = CHSV(eirbot, 255, lumi_eirbot);
	leds[188] = CHSV(eirbot, 255, lumi_eirbot);
	leds[171] = CHSV(eirbot, 255, lumi_eirbot);
	leds[116] = CHSV(eirbot, 255, lumi_eirbot);
	leds[99] = CHSV(eirbot, 255, lumi_eirbot);
	leds[42] = CHSV(eirbot, 255, lumi_eirbot);
	leds[43] = CHSV(eirbot, 255, lumi_eirbot);
	leds[44] = CHSV(eirbot, 255, lumi_eirbot);
	leds[45] = CHSV(eirbot, 255, lumi_eirbot);
	leds[46] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre R
	leds[239] = CHSV(eirbot, 255, lumi_eirbot);
	leds[238] = CHSV(eirbot, 255, lumi_eirbot);
	leds[237] = CHSV(eirbot, 255, lumi_eirbot);
	leds[236] = CHSV(eirbot, 255, lumi_eirbot);
	leds[192] = CHSV(eirbot, 255, lumi_eirbot);
	leds[167] = CHSV(eirbot, 255, lumi_eirbot);
	leds[195] = CHSV(eirbot, 255, lumi_eirbot);
	leds[164] = CHSV(eirbot, 255, lumi_eirbot);
	leds[120] = CHSV(eirbot, 255, lumi_eirbot);
	leds[121] = CHSV(eirbot, 255, lumi_eirbot);
	leds[122] = CHSV(eirbot, 255, lumi_eirbot);
	leds[123] = CHSV(eirbot, 255, lumi_eirbot);
	leds[95] = CHSV(eirbot, 255, lumi_eirbot);
	leds[93] = CHSV(eirbot, 255, lumi_eirbot);
	leds[48] = CHSV(eirbot, 255, lumi_eirbot);
	leds[51] = CHSV(eirbot, 255, lumi_eirbot);
	leds[23] = CHSV(eirbot, 255, lumi_eirbot);
	leds[19] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre B
	leds[270] = CHSV(eirbot, 255, lumi_eirbot);
	leds[271] = CHSV(eirbot, 255, lumi_eirbot);
	leds[272] = CHSV(eirbot, 255, lumi_eirbot);
	leds[233] = CHSV(eirbot, 255, lumi_eirbot);
	//leds[230] = CHSV(eirbot, 255, lumi_eirbot);
	leds[273] = CHSV(eirbot, 255, lumi_eirbot);
	leds[229] = CHSV(eirbot, 255, lumi_eirbot);
	leds[198] = CHSV(eirbot, 255, lumi_eirbot);
	leds[202] = CHSV(eirbot, 255, lumi_eirbot);
	leds[161] = CHSV(eirbot, 255, lumi_eirbot);
	leds[160] = CHSV(eirbot, 255, lumi_eirbot);
	leds[159] = CHSV(eirbot, 255, lumi_eirbot);
	leds[158] = CHSV(eirbot, 255, lumi_eirbot);
	leds[126] = CHSV(eirbot, 255, lumi_eirbot);
	leds[130] = CHSV(eirbot, 255, lumi_eirbot);
	leds[89] = CHSV(eirbot, 255, lumi_eirbot);
	//leds[86] = CHSV(eirbot, 255, lumi_eirbot);
	leds[85] = CHSV(eirbot, 255, lumi_eirbot);
	leds[57] = CHSV(eirbot, 255, lumi_eirbot);
	leds[54] = CHSV(eirbot, 255, lumi_eirbot);
	leds[55] = CHSV(eirbot, 255, lumi_eirbot);
	leds[56] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre O
	leds[227] = CHSV(eirbot, 255, lumi_eirbot);
	leds[226] = CHSV(eirbot, 255, lumi_eirbot);
	leds[225] = CHSV(eirbot, 255, lumi_eirbot);
	leds[224] = CHSV(eirbot, 255, lumi_eirbot);
	leds[223] = CHSV(eirbot, 255, lumi_eirbot);
	leds[204] = CHSV(eirbot, 255, lumi_eirbot);
	leds[208] = CHSV(eirbot, 255, lumi_eirbot);
	leds[155] = CHSV(eirbot, 255, lumi_eirbot);
	leds[151] = CHSV(eirbot, 255, lumi_eirbot);
	leds[132] = CHSV(eirbot, 255, lumi_eirbot);
	leds[136] = CHSV(eirbot, 255, lumi_eirbot);
	leds[83] = CHSV(eirbot, 255, lumi_eirbot);
	leds[79] = CHSV(eirbot, 255, lumi_eirbot);
	leds[60] = CHSV(eirbot, 255, lumi_eirbot);
	leds[64] = CHSV(eirbot, 255, lumi_eirbot);
	leds[11] = CHSV(eirbot, 255, lumi_eirbot);
	leds[10] = CHSV(eirbot, 255, lumi_eirbot);
	leds[9] = CHSV(eirbot, 255, lumi_eirbot);
	leds[8] = CHSV(eirbot, 255, lumi_eirbot);
	leds[7] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre O
	leds[282] = CHSV(eirbot, 255, lumi_eirbot);
	leds[283] = CHSV(eirbot, 255, lumi_eirbot);
	leds[284] = CHSV(eirbot, 255, lumi_eirbot);
	leds[285] = CHSV(eirbot, 255, lumi_eirbot);
	leds[286] = CHSV(eirbot, 255, lumi_eirbot);
	leds[219] = CHSV(eirbot, 255, lumi_eirbot);
	leds[212] = CHSV(eirbot, 255, lumi_eirbot);
	leds[147] = CHSV(eirbot, 255, lumi_eirbot);
	leds[140] = CHSV(eirbot, 255, lumi_eirbot);
	leds[75] = CHSV(eirbot, 255, lumi_eirbot);
	leds[68] = CHSV(eirbot, 255, lumi_eirbot);

	// now, let's first 20 leds to the top 20 leds,
	//leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);

	//}
	FastLED.delay(33);

}
void affichage_fromage() {

	leds.fadeToBlackBy(40);

	lumi_fond = 125;
	lumi_eirbot = 255;
	eirbot++;

	//Arrière plan
	leds = CHSV(hue++, 255, lumi_fond);

	// Lettre L
	leds[180] = CHSV(eirbot, 255, lumi_eirbot);
	leds[179] = CHSV(eirbot, 255, lumi_eirbot);
	leds[108] = CHSV(eirbot, 255, lumi_eirbot);
	leds[107] = CHSV(eirbot, 255, lumi_eirbot);
	leds[36] = CHSV(eirbot, 255, lumi_eirbot);
	leds[37] = CHSV(eirbot, 255, lumi_eirbot);
	leds[38] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre E
	leds[254] = CHSV(eirbot, 255, lumi_eirbot);
	leds[255] = CHSV(eirbot, 255, lumi_eirbot);
	leds[256] = CHSV(eirbot, 255, lumi_eirbot);
	leds[249] = CHSV(eirbot, 255, lumi_eirbot);
	leds[182] = CHSV(eirbot, 255, lumi_eirbot);
	leds[183] = CHSV(eirbot, 255, lumi_eirbot);
	leds[184] = CHSV(eirbot, 255, lumi_eirbot);
	leds[177] = CHSV(eirbot, 255, lumi_eirbot);
	leds[110] = CHSV(eirbot, 255, lumi_eirbot);
	leds[111] = CHSV(eirbot, 255, lumi_eirbot);
	leds[112] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre F
	leds[172] = CHSV(eirbot, 255, lumi_eirbot);
	leds[171] = CHSV(eirbot, 255, lumi_eirbot);
	leds[170] = CHSV(eirbot, 255, lumi_eirbot);
	leds[115] = CHSV(eirbot, 255, lumi_eirbot);
	leds[100] = CHSV(eirbot, 255, lumi_eirbot);
	leds[99] = CHSV(eirbot, 255, lumi_eirbot);
	leds[98] = CHSV(eirbot, 255, lumi_eirbot);
	leds[43] = CHSV(eirbot, 255, lumi_eirbot);
	leds[28] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre R
	leds[263] = CHSV(eirbot, 255, lumi_eirbot);
	leds[264] = CHSV(eirbot, 255, lumi_eirbot);
	leds[265] = CHSV(eirbot, 255, lumi_eirbot);
	leds[240] = CHSV(eirbot, 255, lumi_eirbot);
	leds[237] = CHSV(eirbot, 255, lumi_eirbot);
	leds[191] = CHSV(eirbot, 255, lumi_eirbot);
	leds[192] = CHSV(eirbot, 255, lumi_eirbot);
	leds[193] = CHSV(eirbot, 255, lumi_eirbot);
	leds[168] = CHSV(eirbot, 255, lumi_eirbot);
	leds[166] = CHSV(eirbot, 255, lumi_eirbot);
	leds[119] = CHSV(eirbot, 255, lumi_eirbot);
	leds[122] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre O
	leds[196] = CHSV(eirbot, 255, lumi_eirbot);
	leds[197] = CHSV(eirbot, 255, lumi_eirbot);
	leds[198] = CHSV(eirbot, 255, lumi_eirbot);
	leds[163] = CHSV(eirbot, 255, lumi_eirbot);
	leds[161] = CHSV(eirbot, 255, lumi_eirbot);
	leds[124] = CHSV(eirbot, 255, lumi_eirbot);
	leds[126] = CHSV(eirbot, 255, lumi_eirbot);
	leds[91] = CHSV(eirbot, 255, lumi_eirbot);
	leds[90] = CHSV(eirbot, 255, lumi_eirbot);
	leds[89] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre M
	leds[200] = CHSV(eirbot, 255, lumi_eirbot);
	leds[204] = CHSV(eirbot, 255, lumi_eirbot);
	leds[159] = CHSV(eirbot, 255, lumi_eirbot);
	leds[158] = CHSV(eirbot, 255, lumi_eirbot);
	leds[156] = CHSV(eirbot, 255, lumi_eirbot);
	leds[155] = CHSV(eirbot, 255, lumi_eirbot);
	leds[128] = CHSV(eirbot, 255, lumi_eirbot);
	leds[130] = CHSV(eirbot, 255, lumi_eirbot);
	leds[132] = CHSV(eirbot, 255, lumi_eirbot);
	leds[87] = CHSV(eirbot, 255, lumi_eirbot);
	leds[83] = CHSV(eirbot, 255, lumi_eirbot);
	leds[56] = CHSV(eirbot, 255, lumi_eirbot);
	leds[60] = CHSV(eirbot, 255, lumi_eirbot);
	leds[15] = CHSV(eirbot, 255, lumi_eirbot);
	leds[11] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre A
	leds[278] = CHSV(eirbot, 255, lumi_eirbot);
	leds[279] = CHSV(eirbot, 255, lumi_eirbot);
	leds[280] = CHSV(eirbot, 255, lumi_eirbot);
	leds[225] = CHSV(eirbot, 255, lumi_eirbot);
	leds[223] = CHSV(eirbot, 255, lumi_eirbot);
	leds[206] = CHSV(eirbot, 255, lumi_eirbot);
	leds[207] = CHSV(eirbot, 255, lumi_eirbot);
	leds[208] = CHSV(eirbot, 255, lumi_eirbot);
	leds[153] = CHSV(eirbot, 255, lumi_eirbot);
	leds[151] = CHSV(eirbot, 255, lumi_eirbot);
	leds[134] = CHSV(eirbot, 255, lumi_eirbot);
	leds[136] = CHSV(eirbot, 255, lumi_eirbot);
	leds[81] = CHSV(eirbot, 255, lumi_eirbot);
	leds[79] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre G
	leds[210] = CHSV(eirbot, 255, lumi_eirbot);
	leds[211] = CHSV(eirbot, 255, lumi_eirbot);
	leds[212] = CHSV(eirbot, 255, lumi_eirbot);
	leds[149] = CHSV(eirbot, 255, lumi_eirbot);
	leds[138] = CHSV(eirbot, 255, lumi_eirbot);
	leds[140] = CHSV(eirbot, 255, lumi_eirbot);
	leds[77] = CHSV(eirbot, 255, lumi_eirbot);
	leds[75] = CHSV(eirbot, 255, lumi_eirbot);
	leds[66] = CHSV(eirbot, 255, lumi_eirbot);
	leds[68] = CHSV(eirbot, 255, lumi_eirbot);
	leds[5] = CHSV(eirbot, 255, lumi_eirbot);
	leds[4] = CHSV(eirbot, 255, lumi_eirbot);
	leds[3] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre E
	leds[286] = CHSV(eirbot, 255, lumi_eirbot);
	leds[287] = CHSV(eirbot, 255, lumi_eirbot);
	leds[217] = CHSV(eirbot, 255, lumi_eirbot);
	leds[214] = CHSV(eirbot, 255, lumi_eirbot);
	leds[145] = CHSV(eirbot, 255, lumi_eirbot);
	leds[144] = CHSV(eirbot, 255, lumi_eirbot);
	leds[142] = CHSV(eirbot, 255, lumi_eirbot);
	leds[73] = CHSV(eirbot, 255, lumi_eirbot);
	leds[70] = CHSV(eirbot, 255, lumi_eirbot);
	leds[71] = CHSV(eirbot, 255, lumi_eirbot);

	FastLED.delay(33);
}
void affichage_cest() {

	leds.fadeToBlackBy(40);

	lumi_fond = 125;
	lumi_eirbot = 255;
	eirbot++;

	//Arrière plan
	leds = CHSV(hue++, 255, lumi_fond);

	// Lettre C
	leds[246] = CHSV(eirbot, 255, lumi_eirbot);
	leds[245] = CHSV(eirbot, 255, lumi_eirbot);
	leds[244] = CHSV(eirbot, 255, lumi_eirbot);
	leds[243] = CHSV(eirbot, 255, lumi_eirbot);
	leds[242] = CHSV(eirbot, 255, lumi_eirbot);
	leds[185] = CHSV(eirbot, 255, lumi_eirbot);
	leds[174] = CHSV(eirbot, 255, lumi_eirbot);
	leds[113] = CHSV(eirbot, 255, lumi_eirbot);
	leds[102] = CHSV(eirbot, 255, lumi_eirbot);
	leds[41] = CHSV(eirbot, 255, lumi_eirbot);
	leds[30] = CHSV(eirbot, 255, lumi_eirbot);
	leds[29] = CHSV(eirbot, 255, lumi_eirbot);
	leds[28] = CHSV(eirbot, 255, lumi_eirbot);
	leds[27] = CHSV(eirbot, 255, lumi_eirbot);
	leds[26] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre '
	leds[263] = CHSV(eirbot, 255, lumi_eirbot);
	leds[264] = CHSV(eirbot, 255, lumi_eirbot);
	leds[239] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre E
	leds[237] = CHSV(eirbot, 255, lumi_eirbot);
	leds[236] = CHSV(eirbot, 255, lumi_eirbot);
	leds[235] = CHSV(eirbot, 255, lumi_eirbot);
	leds[234] = CHSV(eirbot, 255, lumi_eirbot);
	leds[194] = CHSV(eirbot, 255, lumi_eirbot);
	leds[165] = CHSV(eirbot, 255, lumi_eirbot);
	leds[122] = CHSV(eirbot, 255, lumi_eirbot);
	leds[123] = CHSV(eirbot, 255, lumi_eirbot);
	leds[124] = CHSV(eirbot, 255, lumi_eirbot);
	leds[125] = CHSV(eirbot, 255, lumi_eirbot);
	leds[93] = CHSV(eirbot, 255, lumi_eirbot);
	leds[50] = CHSV(eirbot, 255, lumi_eirbot);
	leds[21] = CHSV(eirbot, 255, lumi_eirbot);
	leds[20] = CHSV(eirbot, 255, lumi_eirbot);
	leds[19] = CHSV(eirbot, 255, lumi_eirbot);
	leds[18] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre S
	leds[232] = CHSV(eirbot, 255, lumi_eirbot);
	leds[231] = CHSV(eirbot, 255, lumi_eirbot);
	leds[230] = CHSV(eirbot, 255, lumi_eirbot);
	leds[229] = CHSV(eirbot, 255, lumi_eirbot);
	leds[228] = CHSV(eirbot, 255, lumi_eirbot);
	leds[199] = CHSV(eirbot, 255, lumi_eirbot);
	leds[160] = CHSV(eirbot, 255, lumi_eirbot);
	leds[127] = CHSV(eirbot, 255, lumi_eirbot);
	leds[128] = CHSV(eirbot, 255, lumi_eirbot);
	leds[129] = CHSV(eirbot, 255, lumi_eirbot);
	leds[130] = CHSV(eirbot, 255, lumi_eirbot);
	leds[131] = CHSV(eirbot, 255, lumi_eirbot);
	leds[84] = CHSV(eirbot, 255, lumi_eirbot);
	leds[59] = CHSV(eirbot, 255, lumi_eirbot);
	leds[12] = CHSV(eirbot, 255, lumi_eirbot);
	leds[13] = CHSV(eirbot, 255, lumi_eirbot);
	leds[14] = CHSV(eirbot, 255, lumi_eirbot);
	leds[15] = CHSV(eirbot, 255, lumi_eirbot);
	leds[16] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre T
	leds[232] = CHSV(eirbot, 255, lumi_eirbot);
	leds[231] = CHSV(eirbot, 255, lumi_eirbot);
	leds[230] = CHSV(eirbot, 255, lumi_eirbot);
	leds[229] = CHSV(eirbot, 255, lumi_eirbot);
	leds[228] = CHSV(eirbot, 255, lumi_eirbot);
	leds[199] = CHSV(eirbot, 255, lumi_eirbot);
	leds[160] = CHSV(eirbot, 255, lumi_eirbot);
	leds[127] = CHSV(eirbot, 255, lumi_eirbot);
	leds[128] = CHSV(eirbot, 255, lumi_eirbot);
	leds[129] = CHSV(eirbot, 255, lumi_eirbot);
	leds[130] = CHSV(eirbot, 255, lumi_eirbot);
	leds[131] = CHSV(eirbot, 255, lumi_eirbot);
	leds[84] = CHSV(eirbot, 255, lumi_eirbot);
	leds[59] = CHSV(eirbot, 255, lumi_eirbot);
	leds[12] = CHSV(eirbot, 255, lumi_eirbot);
	leds[13] = CHSV(eirbot, 255, lumi_eirbot);
	leds[14] = CHSV(eirbot, 255, lumi_eirbot);
	leds[15] = CHSV(eirbot, 255, lumi_eirbot);
	leds[16] = CHSV(eirbot, 255, lumi_eirbot);

	// Lettre T
	leds[226] = CHSV(eirbot, 255, lumi_eirbot);
	leds[225] = CHSV(eirbot, 255, lumi_eirbot);
	leds[224] = CHSV(eirbot, 255, lumi_eirbot);
	leds[223] = CHSV(eirbot, 255, lumi_eirbot);
	leds[222] = CHSV(eirbot, 255, lumi_eirbot);
	leds[221] = CHSV(eirbot, 255, lumi_eirbot);
	leds[220] = CHSV(eirbot, 255, lumi_eirbot);
	leds[208] = CHSV(eirbot, 255, lumi_eirbot);
	leds[151] = CHSV(eirbot, 255, lumi_eirbot);
	leds[136] = CHSV(eirbot, 255, lumi_eirbot);
	leds[79] = CHSV(eirbot, 255, lumi_eirbot);
	leds[64] = CHSV(eirbot, 255, lumi_eirbot);
	leds[7] = CHSV(eirbot, 255, lumi_eirbot);


	FastLED.delay(33);
}

// FONCTION TETRIS
unsigned char outOfScreen(unsigned char x, unsigned char y) {
	return x < 0 || x >= WIDTH_T || y < 0 || y >= HEIGHT_T;
}
void setGrid(unsigned char g, unsigned char x, unsigned char y, unsigned long v) {
	if (g == 0) {
		setLed(y, x, v);
	} else {
		grid[g - 1][y] = (grid[g - 1][y] & (~(1 << x))) | ((v ? 1 : 0) << x);
	}
}
unsigned long getGrid(unsigned char g, unsigned char x, unsigned char y) {
	if (g == 0) {
		return getLed(y, x);
	} else {
		return (grid[g - 1][y] >> x) & 1;
	}
}
void clearGrid(unsigned char g) {
	for (unsigned char y = 0; y < HEIGHT_T; y++) {
		for (unsigned char x = 0; x < WIDTH_T; x++) {
			setGrid(g, x, y, 0);
		}
	}
}
void copyGrid(unsigned char g0, unsigned char g1) {
	for (unsigned char y = 0; y < HEIGHT_T; y++) {
		for (unsigned char x = 0; x < WIDTH_T; x++) {
			setGrid(g0, x, y, getGrid(g1, x, y));
		}
	}
}
void getForm(unsigned char id, unsigned char o, char x[4], char y[4]) {
	for (int i = 0; i < 4; i++) {
		x[i] = (o & 2 ? -1 : 1) * (((forms2[id] >> ((2 * i + 1 - (o & 1)) << 1)) & 3) - 1);
		y[i] = ((o & 1) ^ ((o & 2) >> 1) ? -1 : 1) * (((forms2[id] >> ((2 * i + (o & 1)) << 1)) & 3) - 1);
	}
}
unsigned char formOk(unsigned char g, unsigned char id, unsigned char o, unsigned char x, unsigned char y) {
	char fx[4];
	char fy[4];
	getForm(id, o, fx, fy);
	for (unsigned char i = 0; i < 4; i++) {
		if (x + fx[i] < 0 || x + fx[i] >= WIDTH_T || y + fy[i] < 0 || y + fy[i] >= HEIGHT_T || getGrid(g, x + fx[i], y + fy[i])) {
			return 0;
		}
	}
	return 1;
}
long printForm(unsigned char g, int id, int o, int x, int y, unsigned long value, unsigned char explode) {
	char fx[4];
	char fy[4];
	long score = 0;
	getForm(id, o, fx, fy);
	for (unsigned char i = 0; i < 4; i++) {
		if (!(x + fx[i] < 0 || x + fx[i] >= WIDTH_T || y + fy[i] < 0 || y + fy[i] >= HEIGHT_T)) {
			setGrid(g, x + fx[i], y + fy[i], value);
		}
	}
	if (explode) {
		char yplace = HEIGHT_T - 1;
		for (char ys = HEIGHT_T - 1; ys >= 0; ys--) {
			unsigned char full = 1;
			for (char xs = 0; xs < WIDTH_T && full; xs++) {
				if (!getGrid(g, xs, ys)) {
					full = 0;
				}
			}
			if (full) {
				score = score * 2 + 1;
			} else {
				if (yplace != ys) {
					for (char xs = 0; xs < WIDTH_T; xs++) {
						unsigned long v = getGrid(g, xs, ys);
						if (v == 1 && g == 0) {
							//v = 0x0000FF;
							Serial.println("AHAHAHAH");
						}
						setGrid(g, xs, yplace, v);
					}
				}
				yplace--;
			}
		}
		for (; yplace >= 0; yplace--) {
			for (char xs = 0; xs < WIDTH_T; xs++) {
				setGrid(g, xs, yplace, 0);
			}
		}
	}
	return score;
}
long evaluate2(unsigned char g) {
	long score = 0;
	for (char x = 0; x < WIDTH_T; x++) {
		char y = 0;
		while (y <= HEIGHT_T && !getGrid(g, x, y))y++;
		while (y < HEIGHT_T) {
			if (getGrid(g, x, y) == 0) {
				score++;
			}
			y++;
		}
	}
	return score;
}
long brutForce(int prof, unsigned char *xp, unsigned char *yp, unsigned char *op) {
	long score = -999999;
	char bestX = 2;
	char bestY = 2;
	char bestO = 0;
	long ev2_base = evaluate2(prof);
	for (char x = 0; x < WIDTH_T; x++) {
		for (char o = 0; o < 4; o++) {
			copyGrid(prof + 1, prof);
			char y = 2;
			while (formOk(prof + 1, form_file[prof], o, x, y)) y++;
			y--;
			long s = -1;
			if (formOk(prof + 1, form_file[prof], o, x, y)) {
				s = printForm(prof + 1, form_file[prof], o, x, y, colors[form_file[prof]], 1);
				long ev2 = evaluate2(prof + 1);
				s = s * 100 - 5 * ev2 + 3 * y;
				//if(prof == 0)printf("%d\n", s);
				if (prof == PMAX) {
				} else if (ev2_base - ev2 > -2 || 1) {
					s = s + brutForce(prof + 1, xp, yp, op);
				}

				if (s > score) {
					bestX = x;
					bestY = y;
					bestO = o;
					score = s;
				}
			}
		}
	}
	if (prof == 0) {
		//printForm(0, form_file[prof], bestO, bestX, bestY, colors[form_file[prof]], 1);
		*xp = bestX;
		*yp = 2;
		*op = bestO;
	}
	return score;
}
void startTetris() {
	clearGrid(0);

	unsigned char orientation = 0;
	unsigned char x = 3;
	unsigned char y = 2;
	unsigned char forme = 0;

	for (unsigned char i = 0; i < FILE; i++) {
		form_file[i] = rand() % 7;
	}

	unsigned char event = 0;

	while (event != EXIT) {
		event = 0;

		if (event == KEY_F && formOk(0, form_file[0], (orientation + 3) & 3, x, y)) {
			orientation = (orientation + 3) & 3;
		}
		if (event == KEY_LEFT && formOk(0, form_file[0], orientation, x - 1, y)) {
			x--;
		}
		if (event == KEY_RIGHT && formOk(0, form_file[0], orientation, x + 1, y)) {
			x++;
		}

		printForm(0, forme, orientation, x, y, colors[forme], 0);
		FastLED.show();
		printForm(0, forme, orientation, x, y, 0, 0);

		y++;
		if (!formOk(0, forme, orientation, x, y)) {
			tetrisLoop++;
			if (tetrisLoop > 40) { // Condition d'arrêt pour le moment, pour passer à la suite
				tetrisLoop = 0;
				break;
			}
			if (y <= 3) { // Si fin du jeu alors
				break;
			}
			printForm(0, forme, orientation, x, y - 1, colors[forme], 1);
			FastLED.show();
			x = 3;
			y = 2;
			brutForce(0, &x, &y, &orientation);
			forme = form_file[0];
			for (unsigned char i = 0; i < FILE - 1; i++) {
				form_file[i] = form_file[i + 1];
			}
			form_file[FILE - 1] = rand() % 7;

			//forme = (forme+1)%7;
		}
		delay(1);
	}
}


// FONCTION LIFE
void setGridLife(char x, char y, unsigned char v) {
	grid_life[x] = (grid_life[x] & (~(1 << y))) | ((v & 1) << y);
}
unsigned char getGridLife(char x, char y) {
	return (grid_life[x] >> y) & 1;
}
void clearLife() {
	for (unsigned char i = 0; i < WH8; i++) {
		grid_life[i] = 0;
	}
}
unsigned char step_life() {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			unsigned char c = 0;
			for (unsigned char i = 0; i < 8; i++) {
#ifdef LIFE_WARP
				if (getLed((x+dx[i]+WIDTH)%WIDTH, (y+dy[i]+HEIGHT)%HEIGHT)) {
				  c++;
				}
#else
				if (coordOK(x + dx[i], y + dy[i]) && getLed(x + dx[i], y + dy[i])) {
					c++;
				}
#endif
			}
			if (getLed(x, y)) {
				setGridLife(x, y, c == 2 || c == 3);
			} else {
				setGridLife(x, y, c == 3);
			}
		}
	}
	unsigned char apparait = 0;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (getGridLife(x, y)) {
				if (!getLed(x, y)) {
					unsigned long c = 0;
					while (!c) {
						char i = rand() % 8;
						if (coordOK(x + dx[i], y + dy[i]))
							c = getLed(x + dx[i], y + dy[i]);
					}
					setLed(x, y, c);
					apparait++;
				}
			}
		}
	}
	unsigned char disparait = 0;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (!getGridLife(x, y)) {
				if (getLed(x, y))disparait++;
				setLed(x, y, COLOR_BLACK);
			}
		}
	}
	return abs(apparait - disparait);
}
void initLife() {
	clearLife();
	fillScreen(COLOR_BLACK);
	for (int i = 0; i < 70; i++) {
		char x = rand() % WIDTH;
		char y = rand() % HEIGHT;
		setLed(x, y, colors[rand() % 7]);
	}
}
void startLife() {
	unsigned long t = millis();
	unsigned long t2 = millis();
	initLife();
	FastLED.show();
	delay(200);
	while (millis() - t < 60000 && millis() - t2 < 3000) {
		unsigned char score = step_life();
		if (score > 0) {
			t2 = millis();
		}
		FastLED.show();
		delay(200);
	}
}

void setLedDefil(int x, int y, long color) {
	leds[(YLettremax - 1 - y + ((YLettremax - 1 - y) % 2)) * Xmax +
		 (2 * ((YLettremax - y) % 2) - 1) * ((Xmax - 1 - x) + ((YLettremax - 1 - y) % 2))] = color;
}

//
//Mode_1
//


void setText(String text, long color) {
	String txt = text;
	txt.toUpperCase();
	while (txt.length() < 6) {
		txt += ("[");
	}
	for (int i = 0; i < Xmax; i++) {
		for (int j = 0; j < YLettremax; j++) {
			temp = pgm_read_word_near(&(dico[txt[i / XLettremax] - 65][i % XLettremax]));
			setLedDefil(i, j, color * ((temp >> (YLettremax - 1) - j) & 00000001));
		}
	}
	FastLED.show();
}


//
// Mode_2
//

void defilement(String txt, long color, int dt, int pause) {


	int L = txt.length();
	int K = Xmax;
	for (int k = 1; k <= K; k++) {
		for (int i = Xmax - k; i < Xmax; i++) {
			for (int j = 0; j < YLettremax; j++) {
				temp = pgm_read_word_near(&(dico[txt[(i + k - Xmax) / (XLettremax)] - 65][(i + k - Xmax) % XLettremax]));
				setLedDefil(i, j, color * ((temp >> (YLettremax - 1) - j) & 00000001));
			}
		}
		FastLED.show();
		delay(dt);
	}

	delay(pause);


	K = L * XLettremax - Xmax;

	for (int k = 1; k <= K; k++) {
		for (int i = 0; i < Xmax; i++) {
			for (int j = 0; j < YLettremax; j++) {
				temp = pgm_read_word_near(&(dico[txt[(i + k) / XLettremax] - 65][(i + k) % XLettremax]));
				setLedDefil(i, j, color * ((temp >> (YLettremax - 1) - j) & 00000001));
			}
		}
		FastLED.show();
		delay(dt);
	}


	K = Xmax;

	for (int k = 1; k <= Xmax; k++) {
		for (int i = 0; i < Xmax - k; i++) {
			for (int j = 0; j < YLettremax; j++) {
				temp = pgm_read_word_near(&(dico[txt[L - 6 + ((i + k) / XLettremax)] - 65][(i + k) % XLettremax]));
				setLedDefil(i, j, color * ((temp >> (YLettremax - 1) - j) & 00000001));
			}
		}
		FastLED.show();
		delay(dt);
	}
}