#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <glm/gtx/io.hpp>

namespace glmlv
{

SimpleGeometry makeTriangle()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        { glm::vec3(-0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        { glm::vec3(0., 0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0.5, 1) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2
    };

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeCube()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        // Bottom side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0) },
        // Right side
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) },
        // Back side
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) },
        // Left side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) },
        // Front side
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        // Top side
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    return{ vertexBuffer, indexBuffer };
}

/*SimpleGeometry makeFlag(std::Vector<PMat>& pMats)
{
    std::vector<Vertex3f3f2f> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    
    uint32_t index = 0;
    
    for(float j = 0; j <= 1; j += 0.1){
		for(float i = 0; i <= 1; i += 0.1){
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i, j, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i + 0.1, j, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i + 0.1, j + 0.1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i, j + 0.1, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			
			indexBuffer.push_back(index);
			indexBuffer.push_back(index+1);
			indexBuffer.push_back(index+2);
			indexBuffer.push_back(index);
			indexBuffer.push_back(index+2);
			indexBuffer.push_back(index+3);
			
			index += 4;
		}
	}
	
	for(auto const& pMat: pMats) {
		vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(pMat[i].getPos().x, pMat[i].getPos().y, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
		indexBuffer.push_back(index);
		index++;
	}

    return{ vertexBuffer, indexBuffer };
}*/

