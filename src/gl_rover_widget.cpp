#include "gl_rover_widget.hpp"

// Proste Shadery zaszyte w kodzie (dla wygody na start)
// Vertex Shader: Liczy matematykę macierzy i pozycję na ekranie
static const char *vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 mvpMatrix;\n"
    "void main() {\n"
    "   gl_Position = mvpMatrix * vec4(vertexPosition, 1.0);\n"
    "}\n";

// Fragment Shader: Koloruje to, co narysowaliśmy (np. na pomarańczowo)
static const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = vec4(1.0, 0.5, 0.2, 1.0);\n" // Kolor RGBA (Pomarańczowy)
    "}\n";

GLRoverWidget::GLRoverWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(nullptr),
      m_xRot(30.0f),
      m_yRot(-45.0f),
      m_zoom(5.0f),
      m_targetX(0.0f), m_targetY(0.0f), m_targetZ(0.0f) {}

GLRoverWidget::~GLRoverWidget() {
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    delete m_program;
    doneCurrent();
}

void GLRoverWidget::resetCamera() {
    m_xRot = 30.0f;
    m_yRot = -45.0f;
    m_zoom = 5.0f;
    m_targetX = 0.0f; m_targetY = 0.0f; m_targetZ = 0.0f;
    update();
}

void GLRoverWidget::setTargetCenter(float x, float y, float z) {
    m_targetX = x;
    m_targetY = y;
    m_targetZ = z;
    update();
}

void GLRoverWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();

    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();

    loadDummyData();

    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));

    m_vao.release();
    m_vbo.release();
}

void GLRoverWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLRoverWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 projection;
    float aspect = float(width()) / float(height() ? height() : 1);
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -m_zoom);
    view.rotate(m_xRot, 1.0f, 0.0f, 0.0f);
    view.rotate(m_yRot, 0.0f, 1.0f, 0.0f);
    view.translate(-m_targetX, -m_targetY, -m_targetZ);

    QMatrix4x4 model;
    model.setToIdentity();

    QMatrix4x4 mvp = projection * view * model;
    m_program->setUniformValue("mvpMatrix", mvp);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_vao.release();

    m_program->release();
}

void GLRoverWidget::mousePressEvent(QMouseEvent *event) {
    m_lastMousePos = event->pos();
}

void GLRoverWidget::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->pos().x() - m_lastMousePos.x();
    int dy = event->pos().y() - m_lastMousePos.y();

    if (event->buttons() & Qt::LeftButton) {
        m_xRot += float(dy) * 0.5f;
        m_yRot += float(dx) * 0.5f;
        update();
    }

    m_lastMousePos = event->pos();
}

void GLRoverWidget::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numDegrees.isNull()) {
        float step = float(numDegrees.y()) / 15.0f;
        m_zoom -= step * 0.5f;

        if (m_zoom < 1.0f) m_zoom = 1.0f;

        update();
    }
}

void GLRoverWidget::loadDummyData() {
    float vertices[] = {
         0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    m_vbo.allocate(vertices, sizeof(vertices));
}