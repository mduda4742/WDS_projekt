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
 * * Maps 2D LiDAR range data into vertical 3D wall geometry, and projects
 * the real-time camera image onto those walls based on a simulated FOV.
 */
class View25DWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    /**
     * @brief Constructor for View25DWidget.
     * @param[in] parent - pointer to the parent widget (default is nullptr).
     */
    explicit View25DWidget(QWidget *parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    ~View25DWidget() override;

public slots:
    /**
     * @brief Updates the camera image texture used for projection.
     * @param[in] image - the incoming camera image to be projected as a texture.
     */
    void updateImage(const QImage &image);

    /**
     * @brief Updates the LiDAR scan data to reconstruct the 3D wall geometry.
     * @param[in] ranges - vector of distance measurements in meters.
     * @param[in] angle_min - minimum scan angle in radians.
     * @param[in] angle_max - maximum scan angle in radians.
     * @param[in] angle_increment - angular resolution in radians.
     */
    void updateLaserScan(const std::vector<float> &ranges, float angle_min, float angle_max, float angle_increment);

protected:
    /**
     * @brief Initializes the OpenGL environment, textures, and settings.
     */
    void initializeGL() override;

    /**
     * @brief Updates the OpenGL viewport and projection matrix when the widget is resized.
     * @param[in] w - new width of the widget.
     * @param[in] h - new height of the widget.
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief Renders the 2.5D environment (walls and textured image projection).
     */
    void paintGL() override;

private:
    QImage current_image_;            /**< The latest camera image converted and ready for texturing. */
    QOpenGLTexture *texture_;         /**< Pointer to the OpenGL texture object mapping the camera feed. */
    
    std::vector<float> ranges_;       /**< Cached vector of LiDAR distance measurements. */
    float angle_min_ = 0;             /**< Cached minimum angle of the LiDAR scan [rad]. */
    float angle_max_ = 0;             /**< Cached maximum angle of the LiDAR scan [rad]. */
    float angle_increment_ = 0;       /**< Cached angular increment between LiDAR measurements [rad]. */
    
    std::mutex data_mutex_;           /**< Mutex to ensure thread-safe access to image and scan data. */
    bool has_new_image_ = false;      /**< Flag indicating if a new image needs to be uploaded to the GPU. */

    /**< Simulated camera field of view in radians (90 degrees). 
         Adjust this to stretch/compress the texture across the LiDAR walls. */
    const float cam_fov_rad_ = 90.0f * (3.14159265f / 180.0f);
    
    /**< Desired visual height of the rendered 2.5D walls. */
    const float wall_height_ = 1.0f;
};

#endif // VIEW_25D_WIDGET_HPP