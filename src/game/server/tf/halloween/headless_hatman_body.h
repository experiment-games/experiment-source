//========= Copyright Valve Corporation, All rights reserved. ============//
#ifndef HEADLESS_HATMAN_BODY_H
#define HEADLESS_HATMAN_BODY_H

#include "animation.h"
#include "NextBotBodyInterface.h"

class INextBot;

//----------------------------------------------------------------------------------------------------------------
/**
 * The interface for control and information about the bot's body state (posture, animation state, etc)
 */
class CHeadlessHatmanBody : public IBody
{
    public:
    CHeadlessHatmanBody( INextBot *bot );
    virtual ~CHeadlessHatmanBody() {}

    virtual void Update( void );

    virtual bool StartActivity( Activity act, unsigned int flags = 0 );
    virtual Activity GetActivity( void ) const;     // return currently animating activity
    virtual bool IsActivity( Activity act ) const;  // return true if currently animating activity matches the given one

    virtual unsigned int GetSolidMask( void ) const;  // return the bot's collision mask (hack until we get a general hull trace abstraction here or in the locomotion interface)

    private:
    int m_currentActivity;
    int m_moveXPoseParameter;
    int m_moveYPoseParameter;
};

inline Activity CHeadlessHatmanBody::GetActivity( void ) const
{
    return ( Activity )m_currentActivity;
}

inline bool CHeadlessHatmanBody::IsActivity( Activity act ) const
{
    return act == m_currentActivity ? true : false;
}

#endif  // HEADLESS_HATMAN_BODY_H
