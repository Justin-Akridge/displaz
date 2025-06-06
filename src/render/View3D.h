// Copyright 2015, Christopher J. Foster and the other displaz contributors.
// Use of this code is governed by the BSD-style license found in LICENSE.txt

#pragma once

#include <cmath>
#include <vector>
#include <memory>
#include <optional>

#include "glutil.h"
#define QT_NO_OPENGL_ES_2


#include <QVector>
#include <QGLWidget>
#include <QModelIndex>
#include <Eigen/Dense>

#include "DrawCostModel.h"
#include "InteractiveCamera.h"
#include "geometrycollection.h"
#include "Annotation.h"
#include "Sphere.h"
#include "Enable.h"
#include "ShaderProgram.h"

class QOpenGLShaderProgram;
class QItemSelectionModel;
class QTimer;
class QGLFormat;
class QSettings;

class MainWindow;
class DataSetUI;
class Enable;
class ShaderProgram;
struct TransformState;

//------------------------------------------------------------------------------
/// OpenGL-based viewer widget for point clouds
class View3D : public QGLWidget
{
    Q_OBJECT
    public:
        View3D(GeometryCollection* geometries, const QGLFormat& format, MainWindow *parent = nullptr, DataSetUI *dataSet = nullptr);
        ~View3D() = default;

        Enable& enable() const { return *m_enable; }

        /// Return shader used for displaying points
        ShaderProgram& shaderProgram() const { return *m_shaderProgram; }

        void setShaderParamsUIWidget(QWidget* widget);

        InteractiveCamera& camera() { return m_camera; }

        QColor background() const { return m_backgroundColor; }

        Imath::V3d cursorPos() const { return m_cursorPos; }

        /// Return current selection of loaded files
        const QItemSelectionModel* selectionModel() const { return m_selectionModel; }
        QItemSelectionModel* selectionModel() { return m_selectionModel; }
        void setSelectionModel(QItemSelectionModel* selectionModel);

        void addAnnotation(const QString& label, const QString& text,
                           const Imath::V3d& pos);

        void addSphere(const Imath::V3d& pos);

        /// Remove all annotations who's label matches the given QRegExp
        void removeAnnotations(const QRegExp& labelRegex);

        // Actions
        QAction* m_boundingBoxAction = nullptr;
        QAction* m_cursorAction = nullptr;
        QAction* m_axesAction = nullptr;
        QAction* m_gridAction = nullptr;
        QAction* m_annotationAction = nullptr;

        /// Settings
        void readSettings(const QSettings& settings);
        void writeSettings(QSettings& settings) const;

        Geometry* currentGeometry() const;

        void setPoles(const std::vector<Eigen::Vector3d>& poles);

        size_t poleCount() const
        {
            return m_poles.size();
        }
        Eigen::Vector3d getPoleAt(size_t idx) const
        {
            if (idx < m_poles.size())
                return m_poles[idx];
            return Eigen::Vector3d(0,0,0); // or throw
        }
        void removePoleAt(int idx) {
          if (idx >= 0 && idx < m_poles.size())
              m_poles.erase(m_poles.begin() + idx);
        }


    public slots:
        /// Set the backgroud color
        void setBackground(QColor col);
        /// Centre on loaded geometry file at the given index
        void centerOnGeometry(const QModelIndex& index);
        void centerOnPoint(const Imath::V3d& pos);
        void setExplicitCursorPos(const Imath::V3d& pos);

    protected:
        // Qt OpenGL callbacks
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

