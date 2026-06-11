#include "gl_rover_widget.hpp"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

// Vertex Shader: Podaje pozycję wierzchołka dalej, do kolorowania
static const char *vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition;\n"
    "uniform mat4 mvpMatrix;\n"
    "out vec3 vPos;\n" // NOWE: Przekazujemy surową pozycję do Fragment Shadera
    "void main() {\n"
    "   vPos = vertexPosition;\n"
    "   gl_Position = mvpMatrix * vec4(vertexPosition, 1.0);\n"
    "}\n";

// Fragment Shader: Tworzy wirtualne oświetlenie
static const char *fragmentShaderSource =
    "#version 330 core\n"
    "in vec3 vPos;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "   // 1. Magiczna sztuczka: karta w locie liczy nachylenie ścian (wektor normalny)\n"
    "   vec3 dx = dFdx(vPos);\n"
    "   vec3 dy = dFdy(vPos);\n"
    "   vec3 normal = normalize(cross(dx, dy));\n"
    "   \n"
    "   // 2. Wirtualne słońce świecące z prawego, górnego rogu\n"
    "   vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));\n"
    "   \n"
    "   // 3. Obliczamy jasność (dot product). 0.2 to moc cienia otoczenia\n"
    "   float diff = max(dot(normal, lightDir), 0.2);\n"
    "   \n"
    "   // 4. Mieszamy kolor pomarańczowy ze światłem\n"
    "   vec3 baseColor = vec3(1.0, 0.5, 0.2);\n"
    "   vec3 finalColor = baseColor * diff;\n"
    "   \n"
    "   fragColor = vec4(finalColor, 1.0);\n"
    "}\n";

GLRoverWidget::GLRoverWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(nullptr),
      m_xRot(30.0f),
      m_yRot(-45.0f),
      m_zoom(10.0f),
      m_targetX(0.0f), m_targetY(0.0f), m_targetZ(0.0f),
      m_vertexCount(0) {}

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
    m_zoom = 10.0f;
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

    // loadDummyData();
    loadOBJ("../model/model_reduced.obj");

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

    model.scale(0.01f);

    QMatrix4x4 mvp = projection * view * model;
    m_program->setUniformValue("mvpMatrix", mvp);

    m_vao.bind();
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
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

bool GLRoverWidget::loadOBJ(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "BLAD: Nie udalo sie otworzyc pliku modelu:" << path;
        return false;
    }

    QVector<QVector3D> temp_vertices; // Pamięć podręczna na linijki z "v"
    QVector<float> final_vertices;    // Płaska tablica punktów, która poleci na kartę graficzną

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Pomijamy puste linijki i komentarze (zaczynające się od #)
        if (line.isEmpty() || line.startsWith("#")) continue;

        // Dzielimy linijkę po spacjach
        QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
        if (tokens.isEmpty()) continue;

        // Jeśli linijka to wierzchołek (v)
        if (tokens[0] == "v") { 
            temp_vertices.append(QVector3D(tokens[1].toFloat(), tokens[2].toFloat(), tokens[3].toFloat()));
        } 
        // Jeśli linijka to ściana/trójkąt (f)
        else if (tokens[0] == "f") { 
            // Zakładamy, że z Blendera wyeksportowałeś "Triangulated Mesh" (3 punkty na ścianę)
            for (int i = 1; i <= 3; ++i) {
                // Blender eksportuje dane ściany jako np. 5/1/1 (Wierzchołek/Tekstura/Normalna)
                // Nas interesuje tylko pierwsza liczba (Wierzchołek). Dzielimy to po ukośniku.
                QStringList indexData = tokens[i].split('/');
                
                // UWAGA: Format OBJ indeksuje wierzchołki od 1, a C++ od 0. Musimy odjąć 1!
                int vertexIndex = indexData[0].toInt() - 1; 
                
                QVector3D vertex = temp_vertices[vertexIndex];
                
                // Wrzucamy X, Y, Z do naszej docelowej, płaskiej tablicy
                final_vertices.append(vertex.x());
                final_vertices.append(vertex.y());
                final_vertices.append(vertex.z());
            }
        }
    }

    // Liczymy z ilu punktów składa się gotowy model (dzielimy przez 3, bo punkt to X,Y,Z)
    m_vertexCount = final_vertices.size() / 3; 

    // Ostateczny krok: wysłanie przeliczonej tablicy prosto do pamięci GPU (VBO)
    m_vbo.allocate(final_vertices.constData(), final_vertices.size() * sizeof(float));
    
    qDebug() << "SUKCES: Zaladowano model! Liczba wierzcholkow na GPU:" << m_vertexCount;
    return true;
}

void GLRoverWidget::loadDummyData() {
    float vertices[] = {
         0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    m_vbo.allocate(vertices, sizeof(vertices));
}