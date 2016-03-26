
#ifndef CAMERA_H
#define	CAMERA_H

#include "math_3d.h"


class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    bool OnSpecialKeyboard(int Key, int x, int y);

    bool OnKeyboard(unsigned char Key, int x, int y);

    void OnMouse(int x, int y);

    void OnRender();

    const Vector3f& GetPos() const
    {
        return m_pos;
    }

    const Vector3f& GetTarget() const
    {
        return m_target;
    }

    const Vector3f& GetUp() const
    {
        return m_up;
    }

    float m_AngleH;
    float m_AngleV;
private:

    void Init();
    void Update();

    Vector3f m_pos;							//Позиция
    Vector3f m_target;						//Направление
    Vector3f m_up;							//Верхний вектор

    int m_windowWidth;
    int m_windowHeight;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    Vector2i m_mousePos;
};

#endif	/* CAMERA_H */

