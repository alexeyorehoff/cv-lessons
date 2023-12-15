#include <opencv2/opencv.hpp>
#include <complex>
#include <cmath>
#include <ccomplex>


#define PI 3.14159265354


// Функция для выполнения двумерного БПФ
cv::Mat dft(cv::Mat& image) {
    int M = image.rows;
    int N = image.cols;
    cv::Mat res(M, N, CV_32FC2);
    for (int k = 0; k < M; k++) {
        for (int l = 0; l < N; l++) {
            std::complex<float> Fkl(0, 0);
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    Fkl += (float)image.at<uint8_t>(i, j) * std::exp(std::complex<float>(0, -2 * PI * (k * i / M + l * j / N)));
                }
            }
            res.at<std::complex<float>>(k, l) = Fkl;
        }
    }
    return res;
}

int main() {
    // Загрузка изображения
    cv::Mat image = imread("../lab2/lenna.png", cv::IMREAD_GRAYSCALE);
    cv::resize(image, image, {128, 128}, cv::INTER_LINEAR);
    // Проверка на успешную загрузку изображения
    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }
    cv::imshow("original", image);
    cv::imshow("fourier", dft(image));
    cv::waitKey(0);

    return 0;
}