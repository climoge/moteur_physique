#include "Application.hpp"
#include "PMat.hpp"
#include "Link.hpp"

#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <random>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <glmlv/load_obj.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        const auto projMatrix = glm::perspective(70.f, float(m_nWindowWidth) / m_nWindowHeight, 0.01f * m_SceneSizeLength, m_SceneSizeLength);
        const auto viewMatrix = m_viewController.getViewMatrix();

        // Geometry pass
        {
            m_geometryPassProgram.use();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferFBO);

            glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //const auto modelMatrix = glm::scale(glm::mat4(), glm::vec3(0.05f, 0.05f, 0.05f)); // Used for cube.obj
            
            const auto modelMatrix = glm::mat4();
            const auto mvMatrix = viewMatrix * modelMatrix;
            const auto mvpMatrix = projMatrix * mvMatrix;
            const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

            glUniformMatrix4fv(m_uModelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(m_uModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(m_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glUniform3f(m_uCameraPositionLocation, viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]);

            // Same sampler for all texture units
            glBindSampler(0, m_textureSampler);
            glBindSampler(1, m_textureSampler);
            glBindSampler(2, m_textureSampler);
            glBindSampler(3, m_textureSampler);

            // Set texture unit of each sampler
            glUniform1i(m_uKaSamplerLocation, 0);
            glUniform1i(m_uKdSamplerLocation, 1);
            glUniform1i(m_uKsSamplerLocation, 2);
            glUniform1i(m_uShininessSamplerLocation, 3);
            glUniform1i(m_uNormalSamplerLocation, 4);

            const auto bindMaterial = [&](const PhongMaterial & material)
            {
                glUniform3fv(m_uKaLocation, 1, glm::value_ptr(material.Ka));
                glUniform3fv(m_uKdLocation, 1, glm::value_ptr(material.Kd));
                glUniform3fv(m_uKsLocation, 1, glm::value_ptr(material.Ks));
                glUniform1fv(m_uShininessLocation, 1, &material.shininess);
                glUniform3fv(m_uNormalLocation, 1, glm::value_ptr(material.normal));

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material.KaTextureId);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, material.KdTextureId);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, material.KsTextureId);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, material.shininessTextureId);
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, material.normalTextureId);
            };

            glBindVertexArray(m_SceneVAO);

            const PhongMaterial * currentMaterial = nullptr;

            // We draw each shape by specifying how much indices it carries, and with an offset in the global index buffer
            for (const auto shape : m_shapes)
            {
                const auto & material = shape.materialID >= 0 ? m_SceneMaterials[shape.materialID] : m_DefaultMaterial;
                if (currentMaterial != &material)
                {
                    bindMaterial(material);
                    currentMaterial = &material;
                }
                glDrawElements(GL_TRIANGLES, m_flagGeometry.indexBuffer.size(), GL_UNSIGNED_INT, nullptr);
            }

            for (GLuint i : {0, 1, 2, 3})
                glBindSampler(0, m_textureSampler);
        
            glBindVertexArray(0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }


		glBindFramebuffer(GL_FRAMEBUFFER, 0);




        // use G-buffer to render SSAO texture
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssaoPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GPosition]);
        glUniform1i(m_uSSAOGPosition,0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GNormal]);
        glUniform1i(m_uSSAOGNormal,1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,m_noiseTexture);
        glUniform1i(m_uSSAOGTexNoise, 2);


         const auto rcpProjMat = glm::inverse(projMatrix);

        const glm::vec4 frustrumTopRight(1, 1, 1, 1);
        const auto frustrumTopRight_view = rcpProjMat * frustrumTopRight;

        glUniform3fv(glGetUniformLocation(m_ssaoPass.glId(), "uSSAOGSceneSize"), 1, glm::value_ptr(frustrumTopRight_view / frustrumTopRight_view.w));

        for (GLuint i = 0; i < 64; ++i){
           glUniform3fv(glGetUniformLocation(m_ssaoPass.glId(), ("samples[" + std::to_string(i) + "]").c_str()), 1, &m_ssaoKernel[i][0]);
        }

        glUniformMatrix4fv(glGetUniformLocation(m_ssaoPass.glId(), "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));
        glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        
        // blur SSAO texture
       
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssaoBlurPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
        glUniform1i(glGetUniformLocation(m_ssaoBlurPass.glId(), "ssaoInput"),0);
        glBindVertexArray(m_TriangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);






        // Put here rendering code
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,  GL_FASTEST );

        if (m_CurrentlyDisplayed == GBufferTextureCount) // Beauty
        {
            // Shading pass
            {
                m_shadingPassProgram.use();

                glUniform3fv(m_uDirectionalLightDirLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
                glUniform3fv(m_uDirectionalLightIntensityLocation, 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

                glUniform3fv(m_uPointLightPositionLocation, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
                glUniform3fv(m_uPointLightIntensityLocation, 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

                int32_t i;

                for (i = GPosition; i < GDepth; ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);

                    glUniform1i(m_uGBufferSamplerLocations[i], i);
                }

                glBindVertexArray(m_TriangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);
            }
        }
        else if (m_CurrentlyDisplayed == GDepth)
        {
            m_displayDepthProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GDepth]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
        else if (m_CurrentlyDisplayed == GPosition)
        {
            m_displayPositionProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[GPosition]);

            glUniform1i(m_uGDepthSamplerLocation, 0);

            const auto rcpProjMat = glm::inverse(projMatrix);

            const glm::vec4 frustrumTopRight(1, 1, 1, 1);
            const auto frustrumTopRight_view = rcpProjMat * frustrumTopRight;

            glUniform3fv(m_uSceneSizeLocation, 1, glm::value_ptr(frustrumTopRight_view / frustrumTopRight_view.w));

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        //beauty + ssao
        else if (m_CurrentlyDisplayed == GSSAO)
        {
            m_displaySSAOProgram.use();

                glUniform3fv(glGetUniformLocation(m_displaySSAOProgram.glId(), "uDirectionalLightDir"), 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(glm::normalize(m_DirLightDirection), 0))));
                glUniform3fv(glGetUniformLocation(m_displaySSAOProgram.glId(), "uDirectionalLightIntensity"), 1, glm::value_ptr(m_DirLightColor * m_DirLightIntensity));

                glUniform3fv(glGetUniformLocation(m_displaySSAOProgram.glId(), "uPointLightPosition"), 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(m_PointLightPosition, 1))));
                glUniform3fv(glGetUniformLocation(m_displaySSAOProgram.glId(), "uPointLightIntensity"), 1, glm::value_ptr(m_PointLightColor * m_PointLightIntensity));

                int32_t i;

                for (i = GPosition; i < GDepth; ++i)
                {
                    glActiveTexture(GL_TEXTURE0 + i);
                    glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);

                    glUniform1i(m_uGBufferSamplerLocations[i], i);
                }
                i++;
               glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);
                glUniform1i(glGetUniformLocation(m_displaySSAOProgram.glId(), "uGSSAO"),i);

                glBindVertexArray(m_TriangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
                glBindVertexArray(0);

            /* m_displaySSAOProgram.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);
            glUniform1i(glGetUniformLocation(m_displaySSAOProgram.glId(), "uGSSAOSampler"),0);

            glBindVertexArray(m_TriangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);*/

        }
        else
        {
            // GBuffer display
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBufferFBO);
            glReadBuffer(GL_COLOR_ATTACHMENT0 + m_CurrentlyDisplayed);
            glBlitFramebuffer(0, 0, m_nWindowWidth, m_nWindowHeight,
                0, 0, m_nWindowWidth, m_nWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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
            if (ImGui::Button("Sort shapes wrt materialID"))
            {
                std::sort(begin(m_shapes), end(m_shapes), [&](auto lhs, auto rhs)
                {
                    return lhs.materialID < rhs.materialID;
                });
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

            if (ImGui::CollapsingHeader("GBuffer"))
            {
                for (int32_t i = GPosition; i <= GBufferTextureCount; ++i)
                {
                    if (ImGui::RadioButton(m_GBufferTexNames[i], m_CurrentlyDisplayed == i))
                        m_CurrentlyDisplayed = GBufferTextureType(i);
                }
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
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    initScene();
    initShadersData();
    
    

    glEnable(GL_DEPTH_TEST);
    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,  GL_FASTEST );
    m_viewController.setSpeed(m_SceneSizeLength * 0.1f); // Let's travel 10% of the scene per second

    // Init GBuffer
    glGenTextures(GBufferTextureCount, m_GBufferTextures);

    for (int32_t i = GPosition; i < GBufferTextureCount; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_GBufferTextures[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, m_GBufferTextureFormat[i], m_nWindowWidth, m_nWindowHeight);
    }

    glGenFramebuffers(1, &m_GBufferFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_GBufferFBO);
    for (int32_t i = GPosition; i < GDepth; ++i)
    {
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_GBufferTextures[i], 0);
    }
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_GBufferTextures[GDepth], 0);

	

    // we will write into 6 textures from the fragment shader
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
    glDrawBuffers(6, drawBuffers);

	
    GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FB error, status: " << status << std::endl;
        throw std::runtime_error("FBO error");
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glGenBuffers(1, &m_TriangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_TriangleVBO);

    GLfloat data[] = { -1, -1, 3, -1, -1, 3 };
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(data), data, 0);

    glGenVertexArrays(1, &m_TriangleVAO);
    glBindVertexArray(m_TriangleVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    computeSSAO();
    
    
}

