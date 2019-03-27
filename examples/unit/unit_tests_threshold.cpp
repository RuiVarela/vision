#include "../../source/vs.hpp"



static void test_threshold() {
    vs::Mat loaded = vs::loadImage("data/gradient.png");
    vs::Mat loaded_gray = vs::rgb2gray(loaded);

    {
        vs::Mat gray = loaded_gray.clone();
        vs::threshold(gray, gray, vs::ThresholdMode::Binary, 0.5f);
        vs::Mat reference = vs::loadImage("test/gradient_threshold_binary.png");
        UTEST(vs::sameMat(reference, gray));
    }

    {
        vs::Mat gray = loaded_gray.clone();
        vs::threshold(gray, gray, vs::ThresholdMode::BinaryInverted, 0.5f);
        vs::Mat reference = vs::loadImage("test/gradient_threshold_binary_inverted.png");
        UTEST(vs::sameMat(reference, gray));
    }

    {
        vs::Mat gray = loaded_gray.clone();
        vs::threshold(gray, gray, vs::ThresholdMode::Truncate, 0.5f, 0.5f);
        vs::Mat reference = vs::loadImage("test/gradient_threshold_truncate.png");
        UTEST(vs::sameMat(reference, gray));
    }

    {
        vs::Mat gray = loaded_gray.clone();
        vs::threshold(gray, gray, vs::ThresholdMode::ToZero, 0.5f);
        vs::Mat reference = vs::loadImage("test/gradient_threshold_to_zero.png");
        UTEST(vs::sameMat(reference, gray));
    }

    {
        vs::Mat gray = loaded_gray.clone();
        vs::threshold(gray, gray, vs::ThresholdMode::ToZeroInverted, 0.5f);
        vs::Mat reference = vs::loadImage("test/gradient_threshold_to_zero_inverted.png");
        UTEST(vs::sameMat(reference, gray));
    }
}

static void test_threshold_ostu() {
    vs::Mat gray = vs::loadImage("data/box.png", 1);
    float otsu = vs::thresholdOtsu(gray, gray, vs::ThresholdMode::Binary);
    vs::Mat reference = vs::loadImage("test/threshold_otsu.png");
    UTEST(vs::sameMat(reference, gray));
}

int unit_tests_threshold(int argc, char **argv)
{
    test_threshold();
    test_threshold_ostu();
    return 0;
}
