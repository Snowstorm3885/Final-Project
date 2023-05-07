// NoahHarrisCGT215FinalProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream> // gives us access to cout and cin
#include <SFML/Graphics.hpp> // gives us access to the SFML window and grapics functions
#include <SFPhysics.h> // Gives us access to all the physics stuff
#include <vector> // Gives us access to the vector class for lists
#include <SFML/Audio.hpp> // Gives us access to the SFML stuff for soundfx and music

using namespace std;  //means we dont have to type std::cout etc
using namespace sf; // means we Dont have to type sf:: for the SFML stuff
using namespace sfp; //means wedont have to type sfp:: for the physics stuff

const float KB_SPEED = 0.2; //A pixels per second measure for the movement of the cannon


/// <summary>
/// This is a utility function for loading SFML Textures
/// </summary>
/// <param name="tex">The Texture object to hold the image data</param>
/// <param name="filename">The path to the image file starting from the project directory</param>
void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) { //try to load the image file data, if fails do body
        cout << "Could not load " << filename << endl; //tell use what failed
        exit(1); //end the program with an exist code of 1
    }
}

/// <summary>
/// This function reads the keybaord for left arrow and right arrow keys
/// and moves the cannon KB_SPEED pixels per elapsed ms one direction or the other
/// </summary>
/// <param name="cannon">A reference to the cannon sprite to move</param>
/// <param name="elapsedMS">The milliseconds passed since last frame</param>
void Movecannon(PhysicsSprite& cannon, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) { // check to see if right arrow is pressed
        Vector2f newPos(cannon.getCenter()); //get the cross bow sprites current center
        newPos.x = newPos.x + (KB_SPEED * elapsedMS); //caluate a new X value based on elapsed time
        cannon.setCenter(newPos); //set the sprite's center position to the new value
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {// check to see if left arrow is pressed
        Vector2f newPos(cannon.getCenter()); //get the cross bow sprites current center
        newPos.x = newPos.x - (KB_SPEED * elapsedMS); //caluate a new X value based on elapsed time
        cannon.setCenter(newPos); //set the sprite's center position to the new value
    }
}

