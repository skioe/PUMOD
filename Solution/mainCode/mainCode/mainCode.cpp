#include "pch.h"
#include "main.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

int currentWindow;

float angle = 0.0f;

int chosenPlayer = 1;

void animSet(TAnim* anm, TObject* obj)
{
	if (anm->obj != NULL) return;
	anm->obj = obj;
	anm->cnt = 10;
	anm->dx = (camera.x - obj->x) / (float)anm->cnt;
	anm->dy = (camera.y - obj->y) / (float)anm->cnt;
	anm->dz = ((camera.z - obj -> scale - 0.2) - obj->z) / (float)anm->cnt;
}

void animMove(TAnim* anm)
{
	if (anm->obj != NULL)
	{
		anm->obj->x += anm->dx;
		anm->obj->y += anm->dy;
		anm->obj->z += anm->dz;
		anm->cnt--;
		if (anm->cnt < 1)
		{
			int i;
			for (i = 0; i < bagSize; i++)
				if (bag[i].type < 0)
				{
					bag[i].type = anm->obj->type;
					break;
				}
			if (i < bagSize)
			{
				anm->obj->x = rand() % mapW;
				anm->obj->y = rand() % mapH;
			}

			anm->obj->z = mapGetHeight(anm->obj->x, anm->obj->y);
			anm->obj = NULL;

		}
	}
}

class playerSpeeding : public player
{
	static int radius;

public:

	playerSpeeding() { radius = 5; }

	void playerMove()
	{
		cameraMoveDirection(GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
			GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0),
			0.2);
		if (mouseBind) cameraAutoMouseMove(400, 400, 0.2);
	}

	void playerTake()
	{
		selectMode = false;

		RECT rct;
		GLubyte clr[3];
		GetClientRect(GetActiveWindow(), &rct);
		glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr); // Записываем цвета пикселей в центре экрана

		if (clr[0] > 0) // Величина красного цвета
		{
			for (int i = 0; i < selectMasCnt; i++)
				if (selectMas[i].colorIndex == clr[0]) // Ищем что мы нарисовали таким цветом
				{
					animSet(&animation, &plantMas[selectMas[i].plantMas_Index]);
				}
		}
	}

	static int getRad() { return radius; }

};

class playerTaking : public player
{
	static int radius;
	
public:

	playerTaking() { radius = 10; }

	void playerMove()
	{
		cameraMoveDirection(GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
			GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0),
			0.1);
		if (mouseBind) cameraAutoMouseMove(400, 400, 0.2);
	}

	void playerTake()
	{
		selectMode = false;

		RECT rct;
		GLubyte clr[3];
		GetClientRect(GetActiveWindow(), &rct);
		glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr); // Записываем цвета пикселей в центре экрана

		if (clr[0] > 0) // Величина красного цвета
		{
			for (int i = 0; i < selectMasCnt; i++)
				if (selectMas[i].colorIndex == clr[0]) // Ищем что мы нарисовали таким цветом
				{
					animSet(&animation, &plantMas[selectMas[i].plantMas_Index]); // Получаем индекс объекта в массиве объектов
				}
		}
	}

	static int getRad() { return radius; }
};

playerSpeeding pSp;
int playerSpeeding::radius;
player* p1 = &pSp;

playerTaking pTk;
int playerTaking::radius;
player* p2 = &pTk;

unsigned int texture;
void loadTexture(const char* fileName, unsigned int* textureID)
{
	int texWidth, texHeight, cnt;
	unsigned char* data = stbi_load(fileName, &texWidth, &texHeight, &cnt, 0);

	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight,
		0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

}

void initialize()
{
	glEnable(GL_DEPTH_TEST);
}

void resize(int width, int height) // С изменением размера окна также будет изменятся содержимое
{
	if (height == 0) height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	scrSize.x = width;
	scrSize.y = height;
	scrKoef = (float)width / (float)height;
	
}

