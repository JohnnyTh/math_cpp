#include <vector>
#include <chrono>

namespace math_cpp_utils {
    int assign_greyscale_color_based_on_value(
            double value,
            double min_value,
            double max_value,
            double max_color,
            int neg_1_color) {
        // Map the value to a number between 0 and 1.
        int color;

        if (value != -1.0) {
            double normalized_value = (value - min_value) / (max_value - min_value);
            color = static_cast<int> (normalized_value * max_color);
        } else {
            color = neg_1_color;
        }

        return color;
    }

    float assign_greyscale_color_based_on_value_float(
            double value,
            double min_value,
            double max_value,
            float neg_1_color) {
        // Map the value to a number between 0 and 1.
        float color;

        if (value != -1.0) {
            color = static_cast<float>((value - min_value) / (max_value - min_value));
        } else {
            color = neg_1_color;
        }

        return color;
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
