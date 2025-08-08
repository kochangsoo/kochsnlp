#pragma once


#include "ds_htoken.h"
#include "ds_list.h"
#include "ds_htoken.h"
#include "na_convert.h"
#include "ds_atoken.h"
#include "ds_ambtable.h"
#include "NAHangulanaly.h"
#include "MorphoLogicalAnalysis.h"

//class CNAHangulAnaly;
class CMorphoLogicalAnalysis;

///////////////////////////////////////////////////
//중의성 관련 처리 클래스(Amb)
///////////////////////////////////////////////////
class CNAAmbiguity
{
public:
	CNAAmbiguity();
	~CNAAmbiguity();

public:

	char* m_pEndBuffer;
	char* m_pOneChar;
	char* m_pStartCountChar;
	char* m_pJongChar;
	char* m_pChoJungChar;
	char* m_pMinusTwoString;
	char* m_pAddTwoString;

	CConvert *m_pConvert;//Convert Class

	//CNAHangulAnaly *m_pHangulAnaly;
	CMorphoLogicalAnalysis *m_pHangulAnaly;

	H_NODE *pThisNode;

public:
	bool IsJungsungVFunction(char* pString, int nStart, char* strCmp);
	bool IsChosungIFunction(char* pString, int nStart, char* strCmp);
	bool IsJongsungCFunction(char *pString, int nStart, char* strCmp);

	bool INT_CON(char* pString, int nStart);
	bool INT_VOW(char* pString, int nStart);

	void GoAmbiguityFunction(/*CNAHangulAnaly*/CMorphoLogicalAnalysis *m_pHan, H_NODE *pNode, int nKindScript);
	void InitBuffer();
	void IninitMemberChar();
	void MoveLeftWordCount(char *pString, int &nCount);

	void GetOneChar(char *pStr, int nPos, int nCount);
	void GetChojungJongsungChar(char *pOneChar, int nKind);
	void GetStartCountChar(char *pStr, int nMoveCount);
	void MinusTwoStr(char *pOneString, char *pTwoString, int nPos);
	bool IsThisWordHangulAscii(char *pThisChar, int nPos);
	bool CalculateWordCount(char *pString, int nCount, int nMove, int nCase);

	bool SaveStemFeature(H_NODE *pNode, int nPos1, int nPos2 = -1, int nPos3 = -1, int nPos4 = -1, int nPos5 = -1, int nPos6 = -1)
	{
		pNode->pHToken->subStem->strFeature[nPos1] = '1';
		if (nPos2 != -1) pNode->pHToken->subStem->strFeature[nPos2] = '1';
		if (nPos3 != -1) pNode->pHToken->subStem->strFeature[nPos3] = '1';
		if (nPos4 != -1) pNode->pHToken->subStem->strFeature[nPos4] = '1';
		if (nPos5 != -1) pNode->pHToken->subStem->strFeature[nPos5] = '1';
		if (nPos6 != -1) pNode->pHToken->subStem->strFeature[nPos6] = '1';
		return true;
	}

	////////////////////////////////////////////////////////////////////
	//테이블 정보 
	bool IsExistTable001(char* pString, int nMumber);
	bool IsExistTable002(char* pString, int nMumber);
	bool IsExistTable003(char* pString, int nMumber);
	bool IsExistTable004(char* pString, int nMumber);
	bool IsExistTable005(char* pString, int nMumber);
	bool IsExistTable006(char* pString, int nMumber);
	bool IsExistTable007(char* pString, int nMumber);

	bool IsExistTable051(char* pString, int nMumber);
	bool IsExistTable052(char* pString, int nMumber);
	bool IsExistTable053(char* pString, int nMumber);
	bool IsExistTable054(char* pString, int nMumber);
	bool IsExistTable055(char* pString, int nMumber);
	bool IsExistTable056(char* pString, int nMumber);
	bool IsExistTable057(char* pString, int nMumber);
	bool IsExistTable058(char* pString, int nMumber);
	bool IsExistTable059(char* pString, int nMumber);
	bool IsExistTable060(char* pString, int nMumber);

	bool IsExistTable061(char* pString, int nMumber);
	bool IsExistTable062(char* pString, int nMumber);
	bool IsExistTable063(char* pString, int nMumber);
	bool IsExistTable064(char* pString, int nMumber);
	bool IsExistTable065(char* pString, int nMumber);
	bool IsExistTable066(char* pString, int nMumber);
	bool IsExistTable067(char* pString, int nMumber);
	bool IsExistTable068(char* pString, int nMumber);
	bool IsExistTable069(char* pString, int nMumber);
	bool IsExistTable070(char* pString, int nMumber);
	bool IsExistTable071(char* pString, int nMumber);
	bool IsExistTable072(char* pString, int nMumber);
	bool IsExistTable073(char* pString, int nMumber);
	bool IsExistTable074(char* pString, int nMumber);
	bool IsExistTable075(char* pString, int nMumber);
	bool IsExistTable076(char* pString, int nMumber);

public:


