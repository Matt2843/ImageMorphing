#include "imageprocessor.h"

#include "imagecontainer.h"
#include "databasepreview.h"
#include "console.h"

#include <string>
#include <unordered_set>
#include <iostream>

#include <QDebug>
#include <QImage>
#include <QUuid>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>

/**
 * @brief ImageProcessor::ImageProcessor
 * The ImageProcessor default ctor
 * @param parent
 */
ImageProcessor::ImageProcessor(QWidget *parent)
    : QWidget(parent) {}

/**
 * @brief ImageProcessor::getFacialFeatures
 * A procedure invoking the dlib's frontal_face_detector to
 * detect faces in the image specifed by img_path. Futhermore
 * the dlib shape_predictor is used to extract facial landmarks.
 * @param img_path
 * @return
 */
std::vector<QPoint> ImageProcessor::getFacialFeatures(ImageContainer *image)
{
    Console::appendToConsole("Detecting facial landmarks: " + image->getImageTitle());
    std::vector<QPoint> landmarks;
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

    dlib::array2d<dlib::rgb_pixel> img;
    dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(qImageToCVMat(image->getSource())));
    //dlib::load_image(img, img_path);

    std::vector<dlib::rectangle> dets = detector(img);
    dlib::full_object_detection shape = sp(img, dets[0]);

    if(shape.num_parts() < 68) {
        qWarning() << "failed to get 68 facial landmarks";
    }

    for(unsigned long i = 0; i < shape.num_parts(); ++i) {
        long x = shape.part(i).x();
        long y = shape.part(i).y();
        landmarks.push_back(QPoint(x, y));
    }

    return landmarks;
}

/**
 * @brief ImageProcessor::delaunayTriangulation
 * @param indices
 * @return
 */
std::vector<TriangleIndices> ImageProcessor::delaunayTriangulation(const std::vector<cv::Point2f> &indices)
{
    auto lookup = [&](const cv::Point2f &point){
        return (unsigned long)(std::find(indices.begin(), indices.end(), point) - indices.begin());
    };

    // Create the bounds for the Subdiv2D region
    cv::Rect rect(0, 0, DatabasePreview::m_image_width, DatabasePreview::m_image_height);
    cv::Subdiv2D subdiv(rect);

    for(auto it = indices.begin(); it != indices.end(); ++it) {
        if(rect.contains(*it)) subdiv.insert(*it);
    }

    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);

    std::vector<TriangleIndices> triangle_indices;
    for(auto it = triangleList.begin(); it != triangleList.end(); ++it) {
        auto triangle = *it;
        auto one = lookup(cv::Point2f(triangle[0], triangle[1]));
        auto two = lookup(cv::Point2f(triangle[2], triangle[3]));
        auto three = lookup(cv::Point2f(triangle[4], triangle[5]));

        // quick test to avoid pushing the fake triangles from Subdiv2d.
        if(one == indices.size() || two == indices.size() || three == indices.size()) continue;

        triangle_indices.push_back({one, two, three});
    }
    return triangle_indices;
}

/**
 * @brief ImageProcessor::affineTransform
 * An auxillary function to affine transform two cv::Mat
 * @param target
 * @param source
 * @param source_triangles
 * @param target_triangles
 */
