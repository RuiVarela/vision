#include "../source/vs.hpp"

int main(int argc, char **argv)
{
    vs::Mat loaded = vs::loadImage("./data/Lenna.png");
 
    loaded.print(std::cout);
    std::cout << "---" << std::endl;
    loaded.print(std::cout, 10, 10);
    
    vs::saveImage("out.jpg", loaded);
}