/// <summary>
/// A utility function to get the height and width of a peice of
/// SFML text
/// </summary>
/// <param name="text">The Text object to measure</param>
/// <returns>a Vector2f containing the width and the height of the text object in pixels</returns>
Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds(); //Gets a rectangle that contains the text
    return Vector2f(r.width, r.height); //returns the width and height of that rectangle
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Balloon Buster"); //create an 800x600 window to draw in
    World world(Vector2f(0, 0)); // create a physics world with no gravity
    int score(0); //declare a place to keep score and initialize it to 0
    int arrows(5); //declare a counter for arrows left and initialize it to 5

    SoundBuffer shotBuffer;
    if (!shotBuffer.loadFromFile("cannon.wav")) {
        cout << "could not load cannon.wav" << endl;
        exit(6);
    }
    Sound shotSound;
    shotSound.setBuffer(shotBuffer);

    SoundBuffer popBuffer; //declare an SFML buffer to hold the sound data
    if (!popBuffer.loadFromFile("shiphit.wav")) { //try to load the sound data into popbuffer
        cout << "coul not load shiphit.wav" << endl; // if failed, tell us
        exit(5); //end the program on failure with an exit code of 5 
    }
    Sound popSound; //declare a sound player to play the data in popBuffer
    popSound.setBuffer(popBuffer); // set the popBuffer to be the sound player's data

    Music music; //declare a streamign music player that will load data from a file as needed
    if (!music.openFromFile("pirate1uf.ogg")) { //open the file to stream the music data from
        cout << "Failed to load pirate1uf.ogg "; //tell us if the open failed
        exit(6); //end the program on failure with an exit code of 6 
    }
    music.setLoop(true); // set the music player to loop the music forever
    music.play(); // start the music playing

    PhysicsSprite cannon; //make a sprite to draw the cannon
    Texture cbowTex; // make a texture to hold the image data for the cannon
    LoadTex(cbowTex, "images/cannon.png"); // load the cannon image data
    cannon.setTexture(cbowTex); //set the cannon sprite to use the cannon image data
    Vector2f sz = cannon.getSize(); // get the size of the image
    cannon.setCenter(Vector2f(400, // set the cannon position to the center/bottom of the screen
        600 - (sz.y / 2)));

    PhysicsSprite arrow;//make a sprite to draw the arrow
    Texture arrowTex; // make a texture to hold the image data for the arrpw
    LoadTex(arrowTex, "images/cannonball.png"); // load the arrow image data
    arrow.setTexture(arrowTex);//set the arrow sprite to use the arrow image data
    bool drawingArrow(false); //set the arrow as not drawn til it is fired

    //create the top and side boundaries
    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true); // so it wont move
    world.AddPhysicsBody(top); //must be added to physics world to interact with
    // physics objects (arrow for top) 

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true); // so it wont move
    world.AddPhysicsBody(left); //must be added to physics world to interact with
    // physics objects (ballons for sides) 

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true); // so it wont move
    world.AddPhysicsBody(right);//must be added to physics world to interact with
    // physics objects (ballons for sides)

    Texture background; //makes a texture that holds the background image
    LoadTex(background, "images/background.png"); // loads background
    background.loadFromFile("images/background.png");
    Sprite Background; //creates a sprite that allows the image to be drawn.
    Vector2u size = background.getSize();
    Background.setTexture(background);
    Background.setOrigin(0, 0);

    Texture redTex; //make a texture to hold the image data of a red balloon
    LoadTex(redTex, "images/pirateship.png"); // load the red balloon image data into the Texture
    //we use the same texture for all the balloon sprites
    PhysicsShapeList<PhysicsSprite> balloons; // declare a PhysicsShapeList to hold the list of balloons
    for (int i(0); i < 6; i++) { //we are going to make 6 balloons
        PhysicsSprite& balloon = balloons.Create(); // create a balloon sprite on the list
        balloon.setTexture(redTex); //set the new balloon sprite's texture
        int x = 75 + ((700 / 5) * i); // calculate its position based on which of the 6 it is
        Vector2f sz = balloon.getSize(); //get the size of the ballon so we can set the location
        balloon.setCenter(Vector2f(x, 20 + (sz.y / 2))); // set the baloon location
        balloon.setVelocity(Vector2f(0.125, 0)); //give the balloon a starting shove to get it moving
        world.AddPhysicsBody(balloon); // add the balloon to the world so it updates and checks for collision
        balloon.onCollision = //the function to handle it when a balloon collides with another object
            //these are all variables declared in the main scope that the lambda function needs access to
            [&drawingArrow, &world, &arrow, &balloon, &balloons, &score, &popSound, &left, &right, &sz, &x, &i]
        (PhysicsBodyCollisionResult result) { //the collision result is passed in by the world it has
            //data for this specific collision
            if (result.object2 == arrow) { // did we collied with an arrow?
                popSound.play(); // if so play the pop sound...
                drawingArrow = false; // ... make the arrow disappear ...
                world.RemovePhysicsBody(arrow); //... remove the arrow from the position and collision updates
                world.RemovePhysicsBody(balloon); // ..remove the balooon from the position and collision updates
                balloons.QueueRemove(balloon); // ... queue the baloonto be deleted
                score += 10; // update the sore
                //NOTE: It is VERY important that an object removed from a PhysicsShapeList is ALSO removed
                // from the world and anywhere else it is used.  Removal form the PhysicsShpe list deletes
                // the object and returns its memory to the system.  Trying to access it after that will
                // cause system errors.
            }
            if (result.object2 == right) {
                balloon.setCenter(Vector2f(700, 200));
            }
            if (result.object2 == left) {
                balloon.setCenter(Vector2f(100, 400));
            }
        };
    }
    // This creates the callback lambda for arrow hitting top and attaches it to the top
    top.onCollision = [&drawingArrow, &world, &arrow] //accesses drawing arrow, world and arrow 
    (PhysicsBodyCollisionResult result) { //the collision data returned by the system when called
        drawingArrow = false; //turn off arrow drawing
        world.RemovePhysicsBody(arrow); //turn off arrow motion and collision by removign from world
    };

    Font fnt; // holds the images of the characters to draw
    if (!fnt.loadFromFile("arial.ttf")) { // load from the true type font file
        cout << "Could not load font." << endl; //tell us if it couldnt load the file
        exit(3); //end with an exit code of 3
    }
    Clock clock; //Used to get the time in order to calculate frame times
    Time lastTime(clock.getElapsedTime()); //initialize lastTime to right now
    Time currentTime(lastTime); //initialize currentTime to right now

    while ((arrows > 0) || drawingArrow) { //keep drawing frames until all arrows have been shot and none
        //are in flight
        currentTime = clock.getElapsedTime(); //get the time right now
        Time deltaTime = currentTime - lastTime; //subtract the time last frame to get the difference
        long deltaMS = deltaTime.asMilliseconds(); // convert that difference to milliseconds
        if (deltaMS > 9) { //if its been more than 9 milliseconds then update the game state
            lastTime = currentTime; //set the lastTime to now for comparsion in the next frame
            world.UpdatePhysics(deltaMS); //update the phsyics world, move things and calculate collisions
            Movecannon(cannon, deltaMS); //function call to move the cannon (see above)
            if (Keyboard::isKeyPressed(Keyboard::Space) && //fire arrow if not already flying
                !drawingArrow) {
                drawingArrow = true; //set flag so we know an arrow is flying
                arrows = arrows - 1; //subtract one from our arrows
                arrow.setCenter(cannon.getCenter()); //start the arrow at the cannon's location 
                arrow.setVelocity(Vector2f(0, -1)); //Give the arrow a velocity so it moves
                world.AddPhysicsBody(arrow); // add to the world for movement and collision
                shotSound.play();
            }
            //draw next frame
            window.clear(); //wipe the window of old stuff 
            if (drawingArrow) { //if the arrow is in flight, draw it
                window.draw(arrow);
            }
            balloons.DoRemovals(); //This removes the balloons queued for removal in the callback
            for (PhysicsSprite& balloon : balloons) { //loop to draw all balloons
                window.draw(balloon); //draw the balloon
            }
            window.draw(cannon); // draw the cannon

            //draw score  in top left corner and arrow count in top right corner
            //Note that, unlike Physics objects, SFML text doesnt have a set center.  It has a set position
            //which sets the location of its top left corner
            Text scoreText; // an SFML text object to draw the score
            scoreText.setString(to_string(score)); //set the text of the object to the current score
            scoreText.setFont(fnt); //set the font of the object to the font we loaded
            window.draw(scoreText); //draw the text, default is top left of screen

            Text arrowCountText;// an SFML text object to draw the number of arrows left
            arrowCountText.setString(to_string(arrows));//set the text of the object to the current arrow count
            arrowCountText.setFont(fnt); //set the font of the object to the font we loaded
            //we need to set the text position to the top left corner
            arrowCountText.setPosition(Vector2f(790 - GetTextSize(arrowCountText).x, 0));
            window.draw(arrowCountText); //draw the arrow count
            //world.VisualizeAllBounds(window); //a debugging function, disable for final build

            window.display(); //show everything we have drawn

        }
    }

    window.draw(Background);
   
    window.display(); // this is needed to see the last frame
    Text gameOverText; //make a Text object to draw to screen center
    gameOverText.setString("GAME OVER"); //set the text of th object
    gameOverText.setFont(fnt); //set the font of theopbject
    sz = GetTextSize(gameOverText); //get the size of the text so we can position it
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2)); //position in center of screen
    window.draw(gameOverText); //draw GAME OVER
    window.display(); //display the new frame with GAME OVER added
    while (true); //keep the game running til the window is closed
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