        // Qt event callbacks
        void mouseDoubleClickEvent(QMouseEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void wheelEvent(QWheelEvent* event);
        void keyPressEvent(QKeyEvent* event);

    private slots:
        void restartRender();
        void setupShaderParamUI();

        void geometryChanged();
        void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
        void geometryInserted(const QModelIndex&, int firstRow, int lastRow);

        void setBoundingBox(bool);
        void setCursor(bool);
        void setAxes(bool);
        void setGrid(bool);
        void setAnnotations(bool);

    private:
        std::vector<Eigen::Vector3d> m_poles;
        void renderPoles();
        double getDevicePixelRatio();
        void initializeGLGeometry(int begin, int end);

        void initCursor(float cursorRadius, float centerPointRadius);
        void drawCursor(const TransformState& transState, const V3d& P, float centerPointRadius) const;

        void initAxes();
        void drawAxes();

        void initGrid(const float scale);
        void drawGrid() const;

        void drawText(const QString& text);

        DrawCount drawPoints(const TransformState& transState,
                             const std::vector<const Geometry*>& geoms,
                             double quality, bool incrementalDraw);

        void drawMeshes(const TransformState& transState,
                        const std::vector<const Geometry*>& geoms) const;
        void drawAnnotations(const TransformState& transState,
                             int viewportPixelWidth,
                             int viewportPixelHeight) const;

        void drawSpheres(const TransformState& transState,
                             int viewportPixelWidth,
                             int viewportPixelHeight) const;

        Imath::V3d guessClickPosition(const QPoint& clickPos);

        bool snapToGeometry(const Imath::V3d& pos, double normalScaling,
                            Imath::V3d* newPos, QString* pointInfo);

        std::optional<Imath::V3d> getClosestPoint(const Imath::V3d& pos, QString* outPointInfo);

        void snapToPoint(const Imath::V3d& pos);
        std::vector<const Geometry*> selectedGeometry() const;

        MainWindow* m_mainWindow = nullptr;
        DataSetUI* m_dataSet = nullptr;
        /// Mouse-based camera positioning
        InteractiveCamera m_camera;
        QPoint m_prevMousePos;
        Qt::MouseButton m_mouseButton;
        bool m_middleButton;
        // if true, an explicit cursor position has been specified
        bool m_explicitCursorPos;
        /// Position of 3D cursor
        V3d m_cursorPos;
        std::optional<V3d> m_prevCursorSnap;
        /// Background color for drawing
        QColor m_backgroundColor;
        /// Option to draw bounding boxes of point clouds
        bool m_drawBoundingBoxes = false;
        bool m_drawCursor = true;
        bool m_drawAxes = true;
        bool m_drawGrid = false;
        bool m_drawAnnotations = true;
        /// If true, OpenGL initialization didn't work properly
        bool m_badOpenGL;
        /// Shader for point clouds
        std::unique_ptr<ShaderProgram> m_shaderProgram;
        std::unique_ptr<Enable>        m_enable;
        /// Shaders for polygonal geometry
        std::unique_ptr<ShaderProgram> m_meshFaceShader;
        std::unique_ptr<ShaderProgram> m_meshEdgeShader;
        /// Collection of geometries
        GeometryCollection* m_geometries;
        QItemSelectionModel* m_selectionModel = nullptr;

        QVector<std::shared_ptr<Annotation>> m_annotations;
        QVector<std::shared_ptr<Sphere>> m_spheres;

        /// UI widget for shader
        QWidget* m_shaderParamsUI;
        /// Timer for next incremental frame
        QTimer* m_incrementalFrameTimer;
        Framebuffer m_incrementalFramebuffer;
        bool m_incrementalDraw;
        /// Controller for amount of geometry to draw
        DrawCostModel m_drawCostModel;
        /// GL textures
        std::unique_ptr<QOpenGLTexture> m_drawAxesBackground;
        std::unique_ptr<QOpenGLTexture> m_drawAxesLabelX;
        std::unique_ptr<QOpenGLTexture> m_drawAxesLabelY;
        std::unique_ptr<QOpenGLTexture> m_drawAxesLabelZ;

        /// Shaders for interface geometry
        std::unique_ptr<ShaderProgram> m_cursorShader;
        std::unique_ptr<ShaderProgram> m_axesShader;
        std::unique_ptr<ShaderProgram> m_gridShader;
        std::unique_ptr<ShaderProgram> m_axesBackgroundShader;
        std::unique_ptr<ShaderProgram> m_axesLabelShader;
        std::unique_ptr<ShaderProgram> m_boundingBoxShader;
        std::unique_ptr<ShaderProgram> m_annotationShader;
        std::unique_ptr<ShaderProgram> m_sphereShader;

        GLuint m_cursorVertexArray = 0;
        GLuint m_axesVertexArray = 0;
        GLuint m_gridVertexArray = 0;
        GLuint m_quadVertexArray = 0;
        GLuint m_quadLabelVertexArray = 0;


        double m_devicePixelRatio;
};
