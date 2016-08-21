#include <iostream>
#include <stdio.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGl.hpp>

#define WW 128
#define WH 128

#define MAXNEIGHBORS 8
#define MINNEIGHBORS 6

#define NOISEPERCENT 90

#define NUMPASSES 50

#define TIMEPERPASS 200

using namespace std;

class Pass{
public:
    static int maxPoints;

    Pass();

    bool getResults(int x){
        return passResult[x];
    }
    int getNumPoints(){
        return numPoints;
    }

    void generateFirstPass(){
        for(int x = 0; x < maxPoints; x++){

            int noise = rand()%100;

            if(noise <= NOISEPERCENT){
                passResult[x] = true;
                numPoints ++;
            }

            else if(noise > NOISEPERCENT){
                passResult[x] = false;
            }
        }
    }

    void generateNextPass(Pass previousPass){
        for(int x = 0; x < maxPoints; x++){

            passResult[x] = previousPass.getResults(x);

            int numNeighbors = 0;

            if(x%WW == 0){
                numNeighbors += 3;

                if(x/WW == 0){
                    numNeighbors +=2;
                    numNeighbors += previousPass.getResults(x + 1);
                    numNeighbors += previousPass.getResults(x + WW);
                    numNeighbors += previousPass.getResults(x + WW + 1);
                }
                else if(x/WW + 1 == WH){
                    numNeighbors += 2;
                    numNeighbors += previousPass.getResults(x - WW);
                    numNeighbors += previousPass.getResults(x - WW + 1);
                    numNeighbors += previousPass.getResults(x + 1);
                }
                else{
                    numNeighbors += previousPass.getResults(x - WW);
                    numNeighbors += previousPass.getResults(x - WW + 1);
                    numNeighbors += previousPass.getResults(x + 1);
                    numNeighbors += previousPass.getResults(x + WW);
                    numNeighbors += previousPass.getResults(x + WW + 1);
                }
            }
            else if((x+1)%WW == 0){
                numNeighbors += 3;

                if(x/WW == 0){
                    numNeighbors += 2;
                    numNeighbors += previousPass.getResults(x - 1);
                    numNeighbors += previousPass.getResults(x + WW - 1);
                    numNeighbors += previousPass.getResults(x + WW);
                }
                else if(x/WW + 1 == WH){
                    numNeighbors += 2;
                    numNeighbors += previousPass.getResults(x - WW - 1);
                    numNeighbors += previousPass.getResults(x - WW);
                    numNeighbors += previousPass.getResults(x - 1);
                }
                else{
                    numNeighbors += previousPass.getResults(x - WW - 1);
                    numNeighbors += previousPass.getResults(x - WW);
                    numNeighbors += previousPass.getResults(x - 1);
                    numNeighbors += previousPass.getResults(x + WW - 1);
                    numNeighbors += previousPass.getResults(x + WW);
                }
            }
            else if(x/WW == 0 && x%WW != 0 && (x+1)%WW != 0){
                numNeighbors += 3;
                numNeighbors += previousPass.getResults(x - 1);
                numNeighbors += previousPass.getResults(x + 1);
                numNeighbors += previousPass.getResults(x + WW - 1);
                numNeighbors += previousPass.getResults(x + WW);
                numNeighbors += previousPass.getResults(x + WW + 1);
            }
            else if(x/WW + 1 == WH && x%WW != 0 && (x+1)%WW != 0){
                numNeighbors += 3;
                numNeighbors += previousPass.getResults(x - WW - 1);
                numNeighbors += previousPass.getResults(x - WW);
                numNeighbors += previousPass.getResults(x - WW + 1);
                numNeighbors += previousPass.getResults(x - 1);
                numNeighbors += previousPass.getResults(x + 1);
            }
            else{
                numNeighbors += previousPass.getResults(x - WW - 1);
                numNeighbors += previousPass.getResults(x - WW);
                numNeighbors += previousPass.getResults(x - WW + 1);

                numNeighbors += previousPass.getResults(x - 1);
                numNeighbors += previousPass.getResults(x + 1);

                numNeighbors += previousPass.getResults(x + WW - 1);
                numNeighbors += previousPass.getResults(x + WW);
                numNeighbors += previousPass.getResults(x + WW + 1);
            }

            if(numNeighbors >= MINNEIGHBORS && numNeighbors <= MAXNEIGHBORS){
                passResult[x] = true;
                numPoints++;
            }
            else{
                passResult[x] = false;
            }
        }
    }

    void displayArray(sf::RenderWindow* window){
        sf::VertexArray points(sf::Points, maxPoints);
        for(int x = 0; x < maxPoints ; x++){

            points[x].position = sf::Vector2f(x%WW, x/WW);

            if(passResult[x] == true){
                points[x].color = sf::Color::Green;
            }
            else if(passResult[x] == false){
                points[x].color = sf::Color::Blue;
            }

        }

 /*
        int numVoids = maxPoints - numPoints;

        sf::VertexArray voids(sf::Points, maxPoints - numPoints);

        int pixelID = 0;
        for(int x = 0; x < numPoints; x++){
            while(passResult[pixelID] == false){
                pixelID ++;
            }
            points[x].position = sf::Vector2f(pixelID%WW, pixelID/WW);
            points[x].color = sf::Color::Green;
            pixelID ++;
        }
        pixelID = 0;

        for(int x = 0; x < numVoids; x++){
            while(passResult[pixelID] == true){
                pixelID ++;
            }
            voids[x].position = sf::Vector2f(pixelID%WW, pixelID/WW);
            points[x].color = sf::Color::Green;
            pixelID ++;
        }
*/
        (*window).draw(points);
    }


private:
    int numPoints;
    bool passResult[WW*WH];
};


Pass::Pass(){
    numPoints = 0;
}

int Pass::maxPoints = WW*WH;


int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(WW, WH), "Map generator 2019");
    char buffer[32];

    int numPasses = NUMPASSES;
    Pass pass[numPasses];

    pass[0].generateFirstPass();
    for(int x = 1; x < numPasses; x++){
        pass[x].generateNextPass(pass[x-1]);
    }


    int count = 0;
    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape){
                window.close();
            }
        }


        count = (count + 1)%(TIMEPERPASS*numPasses);

        window.clear(sf::Color::Black);

        for(int x = 0; x < numPasses; x++){
            if(count > x*TIMEPERPASS && count <= (x+1)*TIMEPERPASS){
                sprintf(buffer, "Pass: %d", x);
                window.setTitle(buffer);
                pass[x].displayArray(&window);
            }

        }



        window.display();
    }

}
