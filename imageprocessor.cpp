#include "imageprocessor.h"

#include "imagecontainer.h"

#include <QDebug>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>

#include <fade2d/include/Fade_2D.h>
#include <fade2d/include/Point2.h>

/**
 * @brief ImageProcessor::ImageProcessor
 * The ImageProcessor default constructor
 * @param parent
 */
ImageProcessor::ImageProcessor(QWidget *parent)
    : QWidget(parent)
{

}

/**
 * @brief ImageProcessor::getFacialFeatures
 * A procedure invoking the dlib's frontal_face_detector to
 * detect faces in the image specifed by img_path. Futhermore
 * the dlib shape_predictor is used to extract facial landmarks.
 * @param img_path
 * @return
 */
std::vector<QPoint> ImageProcessor::getFacialFeatures(const std::string &img_path)
{
    std::vector<QPoint> landmarks;
    dlib::frontal_face_detector det = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

    dlib::array2d<dlib::rgb_pixel> img;
    dlib::load_image(img, img_path);

    std::vector<dlib::rectangle> dets = det(img);
    //std::cout << "number of faces detected: " << dets.size() << std::endl;
    dlib::full_object_detection shape = sp(img, dets[0]);
    //std::cout << "number of facial landmarks detected: " << shape.num_parts() << std::endl;

    if(shape.num_parts() < 68) {
        qWarning() << "failed to get 68 facial landmarks";
    }

    for(unsigned long i = 0; i < shape.num_parts(); ++i) {
        landmarks.push_back(QPoint(shape.part(i).x(), shape.part(i).y()));
    }
    return landmarks;
}

QImage ImageProcessor::delaunayTriangulation(ImageContainer *ref_one,
                                             ImageContainer *ref_two)
{
    // get the average landmarks
    std::vector<QPoint> ref_one_landmarks = ref_one->getLandmarks();
    std::vector<QPoint> ref_two_landmarks = ref_two->getLandmarks();

    if(ref_one_landmarks.size() != ref_two_landmarks.size())
    {
        qWarning() << "not good not good";
    }

    GEOM_FADE2D::Fade_2D dt; // delaunay triangulation empty
    for(unsigned long i = 0; i < ref_one_landmarks.size(); ++i) {
        QPoint one = ref_one_landmarks[i];
        QPoint two = ref_two_landmarks[i];
        int average_x = (one.x() + two.x()) / 2;
        int average_y = (one.y() + two.y()) / 2;
        dt.insert(GEOM_FADE2D::Point2(average_x, average_y));
    }

    qDebug() << dt.numberOfTriangles();

}
