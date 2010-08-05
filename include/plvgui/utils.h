#ifndef UTILS_H
#define UTILS_H

class QString;


namespace plv
{
    class PipelineElement;
}

namespace plvgui
{
    class Utils
    {
    public:
        static QString elementInfoText(plv::PipelineElement* element);
    };
}
#endif // UTILS_H
