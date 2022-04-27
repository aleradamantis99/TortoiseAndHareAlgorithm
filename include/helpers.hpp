#pragma once
#include <random>
#include <cmath>
#include <concepts>
#include <SFML/Graphics.hpp>

template <std::integral I>
class RandomGen
{
public:
    RandomGen(I min, I max):
        dist(min, max),
        gen(std::random_device{}())
    {}

    I operator()()
    {
        return dist(gen);
    }

private:
    std::uniform_int_distribution<I> dist;
    std::mt19937 gen;
};
template <std::integral I>
void fill_with_random(std::vector<I>& v, I min, I max)
{
    RandomGen<I> gen(min, max);
    std::ranges::generate(v, gen);
}

template <std::integral I>
auto find_duplicates(const std::vector<I>& v)
{
    I tortoise=0, hare=0;

    do
    {
        tortoise = v[tortoise];
        hare = v[v[hare]];
    } while(tortoise != hare);

    I ptr1 = 0;
    I ptr2 = hare;

    while(ptr1 != ptr2)
    {
        ptr1 = v[ptr1];
        ptr2 = v[ptr2];
    }
    return ptr2;
}

// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val)
{
  hue %= 360;
  while(hue<0) hue += 360;

  if(sat<0.f) sat = 0.f;
  if(sat>1.f) sat = 1.f;

  if(val<0.f) val = 0.f;
  if(val>1.f) val = 1.f;

  int h = hue/60;
  float f = float(hue)/60-h;
  float p = val*(1.f-sat);
  float q = val*(1.f-sat*f);
  float t = val*(1.f-sat*(1-f));

  switch(h)
  {
    default:
    case 0:
    case 6: return sf::Color(val*255, t*255, p*255);
    case 1: return sf::Color(q*255, val*255, p*255);
    case 2: return sf::Color(p*255, val*255, t*255);
    case 3: return sf::Color(p*255, q*255, val*255);
    case 4: return sf::Color(t*255, p*255, val*255);
    case 5: return sf::Color(val*255, p*255, q*255);
  }
}

float vector_mod(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

void change_size_to(sf::Vector2f& v, float new_size)
{
    float size = vector_mod(v);
    if (size==0)
    {
        throw std::invalid_argument("Can't change the size of Null Vector to " + std::to_string(new_size));
    }
    v *= new_size/vector_mod(v);
}
sf::Vector2f turn_vector(sf::Vector2f v, float rad)
{
    return {v.x * std::cos(rad) - v.y * std::sin(rad), v.x * std::sin(rad) + v.y * std::cos(rad)};
}
float distance(sf::Vector2f p1, sf::Vector2f p2)
{
    return vector_mod(p2-p1);
}
sf::VertexArray get_triangle_for_arrow(sf::Vertex p1, sf::Vertex p2, float triangle_height = 20.f)
{
    sf::Vector2f v = p1.position-p2.position;
    if (v == sf::Vector2f(0,0))
    {
        return {};
    }
    change_size_to(v, triangle_height);
    sf::Vector2f u{v.y, -v.x};
    u *= (triangle_height*0.5f)/triangle_height;
    sf::VertexArray triangle_vertex(sf::Triangles, 3);
    sf::Vector2f mid_vertex = p2.position + v;

    triangle_vertex[0] = p2.position;
    triangle_vertex[0].color = p2.color;
    triangle_vertex[1] = mid_vertex+u;
    triangle_vertex[1].color = p2.color;
    triangle_vertex[2] = mid_vertex-u;
    triangle_vertex[2].color = p2.color;
    return triangle_vertex;
}

void draw_arrow(sf::RenderWindow& window, sf::Vector2f p1, sf::Vector2f p2, sf::Color c, float triangle_height = 20.)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0] = p1;
    line[0].color = c;

    line[1] = p2;
    line[1].color = c;

    auto triangle = get_triangle_for_arrow(line[0], line[1], triangle_height);

    window.draw(line);
    window.draw(triangle);
}


