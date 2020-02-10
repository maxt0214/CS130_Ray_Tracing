#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    
    Ray reflectRay;
    
    reflectRay.endpoint = intersection_point;
    reflectRay.direction = (ray.direction - 2 * dot(ray.direction,normal) * normal).normalized();
    
    vec3 reflectColor = world.Cast_Ray(reflectRay, recursion_depth+1);
    
    color = (1-reflectivity) * shader->Shade_Surface(ray,intersection_point,normal,recursion_depth) + reflectivity * reflectColor;
    
    return color;
}
