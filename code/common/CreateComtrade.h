#if !defined(RECORD_CREATE_COMTRADE_INCLUDE)
#define RECORD_CREATE_COMTRADE_INCLUDE

#include "sys_define.h"

typedef struct comtradeHead
{
	string stationName; //��վ��
	string devName; //װ����
	unsigned int revYear; //comtrade�淶�汾 1991��1999,Ŀǰֻ��99��ʽ�ͷ�99��ʽ������
	unsigned int aiCount; //ģ����ͨ����
	unsigned int diCount; //������ͨ����
	float primary; //һ��ϵ��
	float secondary; //����ϵ��
	float lineFreq; //��·Ƶ��
	float rateCount; //����������
	string startTime; //��ʼ¼��ʱ��,��99��ʽ��ֵ, dd/MM/yyyy,hh:mm:ss.SSSSSS
	string faultTime; //����ʱ��,��99��ʽ��ֵ, dd/MM/yyyy,hh:mm:ss.SSSSSS
	string datType; //�����ļ�����, ASCII��BINARY
	float timemult; //���ļ��е�ʱ���ǣ�timestamp���ĳ���ϵ��,һ��Ϊ1.0
}*comtradeHead_PTR;

typedef struct sampleInfo{
	float rate; //������
	int lastIndex; //�˲���Ƶ�����һ�����������
}*sampleInfo_PTR;

typedef struct aiChannel{
	unsigned int index; //���,��1��ʼ������ģ����ͨ���Ϳ�����ͨ���ֿ�������
	string name; //ͨ�������Ǳ���
	string phase; //��λ���Ǳ���
	string ccbm; //�����ӵĻ�·Ԫ�����Ǳ���
	string unit; //��λ
	float a; //ͨ������
	float b; //ͨ��ƫ�Ƽ���
	float skew; //�ӳ������ڿ�ʼ���ͨ��ʱ�ͣ���s�����Ǳ���
	int min; //��Сֵ
	int max; //���ֵ
	string ps; //ֵ��һ��ֵ���Ƕ���ֵ��һ��Ϊ����ֵ����S
}*aiChannel_PTR;

typedef struct diChannel{
	unsigned int index; //���,��1��ʼ������ģ����ͨ���Ϳ�����ͨ���ֿ�������
	string name; //ͨ�������Ǳ���
	string phase; //��λ���Ǳ���
	string ccbm; //�����ӵĻ�·Ԫ�����Ǳ���
	int y; //��һ������������̬����������ʱ��Ϊ����״̬��״̬ͨ����״̬��һ��Ϊ0
}*diChannel_PTR;

namespace std {

class CreateComtrade
{
public:
	CreateComtrade(void);
	~CreateComtrade(void);
private:
	comtradeHead m_comtradeHead;
	list<aiChannel> m_aiChannels;
	list<diChannel> m_diChannels;
	list<sampleInfo> m_sampleInfos;
	list< list<short> > m_datas;
	vector<unsigned int> m_times;
public:
	/*
	 *  @brief   	create	 
	 *	@param 		[In]a param of Type  string filepath	�����ļ�·��,ʹ��/��ʾ��
	 *	@param 		[In]a param of Type  string filename	�����ļ���,������׺
	 *  @return 	bool	 
	 */
	bool create(string filepath, string filename);

	void setComtradeHead(comtradeHead head){m_comtradeHead = head;};
	void setAiChannels(list<aiChannel> chs){m_aiChannels = chs;};
	void setDiChannels(list<diChannel> chs){m_diChannels = chs;};
	void setSampleInfos(list<sampleInfo> infos){m_sampleInfos = infos;};
	void setDatas(list< list<short> > datas){m_datas=datas;};

private:
	/*
	 *  @brief   	createCFG	 ����CFG�ļ�
	 *	@param 		[In]a param of Type  string path cfg�ļ�ȫ·����������׺	
	 *  @return 	bool	 
	 */
	bool createCFG(string path);

	void clacTimes();

	/*
	 *  @brief   	createAsciiDAT	����ASCII���͵�DAT�ļ�
	 *	@param 		[In]a param of Type  string path dat�ļ�ȫ·����������׺	
	 *  @return 	bool	 
	 */
	bool createAsciiDAT(string path);
	bool createBinaryDAT(string path);
};

};//std namespace

#endif
