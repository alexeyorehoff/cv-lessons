#include <opencv2/opencv.hpp>
#include <complex>
#include <chrono>


#define PI 3.14159265354
using std::chrono::steady_clock;


cv::Mat dft(cv::Mat& image) {
    int M = image.rows;
    int N = image.cols;
    cv::Mat res(M, N, CV_64FC2);
    for (int k = 0; k < M; k++) {
        for (int l = 0; l < N; l++) {
            std::complex<double> Fkl(0, 0);
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    Fkl += (double)image.at<uint8_t>(i, j) *
                            std::exp(std::complex<double>(0, -2 * PI * (k * i / (double)M + l * j / (double)N)));
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
}

std::vector<std::complex<double>> mat2vec(cv::Mat image)
{
    std::vector<uchar> imageVector(image.begin<uint8_t>(), image.end<uint8_t>());
    std::vector<std::complex<double>> complexVector(imageVector.size());
    for (int idx = 0; idx < imageVector.size(); idx++)
        complexVector[idx] = std::complex<double>(imageVector[idx], 0);
    return complexVector;
}

cv::Mat vec2mat(const std::vector<std::complex<double>> &complexVector, cv::Size size) {
    cv::Mat resultMat(size, CV_64FC2);
    for (int i = 0; i < size.height; ++i)
        for (int j = 0; j < size.width; ++j)
            resultMat.at<cv::Vec2d>(i, j) = {complexVector[i * size.width + j].real(), complexVector[i * size.width + j].imag()};
    return resultMat;
}


void fft_radix2(std::vector<std::complex<double>> &src, std::vector<std::complex<double>> &res, bool inverse) {
    size_t N = src.size();
    if (N == 1) {
        res = src;
        return;
    }
    std::vector<std::complex<double>> y_even, y_odd;
    y_even.reserve(N / 2);
    y_odd.reserve(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        y_even.push_back(src[2 * i]);
        y_odd.push_back(src[2 * i + 1]);
    }

    fft_radix2(y_even, y_even, inverse);
    fft_radix2(y_odd, y_odd, inverse);

    res.resize(N);
    std::complex<double> wn(cos(2 * PI / (double)N), sin(2 * PI / (double)N) * (inverse ? 1 : -1));
    std::complex<double> w(1);
    for (int i = 0; i < N / 2; ++i) {
        res[i] = y_even[i] + w * y_odd[i];
        res[i + N / 2] = y_even[i] - w * y_odd[i];
        w *= wn;
    }

    if (inverse) {
        for (int i = 0; i < N; ++i) {
            res[i] /= (double)N;
        }
    }
}


void test_dft(cv::Mat image) {
    auto start = steady_clock::now();
    cv::Mat dft_img = dft(image);
    std::cout << "DIY fourier: " << start - steady_clock::now() << std::endl;
    cv::Mat magnitude = display_magnitude(dft_img);
    cv::imshow("fourier", magnitude);
    cv::Mat idft_res = idft(dft_img);
    cv::imshow("fourier reconstruct", idft_res);
    cv::waitKey();
}

void test_fft(cv::Mat image) {
    int m = cv::getOptimalDFTSize(image.rows);
    int n = cv::getOptimalDFTSize(image.cols);
    cv::resize(image, image, {m, n});
    std::cout << m << " " << n << std::endl;
    auto start = steady_clock::now();
    std::vector<std::complex<double>> t = mat2vec(image);
    std::vector<std::complex<double>> t1;
    fft_radix2(t, t1, false);
    std::cout << "radix: " << start - steady_clock::now() << std::endl;
    cv::imshow("fft", display_magnitude(vec2mat(t1, image.size())));

    cv::waitKey(0);
}


int main() {
        cv::Mat image = imread("../lab4/lenna.png", cv::IMREAD_GRAYSCALE);
//    cv::resize(image, image, {128, 128}, cv::INTER_LINEAR);

    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }

    cv::imshow("original", image);


    test_fft(image);


    return 0;
}