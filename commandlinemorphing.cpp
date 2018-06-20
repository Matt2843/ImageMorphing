#include "commandlinemorphing.h"

#include "globals.h"
#include "imagecontainer.h"

#include <algorithm>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QDir>

/**
 * @brief CommandLineMorphing::CommandLineMorphing
 *
 * The CommandLineMorphing ctor, the class is constructued
 * with three parameters containing relative/absolute file system paths
 * the image input-directory, the image output-directory as well as the
 * *.json settings file.
 *
 * The ctor starts the following procedures: apply_settings(),
 * load_images() and morph_images().
 *
 * @param input_dir a directory path to the input images
 * @param output_dir a directory path to the output images
 * @param json_path a path to the *.json settings file
 */
CommandLineMorphing::CommandLineMorphing(const QString &input_dir,
                                         const QString &output_dir,
                                         const QString &json_path) :
    m_image_width(-1),
    m_image_height(-1),
    m_alpha(0.5),
    m_h_filter(0),
    m_g_filter(0),
    m_m_filter(0),
    m_b_filter(0),
    m_transform(0),
    m_sharpness(0),
    m_contrast(0),
    m_brightness(0),
    m_allow_bad_morphs(false),
    m_format(0)
{
    QDir absolute_path_resolver;
    m_input_directory = absolute_path_resolver.absoluteFilePath(input_dir);
    m_output_directory = absolute_path_resolver.absoluteFilePath(output_dir);
    if(!json_path.isEmpty()) {
        m_json_path = absolute_path_resolver.absoluteFilePath(json_path);
        auto settings_status = apply_settings();
        if(!settings_status) {
            qWarning() << "Failed to parse the json settings";
            exit(1);
        }
    }
    auto loading_status = load_images();
    if(!loading_status) {
        qWarning() << "Failed to load images";
        exit(1);
    }
    morph_images();
    qDebug() << "Morphing completed results saved to:" << m_output_directory;
    exit(0);
}

/**
 * @brief CommandLineMorphing::apply_settings
 *
 * A procedure to apply the settings provided in the
 * *.json settings file.
 *
 * An example json file:
 * {
 *   "resolution": [-1, -1],
 *   "alpha": 0.5,
 *   "h-filter": 0,
 *   "g-filter": 0,
 *   "m-filter": 20,
 *   "b-filter": 100,
 *   "transform": 1,
 *   "sharpness": 30,
 *   "contrast": 40,
 *   "brightness": 50,
 *   "allow-bad-morphs": false,
 *   "format": 1
 * }
 *
 * please note that the json file MUST contain these and ONLY
 * these values.
 *
 * resolution: a 2d array specifying width and height, if
 * the values are -1, -1 it will automatically be determined
 * by the application.
 *
 * float alpha: RANGE: [0,1] a value controlling how much the resulting morph will
 * resmble reference one and two. 0.5 meaning both references are
 * valued equally in the alpha blending stage. Hence alpha=0 should
 * produce an output equal to reference-one image. alpha=1 should produce
 * an output equal to reference-two image.
 *
 * unsigned int h-filter: suggested RANGE: [0,100] a value controlling the amount
 * of homogenous smoothing added as a post-processing effect to the morphed
 * result. h-filter=0 implies that no homogenous smoothing will be added.
 * h-filter=100 results in a high intensity homogenous filtering.
 *
 * unsigned int g-filter: suggested RANGE: [0,100] a value controlling the amount
 * of gaussian smoothing added as a post-processing effect to the morphed
 * result. g-filter=0 implies that no gaussian smoothing will be added.
 * g-filter=100 results in a high intensity gaussian filtering.
 *
 * unsigned int m-filter: suggested RANGE: [0,100] a value controlling the amount
 * of median smoothing added as a post-processing effect to the morphed
 * result. m-filter=0 implies that no median smoothing will be added.
 * m-filter=100 results in a high intensity median filtering.
 *
 * unsigned int b-filter: suggested RANGE: [0,100] a value controlling the amount
 * of bilateral smoothing added as a post-processing effect to the morphed
 * result. b-filter=0 implies that no bilateral smoothing will be added.
 * b-filter=100 results in a high intensity bilateral filtering. WARNING: expensive
 *
 * unsigned int transform: suggested RANGE: [0,1], transform=0 will result in no transformation
 * added to the morphed results. transform=1 will result in a grayscaling transformation
 * of the morphed results.
 *
 * unsigned int sharpness: suggested RANGE: [0,100], a parameter controlling the
 * amount of sharpness added to the morphed results. sharpness=0 implies that no
 * sharpening effect will be added to the morphed results. sharpness=100 results
 * in a high intensity sharpness effect added to the morphed results.
 *
 * unsigned int contrast: suggested RANGE: [0,100], a parameter controlling the
 * amount of contrast added to the morphed results. contrast=0 implies that no
 * contrast will be added to the morphed results. contrast=100 results in a high
 * intensity contrast effect added to the morphed results.
 *
 * unsigned int brightness: suggested RANGE: [0,100], a parameter controlling the
 * amount of brightness added to the morphed results. brightness=0 implies that no
 * brightness will be added to the morphed results. brightness=100 results in a high
 * intensity brightness effect added to the morphed results.
 *
 * bool allow-bad-morphs: RANGE: [true,false], if the input images does not contain
 * an entire face, the dlib library used to identify facial landmarks will falsely
 * report a facial landmark outside the image boundaries, an effect of this is
 * un-warped areas in the resulting morph, the suggested value of this parameter
 * is hence false, as the results will be significantly better.
 *
 * unsigned int format: suggested RANGE: [0,1], the parameter specifies the format
 * of the output images. format=0 results in jpeg formatted outputs. format=1
 * results in png formatted outputs.
 *
 * @return true if the *.json settings file were correctly parsed.
 */
