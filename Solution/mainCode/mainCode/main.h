#pragma once

#ifdef MAIN_EXPORTS
#define MAIN_API __declspec(dllexport)
#else
#define MAIN_API __declspec(dllimport)
#endif

extern "C" MAIN_API void initialize();
extern "C" MAIN_API void mapInit();
extern "C" MAIN_API void draw();
extern "C" MAIN_API void resize(int width, int height);
extern "C" MAIN_API void idle(int);
extern "C" MAIN_API void mouseButton(int button, int state, int x, int y);
extern "C" MAIN_API void pressE(unsigned char key, int x, int y);

struct TCell
{
	float x, y, z;
};

struct TColor
{
	float r, g, b;
};

struct TUV
{
	float u, v;
};

struct TObject
{
	float x, y, z;
	int type;
	float scale;
};

#define mapW 500
#define mapH 500
TCell map[mapW][mapH];
TCell mapNormal[mapW][mapH];
TUV mapUV[mapW][mapH];

GLuint mapInd[mapW - 1][mapH - 1][6];
int mapIndCnt = sizeof(mapInd) / sizeof(GLuint);

float plant[] = { -0.5,0,0, 0.5,0,0, 0.5,0,1, -0.5,0,1,
				  0,-0.5,0, 0,0.5,0, 0,0.5,1, 0,-0.5,1, };
float plantUV[] = { 0,1, 1,1, 1,0, 0,0, 0,1, 1,1, 1,0, 0,0 };
GLuint plantInd[] = { 0,1,2, 2,3,0, 4,5,6, 6,7,4 };
int plantIndCnt = sizeof(plantInd) / sizeof(GLuint);

unsigned int tex_field, tex_grass, tex_flower, tex_flower2, tex_mushroom, tex_tree, tex_tree2, tex_heart;

std::vector<TObject> plantMas(0);
int plantCnt = 0;

float sun[] = { -1,-1,0, 1,-1,0, 1,1,0, -1,1,0 };

bool selectMode = false;

#define objListCnt 255

struct tSelectObj 
{
	int plantMas_Index;
	int colorIndex;
};

tSelectObj selectMas[objListCnt];
int selectMasCnt = 0;

struct TAnim
{
	TObject* obj;
	float dx, dy, dz;
	int cnt;
};

TAnim animation = { 0,0,0,0,0 };

POINT scrSize;
float scrKoef;

typedef struct {
	int type;
} TSlot;
#define bagSize 16
TSlot bag[bagSize];

float bagRect[] = { 0,0, 1,0, 1,1, 0,1 };
float bagRectUV[] = {0,0, 1,0, 1,1, 0,1};

int health = 16;
int healthMax = 16;

float heart[] = { 0,0, 1,0, 1,1.5, 0,1.5 };

bool mouseBind = true;
bool fullsc = false;

class player
{
public:
	virtual void playerMove() = 0;
	virtual void playerTake() = 0;
	static int getRad() {};
};




