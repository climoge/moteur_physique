#include "Application.hpp"
#include "PMat.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

#define SIZE 1000000

int Application::run()
{	
    float clearColor[3] = { 0, 0, 0 };
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
		for(auto currentPMat = pMats.begin(); currentPMat != pMats.end(); ++currentPMat){
			currentPMat->UpdateLeapFrog(1.f);
		}
		int count = 0;
		for(auto currentLink = links.begin(); currentLink != links.end(); ++currentLink)
		{
			
			//currentLink->LinkRessort();
			//currentLink->LinkFrein();
			currentLink->LinkGravite(glm::vec3(0.f, 0.f, 0.f));
			count++;
		}
		
		std::cout << "Links updated : " << count << std::endl;
		
		UpdateFlag();
		
		std::cout << "Flag updated" << std::endl;
			
        const auto seconds = glfwGetTime();

        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f, 100.f);
        const auto viewMatrix = m_viewController.getViewMatrix();

        glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
        glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

        glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
        glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

        {
            //const auto modelMatrix = glm::rotate(glm::mat4(1), 0.2f * float(seconds), glm::vec3(0, 1, 0));
            const auto modelMatrix = glm::scale(glm::mat4(1), glm::vec3(1.f/SIZE, 1.f/SIZE, 1.f/SIZE));

            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform3fv(m_uKdLocation, 1, glm::value_ptr(m_FlagKd));

            glBindVertexArray(m_flagVAO);
            glDrawElements(GL_TRIANGLES, m_flagGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
        }

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::ColorEdit3("DirLightColor", glm::value_ptr(m_DirLightColor));
                ImGui::DragFloat("DirLightIntensity", &m_DirLightIntensity, 0.1f, 0.f, 100.f);
                if (ImGui::DragFloat("Phi Angle", &m_DirLightPhiAngleDegrees, 1.0f, 0.0f, 360.f) ||
                    ImGui::DragFloat("Theta Angle", &m_DirLightThetaAngleDegrees, 1.0f, 0.0f, 180.f)) {
                    m_DirLightDirection = computeDirectionVector(glm::radians(m_DirLightPhiAngleDegrees), glm::radians(m_DirLightThetaAngleDegrees));
                }
            }

            if (ImGui::CollapsingHeader("Point Light"))
            {
                ImGui::ColorEdit3("PointLightColor", glm::value_ptr(m_PointLightColor));
                ImGui::DragFloat("PointLightIntensity", &m_PointLightIntensity, 0.1f, 0.f, 16000.f);
                ImGui::InputFloat3("Position", glm::value_ptr(m_PointLightPosition));
            }

            if (ImGui::CollapsingHeader("Materials"))
            {
                ImGui::ColorEdit3("Flag Kd", glm::value_ptr(m_FlagKd));
            }

            ImGui::End();
        }

        
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    
    unsigned int index = 0;
    
    m_width = 10;
    m_height = 10;
    
    assert(m_width%2 == 0 && m_height%2 == 0);
    
    float jOffset = SIZE/(float) m_height;
    float iOffset = SIZE/(float) m_width;
    
    for(float j = 0; j < SIZE; j += jOffset*2){
		for(float i = 0; i < SIZE; i += iOffset*2){
			pMats.push_back(PMat(glm::vec3(i, j, 0), 2.f, false));
			
			if(index == (m_width * m_height) - (2 * m_width)) pMats.push_back(PMat(glm::vec3(i, j + jOffset, 0), 2.f, true)); // Anchor point		
			else pMats.push_back(PMat(glm::vec3(i, j + jOffset, 0), 2.f, false));
			
			pMats.push_back(PMat(glm::vec3(i + iOffset, j, 0), 2.f, false));
			
			if(index == (m_height*m_width)-4) pMats.push_back(PMat(glm::vec3(i + iOffset, j + jOffset, 0), 2.f, true));  // Anchor point
			else pMats.push_back(PMat(glm::vec3(i + iOffset, j + jOffset, 0), 2.f, false));
			
			index += 4;
		}
	}

	index = 0;
    
    for(float j = 0; j < SIZE; j += jOffset*2){
		for(float i = 0; i < SIZE; i += iOffset*2){
			
			/*if(index >= (2*m_width)){
				std::cout << "index " << index << std::endl;
				int offset = index - (m_width*2-1);
				std::cout << "offset " << offset << std::endl;
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[offset], pMats[index]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index], pMats[offset + 2]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[offset+2], pMats[offset]);
				
				std::cout << "Vertex 1 : " << offset << " " << index << " " << offset+2 << std::endl;
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index], pMats[index+2]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index+2], pMats[offset + 2]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[offset+2], pMats[index]);
				
				std::cout << "Vertex 1 : " << index << " " << index+2 << " " << offset+2 << std::endl;
				
				if(index % (2*m_width) < (m_width*2-4)){
					std::cout << "index " << index << std::endl;
					int offset = index - (m_width*2-1) + 2;
					std::cout << "offset " << offset << std::endl;
					links.push_back(Link(3.f, 2.f));
					links.back().LinkConnect(pMats[offset], pMats[index+2]);
					std::cout << "Vertex 1 : " << offset << " " << index+2 << " " << offset+2 << std::endl;

					links.push_back(Link(3.f, 2.f));
					links.back().LinkConnect(pMats[index+2], pMats[index+4]);
					links.push_back(Link(3.f, 2.f));
					links.back().LinkConnect(pMats[index+4], pMats[offset + 2]);
					links.push_back(Link(3.f, 2.f));
					links.back().LinkConnect(pMats[offset+2], pMats[index+2]);
					std::cout << "Vertex 1 : " << index+2 << " " << index+4 << " " << offset+2 << std::endl << std::endl;
				}
			}*/
			
			
			// TO DO : Supprimer les liaisons diagonales.
			if(index > 0 && index % (2*m_width)){
				std::cout << "index : " << index << " modulo : " <<	index % (4*m_width) << std::endl;
				std::cout << "Vertex 1 : " << index-2 << " " << index-1 << " " << index << std::endl;
				std::cout << "Vertex 2 : " << index-1 << " " << index+1 << " " << index << std::endl << std::endl;
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index-2], pMats[index-1]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index-1], pMats[index]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index], pMats[index-2]);
				
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index-1], pMats[index+1]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index+1], pMats[index]);
				links.push_back(Link(3.f, 2.f));
				links.back().LinkConnect(pMats[index], pMats[index-1]);
			}
			
			links.push_back(Link(3.f, 2.f));
			links.back().LinkConnect(pMats[index], pMats[index+1]);
			links.push_back(Link(3.f, 2.f));
			links.back().LinkConnect(pMats[index+2], pMats[index]);

			links.push_back(Link(3.f, 2.f));
			links.back().LinkConnect(pMats[index+1], pMats[index+3]);
			links.push_back(Link(3.f, 2.f));
			links.back().LinkConnect(pMats[index+3], pMats[index+2]);
			
			index += 4;
		}
	}

	UpdateFlag();

    glEnable(GL_DEPTH_TEST);

    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();

    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_program.glId(), "uNormalMatrix");


    m_uDirectionalLightDirLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

    m_uKdLocation = glGetUniformLocation(m_program.glId(), "uKd");

    m_viewController.setViewMatrix(glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
}

void Application::UpdateFlag(){
    const GLint vboBindingIndex = 0; // Arbitrary choice between 0 and glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS)

    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    glGenBuffers(1, &m_flagVBO);
    glGenBuffers(1, &m_flagIBO);
	
    m_flagGeometry = glmlv::makeFlag(pMats, m_width, m_height);

    glBindBuffer(GL_ARRAY_BUFFER, m_flagVBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_flagGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_flagGeometry.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_flagIBO);
    glBufferStorage(GL_ARRAY_BUFFER, m_flagGeometry.indexBuffer.size() * sizeof(uint32_t), m_flagGeometry.indexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Lets use a lambda to factorize VAO initialization:
    const auto initVAO = [positionAttrLocation, normalAttrLocation, texCoordsAttrLocation](GLuint& vao, GLuint vbo, GLuint ibo)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // We tell OpenGL what vertex attributes our VAO is describing:
        glEnableVertexAttribArray(positionAttrLocation);
        glEnableVertexAttribArray(normalAttrLocation);
        glEnableVertexAttribArray(texCoordsAttrLocation);

        glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

        glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
        glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
        glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

        glBindVertexArray(0);
    };

    initVAO(m_flagVAO, m_flagVBO, m_flagIBO);
}
