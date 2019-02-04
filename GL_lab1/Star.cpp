#include "pch.h"

class Point2D
{
public:
	float x, y;
	Point2D(float xx, float yy) { x = xx;  y = yy; }
	void Rotate(float a)
	{
		float
			ca = cos(a),
			sa = sin(a),
			xc = x * ca,
			xs = x * sa;
		x = xc - y * sa;
		y = xs + y * ca;
	}
};

float gR = 1, gA = acos(-1.) / 5;
bool growing = true;
vector<Point2D*> gP;

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
	for (int i = 1; i<10; i += 2)
	{
		glColor3ub(25 * i, 255 - 25 * i, 25 * i);
		glVertex2f(gR * gP[i]->x, gR * gP[i]->y);
	}
	glEnd();

	for (int i = 0; i<10; i += 2)
	{
		int j = i + 1,
			k = i == 0 ? 9 : i - 1;

		glBegin(GL_POLYGON);
		glColor3ub(255, 0, 0);
		glVertex2f(gP[i]->x, gP[i]->y);
		glColor3ub(255, 255, 0);
		glVertex2f(gR * gP[j]->x, gR * gP[j]->y);
		glColor3ub(0, 0, 255);
		glVertex2f(gR * gP[k]->x, gR * gP[k]->y);
		glEnd();
	}
	glFlush();
}

void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
}

void OnKey(byte ch, int x, int y)
{
	switch (ch)
	{
	case ' ':
		gR += growing ? 0.05 : -0.05;
		if (gR < 0.06 || 0.96 < gR)
			growing = !growing;
		cout << "\nRad: " << gR;
		break;
	case 27: exit(0); break;
	}
	glutPostRedisplay();
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: break;
	case GLUT_KEY_RIGHT:break;
	case GLUT_KEY_DOWN:
		if (gR > 0.03)
			gR -= 0.02;
		cout << "\nRad: " << gR;
		break;
	case GLUT_KEY_UP:
		if (gR < 0.98)
			gR += 0.02;
		cout << "\nRad: " << gR;
		break;
	}
	glutPostRedisplay();
}

void Init()
{
	glClearColor(1, 1, 1, 0);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT, GL_FILL);

	gP.push_back(new Point2D(0, 1));

	for (int i = 1; i<10; i++)
	{
		gP.push_back(new Point2D(*gP[i - 1]));
		gP[i]->Rotate(gA);
	}
}

void main()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(700, 500);
	glutCreateWindow("Star: Up-Down");

	Init();

	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
}
