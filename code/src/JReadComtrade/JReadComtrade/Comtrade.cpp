#include "Comtrade.h"
#include "../../../common/GECodeConvert.h"
#include "../../../common/common_lib.h"

//  载入波形文件 
LoadFileFUN m_LoadFileFUN;
// 	关闭文件
CloseFileFUN m_CloseFileFUN;
//2取得采样频率数
GetRatesCountFUN m_GetRatesCountFUN;
//3 取得CFG文件记录的波形数据配置属性 
GetCFGAttrFUN m_GetCFGAttrFUN;
//4取得采样点总数 
GetTotalSamplesFUN m_GetTotalSamplesFUN;  
//5 取得指定通道的配置信息 
GetChannelCFGFUN m_GetChannelCFGFUN; // 
//6	    取得指定采样点相对于采样开始时刻的时间
GetTimeRelativeStartFUN m_GetTimeRelativeStartFUN; 
//7  	取得指定采样点的采样频率
GetRateFUN m_GetRateFUN; 
//8	 取得故障触发时刻时间
GetTriggerTimeFUN m_GetTriggerTimeFUN; 
//9	取得指定采样频率索引的采样频率和采样点数
GetRateSampleFUN m_GetRateSampleFUN;
//10	取得指定通道的类型
GetChannelTypeFUN m_GetChannelTypeFUN;  
// 取得指定采样点相对于故障触发时刻的时间   
GetTimeRelativeTriggerFUN m_GetTimeRelativeTriggerFUN; 
//	 取得指定通道指定采样点的真实值  
GetRealValueFUN m_GetRealValueFUN; 
//  取得指定通道指定采样点的记录值  
GetRecordValueFUN m_GetRecordValueFUN; 
// 取得总时间   
GetTotalTimeFUN m_GetTotalTimeFUN; 
//取得指定通道的最大值  
GetMaxValueFUN m_GetMaxValueFUN; 
//    取得CTPT变比 
GetCTPTFUN m_GetCTPTFUN; 
//设置CTPT变比  
SetCTPTFUN m_SetCTPTFUN;
GetHdrInfoJSON m_GetHdrInfoJSON;

XJHANDLE hDllInst  ;
void freeQuickComtrade(){

	if(hDllInst){
		xj_free_library(hDllInst);
		hDllInst=NULL;
	}
}

