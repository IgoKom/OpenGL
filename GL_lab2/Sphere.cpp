
#include "pch.h"

//#define PI 3.14159265
float
norm[3] = { 0 },
//spec[3] = { 1, 1, 1 },
rotX, rotY,
dx, dy, dz = -6,
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

uint
nRings = 20,	// Количество колец (широта)
nSects = 20, 	// Количество секций (долгота)
nTria, nVert;// Общее количество вершин

const uint clr1 = RGB(0, 255, 0), clr2 = RGB(0, 0, 255);	// Два цвета
const float
rad = 1.5f, 		// Радиус сферы
PI = acos(-1.0f);
float
pos[] = { 0, 0, 5, 0 },	// Позиция источника света
amb[] = { 0.05f, 0.05f, 0.05f },	// Фоновая подсветка 
dif[] = { 0.9f, 0.9f, 0.9f },	// Свет, обеспечивающий иллюзию трехмерности
spec[] = { 0.8f, 0.8f, 0.8f };	// Свет, обеспечивающий блик
char buf[128];		// Буфер текста
int shine = 40; 	// Блескость материала
char sModel[8] = "smooth";
class Point3D {
	private: float x, y, z;
	public: 
		Point3D() {	x = 0; y = 0; z = 0; }
		Point3D(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
		Point3D operator+=(Point3D& arg) { this->x += arg.x; this->y += arg.y; this->z += arg.z; return *this; }
};

struct VERT { Point3D v, n; uint c; };
struct TRIA { int i1, i2, i3; };

void TriagCalc() {
	nTria = 2 * (nRings + 1) * nSects, // Общее количество треугольников
	nVert = (nRings + 1) * nSects + 2;
}

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
		if (vRef[0] > 0) { vTarg[0] = sqrt(pow(dist, 2) / (1 + pow(xyRat, 2) + pow(xzRat, 2))); }
		else { vTarg[0] = -(sqrt(pow(dist, 2) / (1 + pow(xyRat, 2) + pow(xzRat, 2)))); }
		if (vRef[1] > 0) { vTarg[1] = abs(vTarg[0]) *xyRat; }
		else { vTarg[1] = -(abs(vTarg[0]) * xyRat); }
		if (vRef[2] > 0) { vTarg[2] = abs(vTarg[0]) *xzRat; }
		else { vTarg[2] = -(abs(vTarg[0]) * xzRat); }
	}
	else
	{
		yzRat = abs(vRef[2] / vRef[1]);
		if (vRef[1] > 0) { vTarg[1] = sqrt(pow(dist, 2) / (1 + pow(yzRat, 2))); }
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
		GetNextVertex(norm, norm2, 1.5);		
		glVertex3fv(norm2);
	}
	else {
		float vN1[3] = { 0 }, vN2[3] = { 0 };
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

void Sphere(VERT *v, TRIA* t)
{
	uint last = nVert - 1; // Индекс последней вершины (на южном полюсе)

	v[0].v = Point3D(0, rad, 0); // Формирование массива вершин. Северный полюс
	v[0].n = Point3D(0, 1, 0);
	v[0].c = clr2;

	v[last].v = Point3D(0, -rad, 0); // Южный полюс
	v[last].n = Point3D(0, -1, 0);
	v[last].c = nVert & 1 ? clr2 : clr1;

	float
		da = PI / (nRings + 2),
		db = 2 * PI / nSects,
		af = PI - da / 2,
		bf = 2 * PI - db / 2;

	uint n = 1;	// Индекс вершины, следующей за северным полюсом

	for (float a = da; a < af; a += da) 	// Цикл по широте
	{
		float
			y = rad * cos(a), 		// Координата y постоянна для всего кольца
			xz = rad * sin(a);

		for (float b = 0.; b < bf; n++, b += db) // Цикл по долготе
		{
			float // Координаты проекции в экваториальной плоскости
				x = xz * sin(b),
				z = xz * cos(b);

			v[n].v = Point3D(x, y, z);
			v[n].n = Point3D(x / rad, y / rad, z / rad);
			v[n].c = n & 1 ? clr1 : clr2;
		}
	}

	for (n = 0; n < nSects; n++) 	// Формирование массива индексов. Треугольники вблизи полюсов
	{
		t[n].i1 = 0; // Индекс общей вершины  (северный полюс)
		t[n].i2 = n + 1; // Индекс текущей вершины
		t[n].i3 = n == nSects - 1 ? 1 : n + 2; // Замыкание

		t[nTria - nSects + n].i1 = nVert - 2 - ((1 + n) % nSects); 	// Южный полюс
		t[nTria - nSects + n].i2 = nVert - 1;
		t[nTria - nSects + n].i3 = nVert - 2 - n;
	}

	int k = 1;		// Вершина, следующая за полюсом
	n = nSects;
	for (uint i = 0; i < nRings; i++, k += nSects) // Треугольники разбиения колец
	{
		for (uint j = 0; j < nSects; j++, n += 2)
		{
			t[n].i1 = k + j;
			t[n].i2 = k + nSects + j;
			t[n].i3 = k + nSects + ((j + 1) % nSects);

			t[n + 1].i1 = t[n].i1;
			t[n + 1].i2 = t[n].i3;
			t[n + 1].i3 = k + ((j + 1) % nSects);
		}
	}
}

void Init()
{
	strcpy_s(buf, "Directional");
	glClearColor(0, 0, 0, 0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE); 		// Culling говорит о том, что невидимые поверхности можно не рисовать
	glShadeModel(flat ? GL_FLAT : GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb); // Небольшая фоновая подсветка
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	TriagCalc();

	VERT *Vert = new VERT[nVert];
	TRIA *Tria = new TRIA[nTria];

	Sphere(Vert, Tria); 	// Вычисление координат, нормалей и индексов

	// Задание адресов трех массивов (вершин, нормалей и цветов),  а также шага перемещения по ним
	glVertexPointer(3, GL_FLOAT, sizeof(VERT), &Vert->v);
	glNormalPointer(GL_FLOAT, sizeof(VERT), &Vert->n);
	glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(VERT), &Vert->c);

	glNewList(1, GL_COMPILE); 	// Формирование списка рисующих команд
	glDrawElements(GL_TRIANGLES, nTria * 3, GL_UNSIGNED_INT, Tria);
	glEndList();
	delete[] Vert;
	delete[] Tria;
}

