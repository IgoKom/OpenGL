#include "pch.h"
#define PI 3.14159265
float
norm[3] = {0},
spec[3] = { 1, 1, 1 },
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
bool leftButton, flat = false, growing = true, twoSide = false, showNorm = false;
int depth = 0;


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

void AddRotation(float angle, float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, x, y, z);
	glMultMatrixf(rotMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
	glPopMatrix();
}


void GetNextVertex(float* vRef, float* vTarg, float dist) {
	if (!dist) { 
		vTarg[0] = vRef[0];
		vTarg[1] = vRef[1];
		vTarg[2] = vRef[2];
		return;
	}
	float xyRat, xzRat, yzRat;
	if (vRef[0] != 0)
	{
		xyRat = abs(vRef[1] / vRef[0]);
		xzRat = abs(vRef[2] / vRef[0]);
		if (vRef[0] > 0) { vTarg[0] = sqrt( pow(dist, 2) / (1+pow(xyRat, 2)+pow(xzRat, 2)) ); }
		else { vTarg[0] = -(sqrt(pow(dist, 2) / (1 + pow(xyRat, 2) + pow(xzRat, 2)))); }
		if (vRef[1] > 0) { vTarg[1] = abs(vTarg[0]) *xyRat; }
		else { vTarg[1] = -(abs(vTarg[0]) * xyRat); }
		if (vRef[2] > 0) { vTarg[2] = abs(vTarg[0]) *xzRat; }
		else { vTarg[2] = -(abs(vTarg[0]) * xzRat); }
	}
	else
	{
		yzRat = abs(vRef[2] / vRef[1]);
		if (vRef[1] > 0) { vTarg[1] = sqrt( pow(dist, 2) / (1 + pow(yzRat, 2)) ); }
		else { vTarg[1] = -(sqrt(pow(dist, 2) / (1 + pow(yzRat, 2)))); }
		if (vRef[2] > 0) { vTarg[2] = abs(vTarg[1]) *yzRat; }
		else { vTarg[2] = -(abs(vTarg[1]) *yzRat); }
	}
}

