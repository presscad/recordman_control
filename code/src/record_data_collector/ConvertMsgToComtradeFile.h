#if !defined(RECORD_DFU_CONVERT_MSG_TO_COMTRADE_FILE_INCLUDE)
#define RECORD_DFU_CONVERT_MSG_TO_COMTRADE_FILE_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/MongodbAccess.h"
#include "const_define.h"
#include "DfuMsgAttach.h"
#include "../../common/common_time.h"
#include "../../common/CreateComtrade.h"
#include "../../common/GECodeConvert.h"

class CConvertMsgToComtradeFile
{
public:
	CConvertMsgToComtradeFile(CMongodbAccess* pMongodbAccessObj);
	~CConvertMsgToComtradeFile(void);

public:
	void AttachDfuMsg(DFUMESSAGE* pMsg);

	bool ConvertMsgToFile(string strFilepath);

private:
	bool GetOscInfo(comtradeHead& head);

	bool AnalyzeFileMsgHeader(comtradeHead& head, DFU_COMMU_MSG* pMsg, int& nOffset, UINT& uDatablockNum);

	bool AnalyzeFileMsgSamples(list<sampleInfo>& samples, float uSampleNum, DFU_COMMU_MSG* pMsg, int& nOffset);

	bool AnalyzeFileMsgAis(list<short>& data_vals, UINT uAiNum, DFU_COMMU_MSG* pMsg, int& nOffset);

	bool AnalyzeFileMsgDis(list<short>& data_vals, UINT uDiNum, DFU_COMMU_MSG* pMsg, int& nOffset);

private:
	DFUMESSAGE* m_pFileMsg;

	CMongodbAccess* m_pMongoAccessHandler;
};

#endif
