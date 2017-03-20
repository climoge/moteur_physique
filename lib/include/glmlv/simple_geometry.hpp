#pragma once

#ifndef SIMPLE_GEOMETRY_H
#define SIMPLE_GEOMETRY_H

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../apps/moteur_physique/PMat.hpp"

namespace glmlv
{

struct Vertex3f3f2f
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex3f3f2f() = default;

    Vertex3f3f2f(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords):
        position(position), normal(normal), texCoords(texCoords)
    {
    }
};

struct SimpleGeometry
{
    std::vector<Vertex3f3f2f> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
};

SimpleGeometry makeTriangle();
SimpleGeometry makeCube();
//SimpleGeometry makeFlag(std::Vector<PMat> Pmats);
SimpleGeometry makeFlag();
SimpleGeometry makeFlag(std::vector<PMat> pMats, const unsigned int width, const unsigned int height);
// Pass a number of subdivision to apply on the longitude of the sphere
SimpleGeometry makeSphere(uint32_t subdivLongitude);

}

#endif
