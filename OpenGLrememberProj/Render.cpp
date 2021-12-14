#include "Render.h"

#include <sstream>
#include <iostream>
#include <math.h>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("lava.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


void drawTriangle(double A[3], double B[3], double C[3]) {
	glTexCoord2d(A[0], A[1]);
	glVertex3dv(A);
	glTexCoord2d(B[0], B[1]);
	glVertex3dv(B);
	glTexCoord2d(C[0], C[1]);
	glVertex3dv(C);
}

double *normalno(double p1[3], double p2[3]) {
	
	double x0 = p1[0];
	double y0 = p1[1];
	double x1 = p2[0];
	double y1 = p2[1];

	double height = 3;
	double cum[3];

	double B[3] = { x1 - x0, y1 - y0, height - height };
	double A[3] = { x0 - x0, y0 - y0, 0 - height };

	cum[0] = A[1] * B[2] - B[1] * A[2];
	cum[1] = -(A[0] * B[2]) + B[0] * A[2];
	cum[2] = 0;

	double L = sqrt(cum[0] * cum[0] + cum[1] * cum[1]);
	cum[0] = cum[0] / L;
	cum[1] = cum[1] / L;

	return cum;
}

void Render(OpenGL *ogl)
{


	glBindTexture(GL_TEXTURE_2D, texId);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	double height = 3;

	double green[] = { 0, 1, 0 };
	double purple[] = { 1, 0, 1 };

	double bot_origin[] = { 0, 0, 0 };
	double top_origin[] = { 0, 0, height };
	double bot_triangle_origin[] = { -1, 1, 0 };
	double top_triangle_origin[] = { -1, 1, height };

	double bot_mosttop_point[] = { 0, 7, 0 };
	double top_mosttop_point[] = { 0, 7, height };

	double bot_mostright_point[] = { 6, 0, 0 };
	double top_mostright_point[] = { 6, 0, height };
	double bot_green_point[] = { 5, -5, 0 };
	double top_green_point[] = { 5, -5, height };

	double small_tr1_bot[] = { -2, -2, 0 };
	double small_tr1_top[] = { -2, -2, height };
	double small_trtop_bot[] = { -5, -6, 0 };
	double small_trtop_top[] = { -5, -6, height };
	double small_tr2_bot[] = { -3, -1, 0 };
	double small_tr2_top[] = { -3, -1, height };

	double left_side1_bot[] = { -7, 4, 0 };
	double left_side1_top[] = { -7, 4, height };

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	double *n = normalno(top_origin, top_mosttop_point);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(top_origin, top_mostright_point, bot_mostright_point);
	drawTriangle(top_origin, bot_origin, bot_mostright_point);

	n = normalno(top_green_point, top_mostright_point);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(top_mostright_point, top_green_point, bot_mostright_point);
	drawTriangle(top_green_point, bot_green_point, bot_mostright_point);

	n = normalno(small_tr1_top, top_green_point);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(bot_green_point, top_green_point, small_tr1_top);
	drawTriangle(small_tr1_top, small_tr1_bot, bot_green_point);

	n = normalno(small_trtop_top, small_tr1_top);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(small_tr1_top, small_tr1_bot, small_trtop_top);
	drawTriangle(small_trtop_top, small_trtop_bot, small_tr1_bot);

	n = normalno(small_tr2_top, small_trtop_top);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(small_trtop_top, small_trtop_bot, small_tr2_top);
	drawTriangle(small_tr2_top, small_tr2_bot, small_trtop_bot);

	n = normalno(left_side1_top, small_tr2_top);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(left_side1_bot, left_side1_top, small_tr2_top);
	drawTriangle(small_tr2_top, small_tr2_bot, left_side1_bot);

	n = normalno(top_mosttop_point, left_side1_top);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(top_mosttop_point, bot_mosttop_point, left_side1_top);
	drawTriangle(left_side1_top, left_side1_bot, bot_mosttop_point);

	n = normalno(top_origin, top_mosttop_point);
	glNormal3d(n[0], n[1], n[2]);
	drawTriangle(top_origin, bot_origin, top_mosttop_point);
	drawTriangle(top_mosttop_point, bot_mosttop_point, bot_origin);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 1);
	glNormal3d(0, 0, -1);
	drawTriangle(bot_origin, bot_mostright_point, bot_green_point);
	drawTriangle(bot_triangle_origin, bot_green_point, small_tr1_bot);
	drawTriangle(small_tr1_bot, small_trtop_bot, small_tr2_bot);
	drawTriangle(small_tr1_bot, small_tr2_bot, bot_triangle_origin);
	drawTriangle(small_tr2_bot, left_side1_bot, bot_triangle_origin);
	drawTriangle(left_side1_bot, bot_mosttop_point, bot_triangle_origin);
	drawTriangle(bot_mosttop_point, bot_origin, bot_triangle_origin);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0, 0, 1, 0.3);
	glNormal3d(0, 0, 1);
	drawTriangle(top_origin, top_mostright_point, top_green_point);
	drawTriangle(top_triangle_origin, top_green_point, small_tr1_top);
	drawTriangle(small_tr1_top, small_trtop_top, small_tr2_top);
	drawTriangle(small_tr1_top, small_tr2_top, top_triangle_origin);
	drawTriangle(small_tr2_top, left_side1_top, top_triangle_origin);
	drawTriangle(left_side1_top, top_mosttop_point, top_triangle_origin);
	drawTriangle(top_mosttop_point, top_origin, top_triangle_origin);
	glEnd();

	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}