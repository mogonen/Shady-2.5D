/********************************************************************

	Vector.C	Source File

	Vector Algebra Objects, Methods, and Procedures
		Donald H. House  April 17, 1997
		Visualization Laboratory
		Texas A&M University

*********************************************************************/

//#include "stdafx.h"
#include "Vector.h"

using namespace std;

Vec2::Vec2(double vx, double vy){
  set(vx, vy);
}

Vec2::Vec2(const Vec2 &v){
  set(v);
}

Vec2::Vec2(const Vec3 &v){
  set(v.x, v.y);
}


Vec3::Vec3(double vx, double vy, double vz){
  set(vx, vy, vz);
}

Vec3::Vec3(const Vec3 &v){
  set(v);
}

Vec3::Vec3(const Vec2 &v){
  set(v.x, v.y, 0);
}

Vec4::Vec4(double vx, double vy, double vz, double vw){
  set(vx, vy, vz, vw);
}

Vec4::Vec4(const Vec4 &v){
  set(v);
}

Vec4::Vec4(const Vec3 &v){
  set(v);
}

Vector::Vector(int vN, double *vx){
  int i;

  setsize(vN);
  if(vx != NULL)
    for(i = 0; i <_N; i++)
      v[i] = vx[i];
}

Vector::Vector(const Vector& V){
  set(V);
}

Vector::Vector(double vx, double vy){
  setsize(2);
  set(vx, vy);
}

Vector::Vector(double vx, double vy, double vz){
  setsize(3);
  set(vx, vy, vz);
}

Vector::Vector(double vx, double vy, double vz, double vw){
  setsize(4);
  set(vx, vy, vz, vw);
}

// Destructor
Vector::~Vector(){
  delete []v;
}

//
// Array index form of a vector
// Routines returning an lvalue: i.e. X[i] returns addr of X[i]
//
double& Vec2::operator[](int i)
{
  if(i < 0 || i > 1){
    cerr << "2D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  default:
    return y;
  }
}

double& Vec3::operator[](int i)
{
  if(i < 0 || i > 2){
    cerr << "3D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  case 1:
    return y;
  default:
    return z;
  }
}

double& Vec4::operator[](int i)
{
  if(i < 0 || i > 3){
    cerr << "4D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  case 1:
    return y;
  case 2:
    return z;
  default:
    return w;
  }
}

double& Vector::operator[](int i)
{
  if(i < 0 || i >= _N){
    cerr << _N << " vector index bounds error" << endl;
    exit(1);
  }

  return v[i];
}

//
// Array index form of a vector
// Routines returning an rvalue: i.e. X[i] returns contents of X[i]
//
const double& Vec2::operator[](int i) const
{
  if(i < 0 || i > 1){
    cerr << "2D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  default:
    return y;
  }
}

const double& Vec3::operator[](int i) const
{
  if(i < 0 || i > 2){
    cerr << "3D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  case 1:
    return y;
  default:
    return z;
  }
}

const double& Vec4::operator[](int i) const
{
  if(i < 0 || i > 3){
    cerr << "4D vector index bounds error" << endl;
    exit(1);
  }

  switch(i){
  case 0:
    return x;
  case 1:
    return y;
  case 2:
    return z;
  default:
    return w;
  }
}

const double& Vector::operator[](int i) const
{
  if(i < 0 || i >= _N){
    cerr << _N << " vector index bounds error" << endl;
    exit(1);
  }

  return v[i];
}

// Set size of generic Vector
void Vector::setsize(int vN){
  if(vN < 0){
    cerr << "vector dimension of " << vN << " invalid" << endl;
    exit(1);    
  }

  _N = vN;
  if(_N == 0)
    v = NULL;
  else{
    v = new double[_N];
    if(!v){
      cerr << "not enough memory to allocate vector of size " << _N << endl;
      exit(1);    
    }
  }

  int i;
  for(i = 0; i <_N; i++)
    v[i] = 0;
}

// Projections of vectors into other dimensions
Vec2::operator Vec3(){
  Vec3 v1(x, y, 0);
  return v1;
}

Vec2::operator Vec4(){
  Vec4 v1(x, y, 0, 0);
  return v1;
}

Vec2::operator Vector(){
  Vector v1(x, y);
  return v1;
}

Vec3::operator Vec4(){

  Vec4 v1(x, y, z, 0);
  return v1;
}

Vec3::operator Vector(){
  Vector v1(x, y, z);
  return v1;
}

Vec4::operator Vector(){
  Vector v1(x, y, z, w);
  return v1;
}

