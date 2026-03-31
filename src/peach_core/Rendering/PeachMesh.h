/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///External
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

///PeachCore
#include "PeachMaterial.h"

#define PEACH_SIZE_OF_ARRAY_PTR(fp_Type, fp_Ptr)  sizeof(*fp_Ptr) / sizeof(fp_Type)

///OpenGL
#ifndef __APPLE__

#include <GL/glew.h>

namespace PeachCore::OpenGL {

    struct PeachMesh
    {
         GLuint pm_VAO;
         GLuint pm_VBO;
         uint64_t pm_VertexCount;

         vector<GLuint> pm_ListVBO;

         vector<PeachMaterial> pm_Materials;

        //////////////////////////////////////////////
        //CLASS CONSTRUCTOR
        //////////////////////////////////////////////

         PeachMesh
         (
             float* fp_Vertices, 
             int* fp_Indices, 
             float* fp_TextCoords, 
             float* fp_Normals
         )
         {
            try 
            {
                pm_VertexCount = PEACH_SIZE_OF_ARRAY_PTR(int, fp_Indices);

                glGenVertexArrays(1, &pm_VAO);
                glBindVertexArray(pm_VAO);

                glGenBuffers(1, &pm_VBO);
                pm_ListVBO.push_back(pm_VBO);

                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, PEACH_SIZE_OF_ARRAY_PTR(float, fp_Vertices), fp_Vertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

                // Indices EBO (ELEMENT_ARRAY_BUFFER) - stored in VAO state
                {
                    glGenBuffers(1, &pm_VBO);
                    pm_ListVBO.push_back(pm_VBO);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm_VBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, PEACH_SIZE_OF_ARRAY_PTR(int, fp_Indices), fp_Indices, GL_STATIC_DRAW);

                    glGenBuffers(1, &pm_VBO);
                    pm_ListVBO.push_back(pm_VBO);
                }

                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, PEACH_SIZE_OF_ARRAY_PTR(float, fp_TextCoords), fp_TextCoords, GL_STATIC_DRAW);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

                glGenBuffers(1, &pm_VBO);
                pm_ListVBO.push_back(pm_VBO);

                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, PEACH_SIZE_OF_ARRAY_PTR(float, fp_Normals), fp_Normals, GL_STATIC_DRAW);
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); PROBLEM LINE BREAKS OPENGL IDK WHY

                glBindVertexArray(0);
            }
            catch (const exception& fp_Exception)
            {
                PeachCore::PRINT_ERROR(fmt::format("Unhandled exception: {}", fp_Exception.what()));

            }
        }

         void
             Render()
             const
         {
            const uint64_t f_Texture = pm_Material.GetTextureID();

            if (f_Texture != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, f_Texture);
            }

            glBindVertexArray(pm_VAO);

            glDrawElements(GL_TRIANGLES, pm_VertexCount, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

         [[nodiscard]] const PeachMaterial&
             GetMaterial() 
             const noexcept
         {
            return pm_Material;
        }

         void 
             SetMaterial(const PeachMaterial& fp_Material) 
         {
            pm_Material = fp_Material;
        }

         int 
             GetVAO()
             const noexcept
         {
            return pm_VAO;
        }

         int 
             GetVertexCount() 
             const noexcept
         {
            return pm_VertexCount;
        }

         void 
             CleanUp() 
         {
            glDisableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            for (const GLuint vbo_id : pm_ListVBO)
            { 
                glDeleteBuffers(1, &vbo_id);
            } //delete all vboid buffers

            const uint64_t f_Texture = pm_Material.GetTextureID();

            if (f_Texture != 0)
            { 
                //f_Texture->CleanUp(); 
            } //cleanup textures

            glBindVertexArray(0);
            glDeleteVertexArrays(1, &pm_VAO);
        }
    };
}
#endif

namespace PeachCore::Vulkan {

    struct Mesh
    {

    };
}