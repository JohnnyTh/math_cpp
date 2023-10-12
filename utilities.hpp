#include <vector>
#include <chrono>

#include <opencv4/opencv2/core.hpp>


namespace math_cpp_utils {
    int assign_greyscale_color_based_on_value(double value, double min_value, double max_value) {
        // Map the value to a number between 0 and 1.
        double normalized_value = (value - min_value) / (max_value - min_value);

        int color = static_cast<int> (normalized_value * 255.0);

        return color;
    }

    cv::Mat get_greyscale_mat(std::vector<int> const &greyscale_values, int size_x, int size_y) {
        cv::Mat greyscale_mat(size_x, size_y, CV_8UC1);

        int idx_global = 0;

        for (int i = 0; i < size_y; i++) {
            for (int j = 0; j < size_x; ++j) {
                greyscale_mat.at<uchar>(i, j) = greyscale_values[idx_global];
                idx_global++;
            }
        }

        return greyscale_mat;
    }

    void replace_value_with_another(std::vector<int> &values, int val_to_replace, int val_new) {
        int n_values = static_cast<int>(values.size());

        for (int i = 0; i < n_values; i++) {
            auto value = values[i];

            if (value == val_to_replace) {
                values[i] = val_new;
            }
        }
    }

    void print_time_from_start(const char *msg, std::chrono::time_point<std::chrono::system_clock> start) {
        std::chrono::duration<double> elapsed_seconds = std::chrono::high_resolution_clock::now() - start;
        (void) printf("%s time: %.9f s.\n", msg, elapsed_seconds.count());
    }
}
