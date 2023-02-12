#include "../include/collision.h"
#include <stdbool.h>

bool circles_collide(const Circle *const first, const Circle *const second) {
    float distance_of_centers = abs_point_2d(sub_point_2d(first->center, second->center));
    float sum_of_radii = first->radius + second->radius;
    return distance_of_centers < sum_of_radii;
};
