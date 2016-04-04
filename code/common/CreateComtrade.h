#if !defined(RECORD_CREATE_COMTRADE_INCLUDE)
#define RECORD_CREATE_COMTRADE_INCLUDE

#include "sys_define.h"

typedef struct comtradeHead
{
	string stationName; //厂站名
	string devName; //装置名
	unsigned int revYear; //comtrade规范版本 1991、1999,目前只按99格式和非99格式来处理
	unsigned int aiCount; //模拟量通道数
	unsigned int diCount; //开关量通道数
	float primary; //一次系数
	float secondary; //二次系数
	float lineFreq; //线路频率
	float rateCount; //采样率数量
	string startTime; //开始录波时间,按99格式赋值, dd/MM/yyyy,hh:mm:ss.SSSSSS
	string faultTime; //故障时间,按99格式赋值, dd/MM/yyyy,hh:mm:ss.SSSSSS
	string datType; //数据文件类型, ASCII或BINARY
	float timemult; //据文件中的时间标记（timestamp）的乘数系数,一般为1.0
}*comtradeHead_PTR;

typedef struct sampleInfo{
	float rate; //采样率
	int lastIndex; //此采用频率最后一个采样的序号
}*sampleInfo_PTR;

typedef struct aiChannel{
	unsigned int index; //序号,从1开始计数，模拟量通道和开关量通道分开计数。
	string name; //通道名。非必需
	string phase; //相位。非必需
	string ccbm; //被监视的回路元件。非必需
	string unit; //单位
	float a; //通道乘数
	float b; //通道偏移加数
	float skew; //从抽样周期开始后的通道时滞（μs）。非必需
	int min; //最小值
	int max; //最大值
	string ps; //值是一次值还是二次值。一般为二次值，填S
}*aiChannel_PTR;

typedef struct diChannel{
	unsigned int index; //序号,从1开始计数，模拟量通道和开关量通道分开计数。
	string name; //通道名。非必需
	string phase; //相位。非必需
	string ccbm; //被监视的回路元件。非必需
	int y; //当一次器件处于稳态“服务”条件时作为输入状态的状态通道的状态。一般为0
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
	 *	@param 		[In]a param of Type  string filepath	生成文件路径,使用/表示法
	 *	@param 		[In]a param of Type  string filename	生成文件名,不带后缀
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
	 *  @brief   	createCFG	 创建CFG文件
	 *	@param 		[In]a param of Type  string path cfg文件全路径，包括后缀	
	 *  @return 	bool	 
	 */
	bool createCFG(string path);

	void clacTimes();

	/*
	 *  @brief   	createAsciiDAT	创建ASCII类型的DAT文件
	 *	@param 		[In]a param of Type  string path dat文件全路径，包括后缀	
	 *  @return 	bool	 
	 */
	bool createAsciiDAT(string path);
	bool createBinaryDAT(string path);
};

};//std namespace

#endif
