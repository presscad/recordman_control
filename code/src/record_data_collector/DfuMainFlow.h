#if !defined(RECORD_DFU_MAIN_FLOW_INCLUDE)
#define RECORD_DFU_MAIN_FLOW_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/MongodbAccess.h"
#include "ConfigVariableMgr.h"
#include "JsonMsgParser.h"
#include "../../common/MessageLog.h"
#include "DfuMsgAttach.h"
#include "DfuMsgToJson.h"
#include "../../common/common_time.h"
#include "../../common/CreateComtrade.h"
#include "../../common/GECodeConvert.h"
#include "../../common/SafeDeque.h"

class CDfuMainFlow
{
public:
	CDfuMainFlow(void);
	~CDfuMainFlow(void);
};

#endif

