#include "source/vs.hpp"

int unit_tests_basic(int argc, char **argv);
int unit_tests_filtering(int argc, char **argv);

int main(int argc, char **argv)
{
    //    vs::Mat loaded = vs::loadImage("./data/Lenna.png");
    //    loaded.print(std::cout);
    //    std::cout << "---" << std::endl;
    //    loaded.print(std::cout, 10, 10);
    //    vs::saveImage("out.jpg", loaded);


    std::cout << "unit tests starting" << std::endl;
    unit_tests_basic(argc, argv);
    unit_tests_filtering(argc, argv);
    std::cout << "unit tests finished" << std::endl;
    return 0;
}
