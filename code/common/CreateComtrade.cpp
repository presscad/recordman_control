#include "CreateComtrade.h"

namespace std{

CreateComtrade::CreateComtrade(void)
{
}

CreateComtrade::~CreateComtrade(void)
{
}

bool CreateComtrade::create(string filepath, string filename)
{
	if( filepath.empty() || filename.empty())
		return false;
	
	if( filepath[filepath.size() - 1] != '/' )
	{
		filepath.push_back('/');
	}
	clacTimes();
	string cfgname = filepath;
	cfgname += filename;
	cfgname += ".cfg";
	createCFG( cfgname );

	string datname = filepath;
	datname += filename;
	datname += ".dat";
	if( m_comtradeHead.datType.compare("ASCII") == 0 ||
		m_comtradeHead.datType.compare("ascii")==0){
		createAsciiDAT( datname );
	}else{
		createBinaryDAT( datname );
	}
}


void CreateComtrade::clacTimes()
{
	m_times.clear();
	int lastIndex = 0;
	m_times.push_back(0);
	for( list<sampleInfo>::iterator its = m_sampleInfos.begin(); its != m_sampleInfos.end(); ++ its)
	{
		sampleInfo sa = *its;
		float ft = 1000.0/sa.rate;
		unsigned int dt= ft*1000;
		for(int i = lastIndex+1; i <= sa.lastIndex; i++)
		{
			m_times.push_back( m_times.back()+dt);
			lastIndex=i;
		}
		
	}
}

string get91time(string time99)
{
	if(time99.empty())
		return "";

	int index = time99.find('/',0);
	string day;
	string month;
	string left;

	if( index != string::npos ){
		day = time99.substr(0, index);
		int nextindex = time99.find('/',index+1);
		if( nextindex != string::npos){
			month = time99.substr(index+1, nextindex-index-1);
			left = time99.substr(nextindex, time99.length() - nextindex);
		}
	}

	std::stringstream ss;
	ss<<month<<"/"<<day<<left;
	string str = ss.str();

	return str;
}

bool CreateComtrade::createCFG(string path)
{
	if( path.empty() )
		return false;

	std::ofstream ofs;
	ofs.open(path.c_str(), std::ofstream::out);
	if( !ofs.is_open() ){
		cout<<"Error opening file:" << path.c_str()<<endl;
		ofs.close();
		return false;
	}
	cout<<"Open file:" <<path.c_str()<<endl;

	unsigned int year = m_comtradeHead.revYear;
	//站名、特征和修改年份
	if( 1999 == year){
		ofs<<m_comtradeHead.stationName<<","<<m_comtradeHead.devName<<","<<m_comtradeHead.revYear<<"\r\n";
	}else{
		ofs<<m_comtradeHead.stationName<<","<<m_comtradeHead.devName<<"\r\n";
	}
	
	//通道的数量和类型
	ofs<<m_comtradeHead.aiCount+m_comtradeHead.diCount<<","<<m_comtradeHead.aiCount<<"A,"<<m_comtradeHead.diCount<<"D\r\n";
	//模拟通道信息
	for( list<aiChannel>::iterator it = m_aiChannels.begin(); it != m_aiChannels.end(); ++it )
	{
		aiChannel ai = *it;
		if( 1999 == year ){
			ofs<<ai.index<<","<<ai.name<<","<<ai.phase<<","<<ai.ccbm
				<<","<<ai.unit<<","<<ai.a<<","<<ai.b<<","<<ai.skew<<","
				<<ai.min<<","<<ai.max<<","<<m_comtradeHead.primary<<","
				<<m_comtradeHead.secondary<<","<<ai.ps<<"\r\n";
		}else{
			ofs<<ai.index<<","<<ai.name<<","<<ai.phase<<","<<ai.ccbm
				<<","<<ai.unit<<","<<ai.a<<","<<ai.b<<","<<ai.skew<<","
				<<ai.min<<","<<ai.max<<"\r\n";
		}
	}
	//状态（数字）通道信息
	for( list<diChannel>::iterator itor = m_diChannels.begin(); itor != m_diChannels.end(); ++ itor)
	{
		diChannel di = *itor;
		if( 1999 == year ){
			ofs<<di.index<<","<<di.name<<","<<di.phase<<","
				<<di.ccbm<<","<<di.y<<"\r\n";
		}else{
			ofs<<di.index<<","<<di.name<<","<<di.y<<"\r\n";
		}
	}
	//线路频率
	ofs<<m_comtradeHead.lineFreq<<"\r\n";
	//采样率信息
	ofs<<m_comtradeHead.rateCount<<"\r\n";
	for( list<sampleInfo>::iterator its = m_sampleInfos.begin(); its != m_sampleInfos.end(); ++ its)
	{
		sampleInfo sa = *its;
		ofs<<sa.rate<<","<<sa.lastIndex<<"\r\n";
	}
	//日期/时间标记
	if( 1999 == year ){
		//99标准：dd/mm/yyyy,hh:mm:ss.ssssss
		ofs<<m_comtradeHead.startTime<<"\r\n";
		ofs<<m_comtradeHead.faultTime<<"\r\n";
	}else{
		//91标准：mm/dd/yyyy,hh:mm:ss.ssssss
		string startTime = get91time(m_comtradeHead.startTime);
		ofs<<startTime<<"\r\n";
		string faultTime = get91time(m_comtradeHead.faultTime);
		ofs<<faultTime<<"\r\n";
	}
	//数据文件类型
	ofs<<m_comtradeHead.datType<<"\r\n";
	//时间标记乘数系数
	if( 1999 == year ){
		ofs<<m_comtradeHead.timemult<<"\r\n";
	}
	ofs.flush();
	ofs.close();

	return true;
}


bool CreateComtrade::createAsciiDAT(string path)
{
	if( path.empty() )
		return false;
	std::ofstream ofs;
	ofs.open(path.c_str(), std::ofstream::out);
	if( !ofs.is_open() ){
		cout<<"Error opening file:" << path.c_str()<<endl;
		ofs.close();
		return false;
	}
	int count = 1;
	for( list< list<short> >::iterator it = m_datas.begin(); it != m_datas.end(); it++ )
	{
		ofs<<count<<","<<m_times.at(count-1)<<",";
		list<short> datas = *it;
		int dnum = datas.size();
		int dcount = 1;
		for( list<short>::iterator itor=datas.begin(); itor != datas.end(); itor++)
		{	
			short val = *itor;
			ofs<<val;
			if( dnum == dcount ){
				ofs<<"\r\n";
			}else{
				ofs<<",";
			}
			dcount++;
		}
		count++;
	}

	ofs.flush();
	ofs.close();

	return true;
}

bool CreateComtrade::createBinaryDAT(string path)
{
	if( path.empty() )
		return false;
	std::ofstream ofs;
	ofs.open(path.c_str(), ofstream::out|ofstream::binary);
	if( !ofs.is_open() ){
		cout<<"Error opening file:" << path.c_str()<<endl;
		ofs.close();
		return false;
	}
	int count = 1;
	for( list< list<short> >::iterator it = m_datas.begin(); it != m_datas.end(); it++ )
	{
		unsigned int t = m_times.at(count-1);
		ofs.write((char*)&count, sizeof(int));
		ofs.write((char*)&t, sizeof(unsigned int));
		list<short> datas = *it;
		int dnum = datas.size();
		int dcount = 1;
		unsigned short tmpdival=0;
		for( list<short>::iterator itor=datas.begin(); itor != datas.end(); itor++)
		{	
			short val = *itor;
			if( dcount <= m_comtradeHead.aiCount )
			{
				ofs.write((char*)&val, sizeof(short));
			}
			else
			{
				int dinumber=dcount-m_comtradeHead.aiCount;
				int remainder = dinumber%16;
				if( remainder == 1 )
				{
					//按16位截取的第一个开关量通道
					tmpdival = 0;
				}
				if( val == 1 )
				{
					if( remainder == 0 )
					{
						tmpdival = tmpdival | val<<15;
					}
					else
					{
						tmpdival = tmpdival | val<<(remainder-1);
					}	
				}
				if( remainder == 0 || dcount == dnum )
				{
					ofs.write((char*)&tmpdival, sizeof(unsigned short));
				}
			}
			
			dcount++;
		}
		count++;
	}

	ofs.flush();
	ofs.close();

	return true;
}

};//std namespace