void Application::initScene()
{
    glGenBuffers(1, &m_SceneVBO);
    glGenBuffers(1, &m_SceneIBO);

    {
		const GLint positionAttrLocation = 0;
		const GLint normalAttrLocation = 1;
		const GLint texCoordsAttrLocation = 2;
    
        m_SceneSize = glm::vec3(20,20,20);
        std::cout << "SceneSize " << m_SceneSize << std::endl;
        m_SceneSizeLength = glm::length(m_SceneSize);
        
        m_flagGeometry = glmlv::makeFlag();

        // Fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);
        glBufferStorage(GL_ARRAY_BUFFER, m_flagGeometry.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), m_flagGeometry.vertexBuffer.data(), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Fill IBO
        glBindBuffer(GL_ARRAY_BUFFER, m_SceneIBO);
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
		
		initVAO(m_SceneVAO, m_SceneVBO, m_SceneIBO);

        glGenTextures(1, &m_WhiteTexture);
        glBindTexture(GL_TEXTURE_2D, m_WhiteTexture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1, 1);
        glm::vec4 white(1.f, 1.f, 1.f, 1.f);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &white);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_DefaultMaterial.Ka = glm::vec3(0);
        m_DefaultMaterial.Kd = glm::vec3(1);
        m_DefaultMaterial.Ks = glm::vec3(1);
        m_DefaultMaterial.shininess = 32.f;
        m_DefaultMaterial.KaTextureId = m_WhiteTexture;
        m_DefaultMaterial.KdTextureId = m_WhiteTexture;
        m_DefaultMaterial.KsTextureId = m_WhiteTexture;
        m_DefaultMaterial.normalTextureId = m_WhiteTexture;
        m_DefaultMaterial.shininessTextureId = m_WhiteTexture;
    }

    // Fill VAO
    glGenVertexArrays(1, &m_SceneVAO);
    glBindVertexArray(m_SceneVAO);

    const GLint positionAttrLocation = 0;
    const GLint normalAttrLocation = 1;
    const GLint texCoordsAttrLocation = 2;

    // We tell OpenGL what vertex attributes our VAO is describing:
    glEnableVertexAttribArray(positionAttrLocation);
    glEnableVertexAttribArray(normalAttrLocation);
    glEnableVertexAttribArray(texCoordsAttrLocation);

    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO); // We bind the VBO because the next 3 calls will read what VBO is bound in order to know where the data is stored

    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, position));
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, normal));
    glVertexAttribPointer(texCoordsAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*)offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0); // We can unbind the VBO because OpenGL has "written" in the VAO what VBO it needs to read when the VAO will be drawn

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SceneIBO); // Binding the IBO to GL_ELEMENT_ARRAY_BUFFER while a VAO is bound "writes" it in the VAO for usage when the VAO will be drawn

    glBindVertexArray(0);

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_textureSampler);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Application::initShadersData()
{
    m_geometryPassProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "geometryPass.vs.glsl", m_ShadersRootPath / m_AppName / "geometryPass.fs.glsl" });

    m_ssaoPass = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "ssao.vs.glsl", m_ShadersRootPath / m_AppName / "ssao.fs.glsl" });
    m_ssaoBlurPass = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "ssao.vs.glsl", m_ShadersRootPath / m_AppName / "ssao_blur.fs.glsl" });

    m_uModelViewProjMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uModelViewMatrix");
    m_uNormalMatrixLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uNormalMatrix");

    m_uKaLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKa");
    m_uKdLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKd");
    m_uKsLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKs");
    m_uNormalLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uNormal");
    m_uShininessLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uShininess");
    m_uCameraPositionLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uCameraPosition");
    m_uKaSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKaSampler");
    m_uKdSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKdSampler");
    m_uKsSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uKsSampler");
    m_uShininessSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uShininessSampler");
    m_uNormalSamplerLocation = glGetUniformLocation(m_geometryPassProgram.glId(), "uNormalSampler");

    m_shadingPassProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "shadingPass.fs.glsl" });

    m_uGBufferSamplerLocations[GPosition] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGPosition");
    m_uGBufferSamplerLocations[GNormal] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGNormal");
    m_uGBufferSamplerLocations[GAmbient] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGAmbient");
    m_uGBufferSamplerLocations[GDiffuse] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGDiffuse");
    m_uGBufferSamplerLocations[GGlossyShininess] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGGlossyShininess");
	m_uGBufferSamplerLocations[GSSAO] = glGetUniformLocation(m_shadingPassProgram.glId(), "uGSSAO");

    
    m_uDirectionalLightDirLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightDir");
    m_uDirectionalLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uDirectionalLightIntensity");

    m_uPointLightPositionLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightPosition");
    m_uPointLightIntensityLocation = glGetUniformLocation(m_shadingPassProgram.glId(), "uPointLightIntensity");

    m_displayDepthProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayDepth.fs.glsl" });

    m_uGDepthSamplerLocation = glGetUniformLocation(m_displayDepthProgram.glId(), "uGDepth");

    m_displayPositionProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displayPosition.fs.glsl" });
    m_displaySSAOProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "shadingPass.vs.glsl", m_ShadersRootPath / m_AppName / "displaySSAO.fs.glsl" });

    //ssao uniforms
    m_uSSAOGPosition = glGetUniformLocation(m_ssaoPass.glId(), "uSSAOGPosition");
    m_uSSAOGNormal = glGetUniformLocation(m_ssaoPass.glId(), "uSSAOGNormal");
    m_uSSAOGTexNoise = glGetUniformLocation(m_ssaoPass.glId(), "uSSAOGTexNoise");

    m_uGPositionSamplerLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uGPosition");
    m_uSceneSizeLocation = glGetUniformLocation(m_displayPositionProgram.glId(), "uSceneSize");

	

}

void Application::computeSSAO() {
	//generate ssao fbos for processing
    
	glGenFramebuffers(1, &m_ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);

	//ssao color buffer
	glGenTextures(1, &m_ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_nWindowWidth, m_nWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ssao framebuffer not complete !" << std::endl;
    //ssao BLUR color buffer
    glGenFramebuffers(1, &m_ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    glGenTextures(1, &m_ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, m_ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_nWindowWidth, m_nWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ssao blur framebuffer not complete !" << std::endl;

    glBindBuffer(GL_FRAMEBUFFER, 0);
	
	//Sample Kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	
	for (GLuint i = 0; i < 64; i++) {
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator)*2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		GLfloat scale = GLfloat(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale*scale);
		sample *= scale;
		m_ssaoKernel.push_back(sample);

	}

	// Noise texture

	for (GLuint i = 0; i < 16; i++) {
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator)*2.0 - 1.0, 0.0f);
		m_ssaoNoise.push_back(noise);
        //m_ssaoNoise.push_back(glm::vec3(1, 1, 1));
	}

    glGenTextures(1, &m_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);
    /*glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 4, 4);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, &m_ssaoNoise);*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
}

//utility lerp function
GLfloat Application::lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}
