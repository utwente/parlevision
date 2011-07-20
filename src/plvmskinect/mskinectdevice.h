/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvmskinect module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef PLVKINECTDEVICE_H
#define PLVKINECTDEVICE_H

#include "mskinectplugin_global.h"
#include "mskinectdatatypes.h"

#include <QThread>
#include <plvcore/CvMatData.h>
#include <QMutex>

namespace plvmskinect
{
    class KinectDevice : public QThread
    {
        Q_OBJECT
        Q_DISABLE_COPY( KinectDevice )

    public:

        /** Possible camera states */
        enum KinectState {
            KINECT_UNINITIALIZED,
            KINECT_INITIALIZED,
            KINECT_RUNNING,
            KINECT_STOP_REQUESTED
        };

        KinectDevice( QObject* parent = 0);
        virtual ~KinectDevice();

        bool init();
        bool deinit();

        void setState( KinectState state );
        KinectState getState() const;
        int getId() const;
        int width() const;
        int height() const;

        virtual void run();

    private:
        int         m_id;
        KinectState m_state;
        mutable QMutex m_stateMutex;
        mutable QMutex m_deviceMutex;

        HANDLE      m_hThNuiProcess;
        HANDLE      m_hEvNuiProcessStop;
        HANDLE      m_hNextDepthFrameEvent;
        HANDLE      m_hNextVideoFrameEvent;
        HANDLE      m_hNextSkeletonEvent;
        HANDLE      m_pDepthStreamHandle;
        HANDLE      m_pVideoStreamHandle;
//        HFONT       m_hFontFPS;

//        HDC         m_SkeletonDC;
//        HBITMAP     m_SkeletonBMP;
//        HGDIOBJ     m_SkeletonOldObj;
//        int         m_PensTotal;

//        RGBQUAD     m_rgbWk[640*480];
//        int         m_LastSkeletonFoundTime;
//        bool        m_bScreenBlanked;
//        int         m_FramesTotal;
//        int         m_LastFPStime;
//        int         m_LastFramesTotal;

        void zero();
        void Nui_GotDepthAlert();
        void Nui_GotVideoAlert();
        void Nui_GotSkeletonAlert();
        RGBQUAD Nui_ShortToQuad_Depth( USHORT s );

    signals:
        void newDepthFrame( plv::CvMatData frame );
        void newVideoFrame( plv::CvMatData frame );
        void newSkeletonFrame( plvmskinect::SkeletonFrame frame );

    public slots:
         virtual void start();
         virtual void stop();
    };
}

#endif // PLVKINECTDEVICE_H

