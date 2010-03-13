#ifndef DOLLAR_H_INCLUDED
#define DOLLAR_H_INCLUDED

#include <string>
#include <math.h>
#include <vector>
#include <boost/intrusive_ptr.hpp>

namespace boost
{
 template <class T> inline void intrusive_ptr_add_ref(T* p)
  {
    ++(p->references);
  }

 template <class T> inline void intrusive_ptr_release(T* p)
  {
   if (--(p->references) == 0)
     delete p;
  }
} // namespace boost

#define INTRUSIVE_PTR_REQS long references; template <class T> friend void ::boost::intrusive_ptr_add_ref(T* p); template <class T> friend void ::boost::intrusive_ptr_release(T* p);

namespace dollar
{
    const int NumPoints = 32;
    const double SquareSize = 250.0;
    const double HalfDiagonal = 0.5 * sqrt(2*SquareSize*SquareSize);
    const double AngleRange = 45.0;
    const double AnglePrecision = 2.0;
    const double Phi = 0.5 * (-1.0 + sqrt(5.0)); // Golden Ratio

    class Point
    {
    public:
        double X;
        double Y;

        INTRUSIVE_PTR_REQS;

        Point(double x, double y) : X(x), Y(y), references(0) {}
    };

    typedef boost::intrusive_ptr<Point> PointPtr;
    typedef std::vector<PointPtr> PointVector;

    class Rectangle
    {
    public:
        int X;
        int Y;
        int Width;
        int Height;

        INTRUSIVE_PTR_REQS;

        Rectangle(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height), references(0) {}
    };

    typedef boost::intrusive_ptr<Rectangle> RectanglePtr;

    class Template
    {
    public:
        std::string Name;
        PointVector Points;

        INTRUSIVE_PTR_REQS;

        Template(std::string name, PointVector points);
    };

    typedef boost::intrusive_ptr<Template> TemplatePtr;
    typedef std::vector<TemplatePtr> TemplateVector;

    class Result
    {
    public:
        std::string Name;
        double Score;

        INTRUSIVE_PTR_REQS;

        Result(std::string name, double score) : Name(name), Score(score), references(0) {}
    };

    typedef boost::intrusive_ptr<Result> ResultPtr;

    ResultPtr Recognize(PointVector& points, TemplateVector& templates);

    void Resample(PointVector& points, size_t n);
    double PathLength(PointVector& points);
    double PathDistance(PointVector& pts1, PointVector& pts2);
    double Distance(PointPtr& p1, PointPtr& p2);
    RectanglePtr BoundingBox(PointVector& points);
    PointPtr Centroid(PointVector& points);
    void RotateToZero(PointVector& points);
    void RotateBy(PointVector& points, double theta);
    void ScaleToSquare(PointVector& points, double size);
    void TranslateToOrigin(PointVector& points);
    double DistanceAtBestAngle(PointVector& points, TemplatePtr& T, double minAngle, double maxAngle, double threshold);
    double DistanceAtAngle(PointVector& points, TemplatePtr& T, double theta);

};
#endif // DOLLAR_H_INCLUDED
