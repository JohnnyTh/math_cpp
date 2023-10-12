namespace math_cpp_utils {
    int assign_greyscale_color_based_on_value(double value, double min_value, double max_value) {

        // Map the value to a number between 0 and 1.
        double normalized_value = (value - min_value) / (max_value - min_value);

        // Map this to a value between 0 and 255, which is the range for a single color.
        int color = static_cast<int> (normalized_value * 255.0);

        // As an example, we'll use this value for each of R, G, and B to get a grayscale color, but you could use it differently depending on how you want the colors to vary.
        return color;
    }
}
