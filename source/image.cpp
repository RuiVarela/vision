#include "vs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace vs
{

Mat::Mat()
    : w(0), h(0), c(0)
{
}

Mat::Mat(int w, int h, int c)
    : Mat()
{
    reshape(w, h, c);
}

Mat::~Mat()
{
}

int Mat::size()
{
    return w * h * c;
}

void Mat::reshape(int w, int h, int c)
{
    shared_data.reset();
    data = 0;
    this->w = 0;
    this->h = 0;
    this->c = 0;

    if (w > 0 && h > 0 && c > 0)
    {
        this->w = w;
        this->h = h;
        this->c = c;

        data = static_cast<float *>(malloc(sizeof(float) * size()));
        if (data)
        {
            shared_data = std::shared_ptr<float>(data, free);
        }
    }
}

Mat loadImage(std::string const &path)
{

    int w, h, c;
    unsigned char *data = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!data)
    {
        std::cerr << "Cannot load image \"" << path << "\" - " << stbi_failure_reason();
        return Mat();
    }

    Mat im(w, h, c);
    for (int k = 0; k < c; ++k)
    {
        for (int j = 0; j < h; ++j)
        {
            for (int i = 0; i < w; ++i)
            {
                int dst_index = i + w * j + w * h * k;
                int src_index = k + c * i + c * w * j;
                im.data[dst_index] = float(data[src_index]) / 255.0f;
            }
        }
    }

    free(data);
    return im;
}

bool saveImage(std::string const &path, Mat const &im)
{
    int quality = 80;

    unsigned char *data = static_cast<unsigned char *>(calloc(im.w * im.h * im.c, sizeof(char)));

    for (int k = 0; k < im.c; ++k)
    {
        for (int i = 0; i < im.w * im.h; ++i)
        {
            data[i * im.c + k] = (unsigned char)(255 * im.data[i + k * im.w * im.h]);
        }
    }

    bool ok = false;
    size_t extension_position = path.size() - 4;
    if (path.rfind(".png") == extension_position)
    {
        ok = stbi_write_png(path.c_str(), im.w, im.h, im.c, data, im.w * im.c) > 0;
    }
    else if (path.rfind(".tga") == extension_position)
    {
        ok = stbi_write_tga(path.c_str(), im.w, im.h, im.c, data) > 0;
    }
    else if (path.rfind(".bmp") == extension_position)
    {
        ok = stbi_write_bmp(path.c_str(), im.w, im.h, im.c, data) > 0;
    }
    else if (path.rfind(".jpg") == extension_position)
    {
        ok = stbi_write_jpg(path.c_str(), im.w, im.h, im.c, data, quality) > 0;
    }

    free(data);

    return ok;
}

std::ostream &Mat::print(std::ostream &out, int max_cols, int max_rows)
{
    out << "Header " << w << "x" << h << "x" << c << std::endl;
    out << std::fixed << std::setprecision(2);

    if (max_rows > 0 && max_rows > 0)
    {
        for (int i = 0; i < c; ++i)
        {
            out << "channel " << i << std::endl;
            
            for (int j = 0; j < h; ++j)
            {
                if (j > max_rows)
                {
                    out << "... (" << h - j << ")" << std::endl;
                    break;
                }

                for (int k = 0; k < w; ++k)
                {
                    if (k > max_cols)
                    {
                        out << " ... (" << w - k << ")";
                        break;
                    }

                    if (k > 0)
                    {
                        out << ", ";
                    }

                    out << std::fixed << data[i * h * w + j * w + k];
                }
                out << std::endl;
            }
        }
    }
    return out;
}

} // namespace vs
