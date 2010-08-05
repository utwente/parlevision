#include "utils.h"

#include <QStringBuilder>

#include "PipelineElement.h"
#include "Pin.h"

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
                        % "<td>" % QString(pin->getTypeInfo().name()) %"</td>"
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
                        % "<td>" % QString(pin->getTypeInfo().name()) %"</td>"
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
