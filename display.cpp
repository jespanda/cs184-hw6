/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple shapes   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#include "Transform.h"

using namespace std ; 

#include "variables.h"
#include "readfile.h"

// Particle System
#include "particlesystem.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  

void transformvec (const GLfloat input[4], GLfloat output[4] ) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) 
      output[i] += modelview[4*j+i] * input[j] ; 
  }
}

void setLightingFlag( lighting_flag flag ) {
  lightingFlags |= flag;
  glUniform1i( enablelighting, lightingFlags );
}

void clearLightingFlag( lighting_flag flag ) {
  lightingFlags ^= (lightingFlags & flag );
  glUniform1i( enablelighting, lightingFlags );
}
void display() {
  glClearColor(.010, .01, 0.2, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable( GL_TEXTURE_2D );
  glEnable (GL_BLEND); 
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // I'm including the basic matrix setup for model view to 
  // give some sense of how this works.  

  glMatrixMode( GL_MODELVIEW);
  glPolygonMode( GL_FRONT_AND_BACK, wireframes?GL_LINE:GL_FILL );
  mat4 mv ; 

  //cout << "Center ("<<center.x<<","<<center.y<<","<<center.z<<")\n";
  if (useGlu) mv = glm::lookAt(eye,center,up) ; 
  else {
    mv = Transform::lookAt(eye,center,up) ; 
    mv = glm::transpose(mv) ; // accounting for row major
  }
  glLoadMatrixf(&mv[0][0]) ; 
  // Transformations for shapes, involving translation and scaling 
  mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
  sc = Transform::scale(sx,sy,sz) ; 
  tr = Transform::translate(tx,ty,tz) ;
  transf =  tr * sc ;


  // Set Light and Material properties for the teapot
  // Lights are transformed by current modelview matrix. 
  // The shader can't do this globally. 
  // So we need to do so manually.
  if (numused) {
    setLightingFlag( lightingenabled );
    //glUniform1i(enablelighting,true) ;

    // YOUR CODE FOR HW 2 HERE.  
    // You need to pass the lights to the shader. 
    // Remember that lights are transformed by modelview first.  
    for( int i = 0; i < numused; i++ ) {
      transformvec( &(lightposn[i<<2]), &(lightransf[i<<2]) );
    }
    glUniform1i( numusedcol, numused );
    glUniform4fv( lightcol, numLights, lightcolor );
    glUniform4fv( lightpos, numLights, lightransf );
  }
  else {
    clearLightingFlag( lightingenabled );
    //glUniform1i(enablelighting,false) ; 
  }

  // YOUR CODE FOR HW 2 HERE.  
  // You need to use scale, translate and modelview to 
  // set up the net transformation matrix for the shapes.  
  // Account for GLM issues etc.  

  int oc = 0;
  
  _fountain->draw();

  for (list<shape*>::iterator it = shapes.begin();
      it != shapes.end();
      it++, oc++ ) {
    //int i = 0 ; i < numshapes ; i++) {
    shape * obj = *it;//shapes.at(i) ; 

    // prepare the matrix for object drawing 
    mat4 o =  mv * transf * obj->transform;
	

    //cout << "Drawing "<< oc << "\n";
    //obj->dump();
    glPushMatrix();
    glLoadMatrixf(&o[0][0]) ;
	if (obj->animate==1) {
		glTranslatef(move_boats_x, move_boats_y, 0.0);

	}
	if (obj->animate ==2) {
		glRotatef(spin_degrees, 0, 0, 5);
	}
	if (discomode) {
		glRotatef(spin_degrees, -5, 8, 0);
	}
    obj->draw();
    obj->draw();

    glPopMatrix();
  }
  // Code for particle system

  glutSwapBuffers();
  glFlush();
}
