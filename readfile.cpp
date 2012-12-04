/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple shapes   */
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
//#include <GL/glew.h>
#include <GL/glut.h>
#include "Transform.h" 

using namespace std ;
#include "variables.h" 
#include "readfile.h"

#include "glm.h"

extern object *readobj(const char *fn);
/*
 mat4 m; m[col][row]
 */

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat * values) {
  mat4 transform = transfstack.top() ; 
  vec4 valvec = vec4(values[0],values[1],values[2],values[3]) ; 
  vec4 newval = valvec * transform ; 
  for (int i = 0 ; i < 4 ; i++) values[i] = newval[i] ; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) {
  mat4 &T = transfstack.top() ; 
  // Right multiply M, AND don't do this left to account for row/column major since
  // we return column-major matrices from our Transform:: methods (other than rotate)
  T =  T * M ; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, GLfloat * values) {
  for (int i = 0 ; i < numvals ; i++) {
    s >> values[i] ; 
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n" ; 
      return false ;
    }
  }
  return true ; 
}

void resetscene() {
  numused = 0;
  for( map<string,object*>::iterator it = objects.begin();
      it != objects.end();
      it++ )
  {
    delete it->second;
  }
  objects.clear();
  for( list<shape*>::iterator it = shapes.begin();
      it != shapes.end();
      it++ )
  {
    delete *it;
  }
  shapes.clear();
  for( map<string,texture*>::iterator it = allTextures.begin();
      it != allTextures.end();
      it++ )
  {
    delete it->second;
  }
  allTextures.clear();
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
  sx = sy = sz = 1.0 ; // scales in x and y 
  tx = ty = tz = 0.0 ; // translation in x and y  
  useGlu = false; // don't use the glu perspective/lookat fns
  glEnable(GL_DEPTH_TEST);
/*
  transfstack.push(transfstack.top()) ;
  rightmultiply(Transform::translate(0, 0, 0.1), transfstack);
  rightmultiply(mat4(Transform::rotate(90, vec3(1, 0, 0))), transfstack);
  shape * obj = new shape();
  shapes.push_back(obj); 
  obj -> size = 0.15; 

  obj -> shininess = 100;
  obj -> transform = transfstack.top() ;
  obj -> type = teapot;
  transfstack.pop() ;
  */

}


void clone_and_add( const list<shape*> &pshapes, stack<mat4> transfstack ) {
  for( list<shape*>::const_iterator it = pshapes.begin();
      it != pshapes.end(); it++ ) {
    shape *s = new shape( **it );
    s->transform = transfstack.top() * s->transform;
    shapes.push_back( s );
  }
}