	//엔딩 스크립트 .......
	void Amb_Function_0001(H_NODE *pNode);
	void Amb_Function_0002(H_NODE *pNode);
	void Amb_Function_0003(H_NODE *pNode);
	void Amb_Function_0004(H_NODE *pNode);
	void Amb_Function_0005(H_NODE *pNode);
	void Amb_Function_0006(H_NODE *pNode);
	void Amb_Function_0007(H_NODE *pNode);
	void Amb_Function_0008(H_NODE *pNode);
	void Amb_Function_0009(H_NODE *pNode);
	void Amb_Function_0010(H_NODE *pNode);

	void Amb_Function_0011(H_NODE *pNode);
	void Amb_Function_0012(H_NODE *pNode);
	void Amb_Function_0013(H_NODE *pNode);
	void Amb_Function_0014(H_NODE *pNode);
	void Amb_Function_0015(H_NODE *pNode);
	void Amb_Function_0016(H_NODE *pNode);
	void Amb_Function_0017(H_NODE *pNode);
	void Amb_Function_0018(H_NODE *pNode);
	void Amb_Function_0019(H_NODE *pNode);
	void Amb_Function_0020(H_NODE *pNode);
	void Amb_Function_0021(H_NODE *pNode);

	void Amb_Function_0022(H_NODE *pNode);

public:
	void CreateNewEndStr(H_NODE* pNode, char* pEndString);
	void GoHaveEndAmbStr(H_NODE* pNode);

	void SaveEndBuffer(char* pSave);
	void AddTwoStr(char *pOneString, char *pTwoString);
	void CreateStemEndStr(H_NODE *pNode, int nCase, int nCount);
	bool IsExistTable(int nCase, int strCase);

	bool ThisNodeNull(int nCase);
	bool NextNodeNull(int nCase);
	bool PrevNodeNull(int nCase);

	//현재 노드를 기준으로 값을 확인한다. 
	bool PrevNodestrResult(int nCase);
	bool PrevNodeFeature(int nCase);
	bool NextNodestrResult(int nCase);
	bool NextNodeFeature(int nCase);
	bool ThisNodestrResult(int nCase);
	bool ThisNodeFeature(int nCase);

	void SaveHangulAddress(/*CNAHangulAnaly*/CMorphoLogicalAnalysis* m_pH, H_NODE *pN);
	void CallEndScriptFunction(H_NODE *pNode, char* pEndScript);
	void CallStemScriptFunction(H_NODE *pNode, char* pStemScript);
	
	
	//스템 스크립트 ......
	void Amb_Function_0101(H_NODE *pNode);
	void Amb_Function_0102(H_NODE *pNode);
	void Amb_Function_0103(H_NODE *pNode);
	void Amb_Function_0104(H_NODE *pNode);
	void Amb_Function_0105(H_NODE *pNode);
	void Amb_Function_0106(H_NODE *pNode);
	void Amb_Function_0107(H_NODE *pNode);
	void Amb_Function_0108(H_NODE *pNode);
	void Amb_Function_0109(H_NODE *pNode);
	void Amb_Function_0110(H_NODE *pNode);

	void Amb_Function_0111(H_NODE *pNode);
	void Amb_Function_0112(H_NODE *pNode);
	void Amb_Function_0113(H_NODE *pNode);
	void Amb_Function_0114(H_NODE *pNode);
	void Amb_Function_0115(H_NODE *pNode);
	void Amb_Function_0116(H_NODE *pNode);
	void Amb_Function_0117(H_NODE *pNode);
	void Amb_Function_0118(H_NODE *pNode);
	void Amb_Function_0119(H_NODE *pNode);
	void Amb_Function_0120(H_NODE *pNode);

	void Amb_Function_0121(H_NODE *pNode);
	void Amb_Function_0122(H_NODE *pNode);
	void Amb_Function_0123(H_NODE *pNode);
	void Amb_Function_0124(H_NODE *pNode);
	void Amb_Function_0125(H_NODE *pNode);
	void Amb_Function_0126(H_NODE *pNode);
	void Amb_Function_0127(H_NODE *pNode);
	void Amb_Function_0128(H_NODE *pNode);
	void Amb_Function_0129(H_NODE *pNode);
	void Amb_Function_0130(H_NODE *pNode);

