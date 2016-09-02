#include <iostream>
#include <stdio.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGl.hpp>

#include <math.h>

#define CHUNKWIDTH 128
#define CHUNKHEIGHT 128

#define WW 640
#define WH 640

#define MAXNEIGHBORS 8
#define MINNEIGHBORS 6

#define MAXNUMPASSES 50

#define TIMEPERPASS 200

#define NUMBIOMES 4

#define GRASS 0
#define WATER 1
#define ROCK 2
#define LAVA 3

#define NOISEPERCENT 90

#define SOFTTRANSITION 0
#define HARDTRANSITION 1

#define SOFTTRANSITIONCHANCE 70

using namespace std;

class Biome{
public:

    static int maxPoints;

    Biome();

    void binaryToDecimal(bool binaryPass[]){
        for(int byte = 0; byte < maxPoints/8; byte++){
            int decimalValue = 0;
            for(int bit = 0; bit < 8; bit++){
                decimalValue += (int)(binaryPass[8*byte + bit]*pow(2, 7-bit));
            }
            currentPassDec[byte] = (int)decimalValue;
        }
    }

    void decimalToBinary(bool binaryPass[]){
        for(int byte = 0; byte < maxPoints/8; byte++){
            int decimalValue = (int)currentPassDec[byte];
            for(int bit = 0; bit < 8; bit++){
                binaryPass[8*byte + bit] = (bool)(decimalValue / (int)pow(2, 7-bit));
                decimalValue = decimalValue % (int)pow(2, 7-bit);
            }
        }
    }

    void generateFirstPass(){
        bool currentPassBinary[maxPoints];
        for(int bit = 0; bit < maxPoints; bit++){

            int noise = rand()%100;

            if(noise <= NOISEPERCENT){
                currentPassBinary[bit] = true;
            }

            else if(noise > NOISEPERCENT){
                currentPassBinary[bit] = false;
            }

        }
        binaryToDecimal(currentPassBinary);
    }

