// GL_lab4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "BMPLoader.h"


float
rotText = 0,
moveText = 0,
rotX = 20, rotY = 30,
dx, dy, dz = -15,
v[8][3] = { -1, 1, -1, 1, 1, -1, 1, -1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1, 1, 1 },
norm[6][3] = { 0, 0, -1, 0, 0, 1, -1, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, 0 },
rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
green[] = { 0, 0.6, 0 },
white[] = { 0.9, 0.9, 0.9 },
sx, sy,
texX, texY, texZ;
int
id[6][4] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 3, 5, 4, 7, 6, 2, 1, 0, 4, 7, 1, 5, 3, 2, 6, },
posX, posY,
filter = GL_NEAREST,
wrap = GL_REPEAT,
texGen = GL_OBJECT_LINEAR;
bool leftButton, lighting = false, sphereMap = false,
	decal = true, colored = false, flat = false, twoSide = false,
	textMove = false, textRot = false;
UINT texture;

void AddRotation(float angle, float x, float y, float z)
{
	glPushMatrix();
	glLoadIdentity();
	glRotatef(angle, x, y, z);
	glMultMatrixf(rotMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
	glPopMatrix();
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
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	glPopMatrix();
}

void DrawInfo()
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 3000, 0, 3000);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(0.7f, 1, 0);
	Print(80, 2800, "Decal: %d", decal);
	Print(80, 2650, "Colored: %d", colored);
	Print(80, 2500, "Lighting: %d", lighting);
	Print(80, 200, "Wrapping: %s", (wrap == GL_REPEAT ? "Repeat" : "Clamp"));
	Print(80, 60, "Filtering: %s", (filter == GL_LINEAR ? "Linear" : "Nearest"));
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

void DrawScene() {	
	if (sphereMap) {
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, texGen);
	}
	else {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}	
	glMatrixMode(GL_TEXTURE);
	if (textRot)glRotatef(rotText, 0, 0, 1);
	if (textMove)glTranslatef(moveText, 0, 0);
	textRot = false;
	textMove = false;	
	glMatrixMode(GL_MODELVIEW);
	float t[4][2] = { 0, 1, 0, 0, 1, 0, 1, 1 };
	glNewList(1, GL_COMPILE);
	glBegin(GL_QUADS);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(norm[i]);
		for (int j = 0; j < 4; j++) {
			if (!sphereMap)glTexCoord2fv(t[j]);
			glVertex3fv(v[id[i][j]]);
		}
	}
	glEnd();
	glLoadIdentity();
	glEndList();
}

void OnPaint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	if (!decal) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	}
	else if (colored) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		
	}
	else glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	DrawInfo();
	glTranslated(dx, dy, dz);
	glMultMatrixf(rotMatrix);
	
	glCallList(1);
	glPopMatrix();
	glutSwapBuffers();

}

void SetLight()
{
	if (lighting)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}
	else
		glDisable(GL_LIGHTING);
}

void OnIdle()
{	
	AddRotation(rotX / 200, 1, 0, 0);
	AddRotation(rotY / 200, 0, 1, 0);
	glutPostRedisplay();
}

char* FileDlg()
{
	char *fn = new char[MAX_PATH], dir[MAX_PATH];
	fn[0] = 0;
	GetCurrentDirectory(MAX_PATH - 1, dir);
	OPENFILENAME ofn =
	{
		sizeof(OPENFILENAME), 0, 0, "BMP Files (*.bmp)\0*.bmp\0DIB Files (*.dib)\0*.dib\0", 0, 0, 1,
		fn, MAX_PATH, 0, 0,	dir, "Find a bitmap file",
		OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		0, 0, "bmp", 0, 0, 0, 0, 0, 0
	};
	return GetOpenFileName(&ofn) ? fn : 0;
}

bool LoadTexture(int id)
{
	char *fn = FileDlg();
	if (fn == 0)
		return false;
	BmpLoader* p = new BmpLoader(fn);
	if (!p)
		throw "Error reading texture.\n";
	glBindTexture(GL_TEXTURE_2D, id);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, p->sizeX, p->sizeY, GL_RGB, GL_UNSIGNED_BYTE, p->data);
	return true;
}

void Init()
{
	glClearColor(0, 0, 0, 0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	SetLight();
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glutIdleFunc(0);
	if (LoadTexture(1))
		DrawScene();
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
	case GLUT_KEY_LEFT: //dx -= delta;
		moveText += 0.01;
		if (moveText > 5)moveText -= 5;
		break;
	case GLUT_KEY_RIGHT: //dx += delta;
		moveText -= 0.01;
		if (moveText < -5)moveText += 5;
		break;
	case GLUT_KEY_DOWN: //dy -= delta;
		rotText -= 0.5;
		if (rotText < -360)rotText += 360;
		break;
	case GLUT_KEY_UP: 
		//dy += delta; break;
		rotText += 0.5;
		if (rotText > 360)rotText -= 360;
	}
	glutPostRedisplay();
}

void OnKey(byte ch, int x, int y)
{
	int a[1] = { 0 };
	switch (ch)
	{
	case 27: exit(0); break;
	case '3': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
	case 'p': glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
	//case 'f': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	//case 'l': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
	case 'o': LoadTexture(1); break;
	case 'q':
		glGetIntegerv(GL_SHADE_MODEL, a);
		if (a[0] == GL_SMOOTH)
			glShadeModel(GL_FLAT);
		else glShadeModel(GL_SMOOTH);
		break;
	//case 'w': flat = !flat; DrawScene(); break;
	case 'l': lighting = !lighting; SetLight(); break;
	case 'd': decal = !decal; colored = false; break;
	case 'c': colored = !colored; decal = false; break;
	case 's': sphereMap = !sphereMap; DrawScene(); break;
	case 'f': if(filter == GL_NEAREST)filter = GL_LINEAR;
			  else  filter = GL_NEAREST;
	case 'w': if (wrap == GL_REPEAT)wrap = GL_CLAMP;
			  else  wrap = GL_REPEAT;
		break;
	case 't':
		texGen =
			texGen == GL_EYE_LINEAR ? GL_OBJECT_LINEAR :
			texGen == GL_OBJECT_LINEAR ? GL_SPHERE_MAP : GL_EYE_LINEAR;
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, texGen);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, texGen);
		break;
	case '1':
		textRot = true;		
		DrawScene();
		break;
	case '2': 
		textMove = true;
		DrawScene();
		break;
	case '0': 		
		moveText = 0;
		rotText = 0;
		textMove = false;
		//DrawScene();
		break;
	default: break;
	}
	glutPostRedisplay();
}

void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(w) / h, 0.6, 100);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("lab_4");	
	glutMouseFunc(OnMouse);
	glutMotionFunc(OnMouseMove);
	glutSpecialFunc(OnSpecialKey);
	glutKeyboardFunc(OnKey);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	Init();
	glutMainLoop();
    return 0; 
}