void bagShow(int x, int y, int scale)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, bagRect);
	glTexCoordPointer(2, GL_FLOAT, 0, bagRectUV);

	for (int i = 0; i < bagSize; i++)
	{
		glPushMatrix();

		glTranslatef(x + i * scale, y, 0);
		glScalef(scale, scale, 1);

		glColor4ub(0, 0, 0, 0);
		glDisable(GL_TEXTURE_2D);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (bag[i].type > 0)
		{
			glColor3f(1, 1, 1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, bag[i].type);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		glColor3ub(69, 69, 69);
		glLineWidth(3);
		glDisable(GL_TEXTURE_2D);
		glDrawArrays(GL_LINE_LOOP, 0, 4);

		glPopMatrix();
	}
}

void bagClick(int x, int y, int scale, int mx, int my)
{
	if ((my < y) || (my > y + scale)) return;
	for (int i = 0; i < bagSize; ++i)
	{
		if ((mx > x + 1*scale) && (mx < x + (i + 1)*scale))
		{
			if (bag[i].type == tex_mushroom)
			{
				health++;
				if (health > healthMax) health = healthMax;
			}
			bag[i].type = -1;
		}
	}
}

void healthShow(int x, int y, int scale)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_heart);
	glVertexPointer(2, GL_FLOAT, 0, heart);
		for (int i = 0; i < healthMax; ++i)
		{
			glPushMatrix();
			glTranslatef(x + i * 1.25 * scale, y, 0);
			glScalef(scale, scale, 1);
			if (i < health) glColor3f(1, 0, 0);
			else glColor3f(0, 0, 0);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glPopMatrix();
		}
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void crosshair()
{
	static float cross[] = { 0,-1, 0,1, -1,0, 1,0 };
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, cross);
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(scrSize.x * 0.5, scrSize.y * 0.5, 0);
	glScalef(15, 15, 1);
	glLineWidth(3);
	glDrawArrays(GL_LINES, 0, 4);
	glPopMatrix();
	glDisableClientState(GL_VERTEX_ARRAY);
}

void menuShow()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, scrSize.x, scrSize.y, 0, -1,1); // Устанавливаем матрицу перспективы
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	bagShow(scrSize.x / 100, scrSize.y / 100, 50);
	healthShow(scrSize.x / 90, scrSize.y / 100 + 60, 40);
	crosshair();
}

#define sqr(a) (a) * (a)
void calcNormals(TCell a, TCell b, TCell c, TCell* n)
{
	float wrki;
	TCell v1, v2;

	v1.x = a.x - b.x;
	v1.y = a.y - b.y;
	v1.z = a.z - b.z;
	v2.x = b.x - c.x;
	v2.y = b.y - c.y;
	v2.z = b.z - c.z;

	n->x = (v1.y * v2.z - v1.z * v2.y);
	n->y = (v1.z * v2.x - v1.x * v2.z);
	n->z = (v1.x * v2.y - v1.y * v2.x);
	wrki = sqrt(sqr(n->x) + sqr(n->y) + sqr(n->z));
	n->x /= wrki;
	n->y /= wrki;
	n->z /= wrki;

}

bool isCoordOnMap(float x, float y)
{
	return (x >= 0) && (x < mapW) && (y >= 0) && (y < mapH);
}

void createHill(int posX, int posY, int rad, int hillHeight)
{
	for (int i = posX - rad; i <= posX + rad; i++)
	{
		for (int j = posY - rad; j <= posY + rad; j++)
		{
			if (isCoordOnMap(i, j))
			{
				float len = sqrt(pow(posX - i, 2) + pow(posY - j, 2));
				if (len < rad)
				{
					len = len / rad * M_PI_2;
					map[i][j].z += cos(len) * hillHeight;
				}
			}
		}
	}
}

float mapGetHeight(float x, float y)
{
	if (!isCoordOnMap(x, y)) return 0;

	int cX = (int)x;
	int cY = (int)y;
	float h1 = ((1 - (x - cX)) * map[cX][cY].z + (x - cX) * map[cX + 1][cY].z);
	float h2 = ((1 - (x - cX)) * map[cX][cY + 1].z + (x - cX) * map[cX + 1][cY + 1].z);

	return (1 - (y - cY)) * h1 + (y - cY) * h2;
}