SimpleGeometry makeFlag()
{
    std::vector<Vertex3f3f2f> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    
    uint32_t index = 0;
    
    unsigned int width = 10;
    unsigned int height = 10;
    
    float jOffset = 1/(float) height;
    float iOffset = 1/(float) width;
    
    assert(width%2 == 0 && height%2 == 0);
    
    for(float j = 0; j < 1; j += jOffset*2){
		for(float i = 0; i < 1; i += iOffset*2){
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i, j, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i, j + jOffset, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i + iOffset, j, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			vertexBuffer.push_back(Vertex3f3f2f(glm::vec3(i + iOffset, j + jOffset, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
			
			if(index >= (2*width)){
				//std::cout << "index " << index << std::endl;
				int offset = index - (width*2-1);
				//std::cout << "offset " << offset << std::endl;
				indexBuffer.push_back(offset);
				indexBuffer.push_back(index);
				indexBuffer.push_back(offset + 2);
				//std::cout << "Vertex 1 : " << offset << " " << index << " " << offset+2 << std::endl;
				indexBuffer.push_back(index);
				indexBuffer.push_back(index+2);
				indexBuffer.push_back(offset + 2);
				//std::cout << "Vertex 1 : " << index << " " << index+2 << " " << offset+2 << std::endl;
				
				if(index % (2*width) < (width*2-4)){
					//std::cout << "index " << index << std::endl;
					int offset = index - (width*2-1) + 2;
					//std::cout << "offset " << offset << std::endl;
					indexBuffer.push_back(offset);
					indexBuffer.push_back(index+2);
					indexBuffer.push_back(offset + 2);
					//std::cout << "Vertex 1 : " << offset << " " << index+2 << " " << offset+2 << std::endl;
					indexBuffer.push_back(index+2);
					indexBuffer.push_back(index+4);
					indexBuffer.push_back(offset + 2);
					//std::cout << "Vertex 1 : " << index+2 << " " << index+4 << " " << offset+2 << std::endl << std::endl;
				}
			}
			
			
			
			if(index > 0 && index % (2*width)){
				//std::cout << "index : " << index << " modulo : " <<	index % (4*width) << std::endl;
				//std::cout << "Vertex 1 : " << index-2 << " " << index-1 << " " << index << std::endl;
				//std::cout << "Vertex 2 : " << index-1 << " " << index+1 << " " << index << std::endl << std::endl;
				indexBuffer.push_back(index-2);
				indexBuffer.push_back(index-1);
				indexBuffer.push_back(index);
				indexBuffer.push_back(index-1);
				indexBuffer.push_back(index+1);
				indexBuffer.push_back(index);
			}
			
			indexBuffer.push_back(index);
			indexBuffer.push_back(index+1);
			indexBuffer.push_back(index+2);
			indexBuffer.push_back(index+1);
			indexBuffer.push_back(index+3);
			indexBuffer.push_back(index+2);
			
			index += 4;
		}
	}

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeFlag(const std::vector<PMat> pMats, const unsigned int width, const unsigned int height)
{
	/*std::cout << "P15 : " << pMats[15].getPos() << std::endl;
	std::cout << "P16 : " << pMats[16].getPos() << std::endl;
	std::cout << "P17 : " << pMats[17].getPos() << std::endl;*/
	assert(width%2 == 0 && height%2 == 0);
    std::vector<Vertex3f3f2f> vertexBuffer;
    std::vector<uint32_t> indexBuffer;
    
    uint32_t index = 0;
    
    float jOffset = 1/(float) height;
    float iOffset = 1/(float) width;
    
    for(auto currentPMat : pMats){
		//std::cout << "current : " << currentPMat.getPos() << std::endl;
	}
    
    std::cout << std::endl;
    
    for(auto currentPMat : pMats){
		//std::cout << "current : " << currentPMat.getPos() << std::endl;
		vertexBuffer.push_back(Vertex3f3f2f(currentPMat.getPos(), glm::vec3(0, 0, 1), glm::vec2(0, 0)));
	}
    
    for(float j = 0; j < 1; j += jOffset*2){
		for(float i = 0; i < 1; i += iOffset*2){
			
			if(index >= (2*width)){
				//std::cout << "index " << index << std::endl;
				int offset = index - (width*2-1);
				//std::cout << "offset " << offset << std::endl;
				indexBuffer.push_back(offset);
				indexBuffer.push_back(index);
				indexBuffer.push_back(offset + 2);
				//std::cout << "Vertex 1 : " << offset << " " << index << " " << offset+2 << std::endl;
				indexBuffer.push_back(index);
				indexBuffer.push_back(index+2);
				indexBuffer.push_back(offset + 2);
				//std::cout << "Vertex 1 : " << index << " " << index+2 << " " << offset+2 << std::endl;
				
				if(index % (2*width) < (width*2-4)){
					//std::cout << "index " << index << std::endl;
					int offset = index - (width*2-1) + 2;
					//std::cout << "offset " << offset << std::endl;
					indexBuffer.push_back(offset);
					indexBuffer.push_back(index+2);
					indexBuffer.push_back(offset + 2);
					//std::cout << "Vertex 1 : " << offset << " " << index+2 << " " << offset+2 << std::endl;
					indexBuffer.push_back(index+2);
					indexBuffer.push_back(index+4);
					indexBuffer.push_back(offset + 2);
					//std::cout << "Vertex 1 : " << index+2 << " " << index+4 << " " << offset+2 << std::endl << std::endl;
				}
			}
			
			
			
			if(index > 0 && index % (2*width)){
				//std::cout << "index : " << index << " modulo : " <<	index % (4*width) << std::endl;
				//std::cout << "Vertex 1 : " << index-2 << " " << index-1 << " " << index << std::endl;
				//std::cout << "Vertex 2 : " << index-1 << " " << index+1 << " " << index << std::endl << std::endl;
				indexBuffer.push_back(index-2);
				indexBuffer.push_back(index-1);
				indexBuffer.push_back(index);
				indexBuffer.push_back(index-1);
				indexBuffer.push_back(index+1);
				indexBuffer.push_back(index);
			}
			
			indexBuffer.push_back(index);
			indexBuffer.push_back(index+1);
			indexBuffer.push_back(index+2);
			indexBuffer.push_back(index+1);
			indexBuffer.push_back(index+3);
			indexBuffer.push_back(index+2);
			
			index += 4;
		}
	}

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeSphere(uint32_t subdivLongitude)
{
    const auto discLong = subdivLongitude;
    const auto discLat = 2 * discLong;

    float rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
    float dPhi = glm::pi<float>() * 2.f * rcpLat, dTheta = glm::pi<float>() * rcpLong;

    std::vector<Vertex3f3f2f> vertexBuffer;

    for (uint32_t j = 0; j <= discLong; ++j)
    {
        float cosTheta = cos(-glm::half_pi<float>() + j * dTheta);
        float sinTheta = sin(-glm::half_pi<float>() + j * dTheta);

        for (uint32_t i = 0; i <= discLat; ++i) {
            glm::vec3 coords;

            coords.x = sin(i * dPhi) * cosTheta;
            coords.y = sinTheta;
            coords.z = cos(i * dPhi) * cosTheta;

            vertexBuffer.emplace_back(coords, coords, glm::vec2(i * rcpLat, j * rcpLong));
        }
    }

    std::vector<uint32_t> indexBuffer;

    for (uint32_t j = 0; j < discLong; ++j)
    {
        uint32_t offset = j * (discLat + 1);
        for (uint32_t i = 0; i < discLat; ++i)
        {
            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + (i + 1));
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));

            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));
            indexBuffer.push_back(offset + i + discLat + 1);
        }
    }

    return{ vertexBuffer, indexBuffer };
}

}
