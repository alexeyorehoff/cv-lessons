#include <opencv2/opencv.hpp>
#include <complex>
#include <chrono>


#define PI 3.14159265354


// Функция для выполнения двумерного БПФ
cv::Mat dft(cv::Mat& image) {
    int M = image.rows;
    int N = image.cols;
    cv::Mat res(M, N, CV_64FC2);
    for (int k = 0; k < M; k++) {
        for (int l = 0; l < N; l++) {
            std::complex<double> Fkl(0, 0);
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    Fkl += (double)image.at<uint8_t>(i, j) * std::exp(std::complex<double>(0, -2 * PI * (k * i / (double)M + l * j / (double)N)));
                }
            }
            res.at<std::complex<double>>(k, l) = Fkl;
        }
    }
    return res;
}

cv::Mat idft(cv::Mat src) {
    int M = src.rows;
    int N = src.cols;
    cv::Mat res(M, N, CV_64F);

    for (int k = 0; k < M; k++) {
        for (int l = 0; l < N; l++) {
            double fkl = 0;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    std::complex<double> Fij = src.at<std::complex<double>>(i, j);
                    std::complex<double> exp_term = exp(std::complex<double>(0, 2 * PI * (i * k / double(M) + j * l / double(N))));
                    fkl += Fij.real() * exp_term.real() - Fij.imag() * exp_term.imag();
                }
            }
            res.at<double>(k, l) = fkl / (M * N);
        }
    }
    res.convertTo(res, CV_8U);
    return res;
}



void dft_shuffle(cv::Mat& src) {
    int cx = src.cols / 2;
    int cy = src.rows / 2;
    cv::Mat q0(src, cv::Rect(0, 0, cx, cy)); // Левый верхний угол
    cv::Mat q1(src, cv::Rect(cx, 0, cx, cy)); // Правый верхний угол
    cv::Mat q2(src, cv::Rect(0, cy, cx, cy)); // Левый нижний угол
    cv::Mat q3(src, cv::Rect(cx, cy, cx, cy)); // Правый нижний угол
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}





cv::Mat display_magnitude(cv::Mat image) {
    cv::Mat planes[2], magnitude;
    cv::split(image, planes);
    cv::magnitude(planes[0], planes[1], magnitude);
    cv::log(magnitude + cv::Scalar::all(1), magnitude);
    dft_shuffle(magnitude);
    cv::normalize(magnitude, magnitude, 0, 1, cv::NORM_MINMAX);
    return magnitude;
//    magnitude.convertTo(magnitude, CV_8U);
//    cv::Mat hsv_img, hsv_channels[3] = {magnitude, magnitude.setTo(255), magnitude.setTo(255)};
//    cv::merge(hsv_channels, 3, hsv_img);
//    cv::cvtColor(hsv_img, hsv_img, cv::COLOR_HSV2BGR);
//    return hsv_img;
}

int main() {
    using std::chrono::steady_clock;
    // Загрузка изображения
    cv::Mat image = imread("../lab4/lenna.png", cv::IMREAD_GRAYSCALE);
    cv::resize(image, image, {128, 128}, cv::INTER_LINEAR);
    // Проверка на успешную загрузку изображения
    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }

    cv::imshow("original", image);

    auto start = steady_clock::now();
    cv::Mat dft_img = dft(image);

    std::cout << "DIY fourier: " << start - steady_clock::now() << std::endl;
    cv::Mat magnitude = display_magnitude(dft_img);

    cv::imshow("fourier", magnitude);

    cv::Mat idft_res = idft(dft_img);
    cv::imshow("fourier reconstruct", idft_res);
    cv::waitKey(0);

    return 0;
}