/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
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

#ifndef IMAGEDIRECTORYPRODUCER_H
#define IMAGEDIRECTORYPRODUCER_H

#include <QMutex>
#include <plvcore/PipelineProducer.h>
#include <plvcore/OutputPin.h>
#include <plvcore/CvMatData.h>
#include <plvcore/Enum.h>
#include <QFileInfo>

namespace plv
{
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageDirectoryProducer : public plv::PipelineProducer
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Richard Loos")
        Q_CLASSINFO("name", "Image directory producer")
        Q_CLASSINFO("description", "A producer that loads all the images in a directory.")

        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_PRODUCER

    public:
        ImageDirectoryProducer();
        virtual ~ImageDirectoryProducer();

        virtual bool init();
        virtual bool deinit() throw ();

        QString getDirectory();
        void updateDirectory(const QString& s){ setDirectory(s); directoryChanged(s); }

    signals:
        void directoryChanged(const QString& newValue);

    public slots:
        void setDirectory(const QString& directory);

    protected:
        plv::CvMatData m_loadedImage;
        plv::CvMatDataOutputPin* m_imgOutputPin;
        plv::OutputPin<QString>* m_fileNameOutputPin;
        plv::OutputPin<QString>* m_filePathOutputPin;

    private:
        QString m_directory;
        QFileInfoList m_entryInfoList;
        int m_idx;
    };
}

#endif // IMAGEDIRECTORYPRODUCER_H
