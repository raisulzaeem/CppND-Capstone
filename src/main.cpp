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
    int colums_{ 512 };  // in pixels
    int rows_{ 512 }; // in pixels
    cv::Mat data{ cv::Size(rows_, colums_),CV_8UC3, CV_RGB(color_.R, color_.G, color_.B) }; // in pixels


};


class Shape
{
public:
    virtual ~Shape() = default;

    cv::Point center() const noexcept { return center_; }

    void translate(cv::Point t) { 
        this->center_+= t;
    }
    void rotate(const double angle) { angle_ += angle; }
    void scale(const double scale) { scale_ *= scale; }
    virtual cv::Mat draw(Color) const = 0;

private:
    cv::Point center_{};
    double side_;
    double angle_{ 0 }, scale_{ 1.0 };

protected:
    cv::Mat get_rotationMatrix() const {
        return cv::getRotationMatrix2D( center_, angle_, scale_ );
    }
    Paper canvus_{};
    void setAngle(double angle) {
        this->angle_ = angle;
    }
    void setScale(double scale) {
        this->scale_ = scale;
    }
};



class Circle : public Shape
{
public:
    explicit Circle(double radius)
        : radius_(radius)
    {}

    double radius() const noexcept { return radius_; }


    cv::Mat draw(Color color) const override { std::cout << "Circle::draw()\n"; }

private:
    double radius_{};
};



class Square : public Shape
{
public:
    explicit Square(double side)
        : side_(side)
    {}

    double side() const noexcept { return side_; }

    cv::Mat draw(Color color) const override { std::cout << "Square::draw()\n"; }

private:
    double side_{};
};


class Triangle : public Shape
{
public:
    explicit Triangle(double side)
        : base_(side)
    {
        height_ = side * (sqrt(3)/2);
    }

    explicit Triangle(double base, double height)
        : base_(base), height_(height)
    {}


    double base() const noexcept { return base_; }

    cv::Mat draw(Color color) const override {
        cv::Mat image(height_, base_, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Point vertices[3];
        vertices[0] = cv::Point(0, height_ - 1);           // Bottom-left corner
        vertices[1] = cv::Point(base_ - 1, height_ - 1);   // Bottom-right corner
        vertices[2] = cv::Point(base_ / 2, 0);             // Top-center
        cv::fillConvexPoly(image, vertices, 3, cv::Scalar(color.R, color.G, color.B), cv::LINE_AA);
        cv::Mat M = get_rotationMatrix();
        cv::transform(image, image, M);
        return image;
    }

private:
    double base_{};
    double height_{};
};


int main() {
    std::cout << "My name is Raisul Islam!" << "\n";

  
    Color red(150, 0, 0);

    Paper pap(red);

    Triangle T(1000);

    const cv::Mat bla = T.draw(red);

    pap.show();



    return 0;
}