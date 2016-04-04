// testfile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CreateComtrade.h"
#include <sstream>

int _tmain(int argc, _TCHAR* argv[])
{
	CreateComtrade com;
	comtradeHead head;
	head.stationName = "青烟变";
	head.devName = "220kV录波器";
	head.revYear = 1991;
	head.aiCount=8;
	head.diCount=42;
	head.primary=1.0;
	head.secondary=1.0;
	head.lineFreq=50;
	head.rateCount=2;
	head.startTime="12/02/10,15:41:59.428000";
	head.faultTime="12/02/10,15:41:59.468000";
	head.datType="ascii";
	head.timemult=1.0;

	com.setComtradeHead(head);

	list<aiChannel> aichs;

	aiChannel ai;
	ai.index=1;
	ai.name="Ia";
	ai.phase="A";
	ai.ccbm="";
	ai.unit="A";
	ai.a=0.005418;
	ai.b=0;
	ai.skew=0;
	ai.min=-32767;
	ai.max=32767;
	ai.ps="S";
	aichs.push_back(ai);

	ai.index=2;
	ai.name="Ib";
	ai.phase="B";
	aichs.push_back(ai);

	ai.index=3;
	ai.name="Ic";
	ai.phase="C";
	aichs.push_back(ai);

	ai.index=4;
	ai.name="3I0";
	ai.phase="N";
	aichs.push_back(ai);

	ai.index=5;
	ai.name="Ua";
	ai.phase="A";
	ai.unit="V";
	ai.a=0.018783;
	aichs.push_back(ai);

	ai.index=6;
	ai.name="Ub";
	ai.phase="B";
	aichs.push_back(ai);

	ai.index=7;
	ai.name="Uc";
	ai.phase="C";
	aichs.push_back(ai);

	ai.index=8;
	ai.name="3U0";
	ai.phase="N";
	aichs.push_back(ai);

	com.setAiChannels(aichs);

	list<diChannel> dichs;
	for( int i = 0; i <42; i++ )
	{
		diChannel di;
		di.index = i+1;
		stringstream ss;
		ss<<"测试开关量通道"<<i+1;
		di.name = ss.str();
		di.phase="";
		di.ccbm="";
		di.y=0;
		dichs.push_back(di);
	}
	com.setDiChannels(dichs);

	list<sampleInfo> samples;
	sampleInfo sa;
	sa.rate=1200;
	sa.lastIndex=192;
	samples.push_back(sa);
	sa.rate=50;
	sa.lastIndex=209;
	samples.push_back(sa);
	com.setSampleInfos(samples);

	srand(234802);
	list< list<short> > datas;
	for( int s = 0; s < 209; s++ ){
		list<short> d;
		for( int i = 0; i < head.aiCount; i++ ){
			short v = (rand()%10000)-5000;
			d.push_back(v);
		}
		for( int i = 0; i < head.diCount; i++ ){
			short v = rand()%2;
			d.push_back(v);
		}
		datas.push_back(d);
	}
	com.setDatas(datas);

	com.create("d:/stdown/comtrade", "test");
	return 0;
}

