#include "ConvertMsgToComtradeFile.h"


CConvertMsgToComtradeFile::CConvertMsgToComtradeFile(CMongodbAccess* pMongodbAccessObj)
{
	m_pMongoAccessHandler = pMongodbAccessObj;

	m_pFileMsg = NULL;
}


CConvertMsgToComtradeFile::~CConvertMsgToComtradeFile(void)
{
}

void CConvertMsgToComtradeFile::AttachDfuMsg(DFUMESSAGE* pMsg)
{
	m_pFileMsg = pMsg;
}

bool CConvertMsgToComtradeFile::ConvertMsgToFile(string strFilepath)
{
	if (m_pFileMsg->result_msg.size() <= 0)
	{
		return false;
	}

	DFU_COMMU_MSG* pOneMsg = NULL;
	CDFUMsgAttach msgAttach;
	UINT uDatablockNum(0);//���ݿ���Ŀ
	int nMsgoffset = RECORD_DFU_MSG_HEADER_OFFSET + 12;
	comtradeHead head;
	CreateComtrade com;
	list<aiChannel> aichs;//ģ����ͨ��
	list<diChannel> dichs;//������ͨ��
	list<sampleInfo> samples;//������
	list<list<short> > datas;//����
	aichs.clear();
	dichs.clear();
	samples.clear();
	datas.clear();

	try
	{
		head.revYear = 1991;
		head.datType="ascii";
		head.timemult=1.0;

		GetOscInfo(head);//get osc info

		vector<DFU_COMMU_MSG>::iterator itFile = m_pFileMsg->result_msg.begin();
		for (; itFile != m_pFileMsg->result_msg.end(); itFile++)
		{
			pOneMsg = &*itFile;
			msgAttach.Attach(pOneMsg);

			AnalyzeFileMsgHeader(head, pOneMsg, nMsgoffset, uDatablockNum);//����ͷ
			AnalyzeFileMsgSamples(samples, head.rateCount, pOneMsg, nMsgoffset);//������

			for (UINT uNo = 0; uNo < uDatablockNum; uNo++)//���ݿ�
			{
				list<short> data_val;
				data_val.clear();

				AnalyzeFileMsgAis(data_val, head.aiCount, pOneMsg, nMsgoffset);//ģ����
				AnalyzeFileMsgDis(data_val, head.diCount, pOneMsg, nMsgoffset);//������

				datas.push_back(data_val);
			}
		}
	}
	catch (...)
	{
		return false;
	}

	com.setComtradeHead(head);//��������ͷ
	com.setSampleInfos(samples);//������
	com.setAiChannels(aichs);//ģ����ͨ��
	com.setDiChannels(dichs);//������ͨ��
	com.setDatas(datas);//ͨ����ֵ

	com.create(strFilepath, "test");

	return true;
}

bool CConvertMsgToComtradeFile::GetOscInfo(comtradeHead& head)
{
	long lLen = 0;
	auto_ptr<mongo::DBClientCursor> result;
	m_pMongoAccessHandler->QueryData("runstatus_info", result);
	string strFieldval = "";
	CGECodeConvert code_convert;
	char* pVal = NULL;

	while (result->more())
	{
		mongo::BSONObj obj = result->next();
		BSONForEach(e, obj)
		{
			if (strcmp(e.fieldName(), "devname") == 0)
			{
				strFieldval = e.String();
				lLen = code_convert.Utf82Gbk(strFieldval.c_str(), strFieldval.size(), pVal);
				if (lLen != -1)
				{
					head.devName = pVal;
				}
			}
			else if (strcmp(e.fieldName(), "station") == 0)
			{
				strFieldval = e.String();
				lLen = code_convert.Utf82Gbk(strFieldval.c_str(), strFieldval.size(), pVal);
				if (lLen != -1)
				{
					head.stationName = pVal;
				}
			}

			if (NULL != pVal)
			{
				delete[] pVal;
				pVal = NULL;
			}
		}
	}

	return true;
}

