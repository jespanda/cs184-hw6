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

texture::texture( int i, string n, int x, int y )
: id( i ),
  name( n ),
  xdim( x ),
  ydim( y ),
  texturedata( new GLubyte[xdim*ydim*3] )
{
}

texture::~texture()
{
  if( texturedata ) delete [] texturedata;
}

void texture::read(const char *filename ) {
  int i,j,k ;
  FILE * fp ; 
  GLint err ;
  string fn("textures");//+DIRSEP);
  fn += DIRSEP;
  fn += filename;

  assert(fp = fopen(fn.c_str(),"rb")) ;
  fscanf(fp,"%*s %*d %*d %*d%*c") ;
  for (i = 0 ; i < xdim ; i++)
    for (j = 0 ; j < ydim ; j++)
      for (k = 0 ; k < 3 ; k++) {
        GLubyte b;
        fscanf(fp,"%c", &texturedata[i*3*xdim+3*j+k] ) ;
      }
  fclose(fp) ; 
}

void texture::glinit(GLuint program) {
  /*
  glTexCoordPointer(2,GL_FLOAT,0,0);//BUFFER_OFFSET(0)) ; 
  glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 
  */
  glGenTextures(1, &texName) ; 
  glBindTexture (GL_TEXTURE_2D, texName) ; 

  // TODO drive via flags
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE,
      texturedata) ;
}



shape::shape( shape_type _kind, GLfloat _size, const mat4& _transform )
: kind( _kind ),
  size( _size ),
  //tex( _tex ),
  transform( _transform )
{
}

shape::shape( const shape &src ) {
  kind = src.kind;
  size = src.size;
  memcpy( &ambient[0],    src.ambient,    sizeof(GLfloat)*4 );
  memcpy( &diffuse[0],    src.diffuse,    sizeof(GLfloat)*4 );
  memcpy( &specular[0],   src.specular,   sizeof(GLfloat)*4 );
  memcpy( &emission[0],   src.emission,   sizeof(GLfloat)*4 );
  shininess = src.shininess;
  //tex = src.tex;
  memcpy( &transform, &src.transform, sizeof(mat4) );
}

shape::~shape()
{
}

void shape::glinit(GLuint program)
{
}

void shape::dump()
{

}

void shape::draw() 
{
  const int tessel = 20 ; 

  glUniform4fv( ambientcol, 1, ambient );
  glUniform4fv( diffusecol, 1, diffuse );
  glUniform4fv( specularcol, 1, specular );
  glUniform4fv( emissioncol, 1, emission );
  glUniform1fv( shininesscol, 1, &shininess );
  glUniform1i( hastex, 0 );

  switch( kind ) {
  case cube:
    glutSolidCube(size) ; 
    break;
  case sphere:
    glutSolidSphere( size, tessel, tessel );
    break;
  case teapot:
    glutSolidTeapot( size) ; 
    break;
  }
}



//static
GLenum cshape::toShapeMode( string k ) {
  if( k == "tri" ) {
    return GL_TRIANGLES;
  } if( k == "quad" ) {
    return GL_QUADS;
  } else if( k == "poly" || k == "polygon" ) {
    return GL_POLYGON;
  }
  return 0 ;
} 


#define ppcolor 3
  //shape() : tex(0) {}
cshape::cshape(
    //string n,
    string k,
    list<float>  &source_verts,
    list<float>  &source_colors,
    list<int>    &source_elts,
    list<float>  &source_texcoords,
    texture*     active_texture,
    mat4         &source_transform)
: //name( n ),
  shape( glitem, 1.0, source_transform ),
  mode( toShapeMode(k) ),
  //size( 1.0 ),
  numverts( source_verts.size()/3 ),
  verts( new GLfloat[numverts*3] ),
  numcolors( source_colors.size()/ppcolor ),
  colors( new GLfloat[numcolors*ppcolor] ),
  numelts( source_elts.size()/4 ),
  elts( new GLubyte[numelts*4] ),

  tex( active_texture ),
  numtexcoords( source_texcoords.size()/2 ),
  texcoords( new GLfloat[numtexcoords*2] )
  
  //transform( source_transform )
{
  int oc = 0;
  for ( list<float>::iterator it = source_verts.begin();
      it != source_verts.end();
      it++, oc++ ) {
    verts[oc] = *it;
  }
  oc = 0;
  for ( list<float>::iterator it = source_colors.begin();
      it != source_colors.end();
      it++, oc++ ) {
    colors[oc] = *it;
  }
  oc = 0;
  for ( list<int>::iterator it = source_elts.begin();
      it != source_elts.end();
      it++, oc++ ) {
    elts[oc] = *it;
  }
  oc = 0;
  for ( list<float>::iterator it = source_texcoords.begin();
      it != source_texcoords.end();
      it++, oc++ ) {
    texcoords[oc] = *it;
  }
}

