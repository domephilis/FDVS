#include <math.h>

float f(double a, double b);

// float f(double x, double y)
// {
// 
//   double dx = x - 50.0;
//   double dy = y - 50.0;
//   double r = sqrt(dx*dx + dy*dy);
// 
//   return (float)(100.0
//        * cos(r / 4.0)
//        * exp(-r*r / 1800.0));
// 
// }
//
// float f(double x, double y)
// {
// 
//   double dx = x - 50.0;
//   double dy = y - 50.0;
// 
//   return (float)(100.0
//        * exp(-(dx*dx + dy*dy) / 120.0));
// 
// }
//
// float f(double x, double y)
// {
// 
//   double dx = x - 50.0;
//   double dy = y - 50.0;
// 
//   double sigma = 18.0;
// 
//   return -(float)(100.0
//        * exp(-(dx*dx + dy*dy)
//              / (2.0 * sigma * sigma)));
// 
// }
float f(double x, double y)
{

  double dx = x - 50.0;
  double dy = y - 50.0;
  double r = sqrt(dx*dx + dy*dy);

  return (float)(50.0
       + 50.0
       * cos(r / 4.0)
       * exp(-r*r / 1800.0));

}
