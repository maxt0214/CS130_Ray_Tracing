#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

#include <iostream>

extern bool disable_hierarchy;

Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    double min_t = std::numeric_limits<double>::max();
    Hit closestHit;
    closestHit.object = NULL;
    
    std::vector<int> candidates;
    hierarchy.Intersection_Candidates(ray, candidates);
    
    for(unsigned i = 0; i < candidates.size(); i++) {
        Entry currEntry = hierarchy.entries.at(candidates.at(i));
        Hit currHit = currEntry.obj->Intersection(ray,currEntry.part);
        if(currHit.object != NULL && currHit.dist < min_t && currHit.dist >= small_t) {
            min_t = currHit.dist;
            closestHit = currHit;
        }
    }
    
    return closestHit;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    // set up the initial view ray here
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();
    
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    if(!disable_hierarchy)
        Initialize_Hierarchy();
    
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{
    vec3 color;
    // determine the color here
    vec3 somePoint(0,0,0); //point for background shader
    if(recursion_depth > recursion_depth_limit) {
        color = background_shader->Shade_Surface(ray,somePoint,somePoint,recursion_depth);
        return color;
    }
    
    Hit closestHit = Closest_Intersection(ray);
    if(closestHit.object != NULL) {//there is a hit
        vec3 hitPoint = ray.Point(closestHit.dist);
        vec3 normOfObject = closestHit.object->Normal(hitPoint,closestHit.part);
        color = closestHit.object->material_shader->Shade_Surface(ray,hitPoint,normOfObject,recursion_depth);
    } else {
        color = background_shader->Shade_Surface(ray,somePoint,somePoint,recursion_depth);
    }
    
    return color;
}

void Render_World::Initialize_Hierarchy()
{
    // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.
    for(unsigned i = 0; i < objects.size(); i++) {
        Object * curr = objects.at(i);
        for(int j = 0; j < curr->number_parts; j++) {
            Entry newEntry;
            
            newEntry.obj = curr;
            newEntry.part = j;
            newEntry.box = curr->Bounding_Box(j);
            
            hierarchy.entries.push_back(newEntry);
        }
    }
    
    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