bool CommandLineMorphing::apply_settings()
{
    qDebug() << "Applying the provided json settings\n";
    QFile json_file(m_json_path);
    json_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonDocument document = QJsonDocument::fromJson(json_file.readAll());
    QJsonObject object = document.object();
    json_file.close();

    m_image_width = object["resolution"].toArray()[0].toInt();
    m_image_height = object["resolution"].toArray()[1].toInt();
    m_alpha = (float)object["alpha"].toDouble();
    m_h_filter = object["h-filter"].toInt();
    m_g_filter = object["g-filter"].toInt();
    m_m_filter = object["m-filter"].toInt();
    m_b_filter = object["b-filter"].toInt();
    m_transform = object["transform"].toInt();
    m_sharpness = object["sharpness"].toInt();
    m_contrast = object["contrast"].toInt();
    m_brightness = object["brightness"].toInt();
    m_allow_bad_morphs = object["allow-bad-morphs"].toBool();
    m_format = object["format"].toInt();

    qDebug() << "image_width:" << m_image_width;
    qDebug() << "image_height:" << m_image_height;
    qDebug() << "alpha:" << m_alpha;
    qDebug() << "homogenous filter:" << m_h_filter;
    qDebug() << "gaussian filter:" << m_g_filter;
    qDebug() << "median filter:" << m_m_filter;
    qDebug() << "bilateral filter:" << m_b_filter;
    qDebug() << "transform:" << (m_transform == 0 ? "normal" : "grayscale");
    qDebug() << "sharpness:" << m_sharpness;
    qDebug() << "contrast:" << m_contrast;
    qDebug() << "brightness:" << m_brightness;
    qDebug() << "allow bad morphs:" << m_allow_bad_morphs;
    qDebug() << "format:" << (m_format == 0 ? "jpg" : "png") << "\n";

    return true;
}

/**
 * @brief CommandLineMorphing::load_images
 *
 * A procedure to load images from the m_input_directory,
 * set in the ctor of the CommandLineMorphing class.
 *
 * Initially a filter is added to the input directory to search for
 * *.jpg, *.jpeg or *.png files. If the resulting file count is less than or
 * equal to one, no images will be morphed.
 *
 * If the *.json settings file contained a
 * "resolution": [-1,-1] value, the procedure will automatically find the
 * lowest resolution image the input directory and scale all the remaining
 * images to the found resolution, note that the procedure will not modify
 * the original files but scale copies of these.
 *
 * @return true if images were succesfully loaded.
 */
