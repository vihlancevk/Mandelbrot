// ToDo: 1) parallelization of threads; 2) 

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

const double SCREEN_WIDTH  = 1920;
const double SCREEN_HEIGHT = 1080;

const int    nMax   = 256;
const double r2Max  = 100.;

void GenMandelbrot( sf::VertexArray &va, double shiftX, double shiftY, double scale )
{
    double dx = 1. / scale;

    for ( int i = 0; i < SCREEN_HEIGHT; i++ ) 
    {
        double x0 = (           - shiftX ) / scale;
        double y0 = ( (double)i - shiftY ) / scale;

        for ( int j = 0; j < SCREEN_WIDTH; j++, x0 += dx )
        {
            double X = x0;
            double Y = y0;

            int N = 0;
            for ( ; N < nMax; N++ )
            {
                double x2 = X*X;
                double y2 = Y*Y;
                double xy = X*Y;

                double r2 = x2 + y2;
                       
                if ( r2 >= r2Max ) break;
                        
                X = x2 - y2 + x0,
                Y = xy + xy + y0;
            }
                
            double I = sqrt( sqrt( (double)N / (double)nMax ) ) * 255.0;
            
            va[i * SCREEN_WIDTH + j].position = sf::Vector2f(j, i);
            if (N < nMax)
            {
                sf::Color color(255 - (int)I, (int)I % 2 * 64, (int)I);
                va[i * SCREEN_WIDTH + j].color = color;
            } else
            {
                va[i * SCREEN_WIDTH + j].color = sf::Color::Black;
            }
        }
    }
}

int main()
{
    sf::String title = "Mandelbrot_plotter";
    sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), title );

    window.setFramerateLimit( 30 );
    sf::VertexArray pixels( sf::Points, SCREEN_WIDTH * SCREEN_HEIGHT );

    double shiftX = SCREEN_WIDTH  / 2.;
    double shiftY = SCREEN_HEIGHT / 2.;
    sf::Vector2f previousPos = sf::Vector2f( shiftX, shiftY );
    double scale  = 200.;

    GenMandelbrot( pixels, shiftX, shiftY, scale );

    while ( window.isOpen() )
    {
        sf::Event Event;
        while ( window.pollEvent( Event ) )
        {
            switch ( Event.type )
            {
                case sf::Event::Closed:
                {
                    window.close();
                    break;
                }
                case sf::Event::MouseButtonPressed:
                {
                    if ( sf::Mouse::isButtonPressed( sf::Mouse::Left ) )
                    {
                        previousPos = sf::Vector2f( shiftX, shiftY );
                        sf::Vector2i pos = sf::Mouse::getPosition( window );
                        shiftX -= pos.x - shiftX;
                        shiftY -= pos.y - shiftY;
                        scale *= 2.;

                        for ( int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ )
                        {
                            pixels[i].color = sf::Color::Black;
                        }
                        GenMandelbrot( pixels, shiftX, shiftY, scale );
                    }
                    if ( sf::Mouse::isButtonPressed( sf::Mouse::Right ) )
                    {
                        shiftX = previousPos.x;
                        shiftY = previousPos.y;
                        scale /= 2.;

                        for ( int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ )
                        {
                            pixels[i].color = sf::Color::Black;
                        }
                        GenMandelbrot( pixels, shiftX, shiftY, scale );
                    }
                }  
            }
        }
        window.clear();
        window.draw( pixels );
        window.display();
    }

    return 0;
}