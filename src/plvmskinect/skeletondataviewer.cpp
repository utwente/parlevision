/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#include <plvgui/ImageWidget.h>
#include <QDebug>
#include <QVBoxLayout>
#include <QPainter>

#include "skeletondataviewer.h"
#include "mskinectdatatypes.h"

using namespace plvgui;
using namespace plv;
using namespace plvmskinect;

static const QRgb JointColorTable[NUI_SKELETON_POSITION_COUNT] =
{
    qRgb(169, 176, 155), // NUI_SKELETON_POSITION_HIP_CENTER
    qRgb(169, 176, 155), // NUI_SKELETON_POSITION_SPINE
    qRgb(168, 230, 29), // NUI_SKELETON_POSITION_SHOULDER_CENTER
    qRgb(200, 0,   0), // NUI_SKELETON_POSITION_HEAD
    qRgb(79,  84,  33), // NUI_SKELETON_POSITION_SHOULDER_LEFT
    qRgb(84,  33,  42), // NUI_SKELETON_POSITION_ELBOW_LEFT
    qRgb(255, 126, 0), // NUI_SKELETON_POSITION_WRIST_LEFT
    qRgb(215,  86, 0), // NUI_SKELETON_POSITION_HAND_LEFT
    qRgb(33,  79,  84), // NUI_SKELETON_POSITION_SHOULDER_RIGHT
    qRgb(33,  33,  84), // NUI_SKELETON_POSITION_ELBOW_RIGHT
    qRgb(77,  109, 243), // NUI_SKELETON_POSITION_WRIST_RIGHT
    qRgb(37,   69, 243), // NUI_SKELETON_POSITION_HAND_RIGHT
    qRgb(77,  109, 243), // NUI_SKELETON_POSITION_HIP_LEFT
    qRgb(69,  33,  84), // NUI_SKELETON_POSITION_KNEE_LEFT
    qRgb(229, 170, 122), // NUI_SKELETON_POSITION_ANKLE_LEFT
    qRgb(255, 126, 0), // NUI_SKELETON_POSITION_FOOT_LEFT
    qRgb(181, 165, 213), // NUI_SKELETON_POSITION_HIP_RIGHT
    qRgb(71, 222,  76), // NUI_SKELETON_POSITION_KNEE_RIGHT
    qRgb(245, 228, 156), // NUI_SKELETON_POSITION_ANKLE_RIGHT
    qRgb(77,  109, 243) // NUI_SKELETON_POSITION_FOOT_RIGHT
};


SkeletonDataViewer::SkeletonDataViewer(QWidget *parent)
    : DataRenderer(parent)
{
    m_layout      = new QVBoxLayout(this);
    m_imageWidget = new ImageWidget;

    m_layout->addWidget( m_imageWidget );
    m_width  = 1280;
    m_height = 960;
    QSize size( m_width, m_height );
    m_image = QImage( size, QImage::Format_ARGB32_Premultiplied );

    QPainter painter( &m_image );
    painter.fillRect( m_image.rect(), QColor( Qt::white ));

    // initialize pens
    const int penWidth = m_width / 80;
    m_pens.resize(6);
    m_pens[0] = QPen( QColor( 255, 0,   0   ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_pens[1] = QPen( QColor( 0,   255, 0   ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_pens[2] = QPen( QColor( 64,  255, 255 ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_pens[3] = QPen( QColor( 255, 255, 64  ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_pens[4] = QPen( QColor( 255, 64,  255 ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_pens[5] = QPen( QColor( 128, 128, 255 ), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // TODO make minimum configurable somewhere
    m_imageWidget->setMinimumSize( 160, 120 );
    m_imageWidget->setImage( m_image );

    this->setLayout( m_layout );
}

SkeletonDataViewer::~SkeletonDataViewer()
{
}

void SkeletonDataViewer::newData( unsigned int serial, QVariant data )
{
    if( data.canConvert< plvmskinect::SkeletonFrame >() )
    {
        plvmskinect::SkeletonFrame sf = data.value< plvmskinect::SkeletonFrame >();
        drawSkeletons( sf );
    }
}

void SkeletonDataViewer::drawSkeletons( plvmskinect::SkeletonFrame sf )
{
    const NUI_SKELETON_FRAME* nuiSkeletonFrame = sf.getNuiSkeletonFramePointer();
    QPainter painter(&m_image);
    painter.fillRect( m_image.rect(), QColor( Qt::white ));

    // draw each skeleton color according to the slot within they are found.
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( nuiSkeletonFrame->SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
            drawSkeleton( painter, &nuiSkeletonFrame->SkeletonData[i], i );
        }
    }
    m_imageWidget->setImage( m_image );
}

void SkeletonDataViewer::drawSkeletonSegment( QPainter& painter, int numJoints, ... )
{
    va_list vl;
    va_start(vl,numJoints);
    QVector<QPoint> segmentPositions(numJoints);

    for( int i = 0; i < numJoints; ++i )
    {
        NUI_SKELETON_POSITION_INDEX jointIndex = va_arg(vl,NUI_SKELETON_POSITION_INDEX);
        segmentPositions[i].setX( m_points[jointIndex].x() );
        segmentPositions[i].setY( m_points[jointIndex].y() );
    }

    painter.drawPolyline( segmentPositions.constData(), numJoints );
    va_end(vl);
}

void SkeletonDataViewer::drawSkeleton( QPainter& painter, const NUI_SKELETON_DATA* pSkel, int whichSkeletonColor )
{
    int scaleX = m_width; //scaling up to image coordinates
    int scaleY = m_height;
    float fx = 0;
    float fy = 0;

    for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i)
    {
        NuiTransformSkeletonToDepthImageF( pSkel->SkeletonPositions[i], &fx, &fy );
        m_points[i].setX( (int) ( fx * scaleX + 0.5f ) );
        m_points[i].setY( (int) ( fy * scaleY + 0.5f ) );
    }

    int whichColor = whichSkeletonColor % m_pens.size();
    painter.setPen(m_pens.at(whichColor));

    drawSkeletonSegment( painter, 4, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
    drawSkeletonSegment( painter, 5, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
    drawSkeletonSegment( painter, 5, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
    drawSkeletonSegment( painter, 5, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
    drawSkeletonSegment( painter, 5, NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);

    // Draw the joints in a different color
    for( int i = 0; i < NUI_SKELETON_POSITION_COUNT ; ++i )
    {
        QPen jointPen( QColor(JointColorTable[i]), 9, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen( jointPen );
        painter.drawPoint( m_points[i] );
    }
    return;
}