cshape::~cshape() {
  if( elts ) delete [] elts;
  if( verts ) delete [] verts;
  if( colors ) delete [] colors;
  if( texcoords ) delete [] texcoords;
}

void cshape::dump() {
  shape::dump();
  cout << "Mode: " << mode << ", quads/poly are " << GL_QUADS << "/" << GL_POLYGON << "\n";
  cout << "Verts(" << numverts << ")\n";
  for(int i = 0; i < numverts; i++ ) {
    cout << "(" << verts[3*i] << "," << verts[3*i+1] << "," << verts[3*i+2] << ")\n";
  }

  cout << "Elts(" << numelts << ")\n";
  for(int i = 0; i < numelts; i++ ) {
    cout << "(" << (int) elts[4*i] << "," << (int) elts[4*i+1] << "," << (int) elts[4*i+2] << "," << (int) elts[4*i+3] << ")\n";
  }
  cout << "Colors(" << numcolors << ")\n";
  for(int i = 0; i < numcolors; i++ ) {
    cout << "(" << colors[ppcolor*i] << "," << colors[ppcolor*i+1] << "," << colors[ppcolor*i+2] /*<< "," << colors[4*i+3] */<< ")\n";
  }
  cout << "Tex(" << (tex?tex->name:"None") << "," << numtexcoords << ")\n";
  for(int i = 0; i < numtexcoords; i++ ) {
    cout << "(" << texcoords[2*i] << "," << texcoords[2*i+1] << ")\n";
  }
}

void cshape::glinit(GLuint program) {
  shape::glinit(program);
  glGenBuffers( 4, &buffer[0] );
  glBindBuffer(GL_ARRAY_BUFFER, buffer[Vertices]) ; 
  glBufferData(GL_ARRAY_BUFFER, numverts*sizeof(GLfloat)*3, verts, GL_STATIC_DRAW);
  glVertexPointer(3, GL_FLOAT, 0, 0);// BUFFER_OFFSET(0)) ; 
  glEnableClientState(GL_VERTEX_ARRAY) ; 

  glBindBuffer(GL_ARRAY_BUFFER, buffer[Colors]) ; 
  glBufferData(GL_ARRAY_BUFFER, numcolors*sizeof(GLfloat)*ppcolor, colors,GL_STATIC_DRAW);
  glColorPointer(ppcolor, GL_FLOAT, 0, 0);// BUFFER_OFFSET(0)) ; 
  glEnableClientState(GL_COLOR_ARRAY) ;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[Elements]) ; 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numelts*sizeof(GLubyte)*4, elts, GL_STATIC_DRAW);


  if( tex ) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TexCoords]);//s[numobjects*numperobj+ncolors]) ; 
    glBufferData(GL_ARRAY_BUFFER, numtexcoords*sizeof(GLfloat)*2, texcoords, GL_STATIC_DRAW);
    glActiveTexture( GL_TEXTURE0 + tex->id );//GL_TEXTURE0) ; 
    glEnable(GL_TEXTURE_2D) ; 
    glTexCoordPointer(2,GL_FLOAT,0,0);//BUFFER_OFFSET(0)) ; 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY) ;
  } else {
    glDisable(GL_TEXTURE_2D);
  }

  //*/

  // Define a sampler.  See page 709 in red book, 7th ed.
  /*


  //GLint texsampler = glGetUniformLocation(program, "tex") ; 
  //glUniform1i(texsampler,0) ; // Could also be GL_TEXTURE0 
  //*/
  /*
  glActiveTexture(GL_TEXTURE0) ; 
  glEnable(GL_TEXTURE_2D) ; 
  glTexCoordPointer(2,GL_FLOAT,0,0);//BUFFER_OFFSET(0)) ; 
  glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 
  glGenTextures(1, &texName) ; 
  glBindTexture (GL_TEXTURE_2D, texName) ; 
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, xdim, ydim, 0, GL_RGB, GL_UNSIGNED_BYTE,
      texturedata) ;
  // TODO drive via flags
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  // Define a sampler.  See page 709 in red book, 7th ed.
  GLint texsampler = glGetUniformLocation(program, "tex") ; 
  glUniform1i(texsampler,0) ; // Could also be GL_TEXTURE0 
  */
}

