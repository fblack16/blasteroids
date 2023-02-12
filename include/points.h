#ifndef BLACK_BLASTEROIDS_POINTS_H
#define BLACK_BLASTEROIDS_POINTS_H

struct Point2D {
    float x;
    float y;
};
typedef struct Point2D Point2D;

struct Point3D {
    float x;
    float y;
    float z;
};
typedef struct Point3D Point3D;

float abs_point_2d(Point2D const point);
Point2D add_point_2d(Point2D const first, Point2D const second);
Point2D sub_point_2d(Point2D const first, Point2D const second);
Point2D scale_point_2d(Point2D const point, float const factor);
float abs_point_3d(Point3D const point);

#endif