    void generateSubsequentPass(){
        bool previousPassBinary[maxPoints];
        bool currentPassBinary[maxPoints];

        decimalToBinary(previousPassBinary);
        /*
        for(int x = 0; x < maxPoints; x++){
            previousPassBinary[x] = currentPassBinary[x];
        }
        */

        for(int x = 0; x < maxPoints; x++){
            int numNeighbors = 0;

            if(x%CHUNKWIDTH == 0){
                numNeighbors += 3;

                if(x/CHUNKWIDTH == 0){
                    numNeighbors += 2;
                    numNeighbors += previousPassBinary[x + 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH + 1];
                }
                else if(x/CHUNKWIDTH + 1 == CHUNKHEIGHT){
                    numNeighbors += 2;
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH + 1];
                    numNeighbors += previousPassBinary[x + 1];
                }
                else{
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH + 1];
                    numNeighbors += previousPassBinary[x + 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH + 1];
                }
            }
            else if((x+1)%CHUNKWIDTH == 0){
                numNeighbors += 3;

                if(x/CHUNKWIDTH == 0){
                    numNeighbors += 2;
                    numNeighbors += previousPassBinary[x - 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH - 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                }
                else if(x/CHUNKWIDTH + 1 == CHUNKHEIGHT){
                    numNeighbors += 2;
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH - 1];
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x - 1];
                }
                else{
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH - 1];
                    numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                    numNeighbors += previousPassBinary[x - 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH - 1];
                    numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                }
            }
            else if(x/CHUNKWIDTH == 0 && x%CHUNKWIDTH != 0 && (x+1)%CHUNKWIDTH != 0){
                numNeighbors += 3;
                numNeighbors += previousPassBinary[x - 1];
                numNeighbors += previousPassBinary[x + 1];
                numNeighbors += previousPassBinary[x + CHUNKWIDTH - 1];
                numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                numNeighbors += previousPassBinary[x + CHUNKWIDTH + 1];
            }
            else if(x/CHUNKWIDTH + 1 == CHUNKHEIGHT && x%CHUNKWIDTH != 0 && (x+1)%CHUNKWIDTH != 0){
                numNeighbors += 3;
                numNeighbors += previousPassBinary[x - CHUNKWIDTH - 1];
                numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                numNeighbors += previousPassBinary[x - CHUNKWIDTH + 1];
                numNeighbors += previousPassBinary[x - 1];
                numNeighbors += previousPassBinary[x + 1];
            }
            else{
                numNeighbors += previousPassBinary[x - CHUNKWIDTH - 1];
                numNeighbors += previousPassBinary[x - CHUNKWIDTH];
                numNeighbors += previousPassBinary[x - CHUNKWIDTH + 1];

                numNeighbors += previousPassBinary[x - 1];
                numNeighbors += previousPassBinary[x + 1];

                numNeighbors += previousPassBinary[x + CHUNKWIDTH - 1];
                numNeighbors += previousPassBinary[x + CHUNKWIDTH];
                numNeighbors += previousPassBinary[x + CHUNKWIDTH + 1];
            }

            if(numNeighbors >= MINNEIGHBORS && numNeighbors <= MAXNEIGHBORS){
                currentPassBinary[x] = true;
            }
            else{
                currentPassBinary[x] = false;
            }
        }

        binaryToDecimal(currentPassBinary);
    }

    void setInitialBiome(){
        mediumID = rand()%NUMBIOMES;

        if(mediumID == GRASS || mediumID == WATER){
            spawnID = rand()%LAVA;
        }
        else if(mediumID == ROCK){
            spawnID = rand()%NUMBIOMES;
        }
        else if(mediumID == LAVA){
            spawnID = ROCK + rand()%2;
        }
    }

    void setSubsequentBiome(int previousMedium){

        int transitionType;
        int transitionCheck = rand()%100;
        if(transitionCheck <= SOFTTRANSITIONCHANCE){
            transitionType = SOFTTRANSITION;
        }
        else{
            transitionType = HARDTRANSITION;
        }

        if(transitionType == SOFTTRANSITION){ //Same medium
            mediumID = previousMedium;
            if(mediumID == GRASS || mediumID == WATER){
                spawnID = rand()%LAVA;
            }
            else if(mediumID == ROCK){
                spawnID = rand()%NUMBIOMES;
            }
            else if(mediumID == LAVA){
                spawnID = ROCK + rand()%2;
            }
        }
        else if(transitionType == HARDTRANSITION){ //Different medium
            spawnID = previousMedium;
            mediumID = rand()%NUMBIOMES;
            while(mediumID == previousMedium){
                mediumID = rand()%NUMBIOMES;
            }
            if(mediumID == GRASS || mediumID == WATER){
                spawnID = rand()%LAVA;
            }
            else if(mediumID == ROCK){
                spawnID = rand()%NUMBIOMES;
            }
            else if(mediumID == LAVA){
                spawnID = ROCK + rand()%2;
            }

        }

    }

    void generateInitialChunk(){

        xCoord = 0;
        yCoord = 0;

        generateFirstPass();

        for(int x = 0; x < numPasses; x++){
            generateSubsequentPass();
        }

        setInitialBiome();
    }

    void generateSubsequentChunk(int previousMedium, int previousX, int previousY, int plusX, int plusY){

        xCoord = previousX + plusX;
        yCoord = previousY + plusY;

        generateFirstPass();
        for(int x = 0; x < numPasses; x++){
            generateSubsequentPass();
        }
        setSubsequentBiome(previousMedium);
    }

    void displayBiome(sf::RenderWindow* window){

        sf::Color mediumColor;
        sf::Color spawnColor;

        if(mediumID == GRASS){
            mediumColor = sf::Color::Green;
        }
        else if(mediumID == WATER){
            mediumColor = sf::Color::Blue;
        }
        else if(mediumID == ROCK){
            mediumColor = sf::Color(128, 64, 0);
        }
        else if(mediumID == LAVA){
            mediumColor = sf::Color::Red;
        }

        if(spawnID == GRASS){
            spawnColor = sf::Color::Green;
        }
        else if(spawnID == WATER){
            spawnColor = sf::Color::Blue;
        }
        else if(spawnID == ROCK){
            spawnColor = sf::Color(128, 64, 0);
        }
        else if(spawnID == LAVA){
            spawnColor = sf::Color::Red;
        }

        sf::VertexArray points(sf::Points, maxPoints);

        bool currentPassBinary[maxPoints];
        decimalToBinary(currentPassBinary);

        for(int x = 0; x < maxPoints; x++){

            points[x].position = sf::Vector2f(xCoord + x%CHUNKWIDTH, yCoord + x/CHUNKWIDTH);

            if(currentPassBinary[x] == true){ //Medium
                points[x].color = mediumColor;
            }
            else if(currentPassBinary[x] == false){ //Spawn
                points[x].color = spawnColor;
            }
        }

        (*window).draw(points);
    }


    void setNeighbor(string direction, int biomeID){
        if(direction == "North"){
            northNeighbor = biomeID;
        }
        else if(direction == "South"){
            southNeighbor = biomeID;
        }
        else if(direction == "East"){
            eastNeighbor = biomeID;
        }
        else if(direction == "West"){
            westNeighbor = biomeID;
        }
    }

    int getNeighbor(string direction){
        int biomeID;
        if(direction == "North"){
            biomeID = northNeighbor;
        }
        else if(direction == "South"){
            biomeID = southNeighbor;
        }
        else if(direction == "East"){
            biomeID = eastNeighbor;
        }
        else if(direction == "West"){
            biomeID = westNeighbor;
        }
        return biomeID;
    }

    int getMedium(){
        return mediumID;
    }

    int getSpawn(){
        return spawnID;
    }

    int getXCoordinates(){
        return xCoord;
    }

    int getYCoordinates(){
        return yCoord;
    }