void cshape::draw( ) {//shape *obj) {

  shape::draw();

  // load up our shape's vertices
  glBindBuffer( GL_ARRAY_BUFFER, buffer[Vertices] ) ;//s[Vertices+offset]) ; 
  glVertexPointer(3, GL_FLOAT, 0, 0) ; 
  glEnableClientState(GL_VERTEX_ARRAY) ; 

  // load up our shape's colors
  // Even with texturing, so we can blend if needed.
  glBindBuffer(GL_ARRAY_BUFFER, buffer[Colors]) ; 
  glColorPointer(ppcolor, GL_FLOAT, 0, 0 );//BUFFER_OFFSET(0)) ; 
  glEnableClientState(GL_COLOR_ARRAY) ; 

  if( tex && enable_tex ) {
    glActiveTexture(GL_TEXTURE0+tex->id) ; 
    glEnable(GL_TEXTURE_2D) ; 
  	GLuint texture_name = tex->texName;
	//  map<string,texture*>::iterator it = allTextures.find("wave"); //checks if its the painting, originally the Wave painting.
  	if (animate_tex==1 & texture_name == (*allTextures.find("wave")->second).texName) { //animation (slideshow style)
      if (frame<=1) {
        map<string,texture*>::iterator it = allTextures.find("fuji");
        texture_name=(*it->second).texName;
      } else if (frame<=2) {
        map<string,texture*>::iterator it = allTextures.find("wave");
        texture_name=(*it->second).texName;
      } else {
        map<string,texture*>::iterator it = allTextures.find("koi");
        texture_name=(*it->second).texName;
      }
    }

 	if (animate_tex==1 & texture_name == (*allTextures.find("sashimi")->second).texName) { //animation (slideshow style)
      if (frame<=1) {
        map<string,texture*>::iterator it = allTextures.find("udon");
        texture_name=(*it->second).texName;
      } else if (frame<=2) {
        map<string,texture*>::iterator it = allTextures.find("tempura");
        texture_name=(*it->second).texName;
      } else {
        map<string,texture*>::iterator it = allTextures.find("sashimi");
        texture_name=(*it->second).texName;
      }
    }
  	glBindTexture(GL_TEXTURE_2D, texture_name) ;
    glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 
    glBindBuffer(GL_ARRAY_BUFFER, buffer[TexCoords] );
    //texcoords) ; // Set texcoords
    glTexCoordPointer(2, GL_FLOAT, 0, 0 );
    // BUFFER_OFFSET(0)) ;
    glUniform1i( hastex, 1 );
    glUniform1i( texsampler, tex->id );
  } else {
    glUniform1i(hastex,0);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[Elements]) ;
  glDrawElements  //(PrimType[object], NumElems[object], GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)) ; 
    ( mode, sizeof(GLubyte)*4*numelts, GL_UNSIGNED_BYTE, 0 ); 
}






model::model(GLMmodel *m, const mat4& source_transform)
: shape(glmodel,1.0,source_transform),
  _model( m )
{
}


model::~model()
{
  if( _model ) {
    glmDelete(_model);
    _model = 0;
  }
}


void model::glinit(GLuint program)
{
  glmUnitize(_model);
  glmFacetNormals(_model);
  glmVertexNormals(_model,90);
}
/*
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *             GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */
void model::draw()
{
  //glUniform1i( enablelighting, 0 );//lightingFlags );
  /*
  glUniform4fv( ambientcol, 1, _model->ambient );
  glUniform4fv( diffusecol, 1, _model->diffuse );
  glUniform4fv( specularcol, 1, _model->specular );
  glUniform4fv( emissioncol, 1, _model->emission );
  glUniform1fv( shininesscol, 1, &_model->shininess );
  //*/
  glUniform1i( hastex, 0 );//<_model->numtextures );
  int flags = GLM_SMOOTH|GLM_MATERIAL;
  if( _model->numtextures & enable_tex );
    flags |= GLM_TEXTURE;
  glmDraw(_model, GLM_SMOOTH|GLM_MATERIAL);//GLM_SMOOTH| GLM_MATERIAL);   
  //glUniform1i( enablelighting, lightingFlags );
}
