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
}