void mapInit()
{
	for (int i = 0; i < bagSize; i++) bag[i].type = -1;

	loadTexture("textures/field.png", &tex_field);
	loadTexture("textures/grass.png", &tex_grass);
	loadTexture("textures/mushroom.png", &tex_mushroom);
	loadTexture("textures/flower.png", &tex_flower);
	loadTexture("textures/flower2.png", &tex_flower2);
	loadTexture("textures/tree.png", &tex_tree);
	loadTexture("textures/tree2.png", &tex_tree2);
	loadTexture("textures/heart.png", &tex_heart);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.99);

	for (int i = 0; i < mapW; i++)
	{
		for (int j = 0; j < mapH; j++)
		{
			map[i][j].x = i;
			map[i][j].y = j;
			map[i][j].z = (rand() % 10) * 0.02;

			mapUV[i][j].u = i;
			mapUV[i][j].v = j;
		}

	}

	for (int i = 0; i < mapW - 1; i++)
	{
		int pos = i * mapH;
		for (int j = 0; j < mapH - 1; j++)
		{
			mapInd[i][j][0] = pos;
			mapInd[i][j][1] = pos + 1;
			mapInd[i][j][2] = pos + 1 + mapH;

			mapInd[i][j][3] = pos + 1 + mapH;
			mapInd[i][j][4] = pos + mapH;
			mapInd[i][j][5] = pos;

			pos++;

		}

	}

	for (int i = 0; i < 100; i++)
		createHill(rand() % mapW, rand() % mapH, 10 + rand() % 50, rand() % 30);

	for (int i = 0; i < mapW - 1; i++)
		for (int j = 0; j < mapH - 1; j++)
			calcNormals(map[i][j], map[i + 1][j], map[i][j + 1], &mapNormal[i][j]);

	int grassN = 15000;
	int mushroomN = 1000;
	int treeN = 1000;
	plantCnt = grassN + mushroomN + treeN;
	plantMas.resize(plantCnt);
	for (int i = 0; i < plantCnt; i++)
	{
		plantMas.push_back(TObject());
		if (i < grassN)
		{
			plantMas[i].type = rand() % 10 != 0 ? tex_grass :
				(rand() % 2 == 0 ? tex_flower : tex_flower2);
			plantMas[i].scale = 0.9 + (rand() % 5) * 0.3;
		}
		else if (i < (grassN + mushroomN))
		{
			plantMas[i].type = tex_mushroom;
			plantMas[i].scale = 0.2 + (rand() % 10) * 0.1;
		}
		else
		{
			plantMas[i].type = rand() % 2 == 0 ? tex_tree : tex_tree2;
			plantMas[i].scale = 4 + (rand() % 15);
		}
		plantMas[i].x = rand() % mapW;
		plantMas[i].y = rand() % mapH;
		plantMas[i].z = mapGetHeight(plantMas[i].x, plantMas[i].y);
	}

}

void mapDraw()
{

	float sz = 0.1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, scrKoef, sz * 2, 1000); // Устанавливаем матрицу перспективы
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	static int hunger = 0;
	hunger++;
	if (hunger > 150)
	{
		hunger = 0;
		health--;
		if (health < 1) exit(0);
	}

	static float alpha = 0;
	alpha += 0.03;
	if (alpha > 180) alpha -= 360;

#define abs(a) ((a) > 0 ? (a) : -(a))
	float kcc = 1 - (abs(alpha) / 180);

