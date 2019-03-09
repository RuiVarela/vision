#include "../source/vs.hpp"

static float getMin(vs::Mat const &im, int px, int py, int c, int w)
{
    float v = std::numeric_limits<float>::max();

    for (int dx = -w / 2; dx < (w + 1) / 2; ++dx)
        for (int dy = -w / 2; dy < (w + 1) / 2; ++dy)
        {
            float current = im.getClamp(px + dx, py + dy, c);
            if (current < v)
                v = current;
        }

    return v;
}

// Stitches two images together using a projective transformation.
// image a, b: images to stitch.
// matrix H: homography from image a coordinates to image b coordinates.
// returns: combined image stitched together.
static vs::Mat combine_images(vs::Mat const &a, vs::Mat const &b, vs::Matd const &H)
{
    vs::Matd Hinv = H.invert();

    // Project the corners of image b into image a coordinates.
    vs::Point c1 = vs::projectPoint(Hinv, vs::Point(0, 0));
    vs::Point c2 = vs::projectPoint(Hinv, vs::Point(b.w - 1, 0));
    vs::Point c3 = vs::projectPoint(Hinv, vs::Point(0, b.h - 1));
    vs::Point c4 = vs::projectPoint(Hinv, vs::Point(b.w - 1, b.h - 1));

    // Find top left and bottom right corners of image b warped into image a.
    vs::Point topleft, botright;
    botright.x = vs::maximum(c1.x, vs::maximum(c2.x, vs::maximum(c3.x, c4.x)));
    botright.y = vs::maximum(c1.y, vs::maximum(c2.y, vs::maximum(c3.y, c4.y)));
    topleft.x = vs::minimum(c1.x, vs::minimum(c2.x, vs::minimum(c3.x, c4.x)));
    topleft.y = vs::minimum(c1.y, vs::minimum(c2.y, vs::minimum(c3.y, c4.y)));

    // Find how big our new image should be and the offsets from image a.
    int dx = vs::minimum(0.0f, topleft.x);
    int dy = vs::minimum(0.0f, topleft.y);
    int w = vs::maximum(a.w, int(botright.x)) - dx;
    int h = vs::maximum(a.h, int(botright.y)) - dy;

    vs::Mat c(w, h, a.c);

    // Paste image a into the new image offset by dx and dy.
    c.copy(a, -dx, -dy);

    // Paste in image b as well.
    // You should loop over some points in the new image (which? all?)
    // and see if their projection from a coordinates to b coordinates falls
    // inside of the bounds of image b. If so, use bilinear interpolation to
    // estimate the value of b at that projection, then fill in image c.
    for (int k = 0; k < b.c; ++k)
        for (int y = topleft.y; y < int(botright.y); ++y)
            for (int x = topleft.x; x < int(botright.x); ++x)
            {
                vs::Point p = vs::projectPoint(H, vs::Point(x, y));
                if (p.x >= 0.0f && p.x < b.w && p.y >= 0.0f && p.y < b.h)
                {

                    // this is because of the cylinder black borders
                    if (vs::equivalent(getMin(b, p.x, p.y, k, 3), 0.0f))
                        continue;

                    float value = vs::interpolateBL(b, p.x, p.y, k);
                    c.set(x - dx, y - dy, k, value);
                }
            }

    return c;
}

