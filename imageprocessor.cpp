#include "imageprocessor.h"

#include "imagecontainer.h"
#include "console.h"
#include "globals.h"

#include <string>
#include <unordered_set>
#include <iostream>

#include <QDebug>
#include <QImage>
#include <QUuid>
#include <QProgressDialog>
#include <QCoreApplication>
#include <QString>

/**
 * @brief ImageProcessor::ImageProcessor
 *
 * The ImageProcessor default ctor, deserializing the shape_predictor_68_face_landmarks.dat training set to
 * perform facial feature extraction in 1 millisecond according:
 *
 * https://www.semanticscholar.org/paper/One-millisecond-face-alignment-with-an-ensemble-of-Kazemi-Sullivan/1824b1ccace464ba275ccc86619feaa89018c0ad
 * https://github.com/davisking/dlib-models
 *
 * @param parent the Qt widgets parent
 */
ImageProcessor::ImageProcessor(QWidget *parent)
    : QWidget(parent)
{
    QString path = QCoreApplication::applicationDirPath() + "/" + "shape_predictor_68_face_landmarks.dat";
    dlib::deserialize(path.toStdString()) >> sp;
}

/**
 * @brief ImageProcessor::getFacialFeatures
 *
 * A procedure invoking the dlib's frontal_face_detector to detect faces in the image specifed by img_path. Futhermore
 * the dlib shape_predictor is used to extract facial landmarks. The bottleneck of this routine is the face detection
 * procedure, which has been optimized by the method displayed in:
 *
 * https://www.learnopencv.com/speeding-up-dlib-facial-landmark-detector/
 *
 * Furthermore for future extension https://github.com/nenadmarkus/pico/ would dramatically increase the face
 * detection procedure.
 *
 * @param image the ImageContainer to perform facial feature extraction on
 * @return a std::vector<QPoint> containing the extracted facial features
 */
std::vector<QPoint> ImageProcessor::getFacialFeatures(ImageContainer *image)
{
    #define FACE_DOWNSAMPLE_RATIO 2
    if(fmg::Globals::gui)
        Console::appendToConsole("Detecting facial landmarks: " + image->getImageTitle());
    std::vector<QPoint> landmarks;
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

    cv::Mat cv_img = img2mat(image->getSource());
    dlib::array2d<dlib::rgb_pixel> img;
    dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(cv_img));

    cv::Mat image_small;
    cv::resize(cv_img, image_small, cv::Size(), 1.0/FACE_DOWNSAMPLE_RATIO, 1.0/FACE_DOWNSAMPLE_RATIO);

    dlib::cv_image<dlib::bgr_pixel> dlib_small(image_small);

    std::vector<dlib::rectangle> faces = detector(dlib_small);
    dlib::rectangle rect((long)(faces[0].left()   * FACE_DOWNSAMPLE_RATIO),
                         (long)(faces[0].top()    * FACE_DOWNSAMPLE_RATIO),
                         (long)(faces[0].right()  * FACE_DOWNSAMPLE_RATIO),
                         (long)(faces[0].bottom() * FACE_DOWNSAMPLE_RATIO));
    dlib::full_object_detection shape = sp(img, rect);

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
 * @brief ImageProcessor::morphImages
 *
 * A routine to morph two the images contained in two ImageContainers to one image.
 * The routine calculates the average facial landmarks, triangulates the result,
 * translates the triangulation to the reference landmarks and finally warps and alpha
 * blends the resulting triangle sets into one morphed image. This procedure is described
 * in detail in the term paper.
 *
 * @param ref_one the ImageContainer of the Reference One image
 * @param ref_two the ImageContainer of the Reference Two image
 * @param target
 * @param alpha the alpha-blend value 0-1
 */
