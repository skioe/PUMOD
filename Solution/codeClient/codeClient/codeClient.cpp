#include "pch.h"
#include "main.h"
#include "irklang/irrKlang.h"

int main(int argc, char** argv)
{
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT)); // ��������� ������ ����
	glutCreateWindow("Pick up mushrooms or die!"); // ��� ����
	glutSetCursor(GLUT_CURSOR_NONE);
	glutFullScreen();

	initialize();

	mapInit();
	glutDisplayFunc(draw); // ����� ������� ���������
	glutReshapeFunc(resize); // ����� ������� ��������� ������� �����������
	glutTimerFunc(1000 / 60, idle, 0);
	glutMouseFunc(mouseButton);
	glutKeyboardFunc(pressE);
	engine->play2D("textures/rabotaem.wav", true);

	glutMainLoop();

	return 0;
}