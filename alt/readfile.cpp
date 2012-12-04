/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  
  

// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 
  
/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Transform.h" 

using namespace std ;
#include "variables.h" 
#include "readfile.h"


// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat * values) {
  mat4 transform = transfstack.top() ; 
  vec4 valvec = vec4(values[0],values[1],values[2],values[3]) ; 
  vec4 newval = valvec * transform ; 
  for (int i = 0 ; i < 4 ; i++) values[i] = newval[i] ; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) {
  mat4 &T = transfstack.top() ; 
  // Right multiply M, but do this left to account for row/column major 
  T = M*T;
}

void addLight(float pos[4], float color[4]) {
	lightposn[numused*4] = pos[0];
	lightposn[numused*4+1] = pos[1]; 
	lightposn[numused*4+2] = pos[2];
	lightposn[numused*4+3] = pos[3];
	lightcolor[numused*4] = pos[0];
	lightcolor[numused*4+1] = pos[1];
	lightcolor[numused*4+2] = pos[2];
	lightcolor[numused*4+3] = pos[3];
    ++numused ; 
}

void loadObjects() { 
    stack <mat4> transfstack ; 
    transfstack.push(mat4(1.0)) ;  // identity
	w = 800;
	h = 600;
	eyeinit = vec3(0, -2, 2);
	center = vec3(0, 0, 0);
	upinit = vec3(0, 1, 1); 
	fovy = 30.0;
    eye = eyeinit ; 
	up = upinit ; 
	amount = 5;
        sx = sy = 1.0 ; // scales in x and y 
        tx = ty = 0.0 ; // translation in x and y  
	useGlu = false; // don't use the glu perspective/lookat fns
	glEnable(GL_DEPTH_TEST);

	transfstack.push(transfstack.top()) ;
	rightmultiply(Transform::translate(0, 0, 0.1), transfstack);
	rightmultiply(mat4(Transform::rotate(90, vec3(1, 0, 0))), transfstack);
    object * obj = &(objects[numobjects]) ; 
    obj -> size = 0.15; 

    obj -> shininess = 100;
    obj -> transform = transfstack.top() ;
	obj -> type = teapot;
	transfstack.pop() ;
	numobjects++;

}