Vector::operator Vec2(){
  if(_N > 2){
    cerr << "cannot convert " << _N << " Vector to Vec2" << endl;
    exit(1);    
  }

  Vec2 rv;
  int i;
  for(i = 0; i <_N; i++)
    rv[i] = v[i];
  for(; i < 2; i++)
    rv[i] = 0;
  return rv;
}

Vector::operator Vec3(){
  if(_N > 3){
    cerr << "cannot convert " << _N << " Vector to Vec3" << endl;
    exit(1);    
  }

  Vec3 rv;
  int i;
  for(i = 0; i <_N; i++)
    rv[i] = v[i];
  for(; i < 3; i++)
    rv[i] = 0;
  return rv;
}

Vector::operator Vec4(){
  if(_N > 4){
    cerr << "cannot convert " << _N << " Vector to Vec4" << endl;
    exit(1);    
  }

  Vec4 rv;
  int i;
  for(i = 0; i <_N; i++)
    rv[i] = v[i];
  for(; i < 4; i++)
    rv[i] = 0;
  return rv;
}

int Vector::N() const{
  return _N;
}

// Compute the norm of a vector.
Vec2 Vec2::normalize() const
{
  double magnitude;
  Vec2 newv;
  magnitude = norm();
  if (abs(x) > magnitude * HUGENUMBER ||
      abs(y) > magnitude * HUGENUMBER ){
    cerr << "Attempting to take the norm of a zero 2D vector." << endl;
  }
  newv.x = x / magnitude;
  newv.y = y / magnitude;
  return newv;
}
Vec3 Vec3::normalize() const
{
  double magnitude;
  Vec3 newv;
  magnitude = norm();
  if (abs(x) > magnitude * HUGENUMBER ||
      abs(y) > magnitude * HUGENUMBER ||
      abs(z) > magnitude * HUGENUMBER ){
    cerr << "Attempting to take the norm of a zero 3D vector." << endl;
  }
  newv.x = x / magnitude;
  newv.y = y / magnitude;
  newv.z = z / magnitude;
  return newv;
}
Vec4 Vec4::normalize() const
{
  double magnitude;
  Vec4 newv;
  magnitude = norm();
  if (abs(x) > magnitude * HUGENUMBER ||
      abs(y) > magnitude * HUGENUMBER ||
      abs(z) > magnitude * HUGENUMBER ){
    cerr << "taking the norm of a zero 4D vector." << endl;
  }
  newv.x = x / magnitude;
  newv.y = y / magnitude;
  newv.z = z / magnitude;
  newv.w = 1.0;
  return newv;
}
Vector Vector::normalize() const
{
  double magnitude;
  Vector newv(_N);
  magnitude = norm();
  int i;

  for(i = 0; i <_N; i++){
    newv.v[i] = v[i] / magnitude;
    if(abs(v[i]) > magnitude * HUGENUMBER){
      cerr << "taking the norm of a zero" << _N << " Vector" << endl;
      break;
    }
  }
  for(; i <_N; i++){
    newv.v[i] = v[i] / magnitude;
  }
  return newv;
}

//  Normalize a 4D Vector's x, y, z components by the w component, and
//  set the w component to 1.0
Vec4 Vec4::wnorm() const
{
  Vec4 newv;
  if (w == 1.0)
    return *this;
  else if (fabs(w) < SMALLNUMBER)
    cerr << "w-Normalizing vector [" <<
      x << " " << y << " " << z<< " " << w << "] w ~= 0!" << endl;

  newv.x = x / w;
  newv.y = y / w;
  newv.z = z / w;
  newv.w = 1.0;
  return newv;
}

//  Set the components of a Vector according to the arguments
void Vec2::set(double vx, double vy)
{
  x = vx;
  y = vy;
}
void Vec2::set(const Vec2 &v)
{
  x = v.x;
  y = v.y;
}
void Vec3::set(double vx, double vy, double vz)
{
  x = vx;
  y = vy;
  z = vz;
}
void Vec3::set(const Vec3 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
}
void Vec4::set(double vx, double vy, double vz, double vw)
{
  x = vx;
  y = vy;
  z = vz;
  w = vw;
}
void Vec4::set(const Vec4 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = v.w;
}
void Vec4::set(const Vec3 &v)
{
  x = v.x;
  y = v.y;
  z = v.z;
  w = 1.0;
}
void Vector::set(double *vx){
  int i;
  for(i = 0; i <_N; i++)
    v[i] = vx[i];
}
void Vector::set(const Vector &V){
  int i;

  setsize(V._N);
  for(i = 0; i <_N; i++)
    v[i] = V.v[i];
}
void Vector::set(double vx, double vy){
  if(_N < 2){
    cerr << "too many arguments for " << _N << " Vector set" << endl;
    exit(1);
  }

  v[0] = vx;
  v[1] = vy;
  int i;
  for(i = 2; i <_N; i++)
    v[i] = 0;
}
void Vector::set(double vx, double vy, double vz){
  if(_N < 3){
    cerr << "too many arguments for " << _N << " Vector set" << endl;
    exit(1);
  }

  v[0] = vx;
  v[1] = vy;
  v[2] = vz;
  int i;
  for(i = 3; i <_N; i++)
    v[i] = 0;
}
void Vector::set(double vx, double vy, double vz, double vw){
  if(_N < 4){
    cerr << "too many arguments for " << _N << " Vector set" << endl;
    exit(1);
  }

  v[0] = vx;
  v[1] = vy;
  v[2] = vz;
  v[3] = vw;
  int i;
  for(i = 4; i <_N; i++)
    v[i] = 0;
} 

