// TVremote.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include <conio.h>
#include <Windows.h>
#include <chrono>

using namespace std;

#define LOWER_LIMIT 0
#define UPPER_LIMIT 100

#define DEFAULT_CHANGE_VALUE 1

#define DEFAULT_BRIGHTNESS 50
#define DEFAULT_VOLUME     30
#define DEFAULT_CHANNEL    1


#define UP_BUTTON          'w'
#define DOWN_BUTTON        's'
#define OFF_BUTTON         '1'
#define MENU_BUTTON '2'
#define CONFIRM_BUTTON     '3'


class AdjustableObject
{
    private:
        uint8_t value = 0;

    public:

        uint8_t GetValue() { return value; };
        AdjustableObject(uint8_t defaultValue);

        void ChangeValue(int8_t valueToChange);

};






class Remote
{


    public:

        enum RemoteState
        {
            //IMPORTANT: Do not modify the enum values order
            REMOTE_STATE_OFF,          //0
            REMOTE_STATE_MENU,         //1
            REMOTE_STATE_BRIGHTNESS,   //2
            REMOTE_STATE_VOLUME,       //3
            REMOTE_STATE_CHANNEL,      //4

            //Do not add state after state total
            REMOTE_STATE_TOTAL         //5
        };

        enum RemoteButtonAction
        {
            REMOTE_BUTTON_ACTION_PRESS_UP,            //0
            REMOTE_BUTTON_ACTION_PRESS_DOWN,          //1
            REMOTE_BUTTON_ACTION_TURN_OFF,            //2
            REMOTE_BUTTON_ENTER_MENU,                 //3
            REMOTE_BUTTON_CONFIRM_MENU_SELECTION,     //4
            REMOTE_BUTTON_NO_ACTION                   //5
        };

        AdjustableObject brightness = AdjustableObject(DEFAULT_BRIGHTNESS);
        AdjustableObject volume = AdjustableObject(DEFAULT_VOLUME);
        AdjustableObject channel = AdjustableObject(DEFAULT_CHANNEL);



        uint8_t GetBrightness();
        uint8_t GetVolume();
        uint8_t GetChannel();

        uint8_t CheckForKeyPress();

        RemoteState GetState() { return mState; };
        RemoteState GetTransitionState() { return mTransitionState; }
        void ChangeState(RemoteState state);


        void RemoteStateMachine();

        void DisplayMessage(RemoteState state, RemoteState transitionState);


    private:

        RemoteState mState = REMOTE_STATE_OFF;
        RemoteState mTransitionState = REMOTE_STATE_BRIGHTNESS;
        uint8_t mButtonAction = REMOTE_BUTTON_NO_ACTION;

};




int main()
{

    Remote myRemote;

    //Calling this function to display the start up message
    myRemote.DisplayMessage(myRemote.GetState(), Remote::REMOTE_STATE_BRIGHTNESS);

    uint8_t bAction;

    while (1)
    {
        //check for key strokes and register the corresponding action. 
        bAction = myRemote.CheckForKeyPress();  //Note the function blocks until a key is pressed

        //
        myRemote.RemoteStateMachine();


        //Print message to the console depending on the states
        myRemote.DisplayMessage(myRemote.GetState(), myRemote.GetTransitionState());


        Sleep(50);
    }


}



/*
-------------------------------------Adjustableobject function definitions ------------------------------------------------------
*/

/*
* @brief: constructor function with a defined boundary limit for the input.
* @input: 1 byte unsgined number (0 - 255)
* 
*/
AdjustableObject::AdjustableObject(uint8_t defaultValue)
{
    if (defaultValue > UPPER_LIMIT || defaultValue < LOWER_LIMIT)
    {
        printf("Default value invalid");
    }
    else
    {
        value = defaultValue;
    }
}


/*
* @brief: The function below increments/decrements the AdjustableObject's 'value'.
*         The value to change is bounded within 0 and 100
          
  @input: 1 byte signed integer. Please note that the input 'valueToChange' is 1 byte signed integer, so if you put a number outside of the -128 and 128, the behaviour becomes funky
*
*/
void AdjustableObject::ChangeValue(int8_t valueToChange)
{
    int8_t newValue = value + valueToChange;

    if (newValue < LOWER_LIMIT)
    {
        newValue = LOWER_LIMIT;
    }
    else if (newValue > UPPER_LIMIT)
    {
        newValue = UPPER_LIMIT;
    }

    value = newValue;
}


