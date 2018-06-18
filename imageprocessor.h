#pragma once

#include <QWidget>

#include <vector>
#include <string>

#include <QImage>
#include <QPoint>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>

struct TriangleIndices
{
    unsigned long A;
    unsigned long B;
    unsigned long C;
};

class ImageContainer;
class ImageProcessor : public QWidget
{
    Q_OBJECT

public:
    explicit ImageProcessor(QWidget * parent = nullptr);
    ~ImageProcessor() = default;

    enum Filter {
      HOMOGENEOUS, GAUSSIAN, MEDIAN, BILATERAL, SHARPNESS,
        CONTRAST, BRIGHTNESS
    };

public:
    std::vector<QPoint> getFacialFeatures(ImageContainer *image);
    // These could be private
    std::vector<TriangleIndices> delaunayTriangulation(const std::vector<cv::Point2f> &indices);
    void affineTransform(const cv::Mat &target,
                         const cv::Mat &source,
                         const std::vector<cv::Point2f> &source_triangles,
                         const std::vector<cv::Point2f> &target_triangles);

    void warpAndAlphaBlendTriangles(const cv::Mat &cv_ref_one,
                                       const cv::Mat &cv_ref_two,
                                       const cv::Mat &morphed_image,
                                       const std::vector<cv::Point2f> &t_one,
                                       const std::vector<cv::Point2f> &t_two,
                                       const std::vector<cv::Point2f> &t_target,
                                       float alpha);
    void morphImages(ImageContainer *ref_one,
                     ImageContainer *ref_two,
                     ImageContainer *target,
                     float alpha);
    void applyFilter(QImage &target, Filter filter, int intensity);
    void fourierTransform(QImage &target);

public:
    QImage MatToQImage(const cv::Mat &mat, QImage::Format format);
    cv::Mat img2mat(const QImage &qimg, bool copy = true);
    QImage mat2img(const cv::Mat &img);

private:
    dlib::shape_predictor sp;
};
