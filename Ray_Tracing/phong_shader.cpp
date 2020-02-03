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
    vec3 color;
    
    //assign ambient light
    color = color_ambient * (world.ambient_color * world.ambient_intensity);
    
    Ray LightRay;
    Light* currLight;
    vec3 lightIntensity;
    for(unsigned i = 0; i < world.lights.size(); i++) {
        //draw shadow ray to each light source
        currLight = world.lights.at(i);
        LightRay.direction = (currLight->position - intersection_point).normalized();
        LightRay.endpoint = currLight->position;
        
        if(world.enable_shadows) {
            Ray shadowRay;
            shadowRay.direction = LightRay.direction;
            shadowRay.endpoint = intersection_point;
            
            Hit intersecToLight = world.Closest_Intersection(shadowRay);
            if(intersecToLight.object) {
                if(intersecToLight.dist < (currLight->position - intersection_point).magnitude()) {
                    return color;
                }
            }
        }
        
        lightIntensity = currLight->Emitted_Light(currLight->position - intersection_point);
        
        //compute diffuse shading
        double cosAngle = std::max(0.0,dot(normal,LightRay.direction));
        //amount of light accourding to the distance from the light
        color += color_diffuse * lightIntensity * cosAngle;
        
        //compute specular shading
        vec3 r = 2 * dot(LightRay.direction, normal) * normal - LightRay.direction;
        double rv = pow(std::max(0.0,dot(r.normalized(), -ray.direction.normalized() ) ), specular_power);
        color += color_specular * lightIntensity * rv;
    }
    
    return color;
}