	void Amb_Function_0131(H_NODE *pNode);
	void Amb_Function_0132(H_NODE *pNode);
	void Amb_Function_0133(H_NODE *pNode);
	void Amb_Function_0134(H_NODE *pNode);
	void Amb_Function_0135(H_NODE *pNode);
	void Amb_Function_0136(H_NODE *pNode);
	void Amb_Function_0137(H_NODE *pNode);
	void Amb_Function_0138(H_NODE *pNode);
	void Amb_Function_0139(H_NODE *pNode);
	void Amb_Function_0140(H_NODE *pNode);

	void Amb_Function_0141(H_NODE *pNode);
	void Amb_Function_0142(H_NODE *pNode);
	void Amb_Function_0143(H_NODE *pNode);
	void Amb_Function_0144(H_NODE *pNode);
	void Amb_Function_0145(H_NODE *pNode);
	void Amb_Function_0146(H_NODE *pNode);
	void Amb_Function_0147(H_NODE *pNode);
	void Amb_Function_0148(H_NODE *pNode);
	void Amb_Function_0149(H_NODE *pNode);
	void Amb_Function_0150(H_NODE *pNode);

	void Amb_Function_0151(H_NODE *pNode);
	void Amb_Function_0152(H_NODE *pNode);
	void Amb_Function_0153(H_NODE *pNode);
	void Amb_Function_0154(H_NODE *pNode);
	void Amb_Function_0155(H_NODE *pNode);
	void Amb_Function_0156(H_NODE *pNode);
	void Amb_Function_0157(H_NODE *pNode);
	void Amb_Function_0158(H_NODE *pNode);
	void Amb_Function_0159(H_NODE *pNode);
	void Amb_Function_0160(H_NODE *pNode);

	void Amb_Function_0161(H_NODE *pNode);
	void Amb_Function_0162(H_NODE *pNode);
	void Amb_Function_0163(H_NODE *pNode);
	void Amb_Function_0164(H_NODE *pNode);
	void Amb_Function_0165(H_NODE *pNode);
	void Amb_Function_0166(H_NODE *pNode);
	void Amb_Function_0167(H_NODE *pNode);
	void Amb_Function_0168(H_NODE *pNode);
	void Amb_Function_0169(H_NODE *pNode);
	void Amb_Function_0170(H_NODE *pNode);

	void Amb_Function_0171(H_NODE *pNode);
	void Amb_Function_0172(H_NODE *pNode);
	void Amb_Function_0173(H_NODE *pNode);
	void Amb_Function_0174(H_NODE *pNode);
	void Amb_Function_0175(H_NODE *pNode);
	void Amb_Function_0176(H_NODE *pNode);
	void Amb_Function_0177(H_NODE *pNode);
	void Amb_Function_0178(H_NODE *pNode);
	void Amb_Function_0179(H_NODE *pNode);
	void Amb_Function_0180(H_NODE *pNode);

	void Amb_Function_0181(H_NODE *pNode);
	void Amb_Function_0182(H_NODE *pNode);
	void Amb_Function_0183(H_NODE *pNode);
	void Amb_Function_0184(H_NODE *pNode);
	void Amb_Function_0185(H_NODE *pNode);
	void Amb_Function_0186(H_NODE *pNode);
	void Amb_Function_0187(H_NODE *pNode);
	void Amb_Function_0188(H_NODE *pNode);
	void Amb_Function_0189(H_NODE *pNode);
	void Amb_Function_0190(H_NODE *pNode);

	void Amb_Function_0191(H_NODE *pNode);
	void Amb_Function_0192(H_NODE *pNode);
	void Amb_Function_0193(H_NODE *pNode);
	void Amb_Function_0194(H_NODE *pNode);
	void Amb_Function_0195(H_NODE *pNode);
	void Amb_Function_0196(H_NODE *pNode);
	void Amb_Function_0197(H_NODE *pNode);
	void Amb_Function_0198(H_NODE *pNode);
	void Amb_Function_0199(H_NODE *pNode);
	void Amb_Function_0200(H_NODE *pNode);


	void Amb_Function_0201(H_NODE *pNode);
	void Amb_Function_0202(H_NODE *pNode);
	void Amb_Function_0203(H_NODE *pNode);
	void Amb_Function_0204(H_NODE *pNode);
	void Amb_Function_0205(H_NODE *pNode);
	void Amb_Function_0206(H_NODE *pNode);
	void Amb_Function_0207(H_NODE *pNode);
	void Amb_Function_0208(H_NODE *pNode);
	void Amb_Function_0209(H_NODE *pNode);
	void Amb_Function_0210(H_NODE *pNode);

	void Amb_Function_0211(H_NODE *pNode);
	void Amb_Function_0212(H_NODE *pNode);
	void Amb_Function_0213(H_NODE *pNode);
	void Amb_Function_0214(H_NODE *pNode);
	void Amb_Function_0215(H_NODE *pNode);
	void Amb_Function_0216(H_NODE *pNode);
	void Amb_Function_0217(H_NODE *pNode);
	void Amb_Function_0218(H_NODE *pNode);
	void Amb_Function_0219(H_NODE *pNode);
	void Amb_Function_0220(H_NODE *pNode);

