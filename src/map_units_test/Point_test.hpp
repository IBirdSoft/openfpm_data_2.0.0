#include <iostream.h>

#pragma openfpm struct(align : 0, padding : 0 , order : none, options : none)
template<typename T> class Point_orig
{
public:

  T x;
  T y;
  T z;

  T s;

  T v[3];
  T t[3][3];

  inline void setx(T x_)	{x = x_;};
  inline void sety(T y_)	{y = y_;};
  inline void setz(T z_)	{z = z_;};
};
