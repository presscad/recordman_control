
#include "DfuMsgAttach.h"

CDFUMsgAttach::CDFUMsgAttach():
m_wLengthMask(DEFAULT_LENGTH_MASK)
{
	m_pMsg = NULL;
}

CDFUMsgAttach::CDFUMsgAttach(WORD pLengthMask)
{
	m_wLengthMask = pLengthMask;
	m_pMsg = NULL;
}

CDFUMsgAttach::~CDFUMsgAttach()
{

}

void CDFUMsgAttach::Attach(DFU_COMMU_MSG* pMsg)
{
	m_pMsg = pMsg;
}

void CDFUMsgAttach::SetMsgStartMask()
{
	if (NULL == m_pMsg)
	{
		return;
	}

	if (m_pMsg->size() <= 2)
	{
		m_pMsg->resize(2);
	}

	(*m_pMsg)[0] = RECORD_COMMU_CHAR_START_VAR;
	(*m_pMsg)[1] = RECORD_COMMU_CHAR_START_VAR;
}

bool CDFUMsgAttach::CheckStartMask()
{
	if (NULL == m_pMsg)
	{
		return false;
	}

	if (m_pMsg->size() < 2)
	{
		return false;
	}

	if ((RECORD_COMMU_CHAR_START_VAR != (*m_pMsg)[0]) || 
		(RECORD_COMMU_CHAR_START_VAR != (*m_pMsg)[1]))
	{
		return false;
	}

	return true;
}

UINT CDFUMsgAttach::GetMsgTransMask()
{
	int nMsgTransMsk = -1;

    if (NULL == m_pMsg)
    {
		return nMsgTransMsk;
    }

	if (m_pMsg->size() < 4)
	{
		return nMsgTransMsk;
	}

	nMsgTransMsk = (*m_pMsg)[2] * 256 + (*m_pMsg)[3];

	return nMsgTransMsk;
}

void CDFUMsgAttach::SetMsgTransMask(UINT uTransMark)
{
	if (NULL == m_pMsg)
	{
		return;
	}

	if (m_pMsg->size() < 4)
	{
		m_pMsg->resize(4);
	}

	int nDfuTransMark = uTransMark;
	(*m_pMsg)[2] = nDfuTransMark / 256;
	(*m_pMsg)[3] = nDfuTransMark % 256;
}

int CDFUMsgAttach::GetMsgProtocolMask()
{
	return 1;
}

void CDFUMsgAttach::SetMsgProtocolMask(int nProtocol)
{
	if (m_pMsg->size() < 6)
	{
		m_pMsg->resize(6);
	}

	switch (nProtocol)
	{
	case DEFINE_COMMU_PROTOCOL_RECORD_SELF:
		{
			(*m_pMsg)[4] = RECORD_COMMU_CHAR_PROTOCOL_START_VAR;
			(*m_pMsg)[5] = RECORD_COMMU_CHAR_PROTOCOL_END_VAR;
		}
		break;
	}
}

bool CDFUMsgAttach::CheckMsgProtocolMask()
{
	if (m_pMsg->size() < 6)
	{
		return false;
	}

	if ((RECORD_COMMU_CHAR_PROTOCOL_START_VAR != (*m_pMsg)[4] || 
		(0x01 != (*m_pMsg)[5])))
	{
		printf("recv wrong protocol code msg！\n");
	}

	return true;
}

void CDFUMsgAttach::SetMsgReserve()
{
	if (m_pMsg->size() < 8)
	{
		m_pMsg->resize(8);
	}

	(*m_pMsg)[6] = 0x00;
	(*m_pMsg)[7] = 0x00;
}

void CDFUMsgAttach::SetMsgDirection(int nDircetion)
{
	if (m_pMsg->size() < 12)
	{
		m_pMsg->resize(12);
	}

	if (nDircetion == 0)
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] & 0xBF;
	}
	else
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] | 0x40;
	}
}

int CDFUMsgAttach::GetMsgdirection()
{
	if (m_pMsg->size() < 12)
	{
		return -1;
	}

	int nDriection = 0;
	BYTE bMask = (*m_pMsg)[10] & 0x40;
	if (bMask == 0)
	{
		nDriection = 0;
	}
	else
	{
		nDriection = 1;
	}

	return nDriection;
}

void CDFUMsgAttach::SetMsgErrorFlag(int nErrorFlag)
{
	if (m_pMsg->size() < 12)
	{
		m_pMsg->resize(12);
	}

	if (0 == nErrorFlag)
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] & 0x7F;
	}
	else
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] | 0x80;
	}
}

bool CDFUMsgAttach::GetMsgErrorFlag()
{
	if (m_pMsg->size() < 12)
	{
		return false;
	}

	bool bError = false;
	BYTE bMask = (*m_pMsg)[10] & 0x80;

	if (bMask == 0)
	{
		bError = false;
	}
	else
	{
		bError = true;
	}

	return bError;
}