	void Amb_Function_0221(H_NODE *pNode);
	void Amb_Function_0222(H_NODE *pNode);
	void Amb_Function_0223(H_NODE *pNode);
	void Amb_Function_0224(H_NODE *pNode);
	void Amb_Function_0225(H_NODE *pNode);
	void Amb_Function_0226(H_NODE *pNode);
	void Amb_Function_0227(H_NODE *pNode);
	void Amb_Function_0228(H_NODE *pNode);
	void Amb_Function_0229(H_NODE *pNode);
	void Amb_Function_0230(H_NODE *pNode);

	void Amb_Function_0231(H_NODE *pNode);
	void Amb_Function_0232(H_NODE *pNode);
	void Amb_Function_0233(H_NODE *pNode);
	void Amb_Function_0234(H_NODE *pNode);
	void Amb_Function_0235(H_NODE *pNode);
	void Amb_Function_0236(H_NODE *pNode);
	void Amb_Function_0237(H_NODE *pNode);
	void Amb_Function_0238(H_NODE *pNode);
	void Amb_Function_0239(H_NODE *pNode);
	void Amb_Function_0240(H_NODE *pNode);

	void Amb_Function_0241(H_NODE *pNode);
	void Amb_Function_0242(H_NODE *pNode);
	void Amb_Function_0243(H_NODE *pNode);
	void Amb_Function_0244(H_NODE *pNode);
	void Amb_Function_0245(H_NODE *pNode);
	void Amb_Function_0246(H_NODE *pNode);
	void Amb_Function_0247(H_NODE *pNode);
	void Amb_Function_0248(H_NODE *pNode);
	void Amb_Function_0249(H_NODE *pNode);
	void Amb_Function_0250(H_NODE *pNode);

	void Amb_Function_0251(H_NODE *pNode);
	void Amb_Function_0252(H_NODE *pNode);
	void Amb_Function_0253(H_NODE *pNode);
	void Amb_Function_0254(H_NODE *pNode);
	void Amb_Function_0255(H_NODE *pNode);
	void Amb_Function_0256(H_NODE *pNode);
	void Amb_Function_0257(H_NODE *pNode);
	void Amb_Function_0258(H_NODE *pNode);
	void Amb_Function_0259(H_NODE *pNode);
	void Amb_Function_0260(H_NODE *pNode);

	void Amb_Function_0261(H_NODE *pNode);
	void Amb_Function_0262(H_NODE *pNode);
	void Amb_Function_0263(H_NODE *pNode);
	void Amb_Function_0264(H_NODE *pNode);
	void Amb_Function_0265(H_NODE *pNode);
	void Amb_Function_0266(H_NODE *pNode);
	void Amb_Function_0267(H_NODE *pNode);
	void Amb_Function_0268(H_NODE *pNode);
	void Amb_Function_0269(H_NODE *pNode);
	void Amb_Function_0270(H_NODE *pNode);
	void Amb_Function_0271(H_NODE *pNode);

	void Amb_Function_0272(H_NODE *pNode);
	void Amb_Function_0273(H_NODE *pNode);
	void Amb_Function_0274(H_NODE *pNode);
	void Amb_Function_0275(H_NODE *pNode);
	void Amb_Function_0276(H_NODE *pNode);
	void Amb_Function_0277(H_NODE *pNode);
	void Amb_Function_0278(H_NODE *pNode);
	void Amb_Function_0279(H_NODE *pNode);
	void Amb_Function_0280(H_NODE *pNode);

	void Amb_Function_0281(H_NODE *pNode);
	void Amb_Function_0282(H_NODE *pNode);
	void Amb_Function_0283(H_NODE *pNode);
	void Amb_Function_0284(H_NODE *pNode);
	void Amb_Function_0285(H_NODE *pNode);
	void Amb_Function_0286(H_NODE *pNode);
	void Amb_Function_0287(H_NODE *pNode);
	void Amb_Function_0288(H_NODE *pNode);
	void Amb_Function_0289(H_NODE *pNode);
	void Amb_Function_0290(H_NODE *pNode);

	void Amb_Function_0291(H_NODE *pNode);
	void Amb_Function_0292(H_NODE *pNode);
	void Amb_Function_0293(H_NODE *pNode);
	void Amb_Function_0294(H_NODE *pNode);
	void Amb_Function_0295(H_NODE *pNode);
	void Amb_Function_0296(H_NODE *pNode);
	void Amb_Function_0297(H_NODE *pNode);
	void Amb_Function_0298(H_NODE *pNode);
	void Amb_Function_0299(H_NODE *pNode);
	void Amb_Function_0300(H_NODE *pNode);