#define sunset 40.0
	float k = 90 - abs(alpha);
	k = (sunset - abs(k));
	k = k < 0 ? 0 : k / sunset;

	if (selectMode) glClearColor(0, 0, 0, 0);
	else glClearColor(0.6f * kcc, 0.8f * kcc, 1.0f * kcc, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (selectMode)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}

	if (!selectMode)
	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}

	animMove(&animation);

	glPushMatrix();

	if (!selectMode)
	{
		glPushMatrix();

		glRotatef(-camera.xRot, 1, 0, 0);
		glRotatef(-camera.zRot, 0, 0, 1);
		glRotatef(alpha, 0, 1, 0);
		glTranslatef(0, 0, 20);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1, 1 - k * 0.8, 1 - k);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, sun);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
	cameraApply();

	glPushMatrix();
	glRotatef(alpha, 0, 1, 0);
	GLfloat position[] = { 0, 0, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	float mas[] = { 1 + k * 2, 1,1,0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mas);

	float clr = kcc * 0.15 + 0.05;
	float mas0[] = { clr, clr, clr, 0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mas0);

	glPopMatrix();
	if (!selectMode)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, map);
		glTexCoordPointer(2, GL_FLOAT, 0, mapUV);
		glColor3f(0.7, 0.7, 0.7);
		glNormalPointer(GL_FLOAT, 0, mapNormal);
		glBindTexture(GL_TEXTURE_2D, tex_field);
		glDrawElements(GL_TRIANGLES, mapIndCnt, GL_UNSIGNED_INT, mapInd);

		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, plant);
	glTexCoordPointer(2, GL_FLOAT, 0, plantUV);
	glColor3f(0.7, 0.7, 0.7);
	glNormal3f(0, 0, 1);

	selectMasCnt = 0;
	int selectColor = 1;
	for (int i = 0; i < plantCnt; i++)
	{
		if (selectMode)
		{
			if ((plantMas[i].type == tex_tree) || (plantMas[i].type == tex_tree2)) continue;

			static int radius = NULL;

			if (chosenPlayer == 1) radius = playerTaking::getRad();
			if (chosenPlayer == 2) radius = playerSpeeding::getRad();

			if ((plantMas[i].x > camera.x - radius)
				&& (plantMas[i].x < camera.x + radius)
				&& (plantMas[i].y > camera.y - radius)
				&& (plantMas[i].y < camera.y + radius))
			{
				glColor3ub(selectColor, 0, 0);
				selectMas[selectMasCnt].colorIndex = selectColor;
				selectMas[selectMasCnt].plantMas_Index = i;
				selectMasCnt++;
				selectColor++;
				if (selectColor >= 255)
					break;
			}
			else continue;
		}
		glBindTexture(GL_TEXTURE_2D, plantMas[i].type);
		glPushMatrix();
		glTranslatef(plantMas[i].x, plantMas[i].y, plantMas[i].z);
		glScalef(plantMas[i].scale, plantMas[i].scale, plantMas[i].scale);
		glDrawElements(GL_TRIANGLES, plantIndCnt, GL_UNSIGNED_INT, plantInd);
		glPopMatrix();
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glPopMatrix();

}

void pressE(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0); break;
	case 101: mouseBind = !mouseBind; break;
	case 102: fullsc = !fullsc; break;
		
	}
}

void draw()
{
	if (chosenPlayer == 1) p1->playerMove();
	if (chosenPlayer == 2) p2->playerMove();

	mapDraw();
	menuShow();

	switch (mouseBind)
	{
	case true: glutSetCursor(GLUT_CURSOR_NONE); break;
	case false: glutSetCursor(GLUT_CURSOR_LEFT_ARROW); break;
	}

	switch (fullsc)
	{
	case true:

		glutReshapeWindow(
			glutGet(GLUT_SCREEN_WIDTH) / 1.5,
			glutGet(GLUT_SCREEN_HEIGHT) / 1.5);

		glutPositionWindow(0, 0);
		
		break;

	case false:
		glutFullScreen();
		break;
	}

	glutSwapBuffers();
}


void mouseButton(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_UP)
		{
			if (mouseBind)
			{
				selectMode = true;
				mapDraw();
				p1->playerTake();
			}
			else bagClick(scrSize.x / 100, scrSize.y / 100, 50, x, y);
		}

		else
		{

		}
	}
}

void idle(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, idle, 0);
}