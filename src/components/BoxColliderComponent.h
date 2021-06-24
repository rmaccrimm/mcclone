#ifndef _BOXCOLLIDERCOMPONENT_H_
#define _BOXCOLLIDERCOMPONENT_H_

struct BoxColliderComponent
{
    static const int TAG = 1 << 4;
    
    bool collision_detected;
    // Dimensions relative to a transform position, <center - width, center + width>
    float x_width;
    float z_width;
    float height;
};


#endif /* _BOXCOLLIDERCOMPONENT_H_ */