bool Comtrade::loadQuickComtrade()
{

	string strFile = "quickComtrade.dll";
#ifndef OS_WINDOWS //其他平台
	strFile = "libQuickComtrade.so";
#endif
	hDllInst   =  xj_load_library(strFile.c_str());  
	
	if(hDllInst)  
	{ 
		//  载入波形文件 
		m_LoadFileFUN =   (LoadFileFUN)xj_get_addr (hDllInst, "LoadFile");  
		// 	关闭文件
		m_CloseFileFUN  =   (CloseFileFUN)xj_get_addr (hDllInst, "CloseFile");  
		//3 取得CFG文件记录的波形数据配置属性 
		m_GetCFGAttrFUN =   (GetCFGAttrFUN)xj_get_addr (hDllInst, "GetCFGAttr"); 
		//2取得采样频率数
		m_GetRatesCountFUN =   (GetRatesCountFUN)xj_get_addr (hDllInst, "GetRatesCount");  
		//9	取得指定采样频率索引的采样频率和采样点数
		m_GetRateSampleFUN =   (GetRateSampleFUN)xj_get_addr (hDllInst, "GetRateSample");  
		//7  	取得指定采样点的采样频率
		m_GetRateFUN =   (GetRateFUN)xj_get_addr (hDllInst, "GetRate");  
		//5 取得指定通道的配置信息 
		m_GetChannelCFGFUN =   (GetChannelCFGFUN)xj_get_addr (hDllInst, "GetChannelCFG");  
		//10	取得指定通道的类型
		m_GetChannelTypeFUN =   (GetChannelTypeFUN)xj_get_addr (hDllInst, "GetChannelType"); 
		//8	 取得故障触发时刻时间
		m_GetTriggerTimeFUN =   (GetTriggerTimeFUN)xj_get_addr (hDllInst, "GetTriggerTime");   
		// 取得指定采样点相对于故障触发时刻的时间   
		m_GetTimeRelativeTriggerFUN =   (GetTimeRelativeTriggerFUN)xj_get_addr (hDllInst, "GetTimeRelativeTrigger");
		//6	    取得指定采样点相对于采样开始时刻的时间
		m_GetTimeRelativeStartFUN =   (GetTimeRelativeStartFUN)xj_get_addr (hDllInst, "GetTimeRelativeStart");   
		//4取得采样点总数 
		m_GetTotalSamplesFUN =   (GetTotalSamplesFUN)xj_get_addr (hDllInst, "GetTotalSamples"); 
		//	 取得指定通道指定采样点的真实值  
		m_GetRealValueFUN =   (GetRealValueFUN)xj_get_addr (hDllInst, "GetRealValue");  
		//  取得指定通道指定采样点的记录值  
		m_GetRecordValueFUN =   (GetRecordValueFUN)xj_get_addr (hDllInst, "GetRecordValue");   
		// 取得总时间   
		m_GetTotalTimeFUN =   (GetTotalTimeFUN)xj_get_addr (hDllInst, "GetTotalTime");  
		//取得指定通道的最大值  
		m_GetMaxValueFUN =   (GetMaxValueFUN)xj_get_addr (hDllInst, "GetMaxValue");   
		//    取得CTPT变比 
		m_GetCTPTFUN =   (GetCTPTFUN)xj_get_addr (hDllInst, "GetCTPT");   
		//设置CTPT变比  
		m_SetCTPTFUN =   (SetCTPTFUN)xj_get_addr (hDllInst, "SetCTPT");
		m_GetHdrInfoJSON = (GetHdrInfoJSON)xj_get_addr(hDllInst, "GetHdrInfoJSON");
		return true;
	}
	return false;//-1:加载失败
}


bool  Comtrade::isLoadQuickComtrade=loadQuickComtrade();

Comtrade::Comtrade(void):isFreeQuickComtrade(false)
{
}

