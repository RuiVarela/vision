#include "../source/vs.hpp"

// https://github.com/pjreddie/vision-hw2

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\

void test_structure_matrix(){
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    vs::detectAndDrawHarrisCorners(im, 2.0f, 50.0f, 3);
    vs::saveImage("dump.png", im);

    UTEST(vs::equivalent(0.0f, im.get(0,0,0)));
}

int unit_tests_features(int argc, char **argv)
{
    test_structure_matrix();
    return 0;
}
