#include "../../source/vs.hpp"

// https://github.com/pjreddie/vision-hw2

#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\


static void test_filter(){
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    float const sigma = 2.0f;
    int size = im.w * im.h;

    vs::Mat I(im.w, im.h, 3);
    vs::Mat IxIx = I.channelView(0);
    vs::Mat IyIy = I.channelView(1);
    vs::Mat IxIy = I.channelView(2);
    vs::gradient(im, IxIx, IyIy);

    UTEST(vs::sameChannel(I, IxIx, 0, 0));
    UTEST(vs::sameChannel(I, IyIy, 1, 0));
    UTEST(vs::sameChannel(I, IxIy, 2, 0));
    

    float x, y;
    for (int i = 0; i != size; ++i)
    {
        x = IxIx.data[i];
        y = IyIy.data[i];

        IxIy.data[i] = x * y;
        IxIx.data[i] = x * x;
        IyIy.data[i] = y * y;
    }

    UTEST(vs::sameChannel(I, IxIx, 0, 0));
    UTEST(vs::sameChannel(I, IyIy, 1, 0));
    UTEST(vs::sameChannel(I, IxIy, 2, 0));

    vs::Mat S;
    S.reshape(im.w, im.h, 3);
    vs::Mat Sxx(im.w, im.h, 1, S.data + (0 * size));
    vs::Mat Syy(im.w, im.h, 1, S.data + (1 * size));
    vs::Mat Sxy(im.w, im.h, 1, S.data + (2 * size));
    vs::smoothImage(IxIx, Sxx, sigma);
    vs::smoothImage(IyIy, Syy, sigma);
    vs::smoothImage(IxIy, Sxy, sigma);

    UTEST(vs::sameChannel(S, Sxx, 0, 0));
    UTEST(vs::sameChannel(S, Syy, 1, 0));
    UTEST(vs::sameChannel(S, Sxy, 2, 0));

    vs::Mat S1;
    S.reshape(im.w, im.h, 3);
    vs::smoothImage(I, S1, 2.0f);
    UTEST(vs::sameMat(S, S1));
}

static void test_draw_harris() {
    vs::Mat im = vs::loadImage("data/Rainier1.png", 3);

    vs::Mat S;
    harrisStructureMatrix(im, S, 2.0f);
    UTEST(S.w == im.w && S.h == im.h && S.c == 3);

    vs::drawHarrisCorners(im, 2.0f, 50.0f, 3);
    //vs::saveImage("harris_corners.png", im);

    vs::Mat result = vs::loadImage("test/harris_corners.png");
    UTEST(vs::sameMat(im, result));
}

static void test_draw_matches() {
    vs::Mat a = vs::loadImage("data/Rainier1.png", 3);
    vs::Mat b = vs::loadImage("data/Rainier2.png", 3);

    vs::Mat out = vs::drawMatches(a, b, 2.0f, 50.0f, 3);
    //vs::saveImage("harris_matches.png", out);

    vs::Mat result = vs::loadImage("test/harris_matches.png");
    UTEST(vs::sameMat(out, result));
}

static void test_homography() {

    // test project points
    {
        vs::Matches matches;
        vs::Match match;

        match.p.x = 0.0f;
        match.p.y = 0.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 0.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 20.0f;
        matches.push_back(match);

        match.p.x = 0.0f;
        match.p.y = 20.0f;
        matches.push_back(match);
        for(vs::Match& current : matches) {
            current.q = current.p;
            current.q.x += 100.0f;
            current.q.y += 100.0f;
        }

        vs::Mat H = vs::Mat::makeTranslation3x3(100.f, 100.f);
        int inliers = vs::modelInliers(H, matches, 2.0f);
        UTEST(inliers == 4);


    }






    {
        vs::Matches matches;
        vs::Match match;

        match.p.x = 1.0f;
        match.p.y = 1.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 1.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 20.0f;
        matches.push_back(match);

        match.p.x = 1.0f;
        match.p.y = 20.0f;
        matches.push_back(match);

        for(vs::Match& current : matches) {
            current.q = current.p;
            current.q.x += 100.0f;
            current.q.y += 100.0f;
        }


        vs::Mat H = computeHomography(matches);
        assert(H.size() > 0);

        int inliers = vs::modelInliers(H, matches, 2.0f);
        UTEST(inliers == 4);
    }

}

static void test_ransac() {

    srand(10);

    vs::Mat a = vs::loadImage("data/Rainier1.png", 3);
    vs::Mat b = vs::loadImage("data/Rainier2.png", 3);

    float sigma = 2.0;
    int thresh = 50;
    int nms = 3;

    // Calculate corners and descriptors
    vs::Descriptors ad = vs::harrisCornerDetector(a, sigma, thresh, nms);
    vs::Descriptors bd = vs::harrisCornerDetector(b, sigma, thresh, nms);

    // Find matches
    vs::Matches m = vs::matchDescriptors(ad, bd);

    float inlier_thresh = 2.0f;
    int iters = 10000;
    int cutoff = 30;

    // Run RANSAC to find the homography
    vs::Mat H = RANSAC(m, inlier_thresh, iters, cutoff);

    // Mark corners and matches between images
    vs::markCorners(a, ad);
    vs::markCorners(b, bd);
    vs::Mat inlier_matches = vs::drawInliers(a, b, H, m, inlier_thresh);
    vs::saveImage("inliers.png", inlier_matches);


    vs::Mat result = vs::loadImage("test/harris_matches.png");
    UTEST(vs::sameMat(inlier_matches, result));

}

int unit_tests_features(int argc, char **argv)
{
    //test_filter();
    //test_draw_harris();
    //test_draw_matches();
    test_homography();
    //test_ransac();

    return 0;
}
