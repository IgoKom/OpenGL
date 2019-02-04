#include "pch.h"

float
spec[3] = {1, 1, 1},
rotX, rotY,
dx, dy, dz = -4,
r = (1 + sqrt(5)) / 2, // Golden ratio
v[12][3] =		// 12 vertices (x,y,z)
{
	0, 1, r,  0,-1, r,  0, 1,-r,  0,-1,-r,
	1, r, 0, -1, r, 0,  1,-r, 0, -1,-r, 0,
	r, 0, 1, -r, 0, 1,  r, 0,-1, -r, 0,-1
},
rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
color[] = { 0.3f, 0.6f, 0.2f };

int id[20][3] =	// 20 triangular faces
{
	0, 1, 8,   0, 9, 1,   0, 8, 4,   0, 4, 5,   0, 5, 9,
	2, 3, 11,  2, 11, 5,  2, 5, 4,	 2, 4, 10,  2, 10, 3,
	1, 9, 7,   1, 7, 6,   1, 6, 8,   3, 10, 6,  3, 6, 7,
	3, 7, 11,  4, 8, 10,  5, 11, 9,  6, 10, 8,  7, 9, 11
};

short posX, posY;
bool leftButton, flat = false, growing = true, twoSide = false;
int depth;


void DrawTriaSmooth(float* v1, float* v2, float* v3)
{
	glColor3fv(color);
	glBegin(GL_TRIANGLES);
	glNormal3fv(v1);		glVertex3fv(v1);
	glNormal3fv(v2);		glVertex3fv(v2);
	glNormal3fv(v3);		glVertex3fv(v3);
	glEnd();
}

void ToUnit(float v[3]) // Нормируем вектор
{
	float d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0)
	{
		MessageBox(0, "Zero length vector", "Error", MB_OK);
		return;
	}
	v[0] /= d; v[1] /= d; v[2] /= d;
}

void GetNorm(float a[3], float b[3], float n[3])
{
	// Вычисляем координаты вектора нормали по формулам векторного произведения
	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];
	ToUnit(n);
}


void DrawTriaFlat(float* v1, float* v2, float* v3)
{
	float norm[3], a[3], b[3];
	for (int i = 0; i < 3; i++) // Векторы a и b – это стороны треугольника
	{
		a[i] = v3[i] - v2[i];
		b[i] = v1[i] - v2[i];
	}
	GetNorm(a, b, norm); 	// Вычисляем нормаль и создаем треугольник

	glColor3fv(color);
	glBegin(GL_TRIANGLES);
	glNormal3fv(norm);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();
}

void DrawNorm(float* v1, float* v2, float* v3)
{
	float norm[3], a[3], b[3];
	for (int i = 0; i < 3; i++) // Векторы a и b – это стороны треугольника
	{
		a[i] = v3[i] - v2[i];
		b[i] = v1[i] - v2[i];
	}
	GetNorm(a, b, norm); 	// Вычисляем нормаль и создаем треугольник

	glColor3f(0.7, 0.5, 0.3);
	glBegin(GL_LINES);	
	glVertex3f(0,0,0);
	glVertex3fv(norm);	
	glEnd();
}

void DrawScene()
{
	glNewList(1, GL_COMPILE);	
	for (int i = 0; i < 20; i++) {
		if (flat)DrawTriaFlat(v[id[i][0]], v[id[i][1]], v[id[i][2]]);
		else DrawTriaSmooth(v[id[i][0]], v[id[i][1]], v[id[i][2]]);
		//DrawNorm(v[id[i][0]], v[id[i][1]], v[id[i][2]]);
		
	}
	glEndList();
}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(dx, dy, dz);
	glMultMatrixf(rotMatrix); // Вместо поворотов умножаем на матрицу, вобравшую все вращения

	glCallList(1);
	
	glutSwapBuffers();
}

void AddRotation(float angle, float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, x, y, z);
	glMultMatrixf(rotMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
	glPopMatrix();
}

void OnIdle()
{
	AddRotation(rotX / 200, 1, 0, 0);
	AddRotation(rotY / 200, 0, 1, 0);
	glutPostRedisplay();
}

void OnMouse(int button, int state, int x, int y)
{
	leftButton = button == GLUT_LEFT_BUTTON;
	if (state == GLUT_DOWN)
	{
		glutIdleFunc(0);
		rotX = rotY = 0;
	}
	else
	{
		if (fabs(rotX) > 1 || fabs(rotY) > 1)
			glutIdleFunc(OnIdle);
		else
			glutIdleFunc(0);
	}
	posX = x;
	posY = y;
}

void OnMouseMove(int x, int y)
{
	rotX = y - posY;
	rotY = x - posX;

	if (leftButton)
	{
		AddRotation(rotX / 10, 1, 0, 0);
		AddRotation(rotY / 10, 0, 1, 0);
	}
	else
		dz += (rotX + rotY) / 60;
	posX = x;
	posY = y;
	glutPostRedisplay();
}



void OnSpecialKey(int key, int x, int y)
{
	float delta = -0.005 * dz;
	switch (key)
	{
	case GLUT_KEY_LEFT: dx -= delta; break;
	case GLUT_KEY_RIGHT: dx += delta; break;
	case GLUT_KEY_DOWN: dy -= delta; break;
	case GLUT_KEY_UP:  dy += delta; break;
	}
	glutPostRedisplay();
}



void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.6, 100);
}


void Init()
{
	
		float d = sqrt(1 + r * r);
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 3; j++)
				v[i][j] /= d;
		}
	
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glPointSize(7);
	glEnable(GL_POINT_SMOOTH);	
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glLineWidth(3); 
	DrawScene();
}

void OnKey(byte ch, int x, int y)
{
	int a[1] = { 0 };
	switch (ch)
	{
	case 27: exit(0); break;
	case '2': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
	case 'p': glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	case 'f': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	case 'l': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
	case 's': rotX = 0; rotY = 0; break;
	case 'q':
		glGetIntegerv(GL_SHADE_MODEL, a);
		if (a[0] == GL_SMOOTH)
			glShadeModel(GL_FLAT);
		else glShadeModel(GL_SMOOTH);
		break;
	case 'w': flat = !flat; DrawScene(); break;
	default: break;
	}
	glutPostRedisplay();
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

	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
	return 0;
}
