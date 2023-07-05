#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <vector>


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
    virtual void rotate(float angle) { std::cout << "Shape::rotate()\n"; }
    virtual cv::Mat draw() const = 0;

private:
    cv::Point center_{};
    double angle_{0}, scale_{ 1.0 };
    Paper canvus_{};
    cv::Mat rotationMatrix{};

protected:
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

    void rotate() override { std::cout << "Circle::rotate()\n"; }
    cv::Mat draw() const override { std::cout << "Circle::draw()\n"; }

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

    cv::Mat draw() const override { std::cout << "Square::draw()\n"; }

private:
    double side_{};
};


int main() {
    std::cout << "My name is Raisul Islam!" << "\n";

  
    Color red(150, 150, 0);

    Paper pap(red);

    pap.show();



    return 0;
}