private:

    unsigned char currentPassDec[CHUNKWIDTH*CHUNKHEIGHT/8];
    //bool currentPassBinary[CHUNKWIDTH*CHUNKHEIGHT];

    int mediumID;
    int spawnID;
    int numPasses;
    int xCoord;
    int yCoord;

    int northNeighbor;
    int southNeighbor;
    int eastNeighbor;
    int westNeighbor;
};

Biome::Biome(){
    numPasses = 1 + rand()%MAXNUMPASSES;

    //-1 means not set
    northNeighbor = -1;
    southNeighbor = -1;
    eastNeighbor = -1;
    westNeighbor = -1;
}


int Biome::maxPoints = CHUNKWIDTH*CHUNKHEIGHT;

int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(WW, WH), "Map generator 2019");

    int numBiomes = 50;
    int currentBiome = 0;

    Biome biomes[numBiomes];
    biomes[0].generateInitialChunk();

    while(window.isOpen()){

        bool upPressed = false;
        bool downPressed = false;
        bool rightPressed = false;
        bool leftPressed = false;
        string direction;

        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape){
                window.close();
            }

            if(event.type == sf::Event::KeyReleased){
                if(event.key.code == sf::Keyboard::Up){
                    upPressed = true;
                    if(biomes[currentBiome].getNeighbor("North") == -1){

                        biomes[currentBiome + 1].generateSubsequentChunk(biomes[currentBiome].getMedium(), biomes[currentBiome].getXCoordinates(), biomes[currentBiome].getYCoordinates(), 0, -CHUNKHEIGHT);
                        biomes[currentBiome].setNeighbor("North", currentBiome + 1);
                        biomes[currentBiome + 1].setNeighbor("South", currentBiome);
                        currentBiome ++;
                    }
                }
                else if(event.key.code == sf::Keyboard::Down){
                    downPressed = true;
                    if(biomes[currentBiome].getNeighbor("South") == -1){
                        biomes[currentBiome + 1].generateSubsequentChunk(biomes[currentBiome].getMedium(), biomes[currentBiome].getXCoordinates(), biomes[currentBiome].getYCoordinates(), 0, CHUNKHEIGHT);
                        biomes[currentBiome].setNeighbor("South", currentBiome + 1);
                        biomes[currentBiome + 1].setNeighbor("North", currentBiome);
                        currentBiome ++;
                    }
                }
                else if(event.key.code == sf::Keyboard::Right){
                    rightPressed = true;
                    if(biomes[currentBiome].getNeighbor("East") == -1){
                        biomes[currentBiome + 1].generateSubsequentChunk(biomes[currentBiome].getMedium(), biomes[currentBiome].getXCoordinates(), biomes[currentBiome].getYCoordinates(), CHUNKWIDTH, 0);
                        biomes[currentBiome].setNeighbor("East", currentBiome + 1);
                        biomes[currentBiome + 1].setNeighbor("West", currentBiome);
                        currentBiome ++;
                    }
                }
                else if(event.key.code == sf::Keyboard::Left){
                    leftPressed = true;
                    if(biomes[currentBiome].getNeighbor("West") == -1){
                        biomes[currentBiome + 1].generateSubsequentChunk(biomes[currentBiome].getMedium(), biomes[currentBiome].getXCoordinates(), biomes[currentBiome].getYCoordinates(), -CHUNKWIDTH, 0);
                        biomes[currentBiome].setNeighbor("West", currentBiome + 1);
                        biomes[currentBiome + 1].setNeighbor("East", currentBiome);
                        currentBiome ++;
                    }
                }
            }
        }
        cout << sizeof(biomes) << endl;
        window.clear(sf::Color::Black);

        for(int x = 0; x < currentBiome + 1; x++){
            biomes[x].displayBiome(&window);
        }

        window.display();
    }

}
