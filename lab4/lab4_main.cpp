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

std::vector<std::complex<double>> mat2vec(cv::Mat image) {
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
    y_even.resize(N / 2);
    y_odd.resize(N / 2);
    for (int i = 0; i < N / 2; ++i) {
        y_even[i] = src[2 * i];
        y_odd[i] = src[2 * i + 1];
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


cv::Mat convolution(cv::Mat image, cv::Mat kernel)
{
    image.convertTo(image, CV_32F);
    kernel.convertTo(kernel, CV_32F);

    int m = cv::getOptimalDFTSize(image.rows + kernel.rows - 1);
    int n = cv::getOptimalDFTSize(image.cols + kernel.cols - 1);

    cv::Mat padded_image, padded_kernel;
    copyMakeBorder(image, padded_image, 0, m - image.rows, 0, n - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    copyMakeBorder(kernel, padded_kernel, 0, m - kernel.rows, 0, n - kernel.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat complex_image, complex_kernel;
    cv::dft(padded_image, complex_image, cv::DFT_COMPLEX_OUTPUT);
    cv::dft(padded_kernel, complex_kernel, cv::DFT_COMPLEX_OUTPUT);

    cv::Mat complex_result;
    cv::mulSpectrums(complex_image, complex_kernel, complex_result, 0);

    return complex_result;
}

cv::Mat reconstruct(cv::Mat src) {
    cv::Mat result;
    cv::idft(src, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    normalize(result, result, 0, 255, cv::NORM_MINMAX);
    result.convertTo(result, CV_8U);
    return result;
}


void test_dft(cv::Mat image) {
    auto start = steady_clock::now();
    cv::Mat dft_img = dft(image);
    std::cout << "DIY fourier: " << steady_clock::now() - start << std::endl;
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
    std::vector<std::complex<double>> image_vector = mat2vec(image);
    std::vector<std::complex<double>> fft_result;
    fft_radix2(image_vector, fft_result, false);
    std::cout << "radix: " << steady_clock::now() - start << std::endl;
    cv::imshow("fft", display_magnitude(vec2mat(fft_result, image.size())));

    cv::waitKey(0);
}

void test_cv_fft(cv::Mat image) {
    cv::Mat float_image;
    image.convertTo(float_image, CV_32F);
    cv::Mat complex_image = cv::Mat(float_image.size(), CV_32FC2);
    cv::Mat plane[] = {float_image, cv::Mat::zeros(float_image.size(), CV_32F)};
    cv::merge(plane, 2, complex_image);

    auto start = steady_clock::now();
    cv::dft(complex_image, complex_image);
    std::cout << "opencv fft time: " << steady_clock::now() - start << std::endl;

    cv::imshow("cv fft", display_magnitude(complex_image));
    cv::waitKey();
}

void test_convolution(cv::Mat image) {
    cv::Mat sobel_kernel_x = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    cv::Mat sobel_kernel_y = (cv::Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
    cv::Mat box_kernel = ((cv::Mat_<double>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1) / 9);
    cv::Mat laplace_kernel = (cv::Mat_<double>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);

    cv::Mat sobel_res_x = convolution(image, sobel_kernel_x);
    cv::Mat sobel_res_y = convolution(image, sobel_kernel_y);
    cv::Mat box_res = convolution(image, box_kernel);
    cv::Mat laplace_res = convolution(image, laplace_kernel);

    cv::imshow("sx", display_magnitude(sobel_res_x));
    cv::imshow("sy", display_magnitude(sobel_res_y));
    cv::imshow("box", display_magnitude(box_res));
    cv::imshow("lp", display_magnitude(laplace_res));

    cv::imshow("sxr", reconstruct(sobel_res_x));
    cv::imshow("syr", reconstruct(sobel_res_y));
    cv::imshow("boxr", reconstruct(box_res));
    cv::imshow("lpr", reconstruct(laplace_res));
    cv::waitKey();
}

cv::Mat high_low_filter(const cv::Mat& input_image, double ratio, bool highPass = false) {
    cv::Mat result = input_image.clone();
    cv::Size size = input_image.size();
    cv::Point center(size.width / 2, size.height / 2);
    double radius = std::min(size.width, size.height) * ratio;

    for (int y = 0; y < size.height; ++y) {
        for (int x = 0; x < size.width; ++x) {
            auto& pixel = result.at<cv::Vec2f>(y, x);
            double distance = cv::norm(cv::Point(x, y) - center);
            if ((highPass && distance <= radius) || (!highPass && distance > radius)) pixel = {0, 0};
        }
    }
    return result;
}


void test_lower_upper_filter(cv::Mat image) {
    image.convertTo(image, CV_32F);

    int m = cv::getOptimalDFTSize(image.rows);
    int n = cv::getOptimalDFTSize(image.cols);
    cv::Mat padded_image, complex_image, shuffled;
    copyMakeBorder(image, padded_image, 0, m - image.rows, 0, n - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::dft(padded_image, complex_image, cv::DFT_COMPLEX_OUTPUT);
    dft_shuffle(complex_image);
    cv::Mat high_pass = high_low_filter(complex_image, 0.15, false);
    cv::Mat low_pass = high_low_filter(complex_image, 0.15, true);
    dft_shuffle(high_pass);
    dft_shuffle(low_pass);

    cv::imshow("low", reconstruct(low_pass));
    cv::imshow("high", reconstruct(high_pass));
    cv::waitKey();

}


void correlation(const cv::Mat& img, const cv::Mat& templ, cv::Mat& result) {
    cv::Mat padded_img, padded_templ;
    int m = cv::getOptimalDFTSize(img.rows + templ.rows - 1);
    int n = cv::getOptimalDFTSize(img.cols + templ.cols - 1);
    cv::copyMakeBorder(img, padded_img, 0, m - img.rows, 0, n - img.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    cv::copyMakeBorder(templ, padded_templ, 0, m - templ.rows, 0, n - templ.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat img_float, templ_float;
    padded_img.convertTo(img_float, CV_32F);
    padded_templ.convertTo(templ_float, CV_32F);

//    cv::normalize(img_float, img_float, 0, 1, cv::NORM_MINMAX);
//    cv::normalize(templ_float, templ_float, 0, 1, cv::NORM_MINMAX);

    cv::Mat img_dft, templ_dft;
    cv::dft(img_float, img_dft, cv::DFT_COMPLEX_OUTPUT);
    cv::dft(templ_float, templ_dft, cv::DFT_COMPLEX_OUTPUT);

    std::vector<cv::Mat> planes;
    cv::split(templ_dft, planes);
    planes[1] *= -1;
    cv::merge(planes, templ_dft);

    cv::Mat multiplied;
    cv::mulSpectrums(img_dft, templ_dft, multiplied, 0, false); // Ensure no scaling

    cv::idft(multiplied, result, cv::DFT_REAL_OUTPUT | cv::DFT_SCALE);

    result = result(cv::Rect(0, 0, img.cols - templ.cols, img.rows - templ.rows));
    cv::copyMakeBorder(result, result, templ.rows / 2, 0, templ.cols / 2, 0, cv::BORDER_CONSTANT, cv::Scalar::all(0));
}


void test_correlation() {
    cv::Mat image = imread("../lab4/img_2.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat letter_a = imread("../lab4/a.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat letter_0 = imread("../lab4/0.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat correlation_0, correlation_a;
    correlation(image, letter_0, correlation_0);
    correlation(image, letter_a, correlation_a);
    cv::normalize(correlation_0, correlation_0, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::normalize(correlation_a, correlation_a, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("letter_0", correlation_0  );
    cv::imshow("letter_a", correlation_a);
    cv::waitKey();
}


cv::Mat encircle_brightest(cv::Mat mask, cv::Mat image, int radius = 25) {
    double max_val;
    cv::Point max_loc;
    minMaxLoc(mask, nullptr, &max_val, nullptr, &max_loc);
    circle(image, max_loc, radius, (0, 255, 0), 2);
    return image;
}


void manul_test() {
    cv::Mat image = imread("../lab4/manul.png", cv::IMREAD_GRAYSCALE);
    cv::Mat templ = imread("../lab4/ear.png", cv::IMREAD_GRAYSCALE);

    cv::Mat result, image_temp, templ_temp;

    cv::Laplacian(image, image_temp, CV_8U);
    cv::Laplacian(templ, templ_temp, CV_8U);

    correlation(image_temp, templ_temp, result);

    cv::normalize(result, result, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::imshow("correlation", result);
    cv::imshow("manul_orig", encircle_brightest(result, image));

    cv::waitKey();
}


int main() {
    cv::Mat image = imread("../lab4/lenna.png", cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }
//    cv::imshow("original", image);

//    test_fft(image.clone());
//    test_cv_fft(image.clone());
//    test_convolution(image.clone());
//    test_lower_upper_filter(image.clone());
//    test_correlation();
    manul_test();

    return 0;
}