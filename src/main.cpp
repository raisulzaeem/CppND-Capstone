#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <math.h>

struct Color {
    Color() = default;
    Color(unsigned char r, unsigned char g, unsigned char b) :
        R{ r }, G{ g }, B{ b }
    {}
    unsigned char R{ 255 };
    unsigned char G{ 255 };
    unsigned char B{ 255 };
};



// Nested Point class for center management
class Point {
public:
    Point(double x, double y) : x(x), y(y) {}
    double x;
    double y;
};


class Shape {
public:
    // Constructor
    Shape(double centerX = 0.0, double centerY = 0.0, double angle = 0.0)
        : center(std::make_shared<Point>(centerX, centerY)),
        angle_(angle) 
    {
    }

    // Methods to translate and rotate the shape
    virtual void translate(double dx, double dy) {
        center->x += dx;
        center->y += dy;
    }

    virtual void rotate(double degrees) {
        angle_ += degrees;
    }

    virtual void scale(double factor) {
        center->x += ((factor - 1) / 2) * width_;
        center->y += ((factor - 1) / 2) * height_;
        height_ = height_ * factor;
        width_ = width_ * factor;
    }

    // Virtual function to calculate the area and draw the shape
    virtual double getArea() const = 0;
    virtual cv::Mat draw(Color) const = 0;

    // Getter methods
    double getCenterX() const { return center->x; }
    double getCenterY() const { return center->y; }

    std::shared_ptr<Point> getCenter() const { return center; }

protected:
    double angle_{ 0 }, height_{ 0 }, width_{0};
private:
    std::shared_ptr<Point> center;
};



class Circle : public Shape {
public:
    Circle(double centerX, double centerY, double radius)
        : Shape(centerX, centerY) {
        height_ = radius * 2;
        width_ = radius * 2;
    }

    Circle(double radius)
        : Shape(radius, radius) {
        height_ = radius * 2; // duplicate, combine the two constructors
        width_ = radius * 2;
    }

    double getArea() const override {
        return 3.14159265358979323846 * pow(getRadius(),2) ; // Pi * r^2
    }

    cv::Mat draw(Color color) const {
        cv::Mat image(height_, width_, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Point center(getCenterX(), getCenterY());
        cv::circle(image, center, (height_ / 2), cv::Scalar(color.B, color.G, color.R), cv::FILLED);

        return image;
    }

    double getRadius() const { return height_; }
};



class Triangle : public Shape {
public:
    Triangle(double base, double height)
        : Shape(base/2, height/2) {
        height_ = height;
        width_ = base;
    }
    
    Triangle(double base)
        : Shape(base / 2, (sqrt(3)*base)/4) {
    
        height_ = (sqrt(3) * base) / 2;
        width_ = base;
    }
    double getArea() const override {
        return 0.5 * getBase() * getHeight(); // (1/2) * base * height
    }

    double getBase() const { return width_ ; }
    double getHeight() const { return height_; }

    cv::Mat draw(Color color) const {
        cv::Mat image(height_, width_, CV_8UC3, cv::Scalar(0, 0, 0));

        cv::Point vertices[3];
        vertices[0] = cv::Point(0, height_ - 1);           // Bottom-left corner
        vertices[1] = cv::Point(width_ - 1, height_ - 1);   // Bottom-right corner
        vertices[2] = cv::Point(width_ / 2, 0);             // Top-center
        cv::fillConvexPoly(image, vertices, 3, cv::Scalar(color.B, color.G, color.R), cv::LINE_AA);
        cv::Mat M = cv::getRotationMatrix2D(cv::Point(getCenterX(), getCenterY()), angle_, 1.0);

        cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), cv::Size(height_, width_), angle_).boundingRect2f();
        M.at<double>(0, 2) += bbox.width / 2.0 - (width_) / 2.0;
        M.at<double>(1, 2) += bbox.height / 2.0 - (height_) / 2.0;

        cv::Mat TImage;
        cv::warpAffine(image, TImage, M, TImage.size());
        return TImage;
    }

};



class Rectangle : public Shape {
public:
    Rectangle( double width, double height, double centerX, double centerY)
        : Shape(centerX, centerY) {
        height_ = width;
        width_ = width;
    }

    Rectangle(double width, double height)
        : Shape(width/2, height/2) {
        height_ = width;
        width_ = width;
    }

    Rectangle(double side) //Square
        : Rectangle(side, side) {
    }

    double getArea() const override {
        return pow(getSide(),2); // side^2
    }

    double getSide() const { return width_; }

    cv::Mat draw(Color color) const {
        cv::Mat image(height_, width_, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Point topLeft(0,0);
        cv::Point bottomRight(height_, width_ );
        cv::rectangle(image, topLeft, bottomRight, cv::Scalar(color.B, color.G, color.R), cv::FILLED);

        cv::Mat M = cv::getRotationMatrix2D(cv::Point(getCenterX(), getCenterY()), angle_, 1.0);

        cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), cv::Size(height_, width_), angle_).boundingRect2f();
        M.at<double>(0, 2) += bbox.width / 2.0 - (width_ ) / 2.0;
        M.at<double>(1, 2) += bbox.height / 2.0 - (height_ ) / 2.0;

        cv::Mat TImage;
        cv::warpAffine(image, TImage, M, bbox.size());
        return TImage;
    }
};



class Paper
{
public:
    Paper() = default;
    Paper(Color color) : color_{ color }
    {}
    Paper(int cols, int rows) : colums_{ cols }, rows_{ rows }
    {}
    void show() {
        cv::imshow("window", this->data);
        cv::waitKey(0);
    }
private:
    Color color_;
    int colums_{ 1024 };  // in pixels
    int rows_{ 1024 }; // in pixels
    cv::Mat data{ cv::Size(rows_, colums_),CV_8UC3, CV_RGB(color_.R, color_.G, color_.B) }; // in pixels
};


int main() {
    std::cout << "Udacity is eating my money!!!" << "\n";

  
    Color red(150, 0, 0);

    Paper pap(red);

    Triangle T(1000);

    Rectangle S(500);

    Circle C(100);

    C.draw(red);

    T.rotate(45);
    S.scale(2);
    S.rotate(30);



    const cv::Mat bla = S.draw(red);

    S.scale(0.3);

    S.rotate(20);

    S.draw(red);

    pap.show();



    return 0;
}