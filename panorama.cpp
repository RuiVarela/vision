#include "source/vs.hpp"

// Stitches two images together using a projective transformation.
// image a, b: images to stitch.
// matrix H: homography from image a coordinates to image b coordinates.
// returns: combined image stitched together.
vs::Mat combine_images(vs::Mat const &a, vs::Mat const &b, vs::Mat const &H)
{
    vs::Mat Hinv = H.invert();

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

    // Can disable this if you are making very big panoramas.
    // Usually this means there was an error in calculating H.
    if (w > 7000 || h > 7000)
    {
        fprintf(stderr, "output too big, stopping\n");
        return a;
    }

    vs::Mat c(w, h, a.c);

    // Paste image a into the new image offset by dx and dy.
    for (int k = 0; k < a.c; ++k)
    {
        for (int j = 0; j < a.h; ++j)
        {
            for (int i = 0; i < a.w; ++i)
            {
                // TODO: fill in.
            }
        }
    }

    // TODO: Paste in image b as well.
    // You should loop over some points in the new image (which? all?)
    // and see if their projection from a coordinates to b coordinates falls
    // inside of the bounds of image b. If so, use bilinear interpolation to
    // estimate the value of b at that projection, then fill in image c.

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
vs::Mat panorama_image(vs::Mat &a, vs::Mat &b, float sigma, float thresh, int nms, float inlier_thresh, int iters, int cutoff)
{
    srand(10);
    // Calculate corners and descriptors
    vs::Descriptors ad = vs::harrisCornerDetector(a, sigma, thresh, nms);
    vs::Descriptors bd = vs::harrisCornerDetector(b, sigma, thresh, nms);

    // Find matches
    vs::Matches m = vs::matchDescriptors(ad, bd);

    // Run RANSAC to find the homography
    vs::Mat H = RANSAC(m, inlier_thresh, iters, cutoff);

    if (1)
    {
        // Mark corners and matches between images
        vs::markCorners(a, ad);
        vs::markCorners(b, bd);
        vs::Mat inlier_matches = vs::drawInliers(a, b, H, m, inlier_thresh);
        vs::saveImage("inliers.png", inlier_matches);
    }

    // Stitch the images together with the homography
    vs::Mat comb = combine_images(a, b, H);
    return comb;
}

// Project an image onto a cylinder.
// image im: image to project.
// float f: focal length used to take image (in pixels).
// returns: image projected onto cylinder, then flattened.
vs::Mat cylindrical_project(vs::Mat const &im, float f)
{
    //TODO: project image onto a cylinder
    vs::Mat c = im.clone();
    return c;
}

int main(int argc, char **argv)
{
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
        vs::drawHarrisCorners(a,sigma, thresh, nms);
        vs::saveImage("generated.png", a);
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
        vs::saveImage("generated.png", a);
    }
    else if (!vs::findArg(argc, argv, "panorama"))
    {
        std::cout << "Unknown command" << std::endl;
        return -1;
    }

    return 0;
}
