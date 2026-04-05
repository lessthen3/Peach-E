/*******************************************************************
 *                        Peach-E v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
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

         PeachMesh() = default;

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