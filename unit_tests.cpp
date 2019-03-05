#include "source/vs.hpp"

int unit_tests_basic(int argc, char **argv);
int unit_tests_filtering(int argc, char **argv);
int unit_tests_features(int argc, char **argv);
int unit_tests_matrix(int argc, char **argv);

int main(int argc, char **argv)
{
    std::cout << "unit tests starting" << std::endl;
    //unit_tests_basic(argc, argv);
    //unit_tests_filtering(argc, argv);
    //unit_tests_features(argc, argv);
    unit_tests_matrix(argc, argv);
    std::cout << "unit tests finished" << std::endl;

    return 0;
}
