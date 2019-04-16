#include "Door.h"
 

Door::Door(int height)
{
    SetHeight(height);
}
 

void Door::SetHeight(int height)
{
    m_height = height;
}

void Door::OpenDoor()
{
    m_doorState = 1;
}

void Door::CloseDoor(){
    m_doorState = 0;
}


