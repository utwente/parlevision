#ifndef PLVMSKINECTDATATYPES_H
#define PLVMSKINECTDATATYPES_H

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files:
#include <windows.h>
#include <ole2.h>

// Kinect header files
#include <MSR_NuiApi.h>
#include <QSharedData>
#include <QMetaType>

namespace plvmskinect
{
    /** internal class to SkeletonFrame. Not exported */
    class SkeletonFrameData : public QSharedData
    {
    public:
        inline SkeletonFrameData() : valid(false) {}
        inline SkeletonFrameData( const NUI_SKELETON_FRAME& nframe ) : frame(nframe), valid(true) {}

        // explicit copy of matrix header and data
        inline SkeletonFrameData( const SkeletonFrameData& other ) : QSharedData() 
        { 
            frame = other.frame; 
            valid = other.valid;
        }
        
        inline ~SkeletonFrameData() { valid = false; }

        inline bool isValid() const { return valid; }
        inline void setValid() { valid = true; }

        /** actual opencv matrix */
        NUI_SKELETON_FRAME frame;
        bool valid;
    };

    class SkeletonFrame
    {
    public:
        inline SkeletonFrame() : d( new SkeletonFrameData() ) {}
        inline ~SkeletonFrame() {}

        inline NUI_SKELETON_FRAME* getNuiSkeletonFramePointer() { return &d->frame; }
        inline const NUI_SKELETON_FRAME* getNuiSkeletonFramePointer() const { return &d->frame; }
        inline bool isValid() const { return d->isValid(); }
        inline void setValid() { d->setValid(); }
        
    private:
        /** the internal SkeletonFrameData pointer */
        QSharedDataPointer<SkeletonFrameData> d;
    };
}
Q_DECLARE_METATYPE( plvmskinect::SkeletonFrame )

#endif

