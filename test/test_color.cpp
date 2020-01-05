#include <unity.h>

#include <color.h>


void test_rgb2hsv() {
    std::array<double, 3> rgb = {0.78431372549, 0.392156862745, 0.588235294118};
    std::array<double, 3> hsv = color::rgb2hsv(rgb);
    int expected[] = {330, 50, 78};
    int actual[] = {(int)round(hsv[0]), (int)round(hsv[1] * 100), (int)round(hsv[2] * 100)};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}

void test_hsv2rgb() {
    std::array<double, 3> hsv = {330, 0.5, 0.78};
    std::array<double, 3> rgb = color::hsv2rgb(hsv);
    int expected[] = {78, 39, 59};
    int actual[] = {(int)round(rgb[0] * 100), (int)round(rgb[1] * 100), (int)round(rgb[2] * 100)};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}

void test_rgb2hsv2rgb() {
    int factor = pow(10, 9);
    std::array<double, 3> rgb = {0.78431372549, 0.392156862745, 0.588235294118};
    std::array<double, 3> hsv = color::rgb2hsv(rgb);
    std::array<double, 3> rgb2 = color::hsv2rgb(hsv);
    int expected[] = {(int)round(rgb[0] * factor), (int)round(rgb[1] * factor), (int)round(rgb[2] * factor)};
    int actual[] = {(int)round(rgb2[0] * factor), (int)round(rgb2[1] * factor), (int)round(rgb2[2] * factor)};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}

//HSI

void test_rgb2hsi2rgb() {
    int factor = pow(10, 2);
    std::array<double, 3> rgb = {0.78431372549, 0.392156862745, 0.588235294118};
    std::array<double, 3> hsi = color::rgb2hsi(rgb);
    printf("%f \n", hsi[0]);
    printf("%f \n", hsi[1]);
    printf("%f \n", hsi[2]);
    std::array<double, 3> rgb2 = color::hsi2rgb(hsi);
    int expected[] = {(int)round(rgb[0] * factor), (int)round(rgb[1] * factor), (int)round(rgb[2] * factor)};
    int actual[] = {(int)round(rgb2[0] * factor), (int)round(rgb2[1] * factor), (int)round(rgb2[2] * factor)};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}


void process() {
    UNITY_BEGIN();
    RUN_TEST(test_rgb2hsv);
    RUN_TEST(test_hsv2rgb);
    RUN_TEST(test_rgb2hsv2rgb);
    RUN_TEST(test_rgb2hsi2rgb);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    delay(2000);

    process();
}

void loop() {}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
