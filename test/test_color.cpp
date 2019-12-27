#include <unity.h>

#include <color.h>


void test_rgb2hsv() {
    std::array<double, 3> rgb = {0.78431372549, 0.392156862745, 0.588235294118};
    std::array<double, 3> hsv = color::rgb2hsv(rgb);
    int h = round(hsv[0]);
    int s = round(hsv[1] * 100);
    int v = round(hsv[2] * 100);
    int expected[] = {330, 50, 78};
    int actual[] = {h, s, v};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, 3);
}


void process() {
    UNITY_BEGIN();
    RUN_TEST(test_rgb2hsv);
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