// Create a panoramam between two images.
// image a, b: images to stitch together.
// float sigma: gaussian for harris corner detector. Typical: 2
// float thresh: threshold for corner/no corner. Typical: 1-5
// int nms: window to perform nms on. Typical: 3
// float inlier_thresh: threshold for RANSAC inliers. Typical: 2-5
// int iters: number of RANSAC iterations. Typical: 1,000-50,000
// int cutoff: RANSAC inlier cutoff. Typical: 10-100
static vs::Mat panorama_image(vs::Mat &a, vs::Mat &b, float sigma, float thresh, int nms, float inlier_thresh, int iters, int cutoff)
{
    srand(10);
    // Calculate corners and descriptors
    vs::Descriptors ad = vs::harrisCornerDetector(a, sigma, thresh, nms);
    vs::Descriptors bd = vs::harrisCornerDetector(b, sigma, thresh, nms);

    // Find matches
    vs::Matches m = vs::matchDescriptors(ad, bd);

    // Run RANSAC to find the homography
    vs::Matd H = RANSAC(m, inlier_thresh, iters, cutoff);

    if (H.size() == 0)
    {
        std::cout << "Unable to find homography" << std::endl;
    }

    if (false)
    {
        // Mark corners and matches between images
        vs::markCorners(a, ad);
        vs::markCorners(b, bd);
        vs::Mat inlier_matches = vs::drawInliers(a, b, H, m, inlier_thresh);
        vs::saveImage("inliers.png", inlier_matches);
    }

    // Stitch the images together with the homography
    return combine_images(a, b, H);
}

//
// ./panorama draw_corners img ./data/Rainier1.png img
// ./panorama draw_matches img ./data/Rainier1.png img ./data/Rainier2.png
// ./panorama img ./data/Rainier1.png img ./data/Rainier2.png
// ./panorama thresh 10 img ./data/Rainier1.png img ./data/Rainier2.png img ./data/Rainier5.png img ./data/Rainier6.png img ./data/Rainier3.png img ./data/Rainier4.png
// ./panorama cylindrical 1200 thresh 2 inlier_thresh 3 iters 50000 img ./data/field5.jpg img ./data/field6.jpg img ./data/field7.jpg img ./data/field8.jpg img ./data/field4.jpg img ./data/field3.jpg
int main(int argc, char **argv)
{
    float cylindrical = vs::findArgFloat(argc, argv, "cylindrical", 0.0f);
    float sigma = vs::findArgFloat(argc, argv, "sigma", 2.0f);
    float thresh = vs::findArgFloat(argc, argv, "thresh", 50.0f);
    int nms = vs::findArgInt(argc, argv, "nms", 3);
    float inlier_thresh = vs::findArgFloat(argc, argv, "inlier_thresh", 2.0f);
    int iters = vs::findArgInt(argc, argv, "iters", 10000);
    int cutoff = vs::findArgInt(argc, argv, "cutoff", 30);

    std::vector<std::string> inputs;
    std::string name = vs::findArgStr(argc, argv, "img", "");
    while (!name.empty())
    {
        inputs.push_back(name);
        name = vs::findArgStr(argc, argv, "img", "");
    }

    if (vs::findArg(argc, argv, "draw_corners"))
    {
        if (inputs.empty())
        {
            std::cout << "Insuficient images provided. use img param" << std::endl;
            return -1;
        }

        vs::Mat a = vs::loadImage(inputs[0]);
        vs::drawHarrisCorners(a, sigma, thresh, nms);
        vs::saveImage("generated.png", a);
        return 0;
    }
    else if (vs::findArg(argc, argv, "draw_matches"))
    {
        if (inputs.size() < 2)
        {
            std::cout << "Insuficient images provided. use img param" << std::endl;
            return -1;
        }

        vs::Mat a = vs::loadImage(inputs[0]);
        vs::Mat b = vs::loadImage(inputs[1]);
        vs::Mat out = vs::drawMatches(a, b, sigma, thresh, nms);
        vs::saveImage("generated.png", out);
        return 0;
    }

    if (inputs.size() < 2)
    {
        std::cout << "Insuficient images provided. use img param" << std::endl;
        return -1;
    }

    vs::Mat current = vs::loadImage(inputs[0], 3);
    if (cylindrical > 0.0)
        current = vs::cylindricalProject(current, cylindrical);

    for (size_t i = 1; i != inputs.size(); ++i)
    {
        std::cout << "Merging " << inputs[i] << std::endl;
        vs::Mat next = vs::loadImage(inputs[i], 3);

        if (cylindrical > 0.0)
            next = vs::cylindricalProject(next, cylindrical);

        current = panorama_image(current, next, sigma, thresh, nms, inlier_thresh, iters, cutoff);
        vs::saveImage("generated.png", current);
    }

    return 0;
}