bool CommandLineMorphing::load_images()
{
    QDir files(m_input_directory);
    files.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    files.setNameFilters(QStringList() << "*.jpg" << "*.jpeg" << "*.png");
    if(files.count() <= 0) return false;

    QDirIterator it(files);

    std::vector<int> widths;
    std::vector<int> heights;

    QStringList paths;
    while(it.hasNext()) {
        paths << it.next();
        if(m_image_width == -1 || m_image_height == -1) {
            QImage img(it.filePath());
            if(img.isNull()) return false;
            widths.push_back(img.width());
            heights.push_back(img.height());
        }
    }

    if(m_image_width == -1 || m_image_height == -1) {
        m_image_width = (int)(*std::min_element(widths.begin(), widths.end()));
        m_image_height = (int)(*std::min_element(heights.begin(), heights.end()));
    }

    fmg::Globals::img_width = m_image_width;
    fmg::Globals::img_height = m_image_height;

    for(const QString &path : paths) {
        qDebug() << "Loading:" << path;
        ImageContainer *image = new ImageContainer(this);
        auto load_status = image->setImageSource(path);
        if(!load_status) return false;
        m_database.push_back(image);
    }

    return true;
}

/**
 * @brief CommandLineMorphing::morph_images
 *
 * An auxillary method to morph the images of the input directory
 * and save the results with the specified filters of the *.json
 * settings file. Note that as of this version of the software,
 * the morph_images() procedure morphs every image with each other.
 * Hence resulting in O(N^2) runtime, N = amount of images detected
 * in the input directory. For the users convenience I suggest
 * input directories of smaller sizes to test the software.
 *
 * The individual morphing procedures are explained in the routines
 * of the ImageProcessor class.
 *
 */
void CommandLineMorphing::morph_images()
{
    for(ImageContainer *img : m_database) {
        qDebug() << "Detecting landmarks:" << img->getImageTitle();
        img->setLandmarks(m_image_processor.getFacialFeatures(img));
    }
    for(ImageContainer *one : m_database) {
        if(one->hasBadLandmarks() && !m_allow_bad_morphs) continue;
        for(ImageContainer *two : m_database) {
            if(one == two) continue;
            if(two->hasBadLandmarks() && !m_allow_bad_morphs) continue;
            qDebug() << "Morphing:" << one->getImageTitle() << "with" << two->getImageTitle();
            ImageContainer target;
            m_image_processor.morphImages(one, two, &target, m_alpha);
            QImage img = target.getSource();
            apply_filters(img);
            target.setImage(img);
            QString format = m_format == 0 ? ".jpg" : ".png";
            if(m_transform > 0) {
                target.getGrayscaleSource().save(m_output_directory+"/"+"g_"+target.getImageTitle()+target.getId()+format);
            } else {
                target.getTempSource().save(m_output_directory+"/"+target.getImageTitle()+target.getId()+format);
            }
        }
    }
}

/**
 * @brief CommandLineMorphing::apply_filters
 *
 * An auxillary class method to apply the post-processing routines
 * specified in the *.json settings file provided. If a *.json settings
 * file is not provided, the procedure uses standard values namely:
 *
 * m_image_width(-1)            // automatic resolution detection
 * m_image_height(-1)           // automatic resolution detection
 * m_alpha(0.5)                 // 50/50 alpha blending
 * m_h_filter(0)                // no homogenous filtering
 * m_g_filter(0)                // no gaussian filtering
 * m_m_filter(0)                // no median filtering
 * m_b_filter(0)                // no bilateral filtering
 * m_transform(0)               // no post-processing transformation
 * m_sharpness(0)               // no sharpness increase
 * m_contrast(0)                // no contrast increase
 * m_brightness(0)              // no brightness increase
 * m_allow_bad_morphs(false)    // do not allow bad morphs
 * m_format(0)                  // .jpg
 *
 * @param img the image which the filters will be applied to.
 */
void CommandLineMorphing::apply_filters(QImage &img)
{
    if(img.isNull()) return;
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::BRIGHTNESS,
                                  m_brightness);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::CONTRAST,
                                  m_contrast);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::SHARPNESS,
                                  m_sharpness);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::BILATERAL,
                                  m_b_filter);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::MEDIAN,
                                  m_m_filter);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::GAUSSIAN,
                                  m_g_filter);
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::HOMOGENEOUS,
                                  m_h_filter);
}


























