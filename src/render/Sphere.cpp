#include "Sphere.h"

Sphere::Sphere(GLuint shaderProgram, Imath::V3d position, float radius, int stacks, int slices)
    : m_position(position), m_radius(radius)
{
    generateGeometry(stacks, slices);
    setupBuffers();
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &m_vao);
}

void Sphere::generateGeometry(int stacks, int slices)
{

    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;
            float x = m_radius * sinf(phi) * cosf(theta);
            float y = m_radius * cosf(phi);
            float z = m_radius * sinf(phi) * sinf(theta);
            m_vertices.insert(m_vertices.end(), { x, y, z });

            // Normal = normalized position vector since sphere center is at origin
            float length = sqrt(x*x + y*y + z*z);
            m_normals.insert(m_normals.end(), { x/length, y/length, z/length });
        }
    }

    for (int i = 0; i < stacks; ++i) {
      for (int j = 0; j < slices; ++j) {
          int first = i * (slices + 1) + j;
          int second = first + slices + 1;

          m_indices.push_back(first);
          m_indices.push_back(second);
          m_indices.push_back(first + 1);

          m_indices.push_back(second);
          m_indices.push_back(second + 1);
          m_indices.push_back(first + 1);
      }
    }
}

void Sphere::setupBuffers()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Vertex buffer for sphere positions
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Normal buffer for sphere normals
    GLuint nbo;
    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float), m_normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Element buffer for indices
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}


void Sphere::draw(QOpenGLShaderProgram& shaderProgram, const TransformState& transState) const
{
    glBindVertexArray(m_vao);
    shaderProgram.bind();

    // Set model-view-projection and other uniforms
    transState.translate(m_position).setUniforms(shaderProgram.programId());

    // Draw tessellated sphere
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

