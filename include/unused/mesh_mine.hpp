#pragma once

struct Mesh {
    void init() {
        struct Vertex { // sizeof(Vertex) = sizeof(float) * 4 = 32 bytes
            float x, y, z, w; // position 16 Bytes
            float r, g, b, a; // color 16 Bytes
            float tx, ty;
        };
        // Square:
        // std::vector<Vertex> vertices = {
        //     Vertex{ -0.5, -0.5, +0.0, +1.0, 0, 0, 1, 1}, // bottom left
        //     Vertex{ -0.5, +0.5, +0.0, +1.0, 1, 0, 0, 1}, // top left
        //     Vertex{ +0.5, +0.5, +0.0, +1.0, 0, 1, 0, 1}, // top right
        //     Vertex{ +0.5, +0.5, +0.0, +1.0, 0, 1, 0, 1}, // top right
        //     Vertex{ +0.5, -0.5, +0.0, +1.0, 1, 1, 1, 1 }, // bottom right
        //     Vertex{ -0.5, -0.5, +0.0, +1.0, 0, 0, 1, 1}, // bottom left
        // };

        // Cube:
        std::vector<Vertex> vertices = {
            // Front Face (+Z) - Red
            Vertex{ -0.5f, -0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.75f }, // Bottom-left
            Vertex{ +0.5f, -0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.75f }, // Bottom-right
            Vertex{ +0.5f, +0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.5f }, // Top-right
            Vertex{ +0.5f, +0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.5f }, // Top-right
            Vertex{ -0.5f, +0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.5f }, // Top-left
            Vertex{ -0.5f, -0.5f, +0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.75f }, // Bottom-left

            // Back Face (-Z) - Red
            Vertex{ -0.5f, -0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.0f }, // Bottom-left
            Vertex{ -0.5f, +0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.25f }, // Top-left
            Vertex{ +0.5f, +0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.25f }, // Top-right
            Vertex{ +0.5f, +0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.25f }, // Top-right
            Vertex{ +0.5f, -0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.66f, 0.0f }, // Bottom-right
            Vertex{ -0.5f, -0.5f, -0.5f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.33f, 0.0f }, // Bottom-left

            // Left Face (-X) - Blue
            Vertex{ -0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.33f, 0.5f }, // Top-front
            Vertex{ -0.5f, +0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.33f, 0.25f }, // Top-back
            Vertex{ -0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.25f }, // Bottom-back
            Vertex{ -0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.25f }, // Bottom-back
            Vertex{ -0.5f, -0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.5f }, // Bottom-front
            Vertex{ -0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.33f, 0.5f }, // Top-front

            // Right Face (+X) - Blue
            Vertex{ +0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.66f, 0.5f }, // Top-front
            Vertex{ +0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.25f }, // Bottom-back
            Vertex{ +0.5f, +0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.66f, 0.25f }, // Top-back
            Vertex{ +0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.25f }, // Bottom-back
            Vertex{ +0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.66f, 0.5f }, // Top-front
            Vertex{ +0.5f, -0.5f, +0.5f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.5f }, // Bottom-front

            // Top Face (+Y) - Cyan
            Vertex{ -0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 0.25f }, // Front-left
            Vertex{ +0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 0.25f }, // Front-right
            Vertex{ +0.5f, +0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 0.5f }, // Back-right
            Vertex{ +0.5f, +0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 0.5f }, // Back-right
            Vertex{ -0.5f, +0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 0.5f }, // Back-left
            Vertex{ -0.5f, +0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 0.25f }, // Front-left

            // Bottom Face (-Y) - Cyan
            Vertex{ -0.5f, -0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 0.75f }, // Front-left
            Vertex{ -0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 1.0f }, // Back-left
            Vertex{ +0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 1.0f }, // Back-right
            Vertex{ +0.5f, -0.5f, -0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 1.0f }, // Back-right
            Vertex{ +0.5f, -0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.66f, 0.75f }, // Front-right
            Vertex{ -0.5f, -0.5f, +0.5f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  0.33f, 0.75f }  // Front-left
        };

        // create GPU buffer to store vertices
        glCreateBuffers(1, &_buffer_vertex);
        // upload to GPU
        glNamedBufferStorage(_buffer_vertex, 
            vertices.size() * sizeof(Vertex), // number of bytes to upload
            vertices.data(), // raw data to upload
            BufferStorageMask::GL_NONE_BIT);
        
        // create index buffer the same way
        std::vector<GLuint> indices = { 
            0, 1, 2, 3, 4, 5 , 
            6, 7, 8, 9, 10, 11, 
            12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23,
            24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35
        };
        // std::vector<GLuint> indices(vertices.size()); 
        // std::iota(indices.begin(), indices.end(), 0u);
        glCreateBuffers(1, &_buffer_index);
        glNamedBufferStorage(_buffer_index,
            indices.size() * sizeof(GLuint),
            indices.data(),
            BufferStorageMask::GL_NONE_BIT);

        // create vertex array (basically a mesh)
        glCreateVertexArrays(1, &_buffer_mesh);
        // assign vertex and index buffer
        glVertexArrayVertexBuffer(_buffer_mesh, 0, _buffer_vertex, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(_buffer_mesh, _buffer_index);
        // describe the data inside each Vertex
         
        // describes vertex::position
        glVertexArrayAttribFormat(_buffer_mesh, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(_buffer_mesh, 0, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 0);
        // describes vertex::color
        glVertexArrayAttribFormat(_buffer_mesh, 1, 4, GL_FLOAT, GL_FALSE, 16 /*offset inside*/);
        glVertexArrayAttribBinding(_buffer_mesh, 1, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 1);
        // describes vertex::texture 
        glVertexArrayAttribFormat(_buffer_mesh, 2, 2, GL_FLOAT, GL_FALSE, 32 /*offset inside*/);
        glVertexArrayAttribBinding(_buffer_mesh, 2, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 2);

        _index_count = indices.size();
    }
    void destroy() {
        // TODO
    }
    void bind() {
        glBindVertexArray(_buffer_mesh);
    }
    void draw() {
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, nullptr);
    }
    
    GLuint _buffer_mesh; // GPU buffer, don't want it to be on the CPU
    GLuint _buffer_index; // element = index
    GLuint _buffer_vertex;
    GLuint _index_count;
};