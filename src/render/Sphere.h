#ifndef DISPLAZ_SPHERE_H_INCLUDED
#define DISPLAZ_SPHERE_H_INCLUDED


#include <memory>
#include "glutil.h"
#include <QString>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>


struct TransformState;

/// Render a 3D sphere at a specified position
class Sphere
{
public:
    Sphere(GLuint shaderProgram, Imath::V3d position, float radius = 1.0f, int stacks = 16, int slices = 16);
    ~Sphere();

    void draw(QOpenGLShaderProgram& shaderProgram, const TransformState& transState) const;

private:
    void generateGeometry(int stacks, int slices);
    void setupBuffers();

    Imath::V3d m_position;
    float m_radius;

    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint m_vao = 0;
    GlBuffer m_vbo;
    GlBuffer m_ebo;
    std::vector<float> m_normals;
    GlBuffer m_nbo;  // Normal buffer object

};

#endif // DISPLAZ_SPHERE_H_INCLUDED

