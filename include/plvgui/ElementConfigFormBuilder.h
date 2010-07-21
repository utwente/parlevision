#ifndef ELEMENTCONFIGFORMBUILDER_H
#define ELEMENTCONFIGFORMBUILDER_H

class QWidget;

namespace plv
{
    class PipelineElement;
}

using namespace plv;

namespace plvgui
{
    /** Interface for plugins that need custom configuration forms.
      * To enable custom config forms,
      * implement this interface for your PipelineElement
      * and call plvRegisterConfigFormBuilder<YourElement,YourFormBuilder>();
      */
    class ElementConfigFormBuilder
    {
    public:
        /** Implement this method to return a custom QWidget
          * that knows how to configure element
          */
        QWidget* buildForm(PipelineElement* element, QWidget* parent=0);
    };
}

#endif // ELEMENTCONFIGFORMBUILDER_H