/*
--------------------------------------Remote object function defintions -----------------------------------------------------------
*/
uint8_t Remote::GetBrightness()
{
    return brightness.GetValue();
}

uint8_t Remote::GetVolume()
{
    return volume.GetValue();
}

uint8_t Remote::GetChannel()
{
    return channel.GetValue();
}


void Remote::ChangeState(RemoteState state)
{
    mState = state;
}



/*
* @brief: The function below is the state machine of the remote which controls the logic, state transition, operations inside states and etc.
*         Please refer to the state machine diagram drawn to make it easier to understand the state machine.
*/
void Remote::RemoteStateMachine()
{

    switch (mState) //The member variable 'mState' is strictly controlled and updated by the state machine
    {

    case REMOTE_STATE_BRIGHTNESS:
    {
        switch (mButtonAction) //The member variable 'mButtonAction' is updated by the 'CheckForKeyPress()' function
        {
        case REMOTE_BUTTON_ACTION_PRESS_UP: //Increase brightness

            brightness.ChangeValue(DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_PRESS_DOWN: //Decrease brightness

            brightness.ChangeValue(-DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_TURN_OFF:

            ChangeState(REMOTE_STATE_OFF);

            break;

        case REMOTE_BUTTON_ENTER_MENU:
            ChangeState(REMOTE_STATE_MENU); //Enter menu state
            break;

        default:
            break;
        }

        break;
    }


    case REMOTE_STATE_CHANNEL:
    {
        switch (mButtonAction)
        {

        case REMOTE_BUTTON_ACTION_PRESS_UP:

            channel.ChangeValue(DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_PRESS_DOWN:

            channel.ChangeValue(-DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_TURN_OFF:

            ChangeState(REMOTE_STATE_OFF);

            break;

        case REMOTE_BUTTON_ENTER_MENU:
            ChangeState(REMOTE_STATE_MENU);
            break;

        default:
            break;

        }

        break;
    }


    case REMOTE_STATE_VOLUME:
    {
        switch (mButtonAction)
        {

        case REMOTE_BUTTON_ACTION_PRESS_UP:

            volume.ChangeValue(DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_PRESS_DOWN:

            volume.ChangeValue(-DEFAULT_CHANGE_VALUE);

            break;

        case REMOTE_BUTTON_ACTION_TURN_OFF:

            ChangeState(REMOTE_STATE_OFF);

            break;

        case REMOTE_BUTTON_ENTER_MENU:
            ChangeState(REMOTE_STATE_MENU);
            break;

        default:

            break;
        }


        break;
    }

    case REMOTE_STATE_MENU:
    {
        //Changing from adjusting one parameter to another via a transitional state

        static int8_t bTransitionState = REMOTE_STATE_BRIGHTNESS;

        /*
        * The 'mTransitionState' member variable is used to help with displaying which parameter(brightness, channel or volume) the user wants to change, and by default it starts at brightness.
        * 
        * The 'mState' member varialbe is the current state of the state machine (REMOTE_STATE_MENU, REMOTE_STATE_OFF.....). 
        * 
        */
        mTransitionState = REMOTE_STATE_BRIGHTNESS;

        switch (mButtonAction)
        {

        /*
        * While in the menu state, pressing up or down button will cycle through all the availalbe states (brightness, volume, channel).
        */
        case REMOTE_BUTTON_ACTION_PRESS_UP:

            //Only increment the state if the value after addition falls within the enum range
            if (bTransitionState + 1 < REMOTE_STATE_TOTAL)
            {
                bTransitionState += 1;
            }

            break;

        case REMOTE_BUTTON_ACTION_PRESS_DOWN:
            //Only decrement the state if the reduced value falls within the enum range
            if ((bTransitionState - 1) > REMOTE_STATE_MENU)
            {
                bTransitionState -= 1;
            }

            break;

        /*
        * The user has confirmed they want to adjust the selected parameter (brightness, volume or channel)
        */
        case REMOTE_BUTTON_CONFIRM_MENU_SELECTION:
            ChangeState( static_cast<RemoteState>(bTransitionState));  //This will update 'mState' to bTransitionState
            bTransitionState = REMOTE_STATE_BRIGHTNESS; //Reset this so the next time a state change occurs at brightness again
            break;

        default:
            break;

        }

        mTransitionState = static_cast<RemoteState>(bTransitionState); //Update the member variable so the user selection is being tracked

        break;
    }

    case REMOTE_STATE_OFF:
    {
        switch (mButtonAction)
        {
        case REMOTE_BUTTON_ACTION_PRESS_DOWN:
        case REMOTE_BUTTON_ACTION_PRESS_UP:

            ChangeState(REMOTE_STATE_VOLUME);

            break;
        }

        break;
    }

    default:
        printf("oops, state is not found :c ");
        break;

    }


}


/*
* @brief: The function below monitors the keyboard input and outputs a corresponding action such as UP_BUTTON_PRESSED, DOWN_BUTTON_PRESSED ETC.
* 
* @output: unsigned 1 byte (e.g. RemoteButtonAction enum)
           The output action is stored in a member variable called mButtonAction.
*/
uint8_t Remote::CheckForKeyPress()
{

    uint16_t wKeyPressed = 0;
    uint8_t bOutputAction = 0;

    wKeyPressed = _getch(); //Get the pressed key from keyboard input. Note _getch() is blocking.

    switch (wKeyPressed)
    {

    case UP_BUTTON:

        bOutputAction = REMOTE_BUTTON_ACTION_PRESS_UP;

        break;


    case DOWN_BUTTON:

        bOutputAction = REMOTE_BUTTON_ACTION_PRESS_DOWN;

        break;

    case OFF_BUTTON:

        bOutputAction = REMOTE_BUTTON_ACTION_TURN_OFF;

        break;

    case MENU_BUTTON:

        bOutputAction = REMOTE_BUTTON_ENTER_MENU;

        break;

    case CONFIRM_BUTTON:

        bOutputAction = REMOTE_BUTTON_CONFIRM_MENU_SELECTION;

        break;

    default:
        bOutputAction = REMOTE_BUTTON_NO_ACTION;
        printf("Invalid button");
        break;

    }

    mButtonAction = bOutputAction;

    return bOutputAction;
}


/*
* @brief: This function controls the message displayed on the console depending on the states of the remote.
* @input: state - the current state of the remote (OFF, BRIGHTNESS, VOLUME, CHANNEL, MODE SELECT)
*         transitionState - the temporary state (used for going from adjusting brightness to adjusting other parameters) of the remote (BRIGHTNESS, VOLUME, CHANNEL).
*/
void Remote::DisplayMessage(RemoteState state, RemoteState transitionState)
{

    switch (state)
    {
    case REMOTE_STATE_BRIGHTNESS:

        printf("Brightness %d\n", GetBrightness());

        break;

    case REMOTE_STATE_CHANNEL:

        printf("Channel %d\n", GetChannel());

        break;

    case REMOTE_STATE_VOLUME:

        printf("Volume %d\n", GetVolume());

        break;

    case REMOTE_STATE_MENU: // The "menu" state requires more logic because it needs to show the user's current selection (brightness, channel or volume).

        /*
        * The code below is implemented like the display component of a select menu to inform the user which parameter they are about to adjust.
        * The code has the same function as the more graphical interfacce like the example shown below, but it only displays the selected menu and
        * not the other items.
        *           
        *           1.) Typical graphical interface                         2.) Implemented interface
        *           >>  a.) Brightness                                      - Adjust Brightness? 
        *               b.) Volume                              >>>>
        *               c.) Channel
        */
        switch (transitionState)
        {
        case REMOTE_STATE_BRIGHTNESS:
            printf("Adjust Brightness?\n");
            break;

        case REMOTE_STATE_VOLUME:
            printf("Adjust Volume?\n");
            break;

        case REMOTE_STATE_CHANNEL:
            printf("Adjust Channel?\n");
            break;

        default:
            break;
        }

        break;


    case REMOTE_STATE_OFF:
        printf("Welcome! Press any key to turn on\n");
        break;

    }

}