	void Amb_Function_0301(H_NODE *pNode);
	void Amb_Function_0302(H_NODE *pNode);
	void Amb_Function_0303(H_NODE *pNode);
	void Amb_Function_0304(H_NODE *pNode);
	void Amb_Function_0305(H_NODE *pNode);
	void Amb_Function_0306(H_NODE *pNode);
	void Amb_Function_0307(H_NODE *pNode);
	void Amb_Function_0308(H_NODE *pNode);
	void Amb_Function_0309(H_NODE *pNode);
	void Amb_Function_0310(H_NODE *pNode);

	void Amb_Function_0311(H_NODE *pNode);
	void Amb_Function_0312(H_NODE *pNode);
	void Amb_Function_0313(H_NODE *pNode);
	void Amb_Function_0314(H_NODE *pNode);
	void Amb_Function_0315(H_NODE *pNode);
	void Amb_Function_0316(H_NODE *pNode);
	void Amb_Function_0317(H_NODE *pNode);
	void Amb_Function_0318(H_NODE *pNode);
	void Amb_Function_0319(H_NODE *pNode);
	void Amb_Function_0320(H_NODE *pNode);

	void Amb_Function_0321(H_NODE *pNode);
	void Amb_Function_0322(H_NODE *pNode);
	void Amb_Function_0323(H_NODE *pNode);
	void Amb_Function_0324(H_NODE *pNode);
	void Amb_Function_0325(H_NODE *pNode);
	void Amb_Function_0326(H_NODE *pNode);
	void Amb_Function_0327(H_NODE *pNode);
	void Amb_Function_0328(H_NODE *pNode);
	void Amb_Function_0329(H_NODE *pNode);
	void Amb_Function_0330(H_NODE *pNode);

	void Amb_Function_0331(H_NODE *pNode);
	void Amb_Function_0332(H_NODE *pNode);
	void Amb_Function_0333(H_NODE *pNode);
	void Amb_Function_0334(H_NODE *pNode);
	void Amb_Function_0335(H_NODE *pNode);
	void Amb_Function_0336(H_NODE *pNode);
	void Amb_Function_0337(H_NODE *pNode);
	void Amb_Function_0338(H_NODE *pNode);
	void Amb_Function_0339(H_NODE *pNode);
	void Amb_Function_0340(H_NODE *pNode);

	void Amb_Function_0341(H_NODE *pNode);
	void Amb_Function_0342(H_NODE *pNode);
	void Amb_Function_0343(H_NODE *pNode);
	void Amb_Function_0344(H_NODE *pNode);
	void Amb_Function_0345(H_NODE *pNode);
	void Amb_Function_0346(H_NODE *pNode);
	void Amb_Function_0347(H_NODE *pNode);
	void Amb_Function_0348(H_NODE *pNode);
	void Amb_Function_0349(H_NODE *pNode);
	void Amb_Function_0350(H_NODE *pNode);


	void Amb_Function_0351(H_NODE *pNode);
	void Amb_Function_0352(H_NODE *pNode);
	void Amb_Function_0353(H_NODE *pNode);
	void Amb_Function_0354(H_NODE *pNode);
	void Amb_Function_0355(H_NODE *pNode);
	void Amb_Function_0356(H_NODE *pNode);
	void Amb_Function_0357(H_NODE *pNode);
	void Amb_Function_0358(H_NODE *pNode);
	void Amb_Function_0359(H_NODE *pNode);
	void Amb_Function_0360(H_NODE *pNode);

	void Amb_Function_0361(H_NODE *pNode);
	void Amb_Function_0362(H_NODE *pNode);
	void Amb_Function_0363(H_NODE *pNode);
	void Amb_Function_0364(H_NODE *pNode);
	void Amb_Function_0365(H_NODE *pNode);
	void Amb_Function_0366(H_NODE *pNode);
	void Amb_Function_0367(H_NODE *pNode);
	void Amb_Function_0368(H_NODE *pNode);
	void Amb_Function_0369(H_NODE *pNode);
	void Amb_Function_0370(H_NODE *pNode);

	void Amb_Function_0371(H_NODE *pNode);
	void Amb_Function_0372(H_NODE *pNode);
	void Amb_Function_0373(H_NODE *pNode);
	void Amb_Function_0374(H_NODE *pNode);
	void Amb_Function_0375(H_NODE *pNode);
	void Amb_Function_0376(H_NODE *pNode);
	void Amb_Function_0377(H_NODE *pNode);
	void Amb_Function_0378(H_NODE *pNode);
	void Amb_Function_0379(H_NODE *pNode);
	void Amb_Function_0380(H_NODE *pNode);

