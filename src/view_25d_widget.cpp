#include "view_25d_widget.hpp"
#include <cmath>
#include <GL/gl.h>

View25DWidget::View25DWidget(QWidget *parent)
    : QOpenGLWidget(parent), texture_(nullptr) {
}

View25DWidget::~View25DWidget() {
    makeCurrent();
    if (texture_) {
        texture_->destroy();
        delete texture_;
    }
    doneCurrent();
}

void View25DWidget::updateImage(const QImage &image) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    // Convert to RGBA and mirror vertically (OpenGL expects 0,0 at bottom-left)
    current_image_ = image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    has_new_image_ = true;
    update();
}

void View25DWidget::updateLaserScan(const std::vector<float> &ranges, float angle_min, float angle_max, float angle_increment) {
    std::lock_guard<std::mutex> lock(data_mutex_);
    ranges_ = ranges;
    angle_min_ = angle_min;
    angle_max_ = angle_max;
    angle_increment_ = angle_increment;
    update();
}

void View25DWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void View25DWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Setup basic perspective view
    float aspect = static_cast<float>(w) / static_cast<float>(h ? h : 1);
    float fov = 60.0f; // vertical FOV
    float zNear = 0.1f;
    float zFar = 100.0f;
    float fH = std::tan(fov / 360.0f * M_PI) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    
    glMatrixMode(GL_MODELVIEW);
}

void View25DWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    std::lock_guard<std::mutex> lock(data_mutex_);

    if (has_new_image_) {
        if (!texture_) {
            texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
            texture_->setMinificationFilter(QOpenGLTexture::Linear);
            texture_->setMagnificationFilter(QOpenGLTexture::Linear);
        }
        if (texture_->isCreated()) texture_->destroy();
        texture_->setData(current_image_);
        has_new_image_ = false;
    }

    if (!texture_ || ranges_.empty()) return; // No data to render yet

    texture_->bind();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    for (size_t i = 0; i < ranges_.size() - 1; ++i) {
        float r1 = ranges_[i];
        float r2 = ranges_[i + 1];

        if (!std::isfinite(r1) || !std::isfinite(r2) || r1 < 0.1f || r2 < 0.1f) continue;
        // Exclude invalid long-range/no-return readings (LiDAR hardware max range is 12m)
        if (r1 >= 12.0f || r2 >= 12.0f) continue;
        // Cut off segments forming huge stretched artifacts between distant objects
        if (std::abs(r1 - r2) > 0.5f) continue;

        // Apply 90-degree counter-clockwise rotation to match robot's physical forward
        float a1 = angle_min_ + i * angle_increment_ + (M_PI / 2.0f);
        float a2 = angle_min_ + (i + 1) * angle_increment_ + (M_PI / 2.0f);

        // Normalize angles to [-PI, PI] to properly check against camera FOV centered at 0
        while (a1 > M_PI) a1 -= 2.0f * M_PI;
        while (a1 < -M_PI) a1 += 2.0f * M_PI;
        while (a2 > M_PI) a2 -= 2.0f * M_PI;
        while (a2 < -M_PI) a2 += 2.0f * M_PI;

        // Filter out LiDAR points outside the camera's assumed Field of View
        if (a1 < -cam_fov_rad_ / 2.0f || a1 > cam_fov_rad_ / 2.0f) continue;
        if (a2 < -cam_fov_rad_ / 2.0f || a2 > cam_fov_rad_ / 2.0f) continue;

        // ROS X is forward, Y is left. In OpenGL, camera looks down -Z, X is right.
        float x1 = -r1 * std::sin(a1);
        float z1 = -r1 * std::cos(a1);
        float x2 = -r2 * std::sin(a2);
        float z2 = -r2 * std::cos(a2);

        // Map horizontal field of view to the U texture coordinate (0.0 to 1.0)
        float u1 = (cam_fov_rad_ / 2.0f - a1) / cam_fov_rad_;
        float u2 = (cam_fov_rad_ / 2.0f - a2) / cam_fov_rad_;

        glTexCoord2f(u1, 1.0f); glVertex3f(x1, wall_height_ / 2.0f, z1); // Top Right
        glTexCoord2f(u2, 1.0f); glVertex3f(x2, wall_height_ / 2.0f, z2); // Top Left
        glTexCoord2f(u2, 0.0f); glVertex3f(x2, -wall_height_ / 2.0f, z2); // Bottom Left
        glTexCoord2f(u1, 0.0f); glVertex3f(x1, -wall_height_ / 2.0f, z1); // Bottom Right
    }
    glEnd();
}