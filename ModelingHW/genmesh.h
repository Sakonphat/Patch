#ifndef GENMESH
#define GENMESH

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#define PI 3.1415926
#define sgn(x)  __max(-1, __min(1, x))


glm::vec3 computeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);

void GenerateBezierPatch(std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals, float segment);


#endif