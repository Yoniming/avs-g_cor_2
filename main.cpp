#include "CommandParse.h"
#include "Context.h"

int main(int argc, char **argv)
{   
    Param *paramPtr = Param::GetInstance();
    CommandParse cmdParse;
    cmdParse.parseOptFromCmd(argc, argv);
    Context *contextPtr = new Context;

    switch (paramPtr->m_actionType)
    {
    case ACTIONTYPE_DOINDEX:
        contextPtr->doBuildIndex();
        break;
    case ACTIONTYPE_DOENCODE:
        contextPtr->initEncodeContext();
        break;
    case ACTIONTYPE_DODECODE:
        contextPtr->initDecodeContext();
        break;
    case ACTIONTYPE_EXTRACT:
        contextPtr->initExtractContext();
    default:
        break;
    }

    RELEASEPTR(contextPtr);
    return 0;
}