	void Amb_Function_0381(H_NODE *pNode);
	void Amb_Function_0382(H_NODE *pNode);
	void Amb_Function_0383(H_NODE *pNode);
	void Amb_Function_0384(H_NODE *pNode);
	void Amb_Function_0385(H_NODE *pNode);
	void Amb_Function_0386(H_NODE *pNode);
	void Amb_Function_0387(H_NODE *pNode);
	void Amb_Function_0388(H_NODE *pNode);
	void Amb_Function_0389(H_NODE *pNode);
	void Amb_Function_0390(H_NODE *pNode);

	void Amb_Function_0391(H_NODE *pNode);
	void Amb_Function_0392(H_NODE *pNode);
	void Amb_Function_0393(H_NODE *pNode);
	void Amb_Function_0394(H_NODE *pNode);
	void Amb_Function_0395(H_NODE *pNode);
	void Amb_Function_0396(H_NODE *pNode);
	void Amb_Function_0397(H_NODE *pNode);
	void Amb_Function_0398(H_NODE *pNode);
	void Amb_Function_0399(H_NODE *pNode);
	void Amb_Function_0400(H_NODE *pNode);


	void Amb_Function_0401(H_NODE *pNode);
	void Amb_Function_0402(H_NODE *pNode);
	void Amb_Function_0403(H_NODE *pNode);
	void Amb_Function_0404(H_NODE *pNode);
	void Amb_Function_0405(H_NODE *pNode);
	void Amb_Function_0406(H_NODE *pNode);
	void Amb_Function_0407(H_NODE *pNode);
	void Amb_Function_0408(H_NODE *pNode);
	void Amb_Function_0409(H_NODE *pNode);
	void Amb_Function_0410(H_NODE *pNode);

	void Amb_Function_0411(H_NODE *pNode);
	void Amb_Function_0412(H_NODE *pNode);
	void Amb_Function_0413(H_NODE *pNode);
	void Amb_Function_0414(H_NODE *pNode);
	void Amb_Function_0415(H_NODE *pNode);
	void Amb_Function_0416(H_NODE *pNode);
	void Amb_Function_0417(H_NODE *pNode);
	void Amb_Function_0418(H_NODE *pNode);
	void Amb_Function_0419(H_NODE *pNode);
	void Amb_Function_0420(H_NODE *pNode);

	void Amb_Function_0421(H_NODE *pNode);
	void Amb_Function_0422(H_NODE *pNode);
	void Amb_Function_0423(H_NODE *pNode);
	void Amb_Function_0424(H_NODE *pNode);
	void Amb_Function_0425(H_NODE *pNode);
	void Amb_Function_0426(H_NODE *pNode);
	void Amb_Function_0427(H_NODE *pNode);
	void Amb_Function_0428(H_NODE *pNode);
	void Amb_Function_0429(H_NODE *pNode);
	void Amb_Function_0430(H_NODE *pNode);

	void Amb_Function_0431(H_NODE *pNode);
	void Amb_Function_0432(H_NODE *pNode);
	void Amb_Function_0433(H_NODE *pNode);
	void Amb_Function_0434(H_NODE *pNode);
	void Amb_Function_0435(H_NODE *pNode);
	void Amb_Function_0436(H_NODE *pNode);
	void Amb_Function_0437(H_NODE *pNode);
	void Amb_Function_0438(H_NODE *pNode);

	void Amb_Function_0439(H_NODE *pNode);
	void Amb_Function_0440(H_NODE *pNode);

	void Amb_Function_0441(H_NODE *pNode);
	void Amb_Function_0442(H_NODE *pNode);
	void Amb_Function_0443(H_NODE *pNode);
	void Amb_Function_0444(H_NODE *pNode);
	void Amb_Function_0445(H_NODE *pNode);
	void Amb_Function_0446(H_NODE *pNode);
	void Amb_Function_0447(H_NODE *pNode);
	void Amb_Function_0448(H_NODE *pNode);
	void Amb_Function_0449(H_NODE *pNode);
	void Amb_Function_0450(H_NODE *pNode);

	void Amb_Function_0451(H_NODE *pNode);
	void Amb_Function_0452(H_NODE *pNode);
	void Amb_Function_0453(H_NODE *pNode);
	void Amb_Function_0454(H_NODE *pNode);
	void Amb_Function_0455(H_NODE *pNode);
	void Amb_Function_0456(H_NODE *pNode);
	void Amb_Function_0457(H_NODE *pNode);
	void Amb_Function_0458(H_NODE *pNode);
	void Amb_Function_0459(H_NODE *pNode);
	void Amb_Function_0460(H_NODE *pNode);

