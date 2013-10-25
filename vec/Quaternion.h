/********************************************************************

	Quaternion.h	Header  File

	Quaternion Object, Methods, and Procedures
	Donald H. House  November 9, 2000
		Visualization Laboratory
		Texas A&M University

	Q to M and M to Q code from Baraff & Witkin 
        SIGGRAPH 97 Course Notes, Course 19, Physically Based Modeling

*********************************************************************/

#ifndef _QUATERNION_H_
#define _QUATERNION_H_
#include "Matrix.h"

struct Quaternion{
public:
  Vec4 q;		// quaternion is stored as a 4D vector, q, where:
  			// (x, y, z) is vector part and w is scalar part

  Quaternion(const Quaternion &quat);	// copy constructor
  Quaternion(const Vec3 &vect);	// copy, setting scalar part = 0

  // the following constructors assume a rotation quaternion, where angle
  // will be the rotation angle and the vector is the rotation axis
  // scalar part will be sin(angle/2), vector part will be cos(angle/2)u,
  // where u is the unit vector in the direction of the vector parameter
  Quaternion(double yaw = 0, double pitch = 0, double roll = 0);
  Quaternion(double angle, double x, double y, double z);
  Quaternion(double angle, const Vec3 &axis);
  Quaternion(const Matrix3x3 &rot);
  Quaternion(const Matrix4x4 &rot);

  ~Quaternion();	// quaternion destructor

  operator struct Vec3();

  void print();
  void print(int w, int p);	// print with width and precision

  Quaternion normalize();

  void identity();	// set quaternion to the identity quaternion

  void set(const Quaternion &quat);	// set quaternion to another
  void set(const Vec3 &vect);	// set scalar part to 0
  // the following set quaternion to a rotation quaternion. The matrices
  // must be valid rotations for this to work correctly
  void set(double yaw = 0, double pitch = 0, double roll = 0);
  void set(double angle, double x, double y, double z);
  void set(double angle, const Vec3 & axis);
  void set(const Matrix3x3 &rot);
  void set(const Matrix4x4 &rot);
  void set(float ang);

  void rotate(const Quaternion &rotq); // rotate quaternion by another
  void rotate(double yaw, double pitch, double roll);
  void rotate(double angle, double x, double y, double z);
  void rotate(double angle, const Vec3 &axis);
  void rotate(const Matrix3x3 &rot);
  void rotate(const Matrix4x4 &rot);
  void rotate(double ang);

  Matrix4x4 rotation();		// return rotation matrix for quaternion
  Matrix3x3 matrix3x3();

  double angle();		// rotation angle, assuming rotation quaternion
  Vec3 axis();		// rotation axis, assuming rotation quaternion

  float *GLrotation();		// return rotation matrix for quaternion,
				// stored as 16 floats needed by OpenGL
  void GLrotate();		// do OpenGL rotation according to quaternion

  double norm();
  double normsqr();
  Quaternion inv();

  /* Quaternion operator prototypes */
  friend Quaternion operator-(const Quaternion &q);	// unary negation
  friend Quaternion operator+(const Quaternion &q1, const Quaternion &q2); // +
  friend Quaternion operator-(const Quaternion &q1, const Quaternion &q2); // -
  friend Quaternion operator*(const Quaternion &q, double s); // scalar * q
  friend Quaternion operator*(double s, const Quaternion &q); // q * scalar
  friend Quaternion operator*(const Quaternion &q1, const Quaternion &q2); // *
  friend Quaternion operator*(const Vec3 &v, const Quaternion &q); // vect * q
  friend Quaternion operator*(const Quaternion &q, const Vec3 &v); // q * vect
  friend Quaternion operator/(const Quaternion &q, double s); // q / scalar
  friend short operator==(const Quaternion &q1, const Quaternion &q2); // ==
  const Quaternion& operator=(const Quaternion& qright);  // assignment
};

#endif
