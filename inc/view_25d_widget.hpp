#ifndef VIEW_25D_WIDGET_HPP
#define VIEW_25D_WIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QImage>
#include <vector>
#include <mutex>

/**
 * @class View25DWidget
 * @brief Custom OpenGL widget that constructs a 2.5D textured environment.
 * 
 * Maps 2D LiDAR range data into vertical 3D wall geometry, and projects
 * the real-time camera image onto those walls based on a simulated FOV.
 */
class View25DWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit View25DWidget(QWidget *parent = nullptr);
    ~View25DWidget() override;

public slots:
    void updateImage(const QImage &image);
    void updateLaserScan(const std::vector<float> &ranges, float angle_min, float angle_max, float angle_increment);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QImage current_image_;
    QOpenGLTexture *texture_;
    
    std::vector<float> ranges_;
    float angle_min_ = 0;
    float angle_max_ = 0;
    float angle_increment_ = 0;
    
    std::mutex data_mutex_;
    bool has_new_image_ = false;

    // Simulated camera field of view in radians (90 degrees). 
    // Adjust this to stretch/compress the texture across the LiDAR walls.
    const float cam_fov_rad_ = 90.0f * (3.14159265f / 180.0f);
    // Desired visual height of the rendered 2.5D walls
    const float wall_height_ = 1.0f;
};

#endif // VIEW_25D_WIDGET_HPP