/********************************************************************

	Vector.H	Header File

	Vector Algebra Objects, Methods, and Procedures
		Donald H. House  April 17, 1997
		Visualization Laboratory
		Texas A&M University

*********************************************************************/

#ifndef _H_Vector
#define _H_Vector

#include "Utility.h"

/* Vector Descriptions and Operations */

class Vec2;
class Vec3;
class Vec4;
class Vector;

typedef Vec2* Vec2Ptr;
typedef Vec3* Vec3Ptr;
typedef Vec4* Vec4Ptr;

typedef Vec4 RGBA;
typedef Vec3 RGB;
typedef Vec3 HSV;

class Vec2 {
public:
  double x, y;

  Vec2(double vx = 0, double vy = 0);
  Vec2(const Vec2 &v);
  Vec2(const Vec3 &v);

  double& operator[](int i);
  const double& operator[](int i) const;

  operator Vec3();
  operator Vec4();
  operator Vector();

  void print() const;
  void print(int w, int p) const;	// print with width and precision

  double norm() const;			// magnitude of vector
  double normsqr() const;		// magnitude squared
  Vec2 normalize() const;		// normalize

  void set(double vx = 0, double vy = 0);	// set assuming y = 0
  void set(const Vec2 &v);
  void reset(){set();}

  /* Vec2 operator prototypes */
  friend Vec2 operator-(const Vec2& v1); // unary negation of vector
  friend Vec2 operator+(const Vec2& v1, const Vec2& v2);//addition 
  friend Vec2 operator-(const Vec2& v1, const Vec2& v2);//subtract
  friend Vec2 operator*(const Vec2& v, double s); // scalar mult
  friend Vec2 operator*(double s, const Vec2& v);
  friend double   operator*(const Vec2& v1, const Vec2& v2); // dot
  friend Vec2 operator^(const Vec2& v1, const Vec2& v2); //compt *
  friend Vec3 operator%(const Vec2& v1, const Vec2& v2); // cross
  friend Vec2 operator/(const Vec2& v, double s); // division by scalar
  friend short    operator==(const Vec2& one, const Vec2& two); // eq
};

class Vec3 {
public:
  double x, y, z;

  Vec3(double vx = 0, double vy = 0, double vz = 0);
  Vec3(const Vec3 &v);
  Vec3(const Vec2 &v);

  double& operator[](int i);
  const double& operator[](int i) const;

  operator Vec4();
  operator Vector();

  void print() const;
  void print(int w, int p) const;	// print with width and precision

  double norm() const;			// magnitude of vector
  double normsqr() const;		// magnitude squared
  Vec3 normalize() const;		// normalize

  void set(double vx = 0, double vy = 0, double vz = 0); // set
  void set(const Vec3 &v);
  void reset(){set();}

  /* Vec3 operator prototypes */
  friend Vec3 operator-(const Vec3& v1);	// unary negation
  Vec3 operator+(const Vec3& v2) const; // vector addition 
  friend Vec3 operator-(const Vec3& v1, const Vec3& v2); // subtract
  friend Vec3 operator*(const Vec3& v, double s);      // multiply
  friend Vec3 operator*(double s, const Vec3& v);
  friend double   operator*(const Vec3& v1, const Vec3& v2); // dot
  friend Vec3 operator^(const Vec3& v1, const Vec3& v2); // compt *
  friend Vec3 operator%(const Vec3& v1, const Vec3& v2); // cross
  friend Vec3 operator/(const Vec3& v, double s); // division by scalar
  friend short    operator==(const Vec3& one, const Vec3& two); // equ
};

class Vec4 {
public:
  double x, y, z, w;

  Vec4(double vx = 0, double vy = 0, double vz = 0, double vw = 0);
   Vec4(const Vec3 &v);
  Vec4(const Vec4 &v);

  double& operator[](int i);
  const double& operator[](int i) const;
  
  operator Vector();

  void print() const;
  void print(int w, int p) const;	// print with width and precision

  double norm() const;			// magnitude of vector
  double normsqr() const;		// magnitude squared
  Vec4 normalize() const;		// normalize
  Vec4 wnorm() const;		// normalize to w coord.

  void set(double vx = 0, double vy = 0, double vz = 0, double vw = 1.0); // set
  void set(const Vec4 &v);
  void set(const Vec3 &v);
  void reset(){set();};

  /* Vec4 operator prototypes */
  friend Vec4 operator-(const Vec4& v1);  // unary negation
  Vec4 operator+(const Vec4& v2) const; // vector addition 
  friend Vec4 operator-(const Vec4& v1, const Vec4& v2); //subtract
  friend Vec4 operator*(const Vec4& v, double s);	// multiply
  friend Vec4 operator*(double s, const Vec4& v);
  friend double   operator*(const Vec4& v1, const Vec4& v2); // dot
  friend Vec4 operator^(const Vec4& v1, const Vec4& v2); // compt *
  friend Vec4 operator%(const Vec4& v1, const Vec4& v2); // cross
  friend Vec4 operator/(const Vec4& v, double s); // divide by scalar
  friend short    operator==(const Vec4& one, const Vec4& two); // equ
};

class Vector {
protected:
  int _N;
  double *v;

public:
  Vector(int vN = 0, double *vx = NULL);
  Vector(const Vector& V);
  Vector(double vx, double vy);
  Vector(double vx, double vy, double vz);
  Vector(double vx, double vy, double vz, double vw);

  ~Vector();

  void setsize(int vN);

  double& operator[](int i);
  const double& operator[](int i) const;  

  operator Vec2();
  operator Vec3();
  operator Vec4();

  int N() const;

  void print() const;
  void print(int w, int p) const;	// print with width and precision

  double norm() const;			// magnitude of vector
  double normsqr() const;		// magnitude squared
  Vector normalize() const;		// normalize

  void set(double *vx); 		// set
  void set(const Vector &v);
  void set(double vx, double vy);
  void set(double vx, double vy, double vz);
  void set(double vx, double vy, double vz, double vw);  

  /* Vector operator prototypes */
  const Vector& operator=(const Vector& v2);		// assignment
  friend Vector operator-(const Vector& v1);		// unary negation
  friend Vector operator+(const Vector& v1, const Vector& v2); // vector add
  friend Vector operator-(const Vector& v1, const Vector& v2); // vector sub
  friend Vector operator*(const Vector& v, double s);       // scalar multiply
  friend Vector operator*(double s, const Vector& v);
  friend Vector operator^(const Vector& v1, const Vector& v2); // component *
  friend double operator*(const Vector& v1, const Vector& v2); // dot product 
  friend Vector operator%(const Vector& v1, const Vector& v2); // cross product
  friend Vector operator/(const Vector& v, double s);	  // division by scalar
  friend short  operator==(const Vector& one, const Vector& two); // equality
};

const Vec2& getIntersection(Vec2 p0, Vec2 n0, Vec2 p1, Vec2 n1);
const Vec3& getIntersection(Vec3 p0, Vec3 n0, Vec3 p1, Vec3 n1);

double getIntersectionDist(Vec2 p1, Vec2 n1, Vec2 p0, Vec2 n0 );
double getIntersectionDist(Vec3 p1, Vec3 n1, Vec3 p0, Vec3 n0 );

inline static Vec3 mix(const Vec3& v0, const Vec3& v1, double t = 0.5){return v0*t + v1*(1-t);};
inline static Vec4 mix(const Vec4& v0, const Vec4& v1, double t = 0.5){return v0*t + v1*(1-t);};

#endif


