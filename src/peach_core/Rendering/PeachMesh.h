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

///OpenGL
#ifndef __APPLE__
#include <GL/glew.h>
#endif

///External
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

///PeachCore
#include "PeachMaterial.h"

namespace PeachCore {

    struct PeachMesh
    {
         int pm_VAO;
         int pm_VBO;
         int pm_VertexCount;

         vector<int> pm_ListVBO;

         PeachMaterial pm_Material;

        //////////////////////////////////////////////
        //CLASS CONSTRUCTOR
        //////////////////////////////////////////////

         PeachMesh(float fp_Vertices[], int fp_Indices[], float fp_TextCoords[], float fp_Normals[])
         {

            FloatBuffer verticesBuffer = null;
            IntBuffer indicesBuffer = null;
            FloatBuffer textCoordsBuffer = null;
            FloatBuffer normalsBuffer = null;

            try {
                pm_VertexCount = PEACH_ARRAY_SIZE(fp_Indices);

                pm_VAO = glGenVertexArrays();
                glBindVertexArray(pm_VAO);

                pm_VBO = glGenBuffers();
                pm_ListVBO.add(pm_VBO);
                verticesBuffer = MemoryUtil.memAllocFloat(fp_Vertices.length);
                verticesBuffer.put(fp_Vertices).flip();
                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, verticesBuffer, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

                pm_VBO = glGenBuffers();
                pm_ListVBO.add(pm_VBO);
                indicesBuffer = MemoryUtil.memAllocInt(fp_Indices.length);
                indicesBuffer.put(fp_Indices).flip();
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer, GL_STATIC_DRAW);

                pm_VBO = glGenBuffers();
                pm_ListVBO.add(pm_VBO);
                textCoordsBuffer = MemoryUtil.memAllocFloat(fp_TextCoords.length);
                textCoordsBuffer.put(fp_TextCoords).flip();
                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, textCoordsBuffer, GL_STATIC_DRAW);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

                pm_VBO = glGenBuffers();
                pm_ListVBO.add(pm_VBO);
                normalsBuffer = MemoryUtil.memAllocFloat(fp_Normals.length);
                normalsBuffer.put(fp_Normals).flip();
                glBindBuffer(GL_ARRAY_BUFFER, pm_VBO);
                glBufferData(GL_ARRAY_BUFFER, normalsBuffer, GL_STATIC_DRAW);
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); PROBLEM LINE BREAKS OPENGL IDK WHY

                glBindVertexArray(0);
            }
            finally {
                if (verticesBuffer != null) { MemoryUtil.memFree(verticesBuffer); }

                if (indicesBuffer != null) { MemoryUtil.memFree(indicesBuffer); }

                if (textCoordsBuffer != null) { MemoryUtil.memFree(textCoordsBuffer); }

                if (normalsBuffer != null) { MemoryUtil.memFree(normalsBuffer); }
            }
        }

         void
             Render()
             const
         {
            const PeachTexture* f_Texture = pm_Material.GetTexture();

            if (f_Texture != nullptr)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, f_Texture->GetTextureID());
            }

            glBindVertexArray(pm_VAO);

            glDrawElements(GL_TRIANGLES, pm_VertexCount, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

         [[nodiscard]] PeachMaterial* 
             GetMaterial() 
             const noexcept
         {
            return &pm_Material;
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

            for (int vbo_id : pm_ListVBO)
            { 
                glDeleteBuffers(vbo_id);
            } //delete all vboid buffers

            const PeachTexture* f_Texture = pm_Material.GetTexture();

            if (f_Texture != nullptr)
            { 
                f_Texture->CleanUp(); 
            } //cleanup textures

            glBindVertexArray(0);
            glDeleteVertexArrays(pm_VAO);
        }
    };

}