void ImageProcessor::affineTransform(const cv::Mat &target,
                                     const cv::Mat &source,
                                     const std::vector<cv::Point2f> &source_triangles,
                                     const std::vector<cv::Point2f> &target_triangles)
{
    cv::Mat warped = cv::getAffineTransform(source_triangles, target_triangles);
    cv::warpAffine(source, target, warped, target.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

/**
 * @brief ImageProcessor::warpAndAlphaBlendTriangles
 * A procedure to morph two images
 * Replica implementation of https://www.learnopencv.com/face-morph-using-opencv-cpp-python/
 * @param cv_ref_one
 * @param cv_ref_two
 * @param morphed_image
 * @param t_one
 * @param t_two
 * @param t_target
 * @param alpha
 */
void ImageProcessor::warpAndAlphaBlendTriangles(const cv::Mat &cv_ref_one,
                                                const cv::Mat &cv_ref_two,
                                                const cv::Mat &morphed_image,
                                                const std::vector<cv::Point2f> &t_one,
                                                const std::vector<cv::Point2f> &t_two,
                                                const std::vector<cv::Point2f> &t_target,
                                                float alpha)
{
    cv::Rect cv_morphed_image_bounding_rect = cv::boundingRect(t_target);
    cv::Rect cv_ref_one_bounding_rect = cv::boundingRect(t_one);
    cv::Rect cv_ref_two_bounding_rect = cv::boundingRect(t_two);

    // Some off-set points needed:
    std::vector<cv::Point2f> cv_ref_one_offset, cv_ref_two_offset, cv_target_offset;
    std::vector<cv::Point> rect_ints;

    for(unsigned long i = 0; i < 3; ++i) {
        cv_target_offset.push_back(cv::Point2f(t_target[i].x - cv_morphed_image_bounding_rect.x, t_target[i].y - cv_morphed_image_bounding_rect.y));
        rect_ints.push_back(cv::Point(t_target[i].x - cv_morphed_image_bounding_rect.x, t_target[i].y - cv_morphed_image_bounding_rect.y));
        cv_ref_one_offset.push_back(cv::Point2f(t_one[i].x - cv_ref_one_bounding_rect.x, t_one[i].y - cv_ref_one_bounding_rect.y));
        cv_ref_two_offset.push_back(cv::Point2f(t_two[i].x - cv_ref_two_bounding_rect.x, t_two[i].y - cv_ref_two_bounding_rect.y));
    }

    cv::Mat mask = cv::Mat::zeros(cv_morphed_image_bounding_rect.height, cv_morphed_image_bounding_rect.width, CV_32FC3);
    cv::fillConvexPoly(mask, rect_ints, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

    cv::Mat cv_ref_one_rect, cv_ref_two_rect;
    cv_ref_one(cv_ref_one_bounding_rect).copyTo(cv_ref_one_rect);
    cv_ref_two(cv_ref_two_bounding_rect).copyTo(cv_ref_two_rect);

    cv::Mat warp_ref_one = cv::Mat::zeros(cv_morphed_image_bounding_rect.height,
                                          cv_morphed_image_bounding_rect.width,
                                          cv_ref_one_rect.type());
    cv::Mat warp_ref_two = cv::Mat::zeros(cv_morphed_image_bounding_rect.height,
                                          cv_morphed_image_bounding_rect.width,
                                          cv_ref_two_rect.type());

    affineTransform(warp_ref_one, cv_ref_one_rect, cv_ref_one_offset, cv_target_offset);
    affineTransform(warp_ref_two, cv_ref_two_rect, cv_ref_two_offset, cv_target_offset);

    cv::Mat image_rect = (1.0 - alpha) * warp_ref_one + alpha * warp_ref_two;

    cv::multiply(image_rect, mask, image_rect);
    cv::multiply(morphed_image(cv_morphed_image_bounding_rect), cv::Scalar(1.0, 1.0, 1.0) - mask, morphed_image(cv_morphed_image_bounding_rect));

    morphed_image(cv_morphed_image_bounding_rect) += image_rect;
}

/**
 * @brief ImageProcessor::morphImages
 * @param ref_one
 * @param ref_two
 * @return
 */
void ImageProcessor::morphImages(ImageContainer *ref_one,
                                 ImageContainer *ref_two,
                                 ImageContainer *target,
                                 float alpha)
{
    cv::Mat cv_ref_one = qImageToCVMat(ref_one->getSource());
    cv::Mat cv_ref_two = qImageToCVMat(ref_two->getSource());

    cv_ref_one.convertTo(cv_ref_one, CV_32F);
    cv_ref_two.convertTo(cv_ref_two, CV_32F);

    cv::Mat morphed_image = cv::Mat::zeros(cv_ref_one.size(), CV_32FC3);

    std::vector<cv::Point2f> average_landmarks;
    std::vector<cv::Point2f> average_weighted_landmarks;

    auto landmarks_r1 = ref_one->getLandmarks();
    auto landmarks_r2 = ref_two->getLandmarks();
    for(unsigned long i = 0; i < ref_one->getLandmarks().size(); ++i) {
        float x_w = (1 - alpha) * landmarks_r1[i].x() + alpha * landmarks_r2[i].x();
        float y_w = (1 - alpha) * landmarks_r1[i].y() + alpha * landmarks_r2[i].y();
        average_weighted_landmarks.push_back(cv::Point2f(x_w, y_w));

        float x_a = floor((landmarks_r1[i].x() + landmarks_r2[i].x()) / 2);
        float y_a = floor((landmarks_r1[i].y() + landmarks_r2[i].y()) / 2);
        average_landmarks.push_back(cv::Point2f(x_a, y_a));
    }

    auto triangles = delaunayTriangulation(average_landmarks);

    std::unordered_set<std::string> errors;
    cv::Rect test(0, 0, DatabasePreview::m_image_width, DatabasePreview::m_image_height);
    for(const auto &triangle : triangles) {
        std::vector<cv::Point2f> t_one, t_two, t_target;

        auto t_one_point1 = cv::Point2f(landmarks_r1[triangle.A].x(), landmarks_r1[triangle.A].y());
        auto t_one_point2 = cv::Point2f(landmarks_r1[triangle.B].x(), landmarks_r1[triangle.B].y());
        auto t_one_point3 = cv::Point2f(landmarks_r1[triangle.C].x(), landmarks_r1[triangle.C].y());
        if(!test.contains(t_one_point1)) {
            errors.insert("Landmark: " + std::to_string(triangle.A) + " is not contained in reference one, incomplete result produced.");
            continue;
        } else if(!test.contains(t_one_point2)) {
            errors.insert("Landmark: " + std::to_string(triangle.B) + " is not contained in reference one, incomplete result produced.");
            continue;
        } else if(!test.contains(t_one_point3)) {
            errors.insert("Landmark: " + std::to_string(triangle.C) + " is not contained in reference one, incomplete result produced.");
            continue;
        }
        t_one.push_back(t_one_point1);
        t_one.push_back(t_one_point2);
        t_one.push_back(t_one_point3);

        auto t_two_point1 = cv::Point2f(landmarks_r2[triangle.A].x(), landmarks_r2[triangle.A].y());
        auto t_two_point2 = cv::Point2f(landmarks_r2[triangle.B].x(), landmarks_r2[triangle.B].y());
        auto t_two_point3 = cv::Point2f(landmarks_r2[triangle.C].x(), landmarks_r2[triangle.C].y());
        if(!test.contains(t_two_point1)) {
            errors.insert("Landmark: " + std::to_string(triangle.A) + " is not contained in reference two, incomplete result produced.");
            continue;
        } else if(!test.contains(t_two_point2)) {
            errors.insert("Landmark: " + std::to_string(triangle.B) + " is not contained in reference two, incomplete result produced.");
            continue;
        } else if(!test.contains(t_two_point3)) {
            errors.insert("Landmark: " + std::to_string(triangle.C) + " is not contained in reference two, incomplete result produced.");
            continue;
        }
        t_two.push_back(t_two_point1);
        t_two.push_back(t_two_point2);
        t_two.push_back(t_two_point3);

        auto t_target_point1 = average_weighted_landmarks[triangle.A];
        auto t_target_point2 = average_weighted_landmarks[triangle.B];
        auto t_target_point3 = average_weighted_landmarks[triangle.C];
        if(!test.contains(t_target_point1) || !test.contains(t_target_point2) || !test.contains(t_target_point3)) {
            continue;
        }
        t_target.push_back(t_target_point1);
        t_target.push_back(t_target_point2);
        t_target.push_back(t_target_point3);

        warpAndAlphaBlendTriangles(cv_ref_one, cv_ref_two, morphed_image, t_one, t_two, t_target, alpha);
    }
    for(const auto &error : errors) {
        Console::appendToConsole(QString::fromStdString(error));
    }
    morphed_image.convertTo(morphed_image, CV_8UC4);
    QImage morph_result = cvMatToQImage(morphed_image);
    QString morph_title = "(" + ref_one->getImageTitle() + ")" + "_x_" + "(" + ref_two->getImageTitle() + ")";
    target->setImageTitle(morph_title);
    target->setImageSource(morph_result);
    std::vector<QPoint> morph_result_landmarks;
    for(const auto & landmark : average_weighted_landmarks) {
        morph_result_landmarks.push_back(QPoint(landmark.x, landmark.y));
    }
    target->setLandmarks(morph_result_landmarks, false);
}



/**
 * @brief ImageProcessor::normalFilter
 * @param target
 * @param intensity
 */
void ImageProcessor::applyFilter(QImage &target, Filter filter, int intensity)
{
    if(intensity <= 2) return;
    cv::Mat before = QImageToMat(target, CV_8UC3);
    //cv::Mat before = qImageToCVMat(target);
    cv::Mat destination = cv::Mat::zeros(target.height(),
                                         target.width(),
                                         before.type());
    int intensity_i = ceil(intensity) / 3;
    switch(filter) {
    case HOMOGENEOUS:
        cv::blur(before, destination, cv::Size(intensity_i, intensity_i), cv::Point(-1, -1));
        break;
    case GAUSSIAN:
        if(intensity_i %2 == 0) intensity_i++;
        cv::GaussianBlur(before, destination, cv::Size(intensity_i, intensity_i), 0, 0);
        break;
    case MEDIAN:
        if(intensity_i % 2 == 0) intensity_i++;
        cv::medianBlur(before, destination, intensity_i);
        break;
    case BILATERAL:
        cv::bilateralFilter(before, destination, intensity_i, intensity_i * 2, intensity_i / 2);
        break;
    case SHARPNESS:
        cv::GaussianBlur(before, destination, cv::Size(0, 0), intensity_i);
        cv::addWeighted(before, 1.5, destination, -0.5, 0, destination);
        break;
    case CONTRAST:
        before.convertTo(destination, -1, (1 + (float)intensity/100), 0);
        break;
    case BRIGHTNESS:
        before.convertTo(destination, -1, 1, intensity);
        break;
    }
    target = MatToQImage(destination, QImage::Format_RGB888);
    //target = cvMatToQImage(destination);
}

QImage ImageProcessor::MatToQImage(const cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows,
                  mat.step, format).copy();
}

cv::Mat ImageProcessor::QImageToMat(const QImage &img, int format)
{
    return cv::Mat(img.height(), img.width(), format,
                   const_cast<uchar*>(img.bits()),
                   img.bytesPerLine()).clone();
}

/**
 * @brief ImageProcessor::qImageToCVMat
 * A procedure to convert a QImage to a cv::Mat
 * The procedure is inspired by: https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 * @param qimg
 * @param copy
 * @return
 */
cv::Mat ImageProcessor::qImageToCVMat(const QImage &qimg, bool copy)
{
    cv::Mat image;
    if(qimg.format() == QImage::Format_ARGB32 || qimg.format() == QImage::Format_ARGB32_Premultiplied) {
        cv::Mat cvimg(qimg.height(), qimg.width(), CV_8UC4, const_cast<uchar*>(qimg.bits()), static_cast<std::size_t>(qimg.bytesPerLine()));
        image = copy ? cvimg.clone() : cvimg;
    } else if(qimg.format() == QImage::Format_RGB32) {
        cv::Mat cvimg(qimg.height(), qimg.width(), CV_8UC4, const_cast<uchar*>(qimg.bits()), static_cast<std::size_t>(qimg.bytesPerLine()));
        cv::Mat no_alpha;
        cv::cvtColor(cvimg, no_alpha, cv::COLOR_BGRA2BGR);
        image = copy ? no_alpha.clone() : no_alpha;
    } else if(qimg.format() == QImage::Format_RGB888) {
        QImage swap_rgb_qimage = qimg.rgbSwapped();
        image = cv::Mat(swap_rgb_qimage.height(), swap_rgb_qimage.width(), CV_8UC3, const_cast<uchar*>(swap_rgb_qimage.bits()), static_cast<std::size_t>(swap_rgb_qimage.bytesPerLine())).clone();
    } else if(qimg.format() == QImage::Format_Indexed8) {
        cv::Mat cvimg(qimg.height(), qimg.width(), CV_8UC1, const_cast<uchar*>(qimg.bits()), static_cast<std::size_t>(qimg.bytesPerLine()));
        image = copy ? cvimg.clone() : cvimg;
    }
    return image;
}

/**
 * @brief ImageProcessor::cvMatToQImage
 * A procedure to convert a cv::Mat to a QImage
 * The procedure is inspired by: https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 * @param img
 * @return
 */
QImage ImageProcessor::cvMatToQImage(const cv::Mat &img)
{
    QImage image;
    if(img.type() == CV_8UC4) {
        image = QImage(img.data,
                       img.cols, img.rows,
                       static_cast<int>(img.step),
                       QImage::Format_ARGB32);
    } else if(img.type() == CV_8UC3) {
        image = QImage(img.data,
                       img.cols, img.rows,
                       static_cast<int>(img.step),
                       QImage::Format_RGB888).rgbSwapped();
    } else if(img.type() == CV_8UC1) {
        image = QImage(img.data,
                       img.cols, img.rows,
                       static_cast<int>(img.step),
                       QImage::Format_Grayscale8);
    }
    return image;
}


