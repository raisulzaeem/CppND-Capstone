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



// Nested Point_ class for center management
class Point_ {
public:
    Point_(double x, double y) : x(x), y(y) {}
    double x;
    double y;
};


class Shape {
public:
    static int nextID;

    // Constructor
    Shape(double centerX = 0.0, double centerY = 0.0, double angle = 0.0)
        : center(std::make_shared<Point_>(centerX, centerY)),
        angle_(angle), id_(nextID++)
    {
    }

    // Copy Constructor
    Shape(const Shape & other)
        : center(std::make_shared<Point_>(*other.center)),
        position(other.position),
        angle_(other.angle_),
        height_(other.height_),
        width_(other.width_),
        color(other.color),
        id_(nextID++)
    {
    }

    // Copy Assignment Operator
    Shape& operator=(const Shape & other) {
        if (this != &other) {
            center = std::make_shared<Point_>(*other.center);
            position = other.position;
            angle_ = other.angle_;
            height_ = other.height_;
            width_ = other.width_;
            color = other.color;
            id_ = nextID++;
        }
        return *this;
    }

    // Move Constructor
    Shape(Shape && other) noexcept
        : center(std::move(other.center)),
        position(std::move(other.position)),
        angle_(other.angle_),
        height_(other.height_),
        width_(other.width_),
        color(std::move(other.color)),
        id_(other.id_)
    {
        other.id_ = -1;
    }

    // Move Assignment Operator
    Shape& operator=(Shape && other) noexcept {
        if (this != &other) {
            center = std::move(other.center);
            position = std::move(other.position);
            angle_ = other.angle_;
            height_ = other.height_;
            width_ = other.width_;
            color = std::move(other.color);
            id_ = other.id_;
            other.id_ = -1; // Invalidate the moved object
        }
        return *this;
    }

    // Destructor
    virtual ~Shape() = default;


    // Methods to translate and rotate the shape
    virtual void translate(double dx, double dy) {
        position.x += dx;
        position.y += dy;
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

    void changeColor(Color newColor) {
        color = newColor;
    }

    // Virtual function to calculate the area and draw the shape
    virtual double getArea() const = 0;
    virtual cv::Mat draw() const = 0;

    // Getter methods
    double getCenterX() const { return center->x; }
    double getCenterY() const { return center->y; }

    Point_ getPostion() { return position;};
    double getHeight() { return height_; }
    double getWidth() { return width_; }

    int id() { return id_; }

    std::shared_ptr<Point_> getCenter() const { return center; }

protected:
    double angle_{ 0 }, height_{ 0 }, width_{0};
    Color color{255,0,0};
private:
    std::shared_ptr<Point_> center;
    Point_ position{0, 0}; // Upper left corner
    int id_;
};



class Circle : public Shape {
public:
    Circle(double centerX, double centerY, double radius)
        : Shape(centerX, centerY) {
        height_ = radius * 2;
        width_ = radius * 2;
        color = Color(255, 0, 0);
    }

    Circle(double radius)
        : Shape(radius, radius) {
        height_ = radius * 2; // duplicate, combine the two constructors
        width_ = radius * 2;
    }

    double getArea() const override {
        return 3.14159265358979323846 * pow(getRadius(),2) ; // Pi * r^2
    }

