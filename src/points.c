#include "../include/points.h"
#include "math.h"
#include <math.h>

float abs_point_2d(Point2D const point) {
    return sqrtf(point.x * point.x + point.y * point.y);
}

Point2D add_point_2d(Point2D const first, Point2D const second) {
    Point2D sum = {
        .x = first.x + second.x,
        .y = first.y + second.y,
    };
    return sum;
}

Point2D sub_point_2d(Point2D const first, Point2D const second) {
    Point2D diff = {
        .x = first.x - second.x,
        .y = first.y - second.y,
    };
    return diff;
}

Point2D scale_point_2d(Point2D const point, float const factor) {
    Point2D scaled = {
        .x = factor * point.x,
        .y = factor * point.y,
    };
    return scaled;
}

float abs_point_3d(Point3D const point) {
    return sqrtf(point.x * point.x + point.y * point.y + point.z * point.z);
}
