#pragma once

#include <QWidget>

#include <vector>
#include <string>

#include <QImage>
#include <QPoint>

class ImageContainer;
class ImageProcessor : public QWidget {
    Q_OBJECT

public:
    explicit ImageProcessor(QWidget * parent = nullptr);
    ~ImageProcessor() = default;

public:
    std::vector<QPoint> getFacialFeatures(const std::string &img_path);
    QImage delaunayTriangulation(ImageContainer *ref_one,
                                 ImageContainer *ref_two);

};