Comtrade::~Comtrade(void)
{
	m_CloseFileFUN(hQuickComtrade);
	if(isFreeQuickComtrade){
		freeQuickComtrade();
	}
}
int Comtrade::LoadFile(char* sFilePath, char* sError)
{

	if(hDllInst==NULL){
		isLoadQuickComtrade=loadQuickComtrade();
	}

	if(!isLoadQuickComtrade){
		return  1;
	} 
	return m_LoadFileFUN( sFilePath ,sError,hQuickComtrade);

}
int	Comtrade::GetComtradeCfg (JNIEnv *env, jclass,jobject obj_cfg, char* sError){
	//基本属性结构体
	ComtradeCFG pCFG_Struct;
	m_GetCFGAttrFUN(hQuickComtrade, pCFG_Struct); 

	jclass cls = env->FindClass("com/wave/server/ComtradeCFG");//通道类
	if(cls == NULL){//
		return 1;
	}
	jmethodID cons = env->GetMethodID(cls,"<init>","()V");//构造函数 
	//jobject obj_cfg = env->NewObject(cls,cons);
	//变电站名称
	jfieldID jf_sStation=env->GetFieldID(cls,"sStation","Ljava/lang/String;");
	jstring js_sStation = windowsToJstring(env,(char*)pCFG_Struct.sStation.c_str());
	env->SetObjectField(obj_cfg,jf_sStation,js_sStation);
	//装置名
	jfieldID jf_sDevice=env->GetFieldID(cls,"sDevice","Ljava/lang/String;");
	jstring js_sDevice = windowsToJstring(env,(char*)pCFG_Struct.sDevice.c_str());
	env->SetObjectField(obj_cfg,jf_sDevice,js_sDevice); 
	// 文件标准年份
	jfieldID jf_nYear = env->GetFieldID(cls,"nYear","I");
	env->SetIntField(obj_cfg,jf_nYear,pCFG_Struct.nYear); 
	//模拟量通道个数
	jfieldID jf_nAChannels = env->GetFieldID(cls,"nAChannels","I");
	env->SetIntField(obj_cfg,jf_nAChannels,pCFG_Struct.nAChannels);
	//开关量通道个数
	jfieldID jf_nDChannels = env->GetFieldID(cls,"nDChannels","I");
	env->SetIntField(obj_cfg,jf_nDChannels,pCFG_Struct.nDChannels); 
	//线路频率
	jfieldID jf_fLineFreq = env->GetFieldID(cls,"fLineFreq","F");
	env->SetFloatField(obj_cfg,jf_fLineFreq,pCFG_Struct.fLineFreq);
	//开始采样时间
	jfieldID jf_sStartTime=env->GetFieldID(cls,"sStartTime","Ljava/lang/String;");
	jstring js_sStartTime = windowsToJstring(env,(char*)pCFG_Struct.sStartTime.c_str());
	env->SetObjectField(obj_cfg,jf_sStartTime,js_sStartTime); 
	//故障触发事件
	jfieldID jf_sTriggerTime=env->GetFieldID(cls,"sTriggerTime","Ljava/lang/String;");
	jstring js_sTriggerTime = windowsToJstring(env,(char*)pCFG_Struct.sTriggerTime.c_str());
	env->SetObjectField(obj_cfg,jf_sTriggerTime,js_sTriggerTime); 
	//数据文件格式
	jfieldID jf_nDATType = env->GetFieldID(cls,"nDATType","I");
	env->SetIntField(obj_cfg,jf_nDATType,pCFG_Struct.nDATType); 

	//时标倍乘系数 
	jfieldID jf_fTimeMult = env->GetFieldID(cls,"fTimeMult","F");
	env->SetFloatField(obj_cfg,jf_fTimeMult,pCFG_Struct.fTimeMult); 


	nTotalSamples=m_GetTotalSamplesFUN(hQuickComtrade);//采样点总数 
	jfieldID jf_nTotalSamples = env->GetFieldID(cls,"nTotalSamples","I");
	env->SetIntField(obj_cfg,jf_nTotalSamples,nTotalSamples); 
	///采样点对应的频率
	jfloatArray  time_hz =(env)->NewFloatArray(nTotalSamples );  
	if(time_hz==NULL){
		cout<<"采样点对应的频率==1==malloc memory fail!";
		return 1000;
	}
	jfloat*jt_hz=new jfloat[nTotalSamples];
	if(jt_hz==NULL){
		cout<<"采样点对应的频率==2==malloc memory fail!";
		return 1000;
	}
	for(int i=0;i<nTotalSamples;i++){ 
		jt_hz[i]= m_GetRateFUN(hQuickComtrade,i);
	}
	env->SetFloatArrayRegion(time_hz,0,nTotalSamples,jt_hz);
	jfieldID jf_x_time_hz= env->GetFieldID(cls,"x_time_hz","[F"); 
	env->SetObjectField(obj_cfg,jf_x_time_hz,time_hz);  
	delete[] jt_hz ;

	int nfNum=m_GetRatesCountFUN(hQuickComtrade);//采样频率数目
	if(nfNum>0){

		jfloatArray  fSampleRateArr =(env)->NewFloatArray(nfNum );  
		jfloat*jfSampleRate=new jfloat[nfNum];

		jintArray nSampleRateNumsArr=env->NewIntArray(nfNum);
		jint* jnSampleRateNums=new  jint[nfNum];

		for(int i=0;i<nfNum;i++){
			//GetRateSample(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);
			int ntmp=0;
			m_GetRateSampleFUN(hQuickComtrade,i,jfSampleRate[i], ntmp); 
			 jnSampleRateNums[i]=ntmp;
		} 
		env->SetFloatArrayRegion(fSampleRateArr,0,nfNum,jfSampleRate);
		jfieldID jf_fSampleRate= env->GetFieldID(cls,"fSampleRate","[F");  
		env->SetObjectField(obj_cfg,jf_fSampleRate,fSampleRateArr);  

		env->SetIntArrayRegion(nSampleRateNumsArr,0,nfNum,jnSampleRateNums);
		jfieldID jf_nSampleRateNums= env->GetFieldID(cls,"nSampleRateNums","[I");  
		env->SetObjectField(obj_cfg,jf_nSampleRateNums,nSampleRateNumsArr); 


		delete[] jnSampleRateNums ;
		delete[] jfSampleRate ;
	}

	//时间
	jfloatArray  time =(env)->NewFloatArray(nTotalSamples );  
	if(time==NULL){
		cout<<"采样时间不能开辟空间1"<<endl;
		return 1000;
	}
	jfloat*jt=new jfloat[nTotalSamples];
	if(jt==NULL){
		cout<<"采样时间不能开辟空间2"<<endl;
		return 1000;
	}
	for(int k=0;k<nTotalSamples;k++){ 
		jt[k]= m_GetTimeRelativeTriggerFUN(hQuickComtrade,k);
	}
	env->SetFloatArrayRegion(time,0,nTotalSamples,jt);
	jfieldID jf_time= env->GetFieldID(cls,"x_time","[F"); 
	env->SetObjectField(obj_cfg,jf_time,time);  
	delete[] jt ;
	
	 
	//public int nTriggerTime;//取得故障触发时刻时间
	jfieldID jf_nTriggerTime= env->GetFieldID(cls,"nTriggerTime","I");
	env->SetIntField(obj_cfg,jf_nTriggerTime,m_GetTriggerTimeFUN(hQuickComtrade)); 
	 

	m_nA=pCFG_Struct.nAChannels;//模拟量
	m_nD=pCFG_Struct.nDChannels;//开关量
	return 0;
}

