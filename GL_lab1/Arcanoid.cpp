#include "pch.h"
bool ON = false;
double
Left = -1, Top = 1, Right = 1, Bot = -1, // ������������� �������� ���� (��� ����)
x, y, dx = 0.001, dy = 0.001, r = 0.05, yMin,	// ���: ����������, ��������, ������
xc = 0, x1, x2, 	// �������: �-���������� ������, ����� � ������ ����,
w, h;		// ������, ������

void MoveBall() {
	x += dx;	y += dy;

	if (x < Left + r || Right - r < x)
	{
		dx = -dx;
		x += 2. * dx;
	}
	if (y > Top + r)
	{
		dy = -dy;
		y += 2. * dy;
	}
	if ( (Bot + r + h >= y) && (Bot + r + h/2 <= y) && (x1 <= x) && (x <= x2)) {
		dy = -dy;
		y += 2. * dy;
	}
}

void MoveRacket(int dir)
{
	double delta = dir * 0.5 * r;
	if (Left < x1 + delta && x2 + delta < Right)
	{
		xc += delta;	x1 += delta;	x2 += delta;
	}
}



void Set(double xx, double yy, double c, double ww)
{
	x = xx;		y = yy;    	// ������������� ����������
	xc = c;		w = ww;		h = ww / 10;
	x1 = xc - w / 2;
	x2 = xc + w / 2;
	yMin = Bot + h + r;
}

void OnSize(int w, int h) { glViewport(0, 0, w, h); }

void DrawScene()
{
	glColor3ub(0, 0, 255);
	glRectd(x1, Bot, x2, Bot + h);	// �������

	glPushMatrix();	// ���������� ��������� ������������� ������������
	glTranslated(x, y, 0); // ��������� ��������� (�����)
	glutSolidSphere(r, 100, 100);		// ���
	glPopMatrix();	// ��������������� ��������� ������������
}

void OnPaint()
{
	glClear(GL_COLOR_BUFFER_BIT);
	MoveBall();	// ���������� ���
	DrawScene();	// ������ �����
	glutSwapBuffers();
}

void Init()
{
	Set(0, 0.9, 0, 0.4);		// ������������� �������� ����
	glutIdleFunc(0);	// ��������� ��������
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: MoveRacket(-1); break;
		case GLUT_KEY_RIGHT: MoveRacket(1); break;
		case GLUT_KEY_F9:
			Init(); break;
		default: break;
	}
	glutPostRedisplay();
}

void OnKey(unsigned char ch, int x, int y)
{
	switch (ch)	{
		case 27: exit(0); break;
		case 32: {
			glutIdleFunc(OnPaint);
			break; 
		}	// �������� ��������		
		default: break;
	}
	glutPostRedisplay();
}


int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Arkanoid:");

	Init();

	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
	return 0;
}
