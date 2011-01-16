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

#include "utils.h"

#include <QStringBuilder>

#include <plvcore/PipelineElement.h>
#include <plvcore/Pin.h>

using namespace plvgui;
using namespace plv;

QString Utils::elementInfoText(plv::PipelineElement* element)
{
    QString desc = element->getClassProperty("description");
    if(desc.isEmpty()) { desc = "(no description)"; }

    QString inputPinRows;
    const PipelineElement::InputPinMap& inPins = element->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inPins.begin()
        ; itr!=inPins.end(); ++itr)
    {
        RefPtr<IInputPin> pin = itr->second;
        inputPinRows = inputPinRows % "<tr>"
                        % "<td>" % QString(pin->getName()) %"</td>"
                        % "<td>" % QString(pin->getTypeName()) %"</td>"
                        % "</tr>";
    }


    QString outputPinRows;
    const PipelineElement::OutputPinMap& outPins = element->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outPins.begin()
        ; itr!=outPins.end(); ++itr)
    {
        RefPtr<IOutputPin> pin = itr->second;
        outputPinRows = outputPinRows % "<tr>"
                        % "<td>" % QString(pin->getName()) %"</td>"
                        % "<td>" % QString(pin->getTypeName()) %"</td>"
                        % "</tr>";
    }

    return "<h1>" % element->getName() % "</h1>"
            % "<p>"
            % desc % "</p>"
            % "<h2>Input Pins</h2>"
            % "<table><tr><th>Name</th><th>Type</th>"
            % inputPinRows
            % "</table>"
            % "<h2>Output Pins</h2>"
            % "<table><tr><th>Name</th><th>Type</th>"
            % outputPinRows
            % "</table>";
}