int	Comtrade::GetComtradeA (JNIEnv *env, jclass,jobject a, char* sError){

	jclass listcls = env->GetObjectClass(a);   
	jmethodID listcons = env->GetMethodID(listcls,"<init>","()V"); 
	jmethodID listAdd = env->GetMethodID(listcls,"add","(Ljava/lang/Object;)Z");

	jclass cls1 = env->FindClass("com/wave/server/ComtradeChannel");//通道类 
	jmethodID cons1 = env->GetMethodID(cls1,"<init>","()V");//构造函数 
	 
	for (int i_a = 0; i_a <m_nA; i_a++)
	{
		//cout<<"GetComtradeA========i_a=====1===="<<i_a<<endl;
		//基本属性结构体
		ComtradeChannel  channel ;
		channel.	nChannelNo = 0;
		channel.sChannelName = "";
		channel.sPhase = "";
		channel.sCcbm = "";
		channel.sUnit = "";
		channel.fScale = 0.0;
		channel.fOffset = 0.0;
		channel.fSkew = 0.0;
		channel.nMinValue = 0;
		channel.nMaxValue = 0;
		channel.fPrimary = 0.0;
		channel.fSecondary = 0.0;
		channel.sPSType = "";
		channel.nStatus = -1; 
		m_GetChannelCFGFUN(hQuickComtrade,i_a,channel);  
		//cout<<"GetComtradeA========i_a=====1==1=="<<i_a<<endl;
		jobject ob1 = env->NewObject(cls1,cons1);  
		////通道索引号
		jfieldID jf_nChannelNo = env->GetFieldID(cls1,"nChannelNo","I");
		env->SetIntField(ob1,jf_nChannelNo,channel.nChannelNo);
		//通道名称 
		jfieldID jf_sChannelName=env->GetFieldID(cls1,"sChannelName","Ljava/lang/String;");
		jstring js_sChannelName = windowsToJstring(env,(char*)channel.sChannelName.c_str());
		env->SetObjectField(ob1,jf_sChannelName,js_sChannelName);  
		//** @brief 相位标识符 */
		jfieldID jf_sPhase=env->GetFieldID(cls1,"sPhase","Ljava/lang/String;");
		jstring js_sPhase = windowsToJstring(env,(char*)channel.sPhase.c_str());
		env->SetObjectField(ob1,jf_sPhase,js_sPhase);
		//cout<<"GetComtradeA========i_a=====1==2=="<<i_a<<endl;
		//** @brief 被监视的回路元件 */
		jfieldID jf_sCcbm=env->GetFieldID(cls1,"sCcbm","Ljava/lang/String;");
		jstring js_sCcbm = windowsToJstring(env,(char*)channel.sCcbm.c_str());
		env->SetObjectField(ob1,jf_sCcbm,js_sCcbm);
		//** @brief 单位 */
		jfieldID jf_sUnit=env->GetFieldID(cls1,"sUnit","Ljava/lang/String;");
		jstring js_sUnit = windowsToJstring(env,(char*)channel.sUnit.c_str());
		env->SetObjectField(ob1,jf_sUnit,js_sUnit); 
		//** @brief 通道增益系数a */
		jfieldID jf_fScale = env->GetFieldID(cls1,"fScale","F");
		env->SetFloatField(ob1,jf_fScale,channel.fScale); 
		//** @brief 通道偏移量b */
		jfieldID jf_fOffset = env->GetFieldID(cls1,"fOffset","F");
		env->SetFloatField(ob1,jf_fOffset,channel.fOffset);  
		//** @brief 采样时滞 */
		jfieldID jf_fSkew = env->GetFieldID(cls1,"fSkew","F");
		env->SetFloatField(ob1,jf_fSkew,channel.fSkew);  
		//** @brief 最小值范围 */
		jfieldID jf_nMinValue = env->GetFieldID(cls1,"nMinValue","I");
		env->SetIntField(ob1,jf_nMinValue,channel.nMinValue); 
		//** @brief 最大值范围 */
		jfieldID jf_nMaxValue = env->GetFieldID(cls1,"nMaxValue","I");
		env->SetIntField(ob1,jf_nMaxValue,channel.nMaxValue); 
		//** @brief 一次比例因子 */
		jfieldID jf_fPrimary= env->GetFieldID(cls1,"fPrimary","F");
		env->SetFloatField(ob1,jf_fPrimary,channel.fPrimary);  
		//** @brief 二次比例因子 */
		jfieldID jf_fSecondary= env->GetFieldID(cls1,"fSecondary","F");
		env->SetFloatField(ob1,jf_fSecondary,channel.fSecondary);  
		//** @brief 一次或二次标识符.P,p,S,s */
		jfieldID jf_sPSType=env->GetFieldID(cls1,"sPSType","Ljava/lang/String;");
		jstring js_sPSType = windowsToJstring(env,(char*)channel.sPSType.c_str());
		env->SetObjectField(ob1,jf_sPSType,js_sPSType);  
		//** @brief 通道正常状态.0/1 */
		jfieldID jf_nStatus = env->GetFieldID(cls1,"nStatus","I");
		env->SetIntField(ob1,jf_nStatus,channel.nStatus); 
		/// 取得指定通道的类型 
		jfieldID jf_nChannelType = env->GetFieldID(cls1,"nChannelType","I"); 
		env->SetIntField(ob1,jf_nChannelType, m_GetChannelTypeFUN(hQuickComtrade,i_a) );
		//cout<<"GetComtradeA========i_a=====1==3=="<<i_a<<endl;
		//数据
		jintArray  arrA =(env)->NewIntArray(nTotalSamples ); 
		if(arrA==NULL){
			cout<<i_a<<"===arrA====malloc memory fail!";
			return 3000;
		}
		//cout<<"GetComtradeA========i_a=====1==4=="<<i_a<<endl;
		jint*jA=new jint[nTotalSamples];
		if (jA == NULL)
		{
			cout<<i_a<<"==jA====malloc memory fail!";
			return 3000;
		} 
		for(int ii=0;ii<nTotalSamples;ii++){ 
			jA[ii]= m_GetRecordValueFUN(hQuickComtrade,i_a,ii); 
		} 
		//cout<<"GetComtradeA========i_a=====1==5=="<<i_a<<endl;
		env->SetIntArrayRegion(arrA,0,nTotalSamples, jA);
		jfieldID jf_ListA= env->GetFieldID(cls1,"ListA","[I"); 
		env->SetObjectField(ob1,jf_ListA,arrA);   
		delete[] jA  ; 
		 
		//cout<<"GetComtradeA========i_a====2====="<<i_a<<endl;
		env->CallObjectMethod(a,listAdd,ob1); 

	}  
	return 0;
}

