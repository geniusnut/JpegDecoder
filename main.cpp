//
//  main.cpp
//  JpegDecorder
//
//  Created by yuta.amano on 12/11/09.
//  Copyright (c) 2012年 yuta.amano. All rights reserved.
//

#include <iostream>
#include <GLUT/GLUT.h>
#include "JpegDecorder.h"

GLubyte * bits;
GLuint g_width , g_height;

void disp( void ) {
	glClear(GL_COLOR_BUFFER_BIT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(g_width , g_height , GL_RGB , GL_UNSIGNED_BYTE, bits);
	glFlush();
}

int main(int argc , char ** argv) {
    
    JpegDecorder decoder("/Users/yuta.amano/Xcodes/JPEG/QB.jpeg");
    decoder.Decode();
    
    g_width = decoder.GetWitdh();
    g_height = decoder.GetHeight();
    bits = new GLubyte[decoder.GetWitdh() * decoder.GetHeight() * 3];
    decoder.GetRGB(bits);
    
	glutInit(&argc , argv);
	glutInitWindowSize(g_width , g_height);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    
	glutCreateWindow("");
	glutDisplayFunc(disp);
    
	glutMainLoop();
	delete[] bits;
    
	return 0;
}
