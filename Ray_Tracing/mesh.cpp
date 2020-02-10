#include "mesh.h"
#include <fstream>
#include <string>
#include <limits>
#include <iostream>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e;
    vec3 v;
    box.Make_Empty();
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
            box.Include_Point(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }
    }
    number_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    Hit intersecPoint;
    intersecPoint.object = nullptr;
    
    if(part >= 0) {
        if(Intersect_Triangle(ray, part, intersecPoint.dist)) {
            intersecPoint.object = this;
            intersecPoint.part = part;
            return intersecPoint;
        }
        return intersecPoint;
    }
    
    for(int i = 0; i < triangles.size(); i ++) {
        if(Intersect_Triangle(ray, i, intersecPoint.dist)) {
            intersecPoint.object = this;
            intersecPoint.part = i;
            return intersecPoint;
        }
    }
    return intersecPoint;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3& point, int part) const
{
    assert(part>=0);
    
    ivec3 triangle = triangles.at(part);
    vec3 A = vertices.at(triangle[0]), B = vertices.at(triangle[1]), C = vertices.at(triangle[2]);
    
    return cross(B-A,C-A).normalized();
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
bool Mesh::Intersect_Triangle(const Ray& ray, int tri, double& dist) const
{
    ivec3 triangle = triangles.at(tri);
    vec3 A = vertices.at(triangle[0]), B = vertices.at(triangle[1]), C = vertices.at(triangle[2]);
    
    //first intersect with the plane
    vec3 planeNormal = cross(B-A,C-A).normalized();
    if(dot(ray.direction,planeNormal) == 0) {
        return false;
    }
    dist = - dot(ray.endpoint-A,planeNormal) / dot(ray.direction,planeNormal);
    
    if(dist < 0 && dist <= small_t) {
        return false;
    }
    
    vec3 P = ray.Point(dist);
    //find if the point is in the triangle
    double triArea = cross(B-A,C-A).magnitude();
    double alpha = cross(B-P,C-P).magnitude() / triArea;
    double beta = cross(P-A,C-A).magnitude() / triArea;
    double phi = cross(B-A,P-A).magnitude() / triArea;
    
    bool inTriangle = (phi >= -weight_tolerance && phi <= (1+weight_tolerance) && alpha >= -weight_tolerance && alpha <= (1+weight_tolerance) && beta >= -weight_tolerance && beta <= (1+weight_tolerance));
    bool weightsInBound = (alpha + beta + phi) >= -weight_tolerance && (alpha + beta + phi) <= 1+weight_tolerance;
    
    if(inTriangle && weightsInBound) {
        return true;
    }
    
    return false;
}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const
{
    Box b;
    b.Make_Empty();
    
    ivec3 curr = triangles.at(part);
    b.Include_Point(vertices.at(curr[0]));
    b.Include_Point(vertices.at(curr[1]));
    b.Include_Point(vertices.at(curr[2]));
    
    return b;
}