int	Comtrade::GetComtradeD (JNIEnv *env, jclass,jobject d, char* sError){

	//调用Java 端的ArrayList
	jclass listcls = env->GetObjectClass(d);    
	jmethodID listAdd = env->GetMethodID(listcls,"add","(Ljava/lang/Object;)Z"); 

	jclass ch_cls  = env->FindClass("com/wave/server/ComtradeChannel");//通道类 
	jmethodID cons1 = env->GetMethodID(ch_cls,"<init>","()V");//构造函数 

	for (int i_d = m_nA; i_d <m_nA+m_nD; i_d++)
	{
		//基本属性结构体
		ComtradeChannel channel;
		m_GetChannelCFGFUN(hQuickComtrade,i_d,channel);  
		jobject obj_ch = env->NewObject(ch_cls,cons1); 
		//通道索引号
		jfieldID jf_nChannelNo = env->GetFieldID(ch_cls,"nChannelNo","I");
		env->SetIntField(obj_ch,jf_nChannelNo,channel.nChannelNo);
		//通道名称
		jfieldID jf_sChannelName=env->GetFieldID(ch_cls,"sChannelName","Ljava/lang/String;");
		jstring js_sChannelName = windowsToJstring(env,(char*)channel.sChannelName.c_str());
		env->SetObjectField(obj_ch,jf_sChannelName,js_sChannelName); 
		/** @brief 通道正常状态.0/1 */
		jfieldID jf_nStatus = env->GetFieldID(ch_cls,"nStatus","I");
		env->SetIntField(obj_ch,jf_nStatus,channel.nStatus);

		//数据
		jclass arrayListCls = env->FindClass("java/util/ArrayList");
		jmethodID arrayListCons = env->GetMethodID(arrayListCls,"<init>","()V");
		jobject arrayListObj = env->NewObject(arrayListCls,arrayListCons,"");
		jmethodID arrayListAdd = env->GetMethodID(arrayListCls,"add","(Ljava/lang/Object;)Z");

		jintArray jarr= (env)->NewIntArray(2);  
		jint v[2]; 
		v[0]=0;
		int v_tmp= m_GetRecordValueFUN(hQuickComtrade,i_d,0); 
		v[1]= v_tmp ;//临时保存
		env->SetIntArrayRegion(jarr,0,2, v);
		env->CallObjectMethod(arrayListObj,arrayListAdd,jarr);  
		for(int ii=1;ii<nTotalSamples;ii++){ 
			int nd_value= m_GetRecordValueFUN(hQuickComtrade,i_d,ii);  
			if(nd_value!=v_tmp){//不同,则保存
				jarr= (env)->NewIntArray(2); 
				jint  vv[2]; 
				vv[0]=ii;
				vv[1]=nd_value;
				v_tmp=nd_value;
				env->SetIntArrayRegion(jarr,0,2, vv); 
				env->CallObjectMethod(arrayListObj,arrayListAdd,jarr);   
			}
		}  
		//在最后增加一个变位点,值和上一个变位点相同,序号为最后一个采样点
		jarr= (env)->NewIntArray(2); 
		jint  vv[2]; 
		vv[0]=nTotalSamples-1;
		vv[1]=v_tmp; 
		env->SetIntArrayRegion(jarr,0,2, vv); 
		env->CallObjectMethod(arrayListObj,arrayListAdd,jarr); 


		jfieldID jf_ListD= env->GetFieldID(ch_cls,"listD","Ljava/util/List;"); 
		env->SetObjectField(obj_ch,jf_ListD,arrayListObj);   

		// 
		env->CallObjectMethod(d,listAdd,obj_ch); 
	}

	return 0;
}

