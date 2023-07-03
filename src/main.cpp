#include <iostream>
#include <opencv2/opencv.hpp>

struct Color{
    unsigned char R{0}; 
    unsigned char G{0}; 
    unsigned char B{0};
};


class Paper
{
    public:
        Paper() = default;
        Paper(Color color) : color_{color} 
        {}
        Paper(int cols, int rows) : colums_{cols}, rows_{rows} 
        {}
    private:
        Color color_; // white
        int colums_{512};  // in pixels
        int rows_{512}; // in pixels
        cv::Mat data{cv::Size(rows_, colums_),CV_8UC3, CV_RGB(color_.R, color_.G, color_.B)}; // in pixels
};

int main() {
    std::cout << "My name is Raisul Islam!" << "\n";

    Paper pap(200,200);


    return 0;
}