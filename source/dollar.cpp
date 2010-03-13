#include "dollar.h"
#include <limits.h>
#include <float.h>
#include <algorithm>

namespace dollar
{
    Template::Template(std::string name, PointVector points) : Name(name), references(0)
    {
        for (size_t i = 0; i < points.size(); ++i)
        {
            Points.push_back(PointPtr(new Point(points[i]->X,points[i]->Y)));
        }

        Resample(Points, NumPoints);
        RotateToZero(Points);
        ScaleToSquare(Points, SquareSize);
        TranslateToOrigin(Points);
    }

    ResultPtr Recognize(PointVector& points, TemplateVector& templates)
	{
		Resample(points, NumPoints);
		RotateToZero(points);
		ScaleToSquare(points, SquareSize);
		TranslateToOrigin(points);

		double b = DBL_MAX;
		int index = 0;
		for (size_t i = 0; i < templates.size(); ++i)
		{
			double d = DistanceAtBestAngle(points, templates[i], -AngleRange, +AngleRange, AnglePrecision);
			if (d < b)
			{
				b = d;
				index = i;
			}
		}
		double score = 1.0 - (b / HalfDiagonal);
		return ResultPtr(new Result(templates[index]->Name, score));
	};

    void Resample(PointVector& points, size_t n)
    {
        double I = PathLength(points) / (n - 1); // interval length
        double D = 0.0;
        PointVector newpoints;
        newpoints.push_back(points[0]);

        for (size_t i = 1; i < points.size(); ++i)
        {
            double d = Distance(points[i - 1], points[i]);
            if ((D + d) >= I)
            {
                double qx = points[i - 1]->X + ((I - D) / d) * (points[i]->X - points[i - 1]->X);
                double qy = points[i - 1]->Y + ((I - D) / d) * (points[i]->Y - points[i - 1]->Y);
                PointPtr q(new Point(qx, qy));
                newpoints.push_back(q);
                points.insert(points.begin()+i,q);
                D = 0.0;
            }
            else D += d;
        }
        // somtimes we fall a rounding-error short of adding the last point, so add it if so
        if (newpoints.size() == n - 1)
        {
            newpoints.push_back(points[points.size() - 1]);
        }

        points.clear();
        points.insert(points.begin(), newpoints.begin(), newpoints.end());
    }

    double PathLength(PointVector& points)
    {
        double d = 0.0;
        for (size_t i = 1; i < points.size(); ++i)
        {
            d += Distance(points[i - 1], points[i]);
        }
        return d;
    }

    double Distance(PointPtr& p1, PointPtr& p2)
    {
        double dx = p2->X - p1->X;
        double dy = p2->Y - p1->Y;
        return sqrt(dx * dx + dy * dy);
    }

    RectanglePtr BoundingBox(PointVector& points)
    {
        int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
        for (size_t i = 0; i < points.size(); ++i)
        {
            if (points[i]->X < minX)
                minX = points[i]->X;
            if (points[i]->X > maxX)
                maxX = points[i]->X;
            if (points[i]->Y < minY)
                minY = points[i]->Y;
            if (points[i]->Y > maxY)
                maxY = points[i]->Y;
        }
        return RectanglePtr(new Rectangle(minX, minY, maxX - minX, maxY - minY));
    }

    double PathDistance(PointVector& pts1, PointVector& pts2)
    {
        double d = 0.0;
        for (size_t i = 0; i < pts1.size(); ++i) // assumes pts1.length == pts2.length
        {
            d += Distance(pts1[i], pts2[i]);
        }
        return d / pts1.size();
    }

    PointPtr Centroid(PointVector& points)
    {
        double x = 0;
        double y = 0;
        for (size_t i = 0; i < points.size(); ++i)
        {
            x += points[i]->X;
            y += points[i]->Y;
        }
        x /= points.size();
        y /= points.size();
        return PointPtr(new Point(x, y));
    }

    void RotateToZero(PointVector& points)
    {
        PointPtr c = Centroid(points);
        double theta = atan2(c->Y - points[0]->Y, c->X - points[0]->X);
        RotateBy(points, -theta);
    }

    void RotateBy(PointVector& points, double theta)
    {
        PointPtr c = Centroid(points);
        double cosine = cos(theta);
        double sine = sin(theta);

        PointVector newpoints;
        for (size_t i = 0; i < points.size(); ++i)
        {
            double qx = (points[i]->X - c->X) * cosine - (points[i]->Y - c->Y) * sine + c->X;
            double qy = (points[i]->X - c->X) * sine + (points[i]->Y - c->Y) * cosine + c->Y;
            newpoints.push_back(PointPtr(new Point(qx, qy)));
        }

        points.clear();
        points.insert(points.begin(), newpoints.begin(), newpoints.end());
    }

    void ScaleToSquare(PointVector& points, double size)
    {
        RectanglePtr B = BoundingBox(points);
        PointVector newpoints;
        for (size_t i = 0; i < points.size(); ++i)
        {
            double qx = points[i]->X * (size / B->Width);
            double qy = points[i]->Y * (size / B->Height);
            newpoints.push_back(PointPtr(new Point(qx, qy)));
        }
        points.clear();
        points.insert(points.begin(), newpoints.begin(), newpoints.end());
    }

    void TranslateToOrigin(PointVector& points)
    {
        PointPtr c = Centroid(points);
        PointVector newpoints;
        for (size_t i = 0; i < points.size(); ++i)
        {
            double qx = points[i]->X - c->X;
            double qy = points[i]->Y - c->Y;
            newpoints.push_back(PointPtr(new Point(qx, qy)));
        }
        points.clear();
        points.insert(points.begin(), newpoints.begin(), newpoints.end());
    }

    double DistanceAtBestAngle(PointVector& points, TemplatePtr& T, double minAngle, double maxAngle, double threshold)
    {
        double x1 = Phi * minAngle + (1.0 - Phi) * maxAngle;
        double f1 = DistanceAtAngle(points, T, x1);
        double x2 = (1.0 - Phi) * minAngle + Phi * maxAngle;
        double f2 = DistanceAtAngle(points, T, x2);

        while (abs(maxAngle - minAngle) > threshold)
        {
            if (f1 < f2)
            {
                maxAngle = x2;
                x2 = x1;
                f2 = f1;
                x1 = Phi * minAngle + (1.0 - Phi) * maxAngle;
                f1 = DistanceAtAngle(points, T, x1);
            }
            else
            {
                minAngle = x1;
                x1 = x2;
                f1 = f2;
                x2 = (1.0 - Phi) * minAngle + Phi * maxAngle;
                f2 = DistanceAtAngle(points, T, x2);
            }
        }
        return std::min(f1, f2);
    }

    double DistanceAtAngle(PointVector& points, TemplatePtr& T, double theta)
    {
        PointVector newpoints(points);
        RotateBy(newpoints, theta);

        return PathDistance(newpoints, T->Points);
    }
};
