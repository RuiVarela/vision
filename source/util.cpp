#include "vs.hpp"

#ifdef VS_USE_OPENCV
#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#endif // VS_USE_OPENCV


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

static void del_arg(int argc, char **argv, int index)
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
                    printf("Mismatch (%d %d %d) %f %f\n", x, y, k, double(a.data[index]), double(b.data[index]));
                    return false;
                }
            }

    return true;
}

template <typename T>
inline std::ostream &print(MatT<T> const &m, std::ostream &out, int max_cols, int max_rows)
{
    out << "Header " << m.w << "x" << m.h << "x" << m.c << std::endl;
    out << std::fixed << std::setprecision(3);

    if (max_cols < 0)
        max_cols = m.w;

    if (max_rows < 0)
        max_rows = m.h;

    if (max_rows > 0 && max_rows > 0)
        for (int i = 0; i < m.c; ++i)
        {
            out << "channel " << i << std::endl;

            for (int j = 0; j < m.h; ++j)
            {
                if (j > max_rows)
                {
                    out << "... (" << m.h - j << ")" << std::endl;
                    break;
                }

                for (int k = 0; k < m.w; ++k)
                {
                    if (k > max_cols)
                    {
                        out << " ... (" << m.w - k << ")";
                        break;
                    }

                    if (k > 0)
                        out << ", ";

                    out << std::fixed << m.data[i * m.h * m.w + j * m.w + k];
                }
                out << std::endl;
            }
        }

    return out;
}


#ifdef VS_USE_OPENCV
static std::mutex g_stream_mutex;
static int g_stream_counter = 0;
static std::map<int, cv::VideoCapture*> g_captures;

int showMat(Mat const &a, std::string const &name, int ms)
{
    Mat copy = a.clone();
    cv::Mat frame;
    
    if (a.c == 1) {
        frame = cv::Mat(a.h, a.w, CV_8UC1);
    } else {
        vs::rgb2bgrInplace(copy);
        frame = cv::Mat(a.h, a.w, CV_8UC3);
    }
    
    int step = frame.step1();
    for(int y = 0; y < copy.h; ++y){
        for(int x = 0; x < copy.w; ++x){
            for(int k= 0; k < copy.c; ++k){
                frame.data[y*step + x*copy.c + k] = (unsigned char)(copy.get(x,y,k)*255);
            }
        }
    }

    imshow(name.c_str(), frame);
    return cv::waitKey(ms);
}

int openStream(std::string const &name)
{
    std::lock_guard<std::mutex> guard(g_stream_mutex);
    int id = ++g_stream_counter;

    int camera = -1;
    try
    {
        camera = std::stoi(name);
    }
    catch (...)
    {
    }

    cv::VideoCapture *capture = new cv::VideoCapture();
    bool ok = false;
    if (camera >= 0)
        ok = capture->open(camera);
    else
        ok = capture->open(name);

    if (ok)
    {
        g_captures[id] = capture;
        return id;
    }
    else
    {
        delete (capture);
    }

    return -1;
}

void closeStream(int const id)
{
    std::lock_guard<std::mutex> guard(g_stream_mutex);
    std::map<int, cv::VideoCapture *>::iterator i = g_captures.find(id);
    if (i != g_captures.end())
    {
        delete (i->second);
        g_captures.erase(i);
    }
}

void readStream(int const id, vs::Mat &out)
{
    std::lock_guard<std::mutex> guard(g_stream_mutex);
    std::map<int, cv::VideoCapture *>::iterator i = g_captures.find(id);
    if (i == g_captures.end())
    {
        out.reshape(0, 0, 0);
        return;
    }
    cv::Mat frame;
    *(i->second) >> frame;

    out.reshape(frame.cols, frame.rows, frame.channels());

    int step = frame.step1();
    for (int k = 0; k < out.c; ++k)
        for (int y = 0; y < out.h; ++y)
            for (int x = 0; x < out.w; ++x)
                out.set(x,y,k, frame.data[y * step + x * out.c + k] / 255.);
}

#else
int showMat(Mat const &a, std::string const& name, int ms) {
    bool opencv_installed = false;
    assert(opencv_installed);
    return -1;
}

int openStream(std::string const& name) {
    bool opencv_installed = false;
    assert(opencv_installed);
    return -1;
}

void closeStream(int const id) {
    bool opencv_installed = false;
    assert(opencv_installed);
}

void readStream(int const id, vs::Mat& out) {
    bool opencv_installed = false;
    assert(opencv_installed);
}
#endif // VS_USE_OPENCV



//
// force template instantiation
//
template std::ostream &print(MatT<float> const &m, std::ostream &out, int max_cols, int max_rows);
template std::ostream &print(MatT<double> const &m, std::ostream &out, int max_cols, int max_rows);

} // namespace vs
