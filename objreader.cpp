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
//#include "shaders.h"
#include "Transform.h"
//#include <FreeImage.h>
//#include "UCB/grader.h"

using namespace std ; 
#include "variables.h"

enum { vert, vertnorm, verttext, face, unknown };

list<shape*>* readobj( const char * filename )
{
  list<shape*>* pshapes = new list<shape*>;
  const list<shape*>& shapes = *pshapes;

  shape * shape = 0;


  int mode;
  string line, h ; 
  ifstream in ;
  int linenum = 0;

  list<float> verts;
  list<float> vertnorms;
  list<float> verttexts;
  list<int> faces;

  printf("\nreading model %s\n",filename);
  in.open(filename) ; 
  if (in.is_open()) {
    while( in ) {
      getline(in, line);
      stringstream ss(line);
      float values[8];
      linenum++;
      ss >> h;
      
      if( h[0] == '#' ) {
        continue;
      } else if( h == "v" ) {
        mode = vert;
      } else if( h == "vt" ) {
        mode = verttext;
      } else if( h == "vn" ) {
        mode = vertnorm;
      } else if( h == "f" ) {
        mode = face;
      } else {
        // ignore unsupported line (e.g. vp, s)
        cerr << "Unsupported line " << line << "\n";
        mode = unknown;
      }

      if( mode != unknown ) {
        int i = 0;
        while(ss && i < 8) {
          ss >> values[i++];
        }
        if( shape == 0 ) {
          //shape = new cshape();
        }
        switch( mode ) {
        case vert:
          if( i >= 3 ) {
            verts.push_back(values[0]);
            verts.push_back(values[1]);
            verts.push_back(values[2]);
          } else {
            cerr << "Not enough points for vertex: " << line << "\n";
          }
          break;    
        case verttext:
          if( i >= 2 ) {
            verttexts.push_back(values[0]);
            verttexts.push_back(values[1]);
          } else {
            cerr << "Not enough points for texcoord: " << line << "\n";
          }
          break;  
        case vertnorm:
          if( i >= 3 ) {
            vertnorms.push_back(values[0]);
            vertnorms.push_back(values[1]);
            vertnorms.push_back(values[2]);
          } else {
            cerr << "Not enough points for vertnorm: " << line << "\n";
          }
          break;    
        case face:
          break;    
        }
      }
      
    }
  }

  return pshapes;
}