// Print a Vector to the standard output device.
void Vec2::print() const
{
  cout << "[" << x << " " << y << "]";
}
void Vec2::print(int w, int p) const
{
  cout << "[" << setw(w) << setprecision(p) << Round(x, p) << " ";
  cout << setw(w) << setprecision(p) << Round(y, p) << "]";
}
void Vec3::print() const
{
  cout << "[" << x << " " << y << " " << z << "]";
}
void Vec3::print(int w, int p) const
{
  cout << "[" << setw(w) << setprecision(p) << Round(x, p) << " ";
  cout << setw(w) << setprecision(p) << Round(y, p) << " ";
  cout << setw(w) << setprecision(p) << Round(z, p) << "]";
}
void Vec4::print() const
{
  cout << "[" << x << " " << y << " " << z << " " << w << "]";
}
void Vec4::print(int w, int p) const
{
  cout << "[" << setw(w) << setprecision(p) << Round(x, p) << " ";
  cout << setw(w) << setprecision(p) << Round(y, p) << " ";
  cout << setw(w) << setprecision(p) << Round(z, p) << " ";
  cout << setw(w) << setprecision(p) << Round(w, p) << "]";
}
void Vector::print() const
{
  if(_N == 0)
    cout << "[]";
  else{
    int i;
    cout << "[" << v[0];
    for(i = 1; i <_N; i++)
      cout << " " << v[i];
    cout << "]";
  }
}
void Vector::print(int w, int p) const
{
  if(_N == 0)
    cout << "[]";
  else{
    int i;
    cout << "[" << setw(w) << setprecision(p) << Round(v[0], p);
    for(i = 1; i <_N; i++)
      cout << " " << setw(w) << setprecision(p) << Round(v[i], p);
    cout << "]";
  }
}

// Compute the magnitude of a vector.
double Vec2::norm() const
{
  return sqrt(normsqr());
}
double Vec3::norm() const
{
  return sqrt(normsqr());
}
double Vec4::norm() const
{
  return sqrt(normsqr());
}
double Vector::norm() const
{
  return sqrt(normsqr());
}

// Compute the squared magnitude of a vector.
double Vec2::normsqr() const
{
  return Sqr(x) + Sqr(y);
}
double Vec3::normsqr() const
{
  return Sqr(x) + Sqr(y) + Sqr(z);
}
double Vec4::normsqr() const
{
  return Sqr(x) + Sqr(y) + Sqr(z) + Sqr(w);
}
double Vector::normsqr() const
{
  int i;
  double sumsqr = 0;
  for(i = 0; i <_N; i++)
    sumsqr += Sqr(v[i]);

  return sumsqr;
}

// Unary negation of a vector
Vec2 operator-(const Vec2& v1){
  Vec2 r(-v1.x, -v1.y);
  return(r);
}
Vec3 operator-(const Vec3& v1){
  Vec3 r(-v1.x, -v1.y, -v1.z);
  return(r);
}
Vec4 operator-(const Vec4& v1){
  Vec4 r(-v1.x, -v1.y, -v1.z, -v1.w);
  return(r);
}
Vector operator-(const Vector& v1){
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = v1.v[i];
  return(r);
}

//  Addition of 2 Vectors.
Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
  Vec2 r;
  r.x = v1.x + v2.x;
  r.y = v1.y + v2.y;
  return r;
}

Vec3 Vec3::operator+(const Vec3& v2) const
{
  Vec3 r;
  r.x = x + v2.x;
  r.y = y + v2.y;
  r.z = z + v2.z;
  return r;
}