jstring Comtrade::GetHdrInfo(JNIEnv* env)
{
	string json = m_GetHdrInfoJSON(hQuickComtrade);
	return windowsToJstring(env,(char*)json.c_str());
}


bool bNeedConv = true;

/*************************************************************
函 数 名：jstringToNative()
功能概要：将Java的Unicode字符串转化本地字符;
返 回 值: char* rtn 
参    数：JNIEnv  *env,        
参    数：jstring jstr         字符串      
**************************************************************/
char* jstringToWindows(JNIEnv *env, jstring jstr)  
{  
	CGECodeConvert cc;	
	const jchar* jcstr = (env)->GetStringChars(jstr, 0 );
	int length = env->GetStringLength(jstr);
	char* rtn = 0;

	cout<<"input string is :"<<(char*)jcstr<<endl;
	
	if( cc.Unicode2Gbk((wchar_t*)jcstr, length, rtn) < 0 )
	{
		bNeedConv = false;
		cout<<"jstringToWindows,convert from utf8 to gbk failed"<<endl;
		const char* utf8 = env->GetStringUTFChars(jstr, 0);
		long utfLength = env->GetStringUTFLength(jstr);
		
			//可能传入的就是GBK
			if (env->ExceptionCheck() == JNI_TRUE || jstr == NULL)  
			{  
				env->ExceptionDescribe();  
				env->ExceptionClear();  
				printf("jstringToNative函数转换时,传入的参数str为空");  
				return NULL;  
			}   

			jbyteArray bytes = 0;   
			jthrowable exc;   
			char *result = 0;   
			if (env->EnsureLocalCapacity(2) < 0) {   
				return 0; 
			}   

			jclass jcls_str = env->FindClass("java/lang/String");   
			jmethodID MID_String_getBytes = env->GetMethodID(jcls_str, "getBytes", "()[B");   

			bytes = (jbyteArray)env->CallObjectMethod(jstr, MID_String_getBytes);   

			exc = env->ExceptionOccurred();   
			if (!exc) {   
				jint len = env->GetArrayLength( bytes);   
				result = new char[len + 1];
				memset(result, 0, len+1);
				if (result == 0) { 
					env->DeleteLocalRef(bytes);   
					return 0;   
				}   
				env->GetByteArrayRegion(bytes, 0, len, (jbyte *)result);   
				result[len] = 0; 
			} else {   
				env->DeleteLocalRef(exc);   
			} 

			env->DeleteLocalRef(bytes); 
			return result;
	}
	else
	{
		bNeedConv = true;
		cout<<"input filepath is right unicode"<<endl;
	}
	return rtn;
}   


