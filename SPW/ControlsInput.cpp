#include "ControlsInput.h"

ControlsInput::ControlsInput() :
    InputGroup(), hAxis(0.0f),
    jumpDown(false), jumpPressed(false), goDownDown(false)
{
}

void ControlsInput::OnPreEventProcess()
{
}

void ControlsInput::OnEventProcess(SDL_Event evt)
{
    SDL_Scancode scanCode;
    switch (evt.type)
    {
    case SDL_JOYHATMOTION:
        switch (evt.jhat.value)
        {
        case SDL_HAT_CENTERED:
        case SDL_HAT_UP:
            hAxis = 0.0f;
            goDownDown = false;
            break;
        case SDL_HAT_RIGHT:
        case SDL_HAT_RIGHTUP:
        case SDL_HAT_RIGHTDOWN:
            hAxis = +1.0f;
            goDownDown = false;
            break;
        case SDL_HAT_LEFT:
        case SDL_HAT_LEFTUP:
        case SDL_HAT_LEFTDOWN:
            hAxis = -1.0f;
            goDownDown = false;
            break;
        case SDL_HAT_DOWN:
            goDownDown = true;
            break;
        default:
            break;
        }
        break;

    case SDL_JOYAXISMOTION:
        if (evt.jaxis.axis == 0)
        {
            // Joystick gauche : Axe X
            float value = (float)(evt.jaxis.value);
            if (fabsf(value) < 8000.0f)
                hAxis = 0.0f;
            else
                hAxis = value / 32768.0f;

        }
        break;

    case SDL_JOYBUTTONDOWN:
        switch (evt.jbutton.button)
        {
        case 0: case 2:
            // Saut
            jumpDown = true;
            jumpPressed = true;
            break;
        default:
            break;
        }
        break;

    case SDL_JOYBUTTONUP:
        switch (evt.jbutton.button)
        {
        case 0: case 2:
            // Saut
            jumpDown = false;
            break;
        default:
            break;
        }
        break;

    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_RIGHT:
            // Deplacement à droite
            hAxis = 1.f;
            goRightDown = true;
            break;

        case SDL_SCANCODE_LEFT:
            // Deplacement à gauche
            hAxis = -1.f;
            goLeftDown = true;
            break;

        case SDL_SCANCODE_DOWN:
            crouchDown = true;
            // Descente d'une plateforme
            goDownDown = true;
            break;

        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_UP:
            // Saut
            jumpDown = true;
            jumpPressed = true;
            goUpDown = true;
            break;
        
        case SDL_SCANCODE_LCTRL:
            areaDown = true;
            break;

        case SDL_SCANCODE_LALT:
            fillDown = true;
            break;
        
        case SDL_SCANCODE_LSHIFT:
            crouchDown = true;
            break;

        default:
            break;
        }
        break;

    case SDL_KEYUP:
        scanCode = evt.key.keysym.scancode;

        switch (scanCode)
        {
        case SDL_SCANCODE_RIGHT:
            // Deplacement à droite
            if (hAxis > 0.f)
                hAxis = 0.f;
            goRightDown = false;
            break;

        case SDL_SCANCODE_LEFT:
            // Deplacement à gauche
            if (hAxis < 0.f)
                hAxis = 0.f;
            goLeftDown = false;
            break;

        case SDL_SCANCODE_DOWN:
            crouchDown = false;
            crouchReleased = true;
            // Descente d'une plateforme
            goDownDown = false;
            break;

        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_UP:
            // Saut
            jumpDown = false;
            jumpPressed = false;
            goUpDown = false;
            break;
        case SDL_SCANCODE_F1:
            savePressed = true;
            break;
        case SDL_SCANCODE_LCTRL:
            areaDown = false;
            areaReleased = true;
            break;
        case SDL_SCANCODE_LALT:
            fillDown = false;
            break;

        case SDL_SCANCODE_LSHIFT:
            crouchDown = false;
            crouchReleased = true;
            break;
        case SDL_SCANCODE_C:
            copyPressed = true;
            break;
        case SDL_SCANCODE_V:
            pastePressed = true;
            break;
        case SDL_SCANCODE_R:
            rotatePressed = true;
            break;
        case SDL_SCANCODE_F:
            flipVPressed = true;
            break;
        case SDL_SCANCODE_G:
            flipHPressed = true;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void ControlsInput::Reset()
{
    hAxis = 0.0f;
    jumpDown = false;
    jumpPressed = false;
    goDownDown = false;
    goLeftDown = false;
    goRightDown = false;
    goUpDown = false;
    savePressed = false;
    areaDown = false;
    areaReleased = false;
    crouchDown = false;
    crouchReleased = false;
    fillDown = false;
    copyPressed = false;
    pastePressed = false;
}