Vec4 Vec4::operator+(const Vec4& v2) const
{
  Vec4 r;
  r.x = x + v2.x;
  r.y = y + v2.y;
  r.z = z + v2.z;
  r.w = w + v2.w;
  return r;
}

Vector operator+(const Vector& v1, const Vector& v2)
{
  if(v1._N != v2._N){
    cerr << "cannot add " << v1._N << " Vector to " << v2._N << " Vector" << 
      endl;
    exit(1);
  }
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = v1.v[i] + v2.v[i];
  return r;
}

//  Subtract two Vectors.
Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
  Vec2 r;
  r.x = v1.x - v2.x;
  r.y = v1.y - v2.y;
  return r;
}
Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
  Vec3 r;
  r.x = v1.x - v2.x;
  r.y = v1.y - v2.y;
  r.z = v1.z - v2.z;
  return r;
}
Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
  Vec4 r;
  r.x = v1.x - v2.x;
  r.y = v1.y - v2.y;
  r.z = v1.z - v2.z;
  r.w = v1.w - v2.w;
  return r;
}
Vector operator-(const Vector& v1, const Vector& v2)
{
  if(v1._N != v2._N){
    cerr << "cannot subtract " << v2._N << " Vector from " << v1._N <<
      " Vector" << endl;
    exit(1);
  }

  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = v1.v[i] - v2.v[i];
  return(r);
}

// Product of vector and scalar
Vec2 operator*(const Vec2& v, double s)
{
  Vec2 r;

  r.x = v.x * s;
  r.y = v.y * s;
  return r;
}
Vec2 operator*(double s, const Vec2& v)
{
  Vec2 r;

  r.x = v.x * s;
  r.y = v.y * s;
  return r;
}
Vec3 operator*(const Vec3& v, double s)
{
  Vec3 r;

  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  return r;
}
Vec3 operator*(double s, const Vec3& v)
{
  Vec3 r;

  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  return r;
}
Vec4 operator*(const Vec4& v, double s)
{
  Vec4 r;

  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  r.w = v.w * s;
  return r;
}
Vec4 operator*(double s, const Vec4& v)
{
  Vec4 r;

  r.x = v.x * s;
  r.y = v.y * s;
  r.z = v.z * s;
  r.w = v.w * s;
  return r;
}
Vector operator*(double s, const Vector& v1)
{
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = s * v1.v[i];
  return r;
}
Vector operator*(const Vector& v1, double s)
{
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = s * v1.v[i];
  return r;
}

// Inner product of two Vectors
double operator*(const Vec2& v1, const Vec2& v2)
{
  return(v1.x * v2.x +
	 v1.y * v2.y);
}
double operator*(const Vec3& v1, const Vec3& v2)
{
  return(v1.x * v2.x +
	 v1.y * v2.y +
	 v1.z * v2.z);
}
double operator*(const Vec4& v1, const Vec4& v2)
{
  return(v1.x * v2.x +
	 v1.y * v2.y +
	 v1.z * v2.z +
	 v1.w * v2.w);
}
double operator*(const Vector& v1, const Vector& v2)
{
  if(v1._N != v2._N){
    cerr << "cannot take dot product of " << v1._N << " Vector with " << 
      v2._N << " Vector" << endl;
    exit(1);
  }

  int i;
  double p = 0;

  for(i = 0; i < v1._N; i++)
    p += v1.v[i] * v2.v[i];
  return p;
}
//  Component-wise multiplication of two Vectors
Vec2 operator^(const Vec2& v1, const Vec2& v2)
{
  Vec2 r;
  r.x = v1.x * v2.x;
  r.y = v1.y * v2.y;
  return r;
}
Vec3 operator^(const Vec3& v1, const Vec3& v2)
{
  Vec3 r;
  r.x = v1.x * v2.x;
  r.y = v1.y * v2.y;
  r.z = v1.z * v2.z;
  return r;
}
Vec4 operator^(const Vec4& v1, const Vec4& v2)
{
  Vec4 r;
  r.x = v1.x * v2.x;
  r.y = v1.y * v2.y;
  r.z = v1.z * v2.z;
  r.w = v1.w * v2.w;
  return r;
}
Vector operator^(const Vector& v1, const Vector& v2)
{
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = v1.v[i] * v2.v[i];
  return r;
}