    cv::Mat draw() const {
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
        color = Color(0, 255, 0);
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

    cv::Mat draw() const {
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
        color = Color(0, 0, 255);
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

    cv::Mat draw() const {
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



class Canvas
{
public:
    Canvas() = default;
    Canvas(Color color) : color_{ color }
    {}
    Canvas(int cols, int rows) : colums_{ cols }, rows_{ rows }
    {}
    Canvas(int cols, int rows, Color color) : colums_{ cols }, rows_{ rows }, color_{color}
    {}

    void drawShape(double posX, double posY, cv::Mat image) {
        cv::Rect area(posX, posY, image.cols, image.rows);
        data(area) += image;
        cv::Mat test = data;
    }

    void show() {
        cv::imshow("window", this->data);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    void save(std::string imagePath) {
        cv::imwrite(imagePath, this->data);
    }

private:
    Color color_;
    int colums_{ 1024 };  // in pixels
    int rows_{ 1024 }; // in pixels
    cv::Mat data{ cv::Size(rows_, colums_),CV_8UC3, CV_RGB(color_.R, color_.G, color_.B) }; // in pixels
};

int Shape::nextID = 1; // Initialize the static variable

//int main() {
//    std::cout << "Udacity is eating my money!!!" << "\n";
//  
//    Color red(150, 0, 0);
//
//    Paper pap(red);
//
//    Triangle T(1000);
//
//    Rectangle S(500);
//
//    Circle C(100);
//
//    C.draw(red);
//
//    T.rotate(45);
//    S.scale(2);
//    S.rotate(30);
//
//
//
//    const cv::Mat bla = S.draw(red);
//
//    S.scale(0.3);
//
//    S.rotate(20);
//
//    S.draw(red);
//
//    pap.show();
//
//
//
//    return 0;
//}

int main() {
    std::vector<std::shared_ptr<Shape>> shapes;
    int choice;

    while (true) {
        std::cout << "Menu:\n";
        std::cout << "1. Create Circle\n";
        std::cout << "2. Create Triangle\n";
        std::cout << "3. Create Rectangle\n";
        std::cout << "4. Modify Shape\n";
        std::cout << "5. Show Image\n";
        std::cout << "6. Save Image\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            double radius;
            std::cout << "Enter radius: ";
            std::cin >> radius;

            shapes.push_back(std::make_shared<Circle>(radius));
        }
        else if (choice == 2) {
            double base, height;
            std::cout << "Enter base length: ";
            std::cin >> base;
            std::cout << "Enter height: ";
            std::cin >> height;

            shapes.push_back(std::make_shared<Triangle>(base, height));
        }
        else if (choice == 3) {
            double width, height;
            std::cout << "Enter width: ";
            std::cin >> width;
            std::cout << "Enter height: ";
            std::cin >> height;

            shapes.push_back(std::make_shared<Rectangle>(width, height));
        }
        else if (choice == 4) {
            int shapeId;
            std::cout << "Enter shape ID: ";
            std::cin >> shapeId;

            if (shapeId >= 1 && shapeId <= shapes.size()) {
                std::shared_ptr<Shape> shape = shapes[shapeId - 1];

                int modifyChoice;
                std::cout << "Modify Menu:\n";
                std::cout << "1. Translate\n";
                std::cout << "2. Rotate\n";
                std::cout << "3. Scale\n";
                std::cout << "4. Change Color\n";
                std::cout << "Enter your choice: ";
                std::cin >> modifyChoice;

                if (modifyChoice == 1) {
                    double dx, dy;
                    std::cout << "Enter translation in X direction: ";
                    std::cin >> dx;
                    std::cout << "Enter translation in Y direction: ";
                    std::cin >> dy;

                    shape->translate(dx, dy);
                }
                else if (modifyChoice == 2) {
                    double degrees;
                    std::cout << "Enter rotation angle in degrees: ";
                    std::cin >> degrees;

                    shape->rotate(degrees);
                }
                else if (modifyChoice == 3) {
                    double factor;
                    std::cout << "Enter scaling factor: ";
                    std::cin >> factor;

                    shape->scale(factor);
                }
                else if (modifyChoice == 4) {
                    unsigned char r, g, b;
                    std::cout << "Enter new color (R G B): ";
                    std::cin >> r >> g >> b;

                    shape->changeColor(Color(r, g, b));
                }
                else {
                    std::cout << "Invalid choice\n";
                }
            }
            else {
                std::cout << "Invalid shape ID\n";
            }
        }
        else if (choice == 5) {
            auto canvasHeight = std::max_element(shapes.begin(), shapes.end(), [](Shape* a, Shape* b) {return (a->getPostion().x + a->getHeight()) > (b->getPostion().x + b->getHeight()); });
            Canvas canvas(Color(0,0,0));
            for (const auto& shape : shapes) {
                cv::Mat m = shape->draw();
                Point_ pt = shape->getPostion();
                canvas.drawShape(pt.x, pt.y, m);
            }
            canvas.show();
        }
        else if (choice == 6) {
            std::string imagePath;
            std::cout << "Enter image path: ";
            std::cin >> imagePath;

            Canvas canvas;
            for (const auto& shape : shapes) {
                cv::Mat m = shape->draw();
                Point_ pt = shape->getPostion();
                canvas.drawShape(pt.x, pt.y, m);
            }
            canvas.save(imagePath);
            std::cout << "Image saved successfully\n";
        }
        else if (choice == 7) {
            break;
        }
        else {
            std::cout << "Invalid choice\n";
        }
    }

    return 0;
}