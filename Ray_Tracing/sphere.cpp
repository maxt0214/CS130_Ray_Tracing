#include "sphere.h"
#include "ray.h"
#include <cmath>

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit interSecPoint;
    
    double a = dot(ray.direction, ray.direction), b = 2 * dot(ray.endpoint - center, ray.direction), c = dot(ray.endpoint - center, ray.endpoint - center) - radius * radius;
    double b2_4ac = b * b - 4*a*c;
    if(b2_4ac > 0) { //two intersection
        double dist1 = (-b + sqrt(b2_4ac)) / (2*a);
        double dist2 = (-b - sqrt(b2_4ac)) / (2*a);
        if(dist2 >= 0) {
            interSecPoint.object = this;
            interSecPoint.dist = dist2;
            interSecPoint.part = part;
        } else if(dist1 >= 0) {
            interSecPoint.object = this;
            interSecPoint.dist = dist1;
            interSecPoint.part = part;
        } else {
            interSecPoint.object = NULL;
        }
        
    } else if (b2_4ac == 0) { //one intersection
        double dist3 = -b / (2*a);
        if(dist3 < 0) {
            interSecPoint.object = NULL;
        } else {
            interSecPoint.object = this;
            interSecPoint.dist = dist3;
            interSecPoint.part = part;
        }
    } else { //no intersection
        interSecPoint.object = NULL;
    }
    
    return interSecPoint;
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;
    TODO; // compute the normal direction
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
