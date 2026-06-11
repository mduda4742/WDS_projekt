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

/**
 * @class GLRoverWidget
 * @brief Widget for 3D visualization of the rover using OpenGL.
 * * This class provides a graphical viewport to render the 3D model 
 * of the rover. It handles basic camera controls like rotation and zoom.
 */
class GLRoverWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for GLRoverWidget.
     * @param parent - pointer to the parent widget (default is nullptr).
     */
    explicit GLRoverWidget(QWidget *parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    ~GLRoverWidget() override;

    /**
     * @brief Resets the camera to its default rotation and zoom levels.
     */
    void resetCamera();

    /**
     * @brief Sets the focal point for the camera.
     * @param x - X-coordinate of the target.
     * @param y - Y-coordinate of the target.
     * @param z - Z-coordinate of the target.
     */
    void setTargetCenter(float x, float y, float z);

protected:
    /**
     * @brief Initializes the OpenGL environment, shaders, and buffers.
     */
    void initializeGL() override;

    /**
     * @brief Updates the viewport when the widget is resized.
     * @param w - new width.
     * @param h - new height.
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief Renders the 3D scene and the rover model.
     */
    void paintGL() override;

    /**
     * @brief Records the initial mouse position for camera rotation.
     * @param event - mouse event payload.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Calculates and applies camera rotation based on mouse drag.
     * @param event - mouse event payload.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Applies camera zoom based on the scroll wheel movement.
     * @param event - wheel event payload.
     */
    void wheelEvent(QWheelEvent *event) override;

private:
    QOpenGLShaderProgram *m_program; /**< Shader program used for rendering the model. */
    QOpenGLVertexArrayObject m_vao;  /**< Vertex Array Object for the 3D model. */
    QOpenGLBuffer m_vbo;             /**< Vertex Buffer Object storing model geometry. */

    float m_xRot;                    /**< Camera rotation around the X-axis [deg]. */
    float m_yRot;                    /**< Camera rotation around the Y-axis [deg]. */
    float m_zoom;                    /**< Current camera zoom level. */

    float m_targetX;                 /**< Camera target center X-coordinate. */
    float m_targetY;                 /**< Camera target center Y-coordinate. */
    float m_targetZ;                 /**< Camera target center Z-coordinate. */

    QPoint m_lastMousePos;           /**< Last recorded mouse position for drag calculations. */

    int m_vertexCount;               /**< Total number of vertices to render. */

    /**
     * @brief Parses and loads a 3D model from an OBJ file.
     * @param path - file path to the .obj model.
     * @return true if the model loaded successfully, false otherwise.
     */
    bool loadOBJ(const QString &path);

    /**
     * @brief Dummy data for testing
     */
    void loadDummyData();
};

#endif