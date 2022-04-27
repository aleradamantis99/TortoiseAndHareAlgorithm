#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "TortoiseAndHare.hpp"

void start(TortoiseAndHare tah)
{
   
}

int main(int argc, char* argv[])
{
    sf::Clock clock;
    int n_circles = 6;
    std::vector<int> v;
    if (argc > 1)
    {
        std::string argument(argv[1]);
        try
        {
            n_circles = std::stoi(argument);
            v.resize(n_circles);
            fill_with_random(v, 1, n_circles-1);
        }
        catch (std::invalid_argument)
        {
            std::ifstream in(argument);
            std::string vector_string;
            std::getline(in, vector_string);
            std::stringstream ss(vector_string);
            for (int i; ss >> i;) {
                v.push_back(i);
                if (ss.peek() == ',')
                {
                    ss.ignore();
                }
            }
        } 
    }

    size_t width = 800, height = 700;
    float cwidth = width/2.f, cheight = height/2.f;
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");
    TortoiseAndHare tah(window, 200, v);
    for (int i: v)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    for (int i=0; i<v.size(); i++)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << find_duplicates(v) << '\n';

    tah.start_loop();
    

    return 0;
}