void CDFUMsgAttach::SetMsgMutiFrameConfirmFlag(int nFlag)
{
	if (m_pMsg->size() < 12)
	{
		m_pMsg->resize(12);
	}

	if (nFlag == 0)
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] & 0xDF;
	}
	else
	{
		(*m_pMsg)[10] = (*m_pMsg)[10] | 0x20;
	}
}

int CDFUMsgAttach::GetMsgMutiFrameConfirmFlag()
{
	if (m_pMsg->size() < 12)
	{
		return -1;
	}

	int nMutiFrameConfirmFlag = 0;
	BYTE bMask = (*m_pMsg)[10] & 0x20;

	if (bMask == 0)
	{
		nMutiFrameConfirmFlag = 0;
	}
	else
	{
		nMutiFrameConfirmFlag = 1;
	}

	return nMutiFrameConfirmFlag;
}

UINT CDFUMsgAttach::GetMsgLength()
{
	if (m_pMsg->size() < 10)
	{
		return -1;
	}

	int nMsgLeng = 0;
	nMsgLeng = (*m_pMsg)[8] * 256 + (*m_pMsg)[9];
	return nMsgLeng;
}

void CDFUMsgAttach::SetMsgLength()
{
	if (m_pMsg->size() < 10)
	{
		m_pMsg->resize(10);
	}

	int nLen = m_pMsg->size() - 10;
	int nMsgLen = nLen & m_wLengthMask;
	(*m_pMsg)[8] = nMsgLen / 256;
	(*m_pMsg)[9] = nMsgLen % 256;
}

void CDFUMsgAttach::SetMsgFuncMask(int nErrorFlag /*= 0*/, int nDirect /*= 0*/, int nConfirmFlag /*= 0*/)
{
	SetMsgErrorFlag(nErrorFlag);
	SetMsgDirection(nDirect);
	SetMsgMutiFrameConfirmFlag(nConfirmFlag);
}