	void Amb_Function_0461(H_NODE *pNode);
	void Amb_Function_0462(H_NODE *pNode);
	void Amb_Function_0463(H_NODE *pNode);
	void Amb_Function_0464(H_NODE *pNode);
	void Amb_Function_0465(H_NODE *pNode);
	void Amb_Function_0466(H_NODE *pNode);
	void Amb_Function_0467(H_NODE *pNode);
	void Amb_Function_0468(H_NODE *pNode);
	void Amb_Function_0469(H_NODE *pNode);
	void Amb_Function_0470(H_NODE *pNode);

	void Amb_Function_0471(H_NODE *pNode);
	void Amb_Function_0472(H_NODE *pNode);
	void Amb_Function_0473(H_NODE *pNode);
	void Amb_Function_0474(H_NODE *pNode);
	void Amb_Function_0475(H_NODE *pNode);
	void Amb_Function_0476(H_NODE *pNode);
	void Amb_Function_0477(H_NODE *pNode);
	void Amb_Function_0478(H_NODE *pNode);
	void Amb_Function_0479(H_NODE *pNode);
	void Amb_Function_0480(H_NODE *pNode);

	void Amb_Function_0481(H_NODE *pNode);
	void Amb_Function_0482(H_NODE *pNode);
	void Amb_Function_0483(H_NODE *pNode);
	void Amb_Function_0484(H_NODE *pNode);
	void Amb_Function_0485(H_NODE *pNode);
	void Amb_Function_0486(H_NODE *pNode);
	void Amb_Function_0487(H_NODE *pNode);
	void Amb_Function_0488(H_NODE *pNode);
	void Amb_Function_0489(H_NODE *pNode);
	void Amb_Function_0490(H_NODE *pNode);

	void Amb_Function_0491(H_NODE *pNode);
	void Amb_Function_0492(H_NODE *pNode);
	void Amb_Function_0493(H_NODE *pNode);
	void Amb_Function_0494(H_NODE *pNode);
	void Amb_Function_0495(H_NODE *pNode);
	void Amb_Function_0496(H_NODE *pNode);
	void Amb_Function_0497(H_NODE *pNode);
	void Amb_Function_0498(H_NODE *pNode);
	void Amb_Function_0499(H_NODE *pNode);
	void Amb_Function_0500(H_NODE *pNode);

	void Amb_Function_0501(H_NODE *pNode);
	void Amb_Function_0502(H_NODE *pNode);
	void Amb_Function_0503(H_NODE *pNode);
	void Amb_Function_0504(H_NODE *pNode);
	void Amb_Function_0505(H_NODE *pNode);
	void Amb_Function_0506(H_NODE *pNode);
	void Amb_Function_0507(H_NODE *pNode);
	void Amb_Function_0508(H_NODE *pNode);
	void Amb_Function_0509(H_NODE *pNode);
	void Amb_Function_0510(H_NODE *pNode);

	void Amb_Function_0511(H_NODE *pNode);
	void Amb_Function_0512(H_NODE *pNode);
	void Amb_Function_0513(H_NODE *pNode);
	void Amb_Function_0514(H_NODE *pNode);
	void Amb_Function_0515(H_NODE *pNode);
	void Amb_Function_0516(H_NODE *pNode);
	void Amb_Function_0517(H_NODE *pNode);
	void Amb_Function_0518(H_NODE *pNode);
	void Amb_Function_0519(H_NODE *pNode);
	void Amb_Function_0520(H_NODE *pNode);

	void Amb_Function_0521(H_NODE *pNode);
	void Amb_Function_0522(H_NODE *pNode);
	void Amb_Function_0523(H_NODE *pNode);
	void Amb_Function_0524(H_NODE *pNode);
	void Amb_Function_0525(H_NODE *pNode);
	void Amb_Function_0526(H_NODE *pNode);
	void Amb_Function_0527(H_NODE *pNode);
	void Amb_Function_0528(H_NODE *pNode);
	void Amb_Function_0529(H_NODE *pNode);
	void Amb_Function_0530(H_NODE *pNode);

	void Amb_Function_0531(H_NODE *pNode);
	void Amb_Function_0532(H_NODE *pNode);
	void Amb_Function_0533(H_NODE *pNode);
	void Amb_Function_0534(H_NODE *pNode);
	void Amb_Function_0535(H_NODE *pNode);
	void Amb_Function_0536(H_NODE *pNode);
	void Amb_Function_0537(H_NODE *pNode);
	void Amb_Function_0538(H_NODE *pNode);
	void Amb_Function_0539(H_NODE *pNode);
	void Amb_Function_0540(H_NODE *pNode);

