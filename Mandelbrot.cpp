// ToDo: 1) parallelization of threads; 2) 

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <chrono>
#include <string>
#include <SFML/Graphics.hpp>
#include <immintrin.h>

const double SCREEN_WIDTH  = 1920;
const double SCREEN_HEIGHT = 1080;

const int     nMax   = 256;
const __m256d r2Max  = _mm256_set1_pd( 100. );

void GenMandelbrot( sf::VertexArray &va, double shiftX, double shiftY, double scale )
{
    double dx = 1. / scale;
    const __m256  _255  = _mm256_set1_ps( 255. );
    const __m256d _3210 = _mm256_set_pd( 3., 2., 1., 0. );
    const __m256 nmax   = _mm256_set1_ps( (float)nMax );

    for ( int iy = 0; iy < SCREEN_HEIGHT; iy++ ) 
    {
        double x0 = (            - shiftX ) / scale;
        double y0 = ( (double)iy - shiftY ) / scale;

        for ( int ix = 0; ix < SCREEN_WIDTH; ix += 4, x0 += dx * 4 )
        {
            __m256d X0 = _mm256_add_pd( _mm256_set1_pd(x0), _mm256_mul_pd( _3210, _mm256_set1_pd (dx) ) );
            __m256d Y0 = _mm256_set1_pd( y0 );

            __m256d X = X0, Y = Y0;
                
            __m256i N = _mm256_setzero_si256();

            for ( int n = 0; n < nMax; n++ )
            {
                __m256d x2 = _mm256_mul_pd( X, X ),
                        y2 = _mm256_mul_pd( Y, Y );

                __m256d r2 = _mm256_add_pd( x2, y2 );

                __m256d cmp = _mm256_cmp_pd( r2, r2Max, _CMP_LE_OS );
                int mask    = _mm256_movemask_pd( cmp );
                if ( !mask )
                    break;

                N = _mm256_sub_epi64( N, _mm256_castpd_si256( cmp ) );

                __m256d xy = _mm256_mul_pd( X, Y );

                X = _mm256_add_pd( _mm256_sub_pd( x2, y2 ), X0 );
                Y = _mm256_add_pd( _mm256_add_pd( xy, xy ), Y0 );
            }
                
            __m256 colorElem = _mm256_mul_ps( _mm256_sqrt_ps( _mm256_sqrt_ps( _mm256_div_ps( _mm256_cvtepi32_ps( N ), nmax ) ) ), _255 );

            for (int i = 0; i < 4; i++)
            {
                long int* ptrN         = (long int*)&N;
                float*    ptrColorElem = (float*)&colorElem;

                va[iy * SCREEN_WIDTH + ( ix + i )].position = sf::Vector2f(( ix + i ), iy);
                if ( ptrN[i] < nMax )
                {
                    sf::Color color( 0, 0, ( (int)ptrColorElem[i] * (int)ptrColorElem[i] / 255 ) % 200 + 100 );
                    va[iy * SCREEN_WIDTH + ( ix + i )].color = color;
                } else
                {
                    va[iy * SCREEN_WIDTH + ( ix + i )].color = sf::Color::Black;
                }
            }
        }
    }
}

int main()
{
    sf::String title = "Mandelbrot";
    sf::RenderWindow window( sf::VideoMode( SCREEN_WIDTH, SCREEN_HEIGHT ), title );

    window.setFramerateLimit( 30 );
    sf::VertexArray pixels( sf::Points, SCREEN_WIDTH * SCREEN_HEIGHT );

    double shiftX            = SCREEN_WIDTH  / 2.;
    double shiftY            = SCREEN_HEIGHT / 2.;
    double scale             = 200.;

    long int frameCounter = 0;
    auto beginTime        = std::chrono::steady_clock::now();

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
                        sf::Vector2i pos = sf::Mouse::getPosition( window );
                        shiftX -= pos.x - shiftX;
                        shiftY -= pos.y - shiftY;
                        scale *= 2.;

                        GenMandelbrot( pixels, shiftX, shiftY, scale );
                    }
                    if ( sf::Mouse::isButtonPressed( sf::Mouse::Right ) )
                    {
                        sf::Vector2i pos = sf::Mouse::getPosition( window );
                        shiftX = ( pos.x + shiftX ) / 2;
                        shiftY = ( pos.y + shiftY ) / 2;
                        scale  /= 2.;

                        GenMandelbrot( pixels, shiftX, shiftY, scale );
                    }
                    break;
                }
                case sf::Event::KeyPressed:
                {
                    switch ( Event.key.code )
                    {
                        case sf::Keyboard::W:
                        {
                            shiftY += 50.0;
                            GenMandelbrot( pixels, shiftX, shiftY, scale );
                            break;    
                        }
                        case sf::Keyboard::S:
                        {
                            shiftY -= 50.0;
                            GenMandelbrot( pixels, shiftX, shiftY, scale );
                            break;    
                        }
                        case sf::Keyboard::A:
                        {
                            shiftX += 50.0;
                            GenMandelbrot( pixels, shiftX, shiftY, scale );
                            break;    
                        }
                        case sf::Keyboard::D:
                        {
                            shiftX -= 50.0;
                            GenMandelbrot( pixels, shiftX, shiftY, scale );
                            break;    
                        }
                    }
                    
                    break;
                }  
            }
        }
        window.clear();
        window.draw( pixels );
        window.display();

        frameCounter++;
        auto newTime = std::chrono::steady_clock::now();
        if ( newTime - beginTime >= std::chrono::seconds{ 1 } )
        {
            window.setTitle( "Mandelbrot ( fps - " + std::to_string( frameCounter ) + " )" );

            frameCounter = 0;
            beginTime    = newTime;
        }
    }

    return 0;
}