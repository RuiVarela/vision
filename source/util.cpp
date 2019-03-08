#include "vs.hpp"

namespace vs
{

std::string toNativeSeparators(const std::string &path)
{
    std::string output = path;

#ifdef __linux__

#elif _WIN32
    std::replace(output.begin(), output.end(), '\\', '/');
#endif

    return output;
}

void del_arg(int argc, char **argv, int index)
{
    int i;
    for(i = index; i < argc-1; ++i) 
        argv[i] = argv[i+1];
    argv[i] = nullptr;
}

bool findArg(int argc, char *argv[], std::string arg)
{
    for (int i = 0; i < argc; ++i)
    {
        if (!argv[i])
            continue;

        if (arg == argv[i]) {
            del_arg(argc, argv, i);
            return true;
        }
    }

    return false;
}

int findArgInt(int argc, char **argv, std::string arg, int def)
{
    for (int i = 0; i < argc; ++i)
    {
        if (!argv[i])
            continue;

        if (arg == argv[i])
        {
            def = atoi(argv[i + 1]);
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }

    return def;
}

float findArgFloat(int argc, char **argv, std::string arg, float def)
{
    for (int i = 0; i < argc; ++i)
    {
        if (!argv[i])
            continue;

        if (arg == argv[i])
        {
            def = float(atof(argv[i + 1]));
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }

    return def;
}

std::string findArgStr(int argc, char **argv, std::string arg, std::string def)
{
    for (int i = 0; i < argc; ++i)
    {
        if (!argv[i])
            continue;

        if (arg == argv[i])
        {
            def = argv[i + 1];
            del_arg(argc, argv, i);
            del_arg(argc, argv, i);
            break;
        }
    }

    return def;
}

bool sameChannel(Mat const &a, Mat const &b, int const ac, int const bc)
{
    float const epsilon = 0.005f;

    if (a.w != b.w || a.h != b.h || a.c <= ac || b.c <= bc)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.w, b.h, b.c, a.w, a.h, a.c);
        return false;
    }

    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x)
        {
            int ai = a.w * a.h * ac + a.w * y + x;
            int bi = b.w * b.h * bc + b.w * y + x;
            if (!equivalent(a.data[ai], b.data[bi], epsilon))
            {
                printf("Mismatch (%d %d) %f %f\n", x, y, double(a.data[ai]), double(b.data[bi]));
                return false;
            }
        }

    return true;
}

bool sameMat(const Mat &a, const Mat &b)
{
    float const epsilon = 0.005f;

    if (a.w != b.w || a.h != b.h || a.c != b.c)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.w, b.h, b.c, a.w, a.h, a.c);
        return false;
    }

    for (int k = 0; k < a.c; ++k)
        for (int y = 0; y < a.h; ++y)
            for (int x = 0; x < a.w; ++x)
            {
                int index = a.w * a.h * k + a.w * y + x;
                if (!equivalent(a.data[index], b.data[index], epsilon))
                {
                    printf("Mismatch (%d %d %d) %f %f\n", x, y, k, a.data[index], b.data[index]);
                    return false;
                }
            }

    return true;
}

} // namespace vs
