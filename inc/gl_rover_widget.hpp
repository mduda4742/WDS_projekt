#ifndef GL_ROVER_WIDGET_HPP
#define GL_ROVER_WIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QString>
#include <QVector3D>
#include <QVector>

class GLRoverWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    explicit GLRoverWidget(QWidget *parent = nullptr);
    ~GLRoverWidget() override;

    void resetCamera();
    void setTargetCenter(float x, float y, float z);

protected:

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL()  override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
     
    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    float m_xRot;
    float m_yRot;
    float m_zoom;

    float m_targetX, m_targetY, m_targetZ;

    QPoint m_lastMousePos;

    int m_vertexCount;

    bool loadOBJ(const QString &path);

    void loadDummyData();
};

#endif