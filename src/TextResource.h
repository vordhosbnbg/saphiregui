#ifndef TEXTRESOURCE_H
#define TEXTRESOURCE_H
#include "Resource.h"

class TextResource : public Resource
{
public:
    TextResource(std::string filename);
    ~TextResource();

protected:
};
#endif // TEXTRESOURCE_H
