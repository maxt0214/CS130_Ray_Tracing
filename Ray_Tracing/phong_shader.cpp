#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"
#include <algorithm>

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color(0,0,0);
    
    Ray shadowRay;
    Light* currLight;
    vec3 lightIntensity;
    for(unsigned i = 0; i < world.lights.size(); i++) {
        //draw shadow ray to each light source
        currLight = world.lights.at(i);
        shadowRay.direction = (currLight->position - intersection_point).normalized();
        shadowRay.endpoint = currLight->position;
        
        //compute diffuse color
        double cosAngle = std::max(0.0,dot(normal,shadowRay.direction));
        //amount of light accourding to the distance from the light
        lightIntensity = currLight->Emitted_Light(currLight->position - intersection_point);
        color += lightIntensity * color_diffuse * cosAngle;
    }
    
    return color;
}