void DrawTriaFlat(float* v1, float* v2, float* v3)
{
	float a[3], b[3];
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
	if (!flat && !depth) {
		glPushMatrix();
		glTranslatef(v1[0], v1[1], v1[2]);
		float aX = atan(v1[1] / v1[2])*180.0 / PI, aZ = atan(v1[0] / v1[1])*180.0 / PI;
		if (!v1[0]) {
			if (v1[2] > 0) glRotatef(-aX, 1, 0, 0);
			else  glRotatef(180 - aX, 1, 0, 0);
		}
		if (!v1[2]) {
			glRotatef(-aZ, 0, 0, 1);
			if (v1[1] > 0) glRotatef(-90, 1, 0, 0);
			else  glRotatef(90, 1, 0, 0);
		}
		glTranslatef(0, 0, 0.5);
		glutSolidCone(0.03, 0.1, 10, 10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(v2[0], v2[1], v2[2]);
		float aY = atan(v2[0] / v2[2])*180.0 / PI;
		if (!v2[1]) {
			if (v2[2] < 0)glRotatef(aY + 180, 0, 1, 0);
			else glRotatef(aY, 0, 1, 0);
			glTranslatef(0, 0, 0.5);
			glutSolidCone(0.03, 0.1, 10, 10);
		}
		glPopMatrix();
	}
	glDisable(GL_LIGHTING);
	float a[3], b[3], norm2[3] = { 0 };
	for (int i = 0; i < 3; i++) // Векторы a и b – это стороны треугольника
	{
		a[i] = v3[i] - v2[i];
		b[i] = v1[i] - v2[i];
	}
	GetNorm(a, b, norm); 	// Вычисляем нормаль и создаем треугольник

	glColor3f(0.9, 0.5, 0.3);
	glBegin(GL_LINES);
	if (flat) {
		glVertex3fv(norm);
		//glVertex3f(0,0,0);
		GetNextVertex(norm, norm2, 1.5);
		/*float xyRat , xzRat , yzRat ;
		if (norm[0] != 0)
		{
			xyRat = abs(norm[1] / norm[0]);
			xzRat = abs(norm[2] / norm[0]);
			if (norm[0] > 0) {norm2[0] = norm[0] + 0.5;	}
			else  {	norm2[0] = norm[0] - 0.5; }
			if (norm[1] > 0) { norm2[1] = norm[1] + 0.5*xyRat; }
			else { norm2[1] = norm[1] - 0.5*xyRat; }
			if (norm[2] > 0) { norm2[2] = norm[2] + 0.5*xzRat; }
			else { norm2[2] = norm[2] - 0.5*xzRat; }
		}
		else
		{
			yzRat =  abs(norm[2]/norm[1]);
			if (norm[1] > 0) { norm2[1] = norm[1] + 0.5; }
			else { norm2[1] = norm[1] - 0.5; }
			if (norm[2] > 0) { norm2[2] = norm[2] + 0.5*yzRat; }
			else { norm2[2] = norm[2] - 0.5*yzRat; }
		}	*/
		glVertex3fv(norm2);
	}
	else {		
		float vN1[3] = {0}, vN2[3] = { 0 };
		vN1[0] = v1[0] * 1.5;
		vN1[1] = v1[1] * 1.5;
		vN1[2] = v1[2] * 1.5;
		glVertex3fv(vN2);
		glVertex3fv(vN1);
		
		vN1[0] = v2[0] * 1.5;
		vN1[1] = v2[1] * 1.5;
		vN1[2] = v2[2] * 1.5;
		glVertex3fv(vN2);			
		glVertex3fv(vN1);
		vN1[0] = v3[0] * 1.5;
		vN1[1] = v3[1] * 1.5;
		vN1[2] = v3[2] * 1.5;
		glVertex3fv(vN2);
		glVertex3fv(vN1);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void DrawRecursive(float *v1, float *v2, float *v3, int depth)
{
	if (showNorm) DrawNorm(v1, v2, v3);
	float v12[3], v23[3], v31[3];
	if (depth == 0)
	{
		if (flat)
			DrawTriaFlat(v1, v2, v3);
		else
			DrawTriaSmooth(v1, v2, v3);
		return;
	}
	
	for (int i = 0; i < 3; i++)
	{
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];
	}

	ToUnit(v12);	ToUnit(v23);	ToUnit(v31);
	if (showNorm) DrawNorm(v12, v23, v31);
	DrawRecursive(v1, v12, v31, depth - 1);
	DrawRecursive(v2, v23, v12, depth - 1);
	DrawRecursive(v3, v31, v23, depth - 1);
	DrawRecursive(v12, v23, v31, depth - 1);
}

void DrawScene()
{
	glNewList(1, GL_COMPILE);
	for (int i = 0; i < 20; i++) {
		/*if (flat) DrawTriaFlat(v[id[i][0]], v[id[i][1]], v[id[i][2]]);		
		else DrawTriaSmooth(v[id[i][0]], v[id[i][1]], v[id[i][2]]);*/
		DrawRecursive(v[id[i][0]], v[id[i][1]], v[id[i][2]], depth);

		//if(showNorm)DrawNorm(v[id[i][0]], v[id[i][1]], v[id[i][2]]);


	}

	/*if (flat) {
		DrawTriaFlat(v[id[1][0]], v[id[1][1]], v[id[1][2]]);
		DrawNorm(v[id[1][0]], v[id[1][1]], v[id[1][2]]);
	}*/

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



void OnSize(int w, int h){
	glViewport(0, 0, w, h);	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.6, 100);
	glMatrixMode(GL_MODELVIEW);
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
	case 'n': showNorm = !showNorm; DrawScene(); break;
	case '+': if(depth<6)depth += 1; DrawScene(); break;
	case '-': if(depth>0)depth -= 1; DrawScene(); break;
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
