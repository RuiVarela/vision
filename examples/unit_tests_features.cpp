#include "../source/vs.hpp"

// https://github.com/pjreddie/vision-hw2

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\


void test_filter(){
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    vs::Mat S;
    harrisStructureMatrix(im, S, 2.0f);
    UTEST(S.w == im.w && S.h == im.h && S.c == 3);
}

void test_structure_matrix(){
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);




    vs::Mat S;
    harrisStructureMatrix(im, S, 2.0f);
    UTEST(S.w == im.w && S.h == im.h && S.c == 3);
}

void test_draw_harris() {
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);
    vs::detectAndDrawHarrisCorners(im, 2.0f, 150.0f, 3);
    UTEST(vs::equivalent(0.0f, im.get(0,0,0)));
}

int unit_tests_features(int argc, char **argv)
{
    test_filter();
    test_structure_matrix();
    test_draw_harris();
    return 0;
}