void Print(float x, float y, const char *format, ...)
{
	va_list args;
	char buffer[200];
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	glPushMatrix();
	glTranslatef(x, y, 0);
	for (char* p = buffer; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p); // GLUT_STROKE_MONO_ROMAN // GLUT_STROKE_ROMAN
	glPopMatrix();
}

void DrawInfo()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.7f, 1, 0);
	Print(80, 2800, "Rings: %d", nRings);
	Print(80, 2650, "Sects: %d", nSects);
	Print(80, 2500, "Triangles: %d", nTria);
	Print(80, 340, "Shade model: %s", sModel);
	Print(80, 200, "Light is: %s", buf);
	Print(80, 60, "Light source coordinates: (%3.1f, %3.1f, %3.1f)", pos[0], pos[1], pos[2]);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	DrawInfo();

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glTranslated(dx, dy, dz);
	glMultMatrixf(rotMatrix);

	glCallList(1);

	glPopMatrix();
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
	//float delta = -0.005 * dz;
	switch (key) {
	case GLUT_KEY_LEFT: if (nRings > 5) nRings -= nRings * 0.3; Init(); break;
		case GLUT_KEY_RIGHT: if (nRings < 2000) nRings += nRings * 0.3; Init();  break;
		case GLUT_KEY_DOWN: if(nSects>5) nSects -= nSects*0.3; Init(); break;
		case GLUT_KEY_UP:  if (nSects <2000) nSects += nSects * 0.3; Init(); break;
	}
	glutPostRedisplay();
}

void OnSize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.6, 100);
	glMatrixMode(GL_MODELVIEW);
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
		if (a[0] == GL_SMOOTH) {
			glShadeModel(GL_FLAT);
			strcpy(sModel,"flat");
		}
		else {
			glShadeModel(GL_SMOOTH);
			strcpy(sModel, "smooth");
		}
		break;
	case 'e': if (pos[3]) { strcpy_s(buf, "Directional"); pos[3] = 0; }
			  else { strcpy_s(buf, "Positional"); pos[3] = 1; }
			  break;
	case 'a': pos[0] -= 1; break;
	case 'd': pos[0] += 1; break;
	case 'z': pos[1] -= 1; break;
	case 'x': pos[1] += 1; break;
	case 'c': pos[2] -= 1; break;
	case 'v': pos[2] += 1; break;
	//case 'b': glFrontFace(GL_CW);  break;
	case 'b': glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
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
