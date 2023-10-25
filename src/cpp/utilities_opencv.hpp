#include <opencv2/opencv.hpp>

namespace math_cpp_utils_opencv {
    cv::Mat get_greyscale_mat(std::vector<int> const &greyscale_values, int size_x, int size_y) {
        cv::Mat greyscale_mat(size_y, size_x, CV_8UC1);

        int idx_global = 0;

        for (int i_row = 0; i_row < size_y; i_row++) {
            for (int j_col = 0; j_col < size_x; ++j_col) {
                greyscale_mat.at<uchar>(i_row, j_col) = greyscale_values[idx_global];
                idx_global++;
            }
        }

        return greyscale_mat;
    }
}