// Cross product of two Vectors
Vec3 operator%(const Vec2& v1, const Vec2& v2)
{
  Vec3 cp;
  cp.x = 0;
  cp.y = 0;
  cp.z = v1.x * v2.y - v1.y * v2.x;
  return (cp);
}
Vec3 operator%(const Vec3& v1, const Vec3& v2)
{
  Vec3 cp;
  cp.x = v1.y * v2.z - v1.z * v2.y;
  cp.y = v1.z * v2.x - v1.x * v2.z;
  cp.z = v1.x * v2.y - v1.y * v2.x;
  return (cp);
}
Vec4 operator%(const Vec4& v1, const Vec4& v2)
{
  cerr << "sorry, cross product of Vec4's not yet implemented" << endl;
  exit(1);

  Vec4 cp = v1;	// here only to avoid warning message from compiler
  cp = v2;
  return (cp);
}
Vector operator%(const Vector& v1, const Vector& v2)
{
  if(v1._N != v2._N || v1._N < 3){
    cerr << "cannot take cross product of " << v1._N << " Vector with " <<
      v2._N << " Vector" << endl;
    exit(1);
  }

  cerr << "sorry, (Vector % Vector) not implemented yet" << endl;
  exit(1);
  return v1;
}

// Divide a vector by a scalar.
Vec2 operator/(const Vec2& v, double s)
{
  Vec2 r;
  r.x = v.x / s;
  r.y = v.y / s;
  return(r);
}
Vec3 operator/(const Vec3& v, double s)
{
  Vec3 r;
  r.x = v.x / s;
  r.y = v.y / s;
  r.z = v.z / s;
  return(r);
}
Vec4 operator/(const Vec4& v, double s)
{
  Vec4 r;
  r.x = v.x / s;
  r.y = v.y / s;
  r.z = v.z / s;
  r.w = 1.0;
  return(r);
}
Vector operator/(const Vector& v1, double s)
{
  Vector r(v1._N);
  int i;

  for(i = 0; i < v1._N; i++)
    r[i] = v1.v[i] / s;
  return r;
}

// Determine if two Vectors are identical.
short operator==(const Vec2& one, const Vec2& two)
{
  return((one.x == two.x) && (one.y == two.y));
}
short operator==(const Vec3& one, const Vec3& two)
{
  return((one.x == two.x) && (one.y == two.y) && (one.z == two.z));
}
short operator==(const Vec4& one, const Vec4& two)
{
  return((one.x == two.x) && (one.y == two.y) && (one.z == two.z));
}
short operator==(const Vector& one, const Vector& two)
{
  int n,_N;
  double *v, *V;

  if(one._N >= two._N){
    _N = one._N;
    n = two._N;
    V = one.v;
    v = two.v;
  }
  else{
    _N = two._N;
    n = one._N;
    V = two.v;
    v = one.v;
  }

  int i;
  for(i = 0; i < n; i++)
    if(V[i] != v[i])
      return 0;
  for(;i <_N; i++)
    if(V[i] != 0)
      return 0;
    
  return 1;
}

// assign one vector to another
const Vector& Vector::operator=(const Vector& v2){
  int i;

  if(_N != v2._N){
    delete []v;
    setsize(v2._N);
  }
  for(i = 0; i <_N; i++)
    v[i] = v2.v[i];

  return *this;
}

const Vec2& getIntersection(Vec2 p0, Vec2 n0 , Vec2 p1, Vec2 n1){
		Vec2 p1mp0 = p1 - p0;		
		double tmin = ( (n0*n1) * (n0*p1mp0) - n1*p1mp0 ) / (1 - ((n0*n1) * (n0*n1)) );
		Vec2 cp1 =n1*tmin + p1;
		Vec2 cp0 = n0*(n0*(cp1- p0))+p0;	
		return ((cp0+cp1)*0.5);
}

const Vec3& getIntersection(Vec3 p0, Vec3 n0 , Vec3 p1, Vec3 n1){
		Vec3 p1mp0 = p1 - p0;		
		double tmin = ( (n0*n1) * (n0*p1mp0) - n1*p1mp0 ) / (1 - ((n0*n1) * (n0*n1)) );
		Vec3 cp1 =n1*tmin + p1;
		Vec3 cp0 = n0*(n0*(cp1- p0))+p0;	
		return ((cp0+cp1)*0.5);
}

double getIntersectionDist(Vec2 p1, Vec2 n1, Vec2 p0, Vec2 n0 ){		
		Vec2 p1mp0 = p1 - p0;
		return ( (n0*n1)*(n0*p1mp0) - (n1*p1mp0) ) / (1 - ((n0*n1)*(n0*n1)));	
}

double getIntersectionDist(Vec3 p1, Vec3 n1, Vec3 p0, Vec3 n0 ){		
		Vec3 p1mp0 = p1 - p0;
		return ( (n0*n1)*(n0*p1mp0) - (n1*p1mp0) ) / (1 - ((n0*n1)*(n0*n1)));	
}