void readfile(const char * filename) {

  string str, cmd ; 
  ifstream in ;
  int linenum = 0;

  printf("\nreading %s\n",filename);
  in.open(filename) ; 
  if (in.is_open()) {

    object *defining_object = 0 ;
    texture *active_texture = 0 ;
    stack <mat4> transfstack ; 
    transfstack.push(mat4(1.0)) ;  // identity

    getline (in, str) ;
    ++linenum;
    size_t fpos;
    while (in) {
      if (((fpos=str.find_first_not_of(" \t\r\n")) != string::npos) && (str[fpos] != '#')) {
        // Ruled out comment and blank lines 

        stringstream s(str) ;
        s >> cmd ; 
        int i ; 
        GLfloat values[10] ; // position and color for light, colors for others
                             // Up to 10 params for cameras.  
        bool validinput ; // validity of input 

        // Process the light, add it to database.
        // Lighting Command
        //cout << ":: " << cmd << "\n";
        if (cmd == "light") { 
          if (numused == numLights) // No more Lights 
            cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n" ;
          else {
            validinput = readvals(s, 8, values) ; // Position/color for lts.
            if (validinput) {
              // YOUR CODE FOR HW 2 HERE. 
              // Note that values[0...7] shows the read in values 
              // Make use of lightposn[] and lightcolor[] arrays in variables.h
              // Those arrays can then be used in display too.  

              int base=numused*4,i,j=0;
              for(i=0;i<4;i++)
                lightposn[base+i] = values[j++];
              for(i=0;i<4;i++)
                lightcolor[base+i] = values[j++];
              ++numused;
            }
          }
        }

        // Material Commands 
        // Ambient, diffuse, specular, shininess
        // Filling this in is pretty straightforward, so I've left it in 
        // the skeleton, also as a hint of how to do the more complex ones.
        // Note that no transforms/stacks are applied to the colors. 

        else if (cmd == "ambient") {
          validinput = readvals(s, 4, values) ; // colors 
          if (validinput) 
            for (i = 0 ; i < 4 ; i++) ambient[i] = values[i] ; 
        }
        else if (cmd == "diffuse") {
          validinput = readvals(s, 4, values) ; 
          if (validinput) 
            for (i = 0 ; i < 4 ; i++) diffuse[i] = values[i] ; 
        }
        else if (cmd == "specular") {
          validinput = readvals(s, 4, values) ; 
          if (validinput) 
            for (i = 0 ; i < 4 ; i++) specular[i] = values[i] ; 
        }
        else if (cmd == "emission") {
          validinput = readvals(s, 4, values) ; 
          if (validinput) 
            for (i = 0 ; i < 4 ; i++) emission[i] = values[i] ; 
        }
        else if (cmd == "shininess") {
          validinput = readvals(s, 1, values) ; 
          if (validinput) shininess = values[0] ; 
        }
		else if (cmd == "animate") {
          validinput = readvals(s, 1, values) ; 
          if (validinput) animate_obj = values[0] ;
		}
        else if (cmd == "size") {
          validinput = readvals(s,2,values) ; 
          if (validinput) { w = (int) values[0] ; h = (int) values[1] ; } 
		}
        else if (cmd == "dimensions") {
          validinput = readvals(s,3,values) ; 
          if (validinput) { max_x = (float) values[0] ; max_y = (float) values[1] ; max_z = (float) values[2]; } 
        }
        else if (cmd == "camera") {
          validinput = readvals(s,10,values) ; // 10 values eye cen up fov
          if (validinput) {
            // YOUR CODE FOR HW 2 HERE
            // Use all of values[0...9]
            // You may need to use the upvector fn in Transform.cpp
            // to set up correctly. 
            // Set eyeinit upinit center fovy in variables.h 
            for( i = 0; i < 3; i++ )  {
              eyeinit[i] = values[i];
              center[i] = values[i+3];
              upinit[i] = values[i+6];
            }
            //upinit = Transform.upvector(upinit);
            fovy = values[9];
          }
        }

        //*
        // I've left the code for loading shapes in the skeleton, so 
        // you can get a sense of how this works.  
        else if (cmd == "sphere" || cmd == "cube" || cmd == "teapot") {
          validinput = readvals(s, 1, values) ; 
          if (validinput) {
            //obj -> tex = active_texture;
            shape_type kind;
            if (cmd == "sphere") kind = sphere ; 
            else if (cmd == "cube") kind = cube ; 
            else if (cmd == "teapot") kind = teapot ; 
            
            shape * obj = new shape( kind, values[0], transfstack.top()  );
            if( 0 != defining_object ) {
              defining_object->shapes.push_back(obj);
            } else {
              shapes.push_back(obj);
            }
            
            //obj -> size = values[0] ; 
            for (i = 0 ; i < 4 ; i++) {
              (obj -> ambient)[i] = ambient[i] ; 
              (obj -> diffuse)[i] = diffuse[i] ; 
              (obj -> specular)[i] = specular[i] ; 
              (obj -> emission)[i] = emission[i] ;
            }
            obj -> shininess = shininess ; 
			obj -> animate = animate_obj;
            //obj -> transform = transfstack.top() ;
          }
        }
        //*/

        else if (cmd == "translate") {
          validinput = readvals(s,3,values) ; 
          if (validinput) {
            // YOUR CODE FOR HW 2 HERE.  
            // Think about how the transformation stack is affected
            // You might want to use helper functions on top of file. 

            mat4 t = Transform::translate( values[0], values[1], values[2] );
            rightmultiply( t, transfstack );
          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values) ; 
          if (validinput) {
            // YOUR CODE FOR HW 2 HERE.  
            // Think about how the transformation stack is affected
            // You might want to use helper functions on top of file.  
            rightmultiply( Transform::scale( values[0], values[1], values[2] ),
                transfstack );
          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values) ; 
          if (validinput) {
            vec3  ax = glm::normalize( vec3( values[0], values[1], values[2] ) );
            float an = values[3];
            // YOUR CODE FOR HW 2 HERE. 
            // values[0..2] are the axis, values[3] is the angle.  
            // You may want to normalize the axis (or in Transform::rotate)
            // See how the stack is affected, as above.  
            mat3 r = glm::transpose(Transform::rotate(an,ax));
            mat4 m = mat4(r);
            rightmultiply( m, transfstack );
          }
        }
        
        // I include the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform") { 
          transfstack.push(transfstack.top()) ; 
        } else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) 
            cerr << "Stack has no elements.  Cannot Pop\n" ; 
          else transfstack.pop() ; 
        } else if( cmd == "objectStart" || cmd == "objectDef" ) {
          string name;
          s >> name; 

          if( 0 == defining_object ) {
            if( objects.find(name) != objects.end() ) {
              defining_object = objects[name];
            } else {
              defining_object = new object(name);
              objects[name] = defining_object;
            }
            // implicit push of matrix stack
            transfstack.push(transfstack.top());
          } else {
            cerr << "Ignoring start of new object (" << name << ") while in definition for " << defining_object->name << "\n";
          }
        } else if( cmd == "objectEnd" ) {
          if( 0 != defining_object ) {
            defining_object = 0;
            // implicit pop of matrix stack
            transfstack.pop();
          } else {
            cerr << "Can't end an object when one isn't started\n";
          }
        } else if( cmd == "object" ) {
          string name;
          s >> name;
          map<string,object*>::iterator objit = objects.find(name);
          if( objit != objects.end() ) {
            clone_and_add( objit->second->shapes, transfstack );
          } else {
            cerr << "Couldn't find object definition for " << name << "\n";
          }
        } else if( cmd == "texture" ) {
          // if we have no argument, clear the current texture
          // else if we have one argument, and that arguemnt
          // is in our list of textures, set the active texture
          // else expect name filename xdim ydim
          if( s.eof() ) {
            active_texture = 0;
            //cout << "Active texture cleared\n"; 
          } else {
            string name;
            s >> name;
            map<string,texture*>::iterator it = allTextures.find(name);
            if( allTextures.end() != it ) {
              // If found, set name to be the active texture
              active_texture = it->second;
              cout << "Active texture set to " << name << "\n";
            } else {
              cerr << "Texture error("<< linenum << "), texture "<< name <<" not defined.\n";
            }
          }
        } else if (cmd == "textureDef" || cmd == "defTexture" ) {
          // otherwise, try to extract filename x-dim y-dim
          string name;
          string filename;
          int xdim = 0, ydim = 0;
          validinput = false;
          if( !s.eof() ) {
            s >> name;
            if( !s.eof() ) {
              s >> filename;
              validinput = !filename.empty() && !name.empty();
            }
          }
          if( validinput ) {
            if( s.eof() ) {
              xdim = 256;
              ydim = 256;
              cout << "Texture Info(" << linenum<< ") Using default dims of 256x256 for texture " << name << "(" << linenum<< ")\n";
            } else {
              validinput = readvals(s,2,values) ; 
              if (validinput) {
                xdim = values[0];
                ydim = values[1];
              } else {
                cerr << "Texture Error(" << linenum<< ") retrieving xdim, ydim for texture "<< name<<"(" << linenum<< "), Skipping\n";
              }
            }
            //*
                //cout << "Reading " << name << " xdim, ydim " << values[0] << ", " << values[1] << xdim << ydim << filename << "\n";
            if( validinput ) { //!filename.empty() && xdim && ydim ) {
              //printf( "Declared texture %s, %s (%d,%d).. ", name.c_str(), filename.c_str(), xdim, ydim );
              texture *t = new texture( allTextures.size(), name, xdim, ydim );
              t->read( filename.c_str() );
              t->glinit( shaderprogram );
              allTextures.insert( pair<string,texture*>( name, t ) );
              cout << "Declared texture " << t->id << ", name " << t->name << "\n";
            } else {
              cerr << "No filename for texture\n";
            }
            //*/
          } else {
            cerr << "Texture error:("<<linenum<<") expects name filename xdim ydim, Skipping\n";
          }
        } else if( cmd== "shapeDef" ) {
          string name, kind, mode = "vertices";
          //s >> name;
          s >> kind;
          list<float> vertices;
          list<float> colors;
          list<int>   elements;
          list<float> texcoords;
          texture *shape_texture = 0;
          getline (in, str) ; 
          ++linenum;
          bool polyEnd = false;
          while (in && !polyEnd) {
            // skip empty lines and comments
            if (((fpos=str.find_first_not_of(" \t\r\n")) == string::npos) || (str[fpos] == '#')) {
              getline (in, str) ; 
              ++linenum;
              continue;
            }
            stringstream s(str) ;
            s >> cmd ;
            if( cmd == "shapeEnd" ) {
              // end the loop
              polyEnd = true;
              // capture the polygon
              cout << "Capturing shape, tex: " << (shape_texture?shape_texture->name:"None") << "\n";
              shape *obj = new cshape(
                  kind,
                  vertices,
                  colors,
                  elements,
                  texcoords,
                  shape_texture,
                  transfstack.top() );
              obj -> size = 1;//values[0] ; 
              for (i = 0 ; i < 4 ; i++) {
                (obj -> ambient)[i] = ambient[i] ; 
                (obj -> diffuse)[i] = diffuse[i] ; 
                (obj -> specular)[i] = specular[i] ; 
                (obj -> emission)[i] = emission[i] ;
              }
              obj -> shininess = shininess ;
              obj->glinit(shaderprogram);
              if( 0 != defining_object ) {
                defining_object->shapes.push_back(obj);
              } else {
                shapes.push_back(obj);
              }
              /// done in ctor
              //obj -> transform = transfstack.top() ;
              //obj -> tex = active_texture;
            } else if( cmd=="vertices"
                || cmd == "colors"
                || cmd == "elements"
                || cmd == "texture"
            ) {
              mode = cmd;
              if( cmd == "texture" ) {
                string texname;
                s >> texname;
                map<string,texture*>::iterator it = allTextures.find(texname);
                if( allTextures.end() != it ) {
                  // If found, set name to be the active texture
                  shape_texture = it->second;
                } else {
                  cerr << "Texture error("<< linenum << "), texture "<< texname <<" not defined.\n";
                }
              }
            } else {
              stringstream s(str);
              //cout << mode << " mode: " ;
              if( mode == "vertices" ) {
                validinput = readvals( s, 3, values );
                if( validinput ) {
                  vertices.push_back( values[0] );
                  vertices.push_back( values[1] );
                  vertices.push_back( values[2] );
                }
              } else if( mode=="colors" ) {
                // read in colors
                validinput = readvals( s, 3, values );
                if( validinput ) {
                  colors.push_back( values[0] );
                  colors.push_back( values[1] );
                  colors.push_back( values[2] );
                  //colors.push_back( values[3] );
                }
              } else if( mode=="elements" ) {
                // read in vertex indices
                validinput = readvals( s, 4, values );
                if( validinput ) {
                  elements.push_back( (int) values[0] );
                  elements.push_back( (int) values[1] );
                  elements.push_back( (int) values[2] );
                  elements.push_back( (int) values[3] );
                }
              } else if( mode=="texture" ) {
                // read in texcoords
                validinput = readvals( s, 2, values );
                if( validinput ) {
                  texcoords.push_back( values[0] );
                  texcoords.push_back( values[1] );
                }
              } else {
                cerr << "Unrecognized mode : " << mode << "!!\n";
              }
            }
            if( !polyEnd ) {
              getline (in, str) ; 
              ++linenum;
            }
          }

        } else if( cmd == "model" ) {
          string filename;
          s >> filename;
          if( !(validinput=fileexists( filename.c_str() )) ) {
            string ofn = filename;
            filename.insert(0,"models/");
            if( !(validinput=fileexists( filename.c_str() )) ) {
              cerr << "Can't find model " << ofn << " or " << filename << "\n";
            }
          }
          if( validinput ) {
            cout << "Reading model " << filename << "\n";
            GLMmodel *pmodel = glmReadOBJ(filename.c_str());
            model *pm = new model(pmodel, transfstack.top());
            pm->glinit(shaderprogram);
            shapes.push_back( pm );
            //object *o = readobj( filename.c_str() );
            //objects.insert( std::pair<string,object*>( string("o"), o ) );
          } else {
          }
        } 
        else {
          cerr << "Unknown Command (" << linenum << "): " << cmd << ", Skipping \n" ; 
        }
      }
      getline (in, str) ; 
      ++linenum;
    }

  // Set up initial position for eye, up and amount
  // As well as booleans 

    eye = eyeinit ; 
    up = upinit ; 
    amount = 5;
    sx = sy = sz = 1.0 ; // scales in x and y 
    tx = ty = 0.0 ; // translation in x and y
    enable_tex = true;
    useGlu = false; // don't use the glu perspective/lookat fns

    glEnable(GL_DEPTH_TEST);

  }
  else {
    cerr << "Unable to Open Input Data File " << filename << "\n" ; 
    throw 2 ; 
  }
  
}