	void Amb_Function_0541(H_NODE *pNode);
	void Amb_Function_0542(H_NODE *pNode);
	void Amb_Function_0543(H_NODE *pNode);
	void Amb_Function_0544(H_NODE *pNode);
	void Amb_Function_0545(H_NODE *pNode);
	void Amb_Function_0546(H_NODE *pNode);
	void Amb_Function_0547(H_NODE *pNode);
	void Amb_Function_0548(H_NODE *pNode);
	void Amb_Function_0549(H_NODE *pNode);
	void Amb_Function_0550(H_NODE *pNode);

	void Amb_Function_0551(H_NODE *pNode);
	void Amb_Function_0552(H_NODE *pNode);
	void Amb_Function_0553(H_NODE *pNode);
	void Amb_Function_0554(H_NODE *pNode);
	void Amb_Function_0555(H_NODE *pNode);
	void Amb_Function_0556(H_NODE *pNode);
	void Amb_Function_0557(H_NODE *pNode);
	void Amb_Function_0558(H_NODE *pNode);
	void Amb_Function_0559(H_NODE *pNode);
	void Amb_Function_0560(H_NODE *pNode);

	void Amb_Function_0561(H_NODE *pNode);
	void Amb_Function_0562(H_NODE *pNode);
	void Amb_Function_0563(H_NODE *pNode);
	void Amb_Function_0564(H_NODE *pNode);
	void Amb_Function_0565(H_NODE *pNode);
	void Amb_Function_0566(H_NODE *pNode);
	void Amb_Function_0567(H_NODE *pNode);
	void Amb_Function_0568(H_NODE *pNode);
	void Amb_Function_0569(H_NODE *pNode);
	void Amb_Function_0570(H_NODE *pNode);

	void Amb_Function_0571(H_NODE *pNode);
	void Amb_Function_0572(H_NODE *pNode);
	void Amb_Function_0573(H_NODE *pNode);
	void Amb_Function_0574(H_NODE *pNode);
	void Amb_Function_0575(H_NODE *pNode);
	void Amb_Function_0576(H_NODE *pNode);
	void Amb_Function_0577(H_NODE *pNode);
	void Amb_Function_0578(H_NODE *pNode);
	void Amb_Function_0579(H_NODE *pNode);
	void Amb_Function_0580(H_NODE *pNode);

	void Amb_Function_0581(H_NODE *pNode);
	void Amb_Function_0582(H_NODE *pNode);
	void Amb_Function_0583(H_NODE *pNode);
	void Amb_Function_0584(H_NODE *pNode);
	void Amb_Function_0585(H_NODE *pNode);
	void Amb_Function_0586(H_NODE *pNode);
	void Amb_Function_0587(H_NODE *pNode);
	void Amb_Function_0588(H_NODE *pNode);
	void Amb_Function_0589(H_NODE *pNode);
	void Amb_Function_0590(H_NODE *pNode);

	void Amb_Function_0591(H_NODE *pNode);
	void Amb_Function_0592(H_NODE *pNode);
	void Amb_Function_0593(H_NODE *pNode);
	void Amb_Function_0594(H_NODE *pNode);
	void Amb_Function_0595(H_NODE *pNode);
	void Amb_Function_0596(H_NODE *pNode);
	void Amb_Function_0597(H_NODE *pNode);
	void Amb_Function_0598(H_NODE *pNode);
	void Amb_Function_0599(H_NODE *pNode);
	void Amb_Function_0600(H_NODE *pNode);

	void Amb_Function_0601(H_NODE *pNode);
	void Amb_Function_0602(H_NODE *pNode);
	void Amb_Function_0603(H_NODE *pNode);
	void Amb_Function_0604(H_NODE *pNode);
	void Amb_Function_0605(H_NODE *pNode);
	void Amb_Function_0606(H_NODE *pNode);
	void Amb_Function_0607(H_NODE *pNode);
	void Amb_Function_0608(H_NODE *pNode);
	void Amb_Function_0609(H_NODE *pNode);
	void Amb_Function_0610(H_NODE *pNode);
	void Amb_Function_0611(H_NODE *pNode);
	void Amb_Function_0612(H_NODE *pNode);
	void Amb_Function_0613(H_NODE *pNode);
	void Amb_Function_0614(H_NODE *pNode);
	void Amb_Function_0615(H_NODE *pNode);
	void Amb_Function_0616(H_NODE *pNode);
	void Amb_Function_0617(H_NODE *pNode);
	void Amb_Function_0618(H_NODE *pNode);
	void Amb_Function_0619(H_NODE *pNode);
	void Amb_Function_0620(H_NODE *pNode);
	void Amb_Function_0621(H_NODE *pNode);
	void Amb_Function_0622(H_NODE *pNode);
	void Amb_Function_0623(H_NODE *pNode);
	void Amb_Function_0624(H_NODE *pNode);
	void Amb_Function_0625(H_NODE *pNode);
	void Amb_Function_0626(H_NODE *pNode);
	void Amb_Function_0627(H_NODE *pNode);
};