bool CConvertMsgToComtradeFile::AnalyzeFileMsgHeader(comtradeHead& head, DFU_COMMU_MSG* pMsg, int& nOffset, UINT& uDatablockNum)
{
	uint16 uAnalogchannelNum(0);//ģ����ͨ����
	uint16 uDigitalchannelNum(0);//������ͨ����
	UINT uFilesecond(0);//¼��ʱ�̣���
	UINT uFilenanosecond(0);//¼��ʱ�̣�����
	UINT uFaultsecond(0);//����ʱ�̣���
	UINT uFaultnanosecond(0);//����ʱ�̣�����
	UINT uSamplesectionNum(0);//��������Ŀ

	try
	{
		convert_btol_uint16(&(*pMsg)[nOffset], uAnalogchannelNum);//ģ��ͨ����Ŀ
		head.aiCount += uAnalogchannelNum;
		nOffset += 2;

		convert_btol_uint16(&(*pMsg)[nOffset], uAnalogchannelNum);//����ͨ����Ŀ
		head.diCount += uAnalogchannelNum;
		nOffset += 2;

		convert_btol_float32(&(*pMsg)[nOffset], head.primary);//ģ��ͨ��һ��ϵ��
		nOffset += 4;

		convert_btol_float32(&(*pMsg)[nOffset], head.secondary);//ģ��ͨ������ϵ��
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uFilesecond);//¼��ʱ�̣��룩
		nOffset += 4;
		head.startTime = format_dfu_msg_time(uFilesecond);

		convert_btol_uint32(&(*pMsg)[nOffset], uFilenanosecond);//¼��ʱ�̣����룩
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uFaultsecond);//����ʱ�䣨�룩
		nOffset += 4;
		head.faultTime = format_dfu_msg_time(uFaultsecond);

		convert_btol_uint32(&(*pMsg)[nOffset], uFilenanosecond);//����ʱ�̣����룩
		nOffset += 4;

		convert_btol_float32(&(*pMsg)[nOffset], head.lineFreq);//��·Ƶ��
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uSamplesectionNum);//��������Ŀ
		head.rateCount += uSamplesectionNum;
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uDatablockNum);//���ݿ���Ŀ
		nOffset += 4;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConvertMsgToComtradeFile::AnalyzeFileMsgSamples(list<sampleInfo>& samples, float uSampleNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	try
	{
		for (UINT uNo = 0; uNo < uSampleNum; uNo++)
		{
			sampleInfo sa;

			convert_btol_float32(&(*pMsg)[nOffset], sa.rate);//������
			nOffset += 4;

			convert_btol_int32(&(*pMsg)[nOffset], sa.lastIndex);//�˲���Ƶ�����һ�����������
			nOffset += 4;

			samples.push_back(sa);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConvertMsgToComtradeFile::AnalyzeFileMsgAis(list<short>& data_vals, UINT uAiNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	UINT uDival(0);

	try
	{
		for (UINT uDataNo = 0; uDataNo < uAiNum; uDataNo++)//ģ����
		{
			//ConvertUint32BigedianToL(&(*pMsg)[nOffset], ai.index);
			nOffset += 4;//ģ��ͨ�����

			convert_btol_uint32(&(*pMsg)[nOffset], uDival);//ģ��ͨ�����
			nOffset += 4;

			data_vals.push_back(uDival);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool CConvertMsgToComtradeFile::AnalyzeFileMsgDis(list<short>& data_vals, UINT uDiNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	UINT uDiBlockNo = 0;
	UINT uDival(0);
	UINT uOneDival(0);
	UINT uDiRealNum(0);
	UINT uDataNo(0);

	try
	{
		for (uDiBlockNo = 0; uDiBlockNo <= (uDiNum / 32); uDiBlockNo++)
		{
			convert_btol_uint32(&(*pMsg)[nOffset], uDival);
			nOffset += 4;//������

			uDiRealNum = (uDiBlockNo == (uDiNum / 32))?(uDiNum % 32):32;

			for (uDataNo = 0; uDataNo < uDiRealNum; uDataNo++)
			{
				uOneDival = ((uDival >> uDataNo) & 0x1);

				data_vals.push_back(uOneDival);
			}
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}
