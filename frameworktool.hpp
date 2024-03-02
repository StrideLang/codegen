#ifndef FRAMEWORKTOOL_HPP
#define FRAMEWORKTOOL_HPP

#include <string>
#include "declarationnode.h"

// Perhaps use a different word to tool, as we are already using it 
class FrameworkTool
{
public:
    FrameworkTool(std::string strideRoot,
                  std::string platformPath);
    
private:
    std::vector<DeclarationNode> tools;
};

#endif // FRAMEWORKTOOL_HPP
