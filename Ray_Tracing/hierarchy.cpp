#include <algorithm>
#include "hierarchy.h"

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries()
{
    if(!entries.size()) return;
    
    for(unsigned i = 0; i < entries.size(); i++) {
        int minCenterLoc = i;
        //find the min value in the rest of the array
        for(unsigned j = i+1; j < entries.size(); j++) {
            double minCenter = ((entries.at(i).box.lo + entries.at(i).box.hi)/2).magnitude();
            double currCenter = ((entries.at(j).box.lo + entries.at(j).box.hi)/2).magnitude();
            if(minCenter > currCenter) {
                minCenterLoc = j;
            }
        }
        //swap entry
        std::swap(entries.at(i), entries.at(minCenterLoc));
    }
}

// Populate tree from entries.
void Hierarchy::Build_Tree()
{
    if(!entries.size()) return;
    std::vector<Box> flippedTree;
    //copy last entry as first box so n-1 box in tree is the first entry
    for(int i = entries.size()-1; i >= 0; i --) {
        flippedTree.push_back(entries.at(i).box);
    }
    
    for(unsigned i = 0; i < flippedTree.size(); i += 2) {
        Box unionedBox;
        if(i + 1 >= flippedTree.size()) {
            break;
        }
        unionedBox = flippedTree.at(i).Union(flippedTree.at(i+1));
        
        flippedTree.push_back(unionedBox);
    }
    
    for(int i = flippedTree.size()-1; i >=0; i--) {
        tree.push_back(flippedTree.at(i));
    }
}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray& ray, std::vector<int>& candidates) const
{
    /*
    int count = 0;
    while(count < entries.size() - 1) {
        if(tree.at(count).Intersection(ray)) {
            int left = 2 * count + 1, right = 2 * count + 2;
            
            if(tree.at(left).Intersection(ray)) {
                count = left;
            } else {
                count = right;
            }
        } else {
            return;
        }
    }
    
    candidates.push_back(count - entries.size() + 1);
    */
    intersec_obj(ray, 0, candidates);
}

void Hierarchy::intersec_obj(const Ray& ray, int curr, std::vector<int>& candidates) const {
    if(curr >= entries.size() - 1) { //reached entry
        if(curr < tree.size()) {
            if(tree.at(curr).Intersection(ray)) {
                candidates.push_back(curr - entries.size() + 1);
            }
        }
        return;
    }
    
    if(tree.at(curr).Intersection(ray)) {
        intersec_obj(ray, 2*curr+1, candidates);
        intersec_obj(ray, 2*curr+2, candidates);
    } else {
        return;
    }
}