/*************************************************************
函 数 名：nativeTojstring()  
功能概要：将Java的Unicode字符串转化本地字符;
返 回 值: jstring jstr 
参    数：JNIEnv  *env,        
参    数：char* rtn         字符串      
**************************************************************/
jstring windowsToJstring( JNIEnv* env,const char* str )  
{  

	jstring rtn = 0;
	CGECodeConvert cc;

	int slen = strlen(str);
	unsigned short * buffer = 0;
	if( slen == 0 )
		rtn = (env)->NewStringUTF(str );
	else
	{
		if( bNeedConv )
		{
			wchar_t* pUtf = 0;
			long wlength = cc.Gbk2Unicode(str, slen, pUtf);
			if( pUtf != 0)
			{
				rtn = env->NewString((jchar*)pUtf, wlength);
				if( pUtf )
				{
					delete[] pUtf;
				}
			}
		}
		else
		{
			jclass strClass = env->FindClass( "Ljava/lang/String;");   
			jmethodID ctorID = env->GetMethodID( strClass, "<init>",  "([BLjava/lang/String;)V");   

			if (env->ExceptionCheck() == JNI_TRUE || str == NULL)  
			{  
				env->ExceptionDescribe();  
				env->ExceptionClear();  
				printf("nativeTojstring函数转换时,str为空/n");  
				return NULL;  
			}   

			jbyteArray bytes = env->NewByteArray( strlen(str));   
			//如果str为空则抛出异常给jvm    
			env->SetByteArrayRegion( bytes, 0,  strlen(str), (jbyte*)str);   
			jstring encoding = env->NewStringUTF( "GBK"); 
			jstring strRtn = (jstring)env->NewObject( strClass, ctorID, bytes, encoding);
			return strRtn;
		}	
	}
	return rtn;
}  