//true:the last msg
//false:has follow-up msg
bool CDFUMsgAttach::GetMsgEndFlag()
{
	if (m_pMsg->size() < 18)
	{
		return false;
	}

	BYTE byteEnd = ((*m_pMsg)[17] & 0x01);

	if(byteEnd == 0x01)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CDFUMsgAttach::SetMsgEndFlag(bool bEndFlag)
{
	if (m_pMsg->size() < 18)
	{
		m_pMsg->resize(18);
	}

	if(bEndFlag)
	{
		(*m_pMsg)[14] = 0x00;
	}
	else
	{
		(*m_pMsg)[14] = 0x01;
	}
}

void CDFUMsgAttach::SetMsgSettingZone(int nZone)
{
	if (m_pMsg->size() < 20)
	{
		m_pMsg->resize(20);
	}

	int nMsgZone = nZone & m_wLengthMask;
	(*m_pMsg)[18] = nMsgZone / 256;
	(*m_pMsg)[19] = nMsgZone % 256;
}

int CDFUMsgAttach::GetMsgSettingZone()
{
	if (m_pMsg->size() < 20)
	{
		return -1;
	}

	int nZone = (*m_pMsg)[18] * 256 + (*m_pMsg)[19];
	return nZone;
}

void CDFUMsgAttach::SetMsgSettingGroup(int nGroup)
{
	if (m_pMsg->size() < 22)
	{
		m_pMsg->resize(22);
	}

	if (nGroup == 0)
	{
		(*m_pMsg)[20] = 0xFF;
		(*m_pMsg)[21] = 0xFF;
	}
	else
	{
		int nMsgGroup = nGroup & m_wLengthMask;
		(*m_pMsg)[20] = nMsgGroup / 256;
		(*m_pMsg)[21] = nMsgGroup % 256;
	}
}

void CDFUMsgAttach::SetMsgSettingGroupIndex(int nIndex)
{
	if (m_pMsg->size() < 24)
	{
		m_pMsg->resize(24);
	}

	if (nIndex == 0)
	{
		(*m_pMsg)[22] = 0xFF;
		(*m_pMsg)[23] = 0xFF;
	}
	else
	{
		int nMsgIndex = nIndex & m_wLengthMask;
		(*m_pMsg)[22] = nMsgIndex / 256;
		(*m_pMsg)[23] = nMsgIndex % 256;
	}
}

void CDFUMsgAttach::SetMsgSubModuleNo(int nModuleNo)
{
	if (m_pMsg->size() < 20)
	{
		m_pMsg->resize(20);
	}

	int nMsgModuleNo = nModuleNo & m_wLengthMask;
	(*m_pMsg)[18] = nMsgModuleNo / 256;
	(*m_pMsg)[19] = nMsgModuleNo % 256;
}

void CDFUMsgAttach::SetMsgCurSecond(UINT nCurSecond)
{
	if (m_pMsg->size() < 22)
	{
		m_pMsg->resize(22);
	}

	RECORD_SWAP_32(nCurSecond);
	memcpy(&(*m_pMsg)[18], &nCurSecond, 4);
}

void CDFUMsgAttach::SetMsgCurNanoSecond(UINT nCurNanoSecond)
{
	if (m_pMsg->size() < 26)
	{
		m_pMsg->resize(26);
	}

	RECORD_SWAP_32(nCurNanoSecond);
	memcpy(&(*m_pMsg)[22], &nCurNanoSecond, 4);
}

void CDFUMsgAttach::SetMsgCurTimeZone(int nTimeZone)
{
	if (m_pMsg->size() < 22)
	{
		m_pMsg->resize(22);
	}

	RECORD_SWAP_32(nTimeZone);
	memcpy(&(*m_pMsg)[18], &nTimeZone, 4);
}

void CDFUMsgAttach::SetFileIndex(UINT nFileIndex)
{
	if (m_pMsg->size() < 22)
	{
		m_pMsg->resize(22);
	}

	RECORD_SWAP_32(nFileIndex);
	memcpy(&(*m_pMsg)[18], &nFileIndex, 4);
}

int CDFUMsgAttach::GetErrorNum()
{
	if (m_pMsg->size() < 20)
	{
		return -1;
	}

	int nErrorNum = 0;
	nErrorNum = (*m_pMsg)[18] * 256 + (*m_pMsg)[19];
	return nErrorNum;
}

int CDFUMsgAttach::GetSettingNum()
{
	if (m_pMsg->size() < 22)
	{
		return 0;
	}

	int nSettingNum = (*m_pMsg)[20] * 256 + (*m_pMsg)[21];

	return nSettingNum;
}

int CDFUMsgAttach::GetOldZone()
{
	if (m_pMsg->size() < 20)
	{
		return 0;
	}

	int nOldZone = (*m_pMsg)[18] * 256 + (*m_pMsg)[19];

	return nOldZone;
}

int CDFUMsgAttach::GetNewZone()
{
	if (m_pMsg->size() < 22)
	{
		return 0;
	}

	int nNewZone = (*m_pMsg)[20] * 256 + (*m_pMsg)[21];

	return nNewZone;
}

int CDFUMsgAttach::GetZoneNum()
{
	if (m_pMsg->size() < 22)
	{
		return 0;
	}

	int nZoneNum = (*m_pMsg)[20] * 256 + (*m_pMsg)[21];

	return nZoneNum;
}

//************************************
// 函 数 名： GetMsgCommand
// 功能概要： get command mask
// 访问权限： public 
// 返 回 值： int 
//************************************
int CDFUMsgAttach::GetMsgCommand()
{
	if (m_pMsg->size() < 14)
	{
		return -1;
	}

	int nMsgCommand = -1;

	nMsgCommand = (*m_pMsg)[12] * 256 + (*m_pMsg)[13];

	return nMsgCommand;
}

//************************************
// 函 数 名： SetMsgCommand
// 功能概要： set command mask
// 访问权限： public 
// 返 回 值： void 
// 参    数： int nMsgCommand 
//************************************
void CDFUMsgAttach::SetMsgCommand(int nMsgCommand)
{
	if (m_pMsg->size() < 14)
	{
		m_pMsg->resize(14);
	}

	int nCommand = nMsgCommand;
	int nDfuMsgCommand = nCommand & m_wLengthMask;
	(*m_pMsg)[12] = nDfuMsgCommand / 256;
	(*m_pMsg)[13] = nDfuMsgCommand % 256;
}

//************************************
// 函 数 名： SetEndMask
// 功能概要： set msg end mask，support record internal protocol
// 访问权限： public 
// 返 回 值： void 
// 参    数： int nProtocol protocol id，default DEFINE_COMMU_PROTOCOL_RECORD_SELF
//************************************
void CDFUMsgAttach::SetEndMask(int nProtocol /*= DEFINE_COMMU_PROTOCOL_RECORD_SELF*/)
{
	int nTotallen = GetMsgLength() + 10;
	if (m_pMsg->size() < (nTotallen + 2))
	{
		m_pMsg->resize(nTotallen + 2);
	}

	switch (nProtocol)
	{
	case DEFINE_COMMU_PROTOCOL_RECORD_SELF:
		{
			(*m_pMsg)[nTotallen] = RECORD_COMMU_CHAR_END_VAR;
			(*m_pMsg)[nTotallen + 1] = RECORD_COMMU_CHAR_END_VAR;
		}
		break;
	}
}

//************************************
// 函 数 名： CheckEndMask
// 功能概要： check end mask from recved msg
// 访问权限： public 
// 返 回 值： bool true：right false：wrong
//************************************
bool CDFUMsgAttach::CheckEndMask()
{
	int nmsgsize = m_pMsg->size();
	if ((RECORD_COMMU_CHAR_END_VAR != (*m_pMsg)[nmsgsize - 1]) || 
		(RECORD_COMMU_CHAR_END_VAR != (*m_pMsg)[nmsgsize - 2]))
	{
		return false;
	}

	return true;
}
