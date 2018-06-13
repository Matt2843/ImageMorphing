#pragma once

#include <QWidget>

#include <vector>
#include <string>

#include <QImage>
#include <QPoint>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

struct TriangleIndices
{
    unsigned long A;
    unsigned long B;
    unsigned long C;
};

class ImageContainer;
class ImageProcessor : public QWidget {
    Q_OBJECT

public:
    explicit ImageProcessor(QWidget * parent = nullptr);
    ~ImageProcessor() = default;

public:
    std::vector<QPoint> getFacialFeatures(const std::string &img_path);
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

private:
    cv::Mat qImageToCVMat(const QImage &qimg, bool copy = true);
    QImage cvMatToQImage(const cv::Mat &img);
};
