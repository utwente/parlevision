/**
  * Copyright (C)2011 by Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvtest plugin of ParleVision.
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

#ifndef PLVTESTPRODUCER_H
#define PLVTESTPRODUCER_H

#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>

class TestProducer : public plv::PipelineProducer
{
    Q_OBJECT
    Q_DISABLE_COPY( TestProducer )

    Q_CLASSINFO("author", "Richard Loos")
    Q_CLASSINFO("name", "TestProducer")
    Q_CLASSINFO("description", "A producer to test stuff with")

    /** required standard method declaration for plv::PipelineProcessor */
    PLV_PIPELINE_PRODUCER

public:
    TestProducer();
    virtual ~TestProducer();

    virtual bool init();
    virtual bool deinit() throw();
    virtual bool start();
    virtual bool stop();

signals:

public slots:

private:
    plv::OutputPin<int>* m_intOut;
    plv::OutputPin<QString>* m_stringOut;
    plv::OutputPin<float>* m_floatOut;
    plv::OutputPin<double>* m_doubleOut;
};

#endif // PLVTESTPRODUCER_H
