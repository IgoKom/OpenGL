#include "pch.h"

float
	ax = 20, ay = 45, az = 0, // Углы поворота изображения вокруг осей X и Y
	dx, dy, dz = -6, // Сдвиги вдоль координат
	color[] = { 0.1f, 0.6f, 0.1f },
	speedCoef = 0,
	deltaX = 0,
	deltaY = 0,
	incX = 0,
	incY = 0,
    prevDeltaX = 0,
	prevDeltaY = 0;
short posX, posY; // Текущая позиция указателя мыши
short tmpX = 0, tmpY = 0;
bool leftButton, twoSide, speedCalcEn = false; // Нажата левая кнопка мыши. Свет отражается от обеих поверхностей (лицевой и обратной)
float v[8][3] = { -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1, 1, 1 };
float norm[6][3] = { 0, 0, -1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0 };
int id[6][4] =
{
	0, 1, 2, 3,	// Rear (CCW - counterclockwise)
	4, 5, 6, 7,	// Front
	0, 3, 5, 4,	// Left
	7, 6, 2, 1,	// Right
	0, 4, 7, 1,	// Top
	5, 3, 2, 6,	// Bottom
};
float rotMatrix[16] = { 1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1 };




void OnPaint()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glMatrixMode(GL_MODELVIEW); 	
	glLoadIdentity();
	glTranslatef(dx, dy, dz);
	glRotatef(ax, 1, 0, 0);
	glRotatef(ay, 0, 1, 0);
	glRotatef(az, 0, 0, 1);
	glCallList(1);
	glutSwapBuffers();
}

void DrawScene()
{
	glNewList(1, GL_COMPILE);
	glColor3fv(color);
	glBegin(GL_QUADS);
	for (int i = 0; i < 6; i++)
	{
		glNormal3fv(norm[i]);
		for (int j = 0; j < 4; j++)
			glVertex3fv(v[id[i][j]]);
	}
	glEnd();
	glEndList();
}

void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.6, 100);
}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
		case 27: exit(0); break;
		case '2': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
		case 'p': glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		case 'f': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case 'l': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case 's': incX = 0;
				  incY = 0;
				 
				  break;
		default: break;
	}
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP: dy += 0.1; break;
		case GLUT_KEY_DOWN: dy -= 0.1; break;
		case GLUT_KEY_LEFT: dx -= 0.1; break;
		case GLUT_KEY_RIGHT: dx += 0.1; break;
		default: break;
	}
	posX = x;
	posY = y;
	glutPostRedisplay();
}



void OnIdle()
{
	
	ay += 0.1;
	//glMatrixMode(GL_MODELVIEW);
	//glRotatef(ax, 1, 0, 0);
	glutPostRedisplay();
}

void OnMouse(int button, int state, int x, int y)
{	
	leftButton = button == GLUT_LEFT_BUTTON;
	if (state == GLUT_DOWN)	{
		speedCoef = 0;
		tmpX = x;
		tmpY = y;
		deltaX = 0;
		deltaY = 0;
		incX = 0;
		incY = 0;
		//glutIdleFunc(0);
		speedCalcEn = true;
	}
	else {
		if (x != tmpX || y != tmpY) {
			incX += (deltaY / speedCoef) * 2;
			incY += (deltaX / speedCoef) * 2;
			//glutIdleFunc(OnIdle);
		}		
		speedCalcEn = false;
	}
	posX = x;	
	posY = y;
}

void OnMouseMove(int x, int y)
{	
	if (speedCalcEn) 
		speedCoef += 100;
	if (leftButton)
	{
		deltaX = x - tmpX;
		deltaY = y - tmpY;
		if (abs(prevDeltaX) >= abs(deltaX)) deltaX = -deltaX;
		if (abs(prevDeltaY) >= abs(deltaY)) deltaY = -deltaY;
		ay += (x - posX)*0.3;
		ax += (y - posY)*0.3;	
	}
	else
	{
		dz += ((x - posX) + (posY-y))*0.01;
	}
	posX = x;	
	posY = y;
	prevDeltaX = deltaX;
	prevDeltaY = deltaY;
	glutPostRedisplay();
}



void Init()
{	
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);	
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(7);
	glEnable(GL_POINT_SMOOTH);
	//int nLights;
	//glGetIntegerv(GL_MAX_LIGHTS, &nLights);
	//cout << nLights << endl;
	DrawScene();
}

int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 500);
	glutCreateWindow("lab_1");

	Init();
	glutMouseFunc(OnMouse);
	glutMotionFunc(OnMouseMove);
	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutIdleFunc(OnIdle);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
	return 0;
}
