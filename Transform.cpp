// Transform.cpp: implementation of the Transform class.


#include "Transform.h"
// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
  // YOUR CODE FOR HW1 HERE
  // Please implement this.
  /*
    [ cos(a) + x^2(1-cos(a))     x*y(1-cos(a)) - z*sin(a)  x*z(1-cos(a)) + y*sin(a)  ]
R = [ y*x(1-cos(a)) + z*sin(a)   cos(a) + y^2(1-cos(a)     y*z(1-cos(a)) - x*sin(a)  ]
    [ z*x(1-cos(a)) - y*sin(a)   z*y(1-cos(a)) + x*sin(a)  cos(a) + z^2*(1-cos*a)    ]
   */ 

  float radians = glm::radians(degrees);//pi * degrees/180.0;
  float cosa = cos(radians),
        sina = sin(radians),
        cosaf1 = 1.0 - cosa;
  const vec3 taxis = glm::normalize(axis);
  float x = taxis.x, y = taxis.y, z = taxis.z;

  mat3 result(
    vec3( cosa + x*x*cosaf1,   x*y*cosaf1 - z*sina, x*z*cosaf1 + y*sina ),
    vec3( y*x*cosaf1 + z*sina, cosa + y*y*cosaf1,   y*z*cosaf1 - x*sina ),
    vec3( z*x*cosaf1 - y*sina, z*y*cosaf1 + x*sina, cosa + z*z*cosaf1   )
  );

  return result;
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW1 HERE
  mat3 R = rotate( degrees, up );
  eye = eye * R;
  up = up * R;
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW1 HERE
  vec3 orth = glm::cross(eye,up);
  mat3 R = rotate( degrees, orth );
  eye = eye * R;
  up = up * R;
}

// Your implementation of the glm::lookAt matrix
mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  // YOUR CODE FOR HW1 HERE
  vec3 f = glm::normalize( center-eye );
  vec3 u = glm::normalize( up );
  vec3 s = glm::normalize( glm::cross(f,u) );
  u = glm::cross(s,f);

  vec3 tl8( glm::dot(-eye,s), glm::dot(-eye,u), glm::dot(eye,f) );

  mat4 r(
    s.x,   s.y,  s.z, tl8.x,
    u.x,   u.y,  u.z, tl8.y,
    -f.x, -f.y, -f.z, tl8.z,
    0, 0, 0, 1
  );
  return r;
  //return glm::transpose( glm::lookAt(eye,vec3(0.0,0.0,0.0),up) );
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  // YOUR CODE FOR HW2 HERE
  // New, to implement the perspective transform as well.  
  float theta = fovy/2;
  float d = 1/tan(glm::radians(theta));
  float div =-( zFar-zNear );
  if( 0 == div ) {
    // divide by zero error
  }

  float a = (zFar+zNear) / div;
  float b = (2*zFar*zNear) / div;

  return mat4(
      d/aspect, 0, 0, 0,
      0, d, 0, 0,
      0, 0, a, b,
      0, 0, -1, 0
  );
}
//is both row & column major
mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
  // YOUR CODE FOR HW2 HERE
  // Implement scaling 

  mat4 s(
    sx, 0, 0, 0,
    0, sy, 0, 0,
    0, 0, sz, 0,
    0, 0, 0, 1
  );
  
  return s;
}

//column major
mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
  // YOUR CODE FOR HW2 HERE
  // Implement translation
  mat4 t(
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      tx, ty, tz, 1
  );
  return t;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) {
  vec3 x = glm::cross(up,zvec) ; 
  vec3 y = glm::cross(zvec,x) ; 
  vec3 ret = glm::normalize(y) ; 
  return ret ; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