void ImageProcessor::morphImages(ImageContainer *ref_one,
                                 ImageContainer *ref_two,
                                 ImageContainer *target,
                                 float alpha)
{
    cv::Mat cv_ref_one = img2mat(ref_one->getSource());
    cv::Mat cv_ref_two = img2mat(ref_two->getSource());

    cv_ref_one.convertTo(cv_ref_one, CV_32F);
    cv_ref_two.convertTo(cv_ref_two, CV_32F);

    cv::Mat morphed_image = cv::Mat::zeros(cv_ref_one.size(), CV_32FC3);

    std::vector<cv::Point2f> average_landmarks;
    std::vector<cv::Point2f> average_weighted_landmarks;

    auto landmarks_r1 = ref_one->getLandmarks();
    auto landmarks_r2 = ref_two->getLandmarks();

    for(unsigned long i = 0; i < landmarks_r1.size(); ++i) {
        float x_w = (1 - alpha) * landmarks_r1[i].x() + alpha * landmarks_r2[i].x();
        float y_w = (1 - alpha) * landmarks_r1[i].y() + alpha * landmarks_r2[i].y();
        average_weighted_landmarks.push_back(cv::Point2f(x_w, y_w));

        float x_a = floor((landmarks_r1[i].x() + landmarks_r2[i].x()) / 2);
        float y_a = floor((landmarks_r1[i].y() + landmarks_r2[i].y()) / 2);
        average_landmarks.push_back(cv::Point2f(x_a, y_a));
    }

    auto triangles = delaunayTriangulation(average_landmarks, fmg::Globals::img_width, fmg::Globals::img_height);

    std::unordered_set<std::string> errors;
    cv::Rect test(0, 0, fmg::Globals::img_width, fmg::Globals::img_height);
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
    if(fmg::Globals::gui) {
        for(const auto &error : errors) {
            Console::appendToConsole(QString::fromStdString(error));
        }
    }
    morphed_image.convertTo(morphed_image, CV_8UC4);
    QImage morph_result = mat2img(morphed_image);
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
 * @brief ImageProcessor::applyFilter
 *
 * A procedure to apply a filter specified by the Filter enum, to the target QImage with the
 * given intensity.
 *
 * @param target the reference to the QImage
 * @param filter the Filter to be applied
 * @param intensity the intensity of the filter.
 */
void ImageProcessor::applyFilter(QImage &target, Filter filter, int intensity)
{
    if(intensity <= 2) return;
    cv::Mat before = img2mat(target);
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
    target = MatToQImage(destination, QImage::Format_RGB888).rgbSwapped();
}

/**
 * @brief ImageProcessor::fourierTransform
 *
 * A procedure to transform the target QImage from the spatial domain to the frequency domain
 * via a discrete fourier transform routine offered by OpenCV.
 *
 * https://docs.opencv.org/2.4.13.4/doc/tutorials/core/discrete_fourier_transform/discrete_fourier_transform.html
 *
 * @param target
 */
void ImageProcessor::fourierTransform(QImage &target)
{
    cv::Mat cv_img = img2mat(target.convertToFormat(QImage::Format_Grayscale8));
    cv::Mat padded_cv_img;
    int m = cv::getOptimalDFTSize(cv_img.rows);
    int n = cv::getOptimalDFTSize(cv_img.cols);
    cv::copyMakeBorder(cv_img, padded_cv_img, 0, m - cv_img.rows, 0, n - cv_img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::Mat planes[] = {cv::Mat_<float>(padded_cv_img), cv::Mat::zeros(padded_cv_img.size(), CV_32F)};
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);

    cv::dft(complexI, complexI);

    cv::split(complexI, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magI = planes[0];

    magI += cv::Scalar::all(1);
    cv::log(magI, magI);

    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    int cx = magI.cols/2;
    int cy = magI.rows/2;

    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    cv::normalize(magI, magI, 0, 255, CV_MINMAX);
    magI.convertTo(magI, CV_8UC3);

    target = mat2img(magI);
}

/**
 * @brief ImageProcessor::delaunayTriangulation
 *
 * A triangulation procedure, given a set of facial landmarks, the procedure performs delaunay triangulation
 * to create regions of interest in the average-landmark domain.
 *
 * @param indices a set of facial landmarks
 * @param width a bound for the cv::Subdiv2D routine
 * @param height a bound for the cv::Subdiv2D routine
 * @return std::vector<TriangleIndices> a translation of triangle coordinates to triangle indices.
 */
std::vector<TriangleIndices> ImageProcessor::delaunayTriangulation(const std::vector<cv::Point2f> &indices,
                                                                   int width, int height)
{
    auto lookup = [&](const cv::Point2f &point){
        return (unsigned long)(std::find(indices.begin(), indices.end(), point) - indices.begin());
    };

    // Create the bounds for the Subdiv2D region
    cv::Rect rect(0, 0, width, height);
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
 *
 * An private convenience class method to invoke cv::warpAffine()
 *
 * @param target the morph target
 * @param source the reference source
 * @param source_triangles the results of the interpreted source triangulation
 * @param target_triangles the results of the target triangulation
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
 *
 * A routine to alpha blend and warp triangles from two sources into the target source. Creating
 * a more detailed morph result than a traditional alpha-blend.
 *
 * Replica implementation of https://www.learnopencv.com/face-morph-using-opencv-cpp-python/
 *
 * @param cv_ref_one reference one image in cv::Mat format
 * @param cv_ref_two reference two image in cv::Mat format
 * @param morphed_image target image in cv::Mat format
 * @param t_one triangulation results of cv_ref_one
 * @param t_two triangulation results of cv_ref_two
 * @param t_target triangulation results of morphed_image
 * @param alpha the degree in which the alpha-blend should be applied
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
 * @brief ImageProcessor::MatToQImage
 *
 * A private rescue-method used in some cases for a specfic cv::Mat to QImage conversion.
 *
 * @param mat
 * @param format
 * @return
 */
QImage ImageProcessor::MatToQImage(const cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows,
                  mat.step, format).copy();
}

/**
 * @brief ImageProcessor::qImageToCVMat
 *
 * A procedure to convert a QImage to a cv::Mat
 * The procedure is inspired by: https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 *
 * @param qimg imput image
 * @param copy specifies if the procedure should return a copy
 * @return the converted image
 */
cv::Mat ImageProcessor::img2mat(const QImage &qimg, bool copy)
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
    } else if(qimg.format() == QImage::Format_Grayscale8) {
        cv::Mat cvimg(qimg.height(), qimg.width(), CV_8UC1, const_cast<uchar*>(qimg.bits()), static_cast<std::size_t>(qimg.bytesPerLine()));
        image = copy ? cvimg.clone() : cvimg;
    }
    return image;
}

/**
 * @brief ImageProcessor::cvMatToQImage
 *
 * A procedure to convert a cv::Mat to a QImage
 * The procedure is inspired by: https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
 *
 * @param img input image
 * @return converted image
 */
QImage ImageProcessor::mat2img(const cv::Mat &img)
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


