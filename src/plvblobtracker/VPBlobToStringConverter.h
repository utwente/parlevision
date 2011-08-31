#ifndef VPBLOBTOSTRINGCONVERTER_H
#define VPBLOBTOSTRINGCONVERTER_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/CvMatDataPin.h>
#include "Blob.h"

namespace plvblobtracker
{

class VPBlobToStringConverter : public plv::PipelineProcessor
{
    Q_OBJECT
    Q_DISABLE_COPY( VPBlobToStringConverter )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "VP Blob to String Converter")
    Q_CLASSINFO("description", "A processor which converts a blob list to a Virtual Playground compatible string.")

public:
    VPBlobToStringConverter();
    virtual ~VPBlobToStringConverter();

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PROCESSOR

private:
    plv::InputPin< QList<plvblobtracker::Blob> >* m_inputBlobs;
    plv::CvMatDataInputPin* m_inputImage;
    plv::OutputPin<QString>* m_outputPin;
};

}

#endif // VPBLOBTOSTRINGCONVERTER_H
