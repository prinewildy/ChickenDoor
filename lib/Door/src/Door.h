#ifndef DOOR_H
#define DOOR_H
 
class Door
{
private:
    int m_height;
    bool m_doorState;
    
    
public:
    Door(int height);
 
    void SetHeight(int height);
    void CloseDoor();
    void OpenDoor();
    void Home();
    int getPosition();
      
    int getHeight() { return m_height; }    
    bool getDoorState(){ return m_doorState; }
};
 
#endif