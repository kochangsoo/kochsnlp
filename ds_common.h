#pragma once
//
//ds_common.h
//

#ifndef _DS_COMMON_H__
#define _DS_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define	CLAUSE_CH_MATRIX		0x10000000 // 모절
#define CLAUSE_CH_QUOTE			0x20000000 // 인용절

#define CLAUSE_CH_ANDP			0x00100000 // 순접절
#define CLAUSE_CH_ORP			0x00200000 // 이접절
#define CLAUSE_CH_BUTP			0x00400000 // 역접절

#define CLAUSE_CH_UNC			0x00010000 // 관계 관형절
#define CLAUSE_CH_RUNC			0x00020000 // 동격 관형절

#define CLAUSE_CH_NOM			0x00001000 // 명사절
#define CLAUSE_CH_ConditionP	0x00002000 // 조건절
#define CLAUSE_CH_ReasonP		0x00004000 // 이유절
#define CLAUSE_CH_ETCP			0x00008000 // 기타절
#define CLAUSE_CH_TopicAdv		0x00000100 	// 주제절
#define CLAUSE_CH_PurposeAdv	0x00008200 // 목적절

	int isSetCC(int target, int ch);
	void SetCC(int* target, int ch);
	void UnsetCC(int* target, int ch);

	enum Stem_Value {
		S_Proto = 0, S_Amb, S_Lex, S_Aff, S_N, S_Pro, S_Num, S_V, S_Act, S_Sta,
		S_Adv, S_IC, S_Numb, S_Unc, S_Prp, S_Abt, S_Conc, S_Ani, S_Hum, S_Aml,
		S_Hon, S_Pred, S_Whq, S_Per, S_Ref, S_Unt, S_Con, S_Neg, S_Cau, S_Pss,
		S_Mov, S_Awd, S_Psy, S_Hop, S_Cog, S_Fnc, S_Spe, S_Sen, S_Rel, S_Cmp,
		S_Scmp, S_Cho, S_Sit, S_Dur, S_Prt, S_Bod, S_Nat, S_Atf, S_Veh, S_Fod,
		S_Cur, S_Plc, S_Lnd, S_Aplc, S_Org, S_Rlg, S_Atr, S_Spc, S_Tm, S_Ntu,
		S_App, S_Cls, S_Fom, S_Qnt, S_Lir, S_Pir, S_Rir, S_Tir, S_Sir, S_Uir,
		S_Ruir, S_Hir, S_Ambid, S_MnCat,

		S_T001, S_T002, S_T003, S_T004, S_T005, S_T006, S_T007, S_T008, S_T009, S_T010,
		S_T011, S_T012, S_T013, S_T014, S_T015, S_T016, S_T017, S_T018, S_T019, S_T020,
		S_T021, S_T022, S_T023, S_T024, S_T025, S_T026, S_T027, S_T028, S_T029, S_T030,
		S_T031, S_T032, S_T033, S_T034, S_T035, S_T036, S_T037, S_T038, S_T039, S_T040,
		S_T041, S_T042, S_T043, S_T044, S_T045, S_T046, S_T047, S_T048, S_T049, S_T050,
		S_T051, S_T052, S_T053, S_T054, S_T055, S_T056, S_T057, S_T058, S_T059, S_T060,
		S_T061, S_T062, S_T063, S_T064, S_T065, S_T066, S_T067, S_T068, S_T069, S_T070,
		S_T071, S_T072, S_T073, S_T074, S_T075, S_T076, S_T077, S_T078, S_T079, S_T080,
		S_T081, S_T082, S_T083, S_T084, S_T085, S_T086, S_T087, S_T088, S_T089, S_T090,
		S_T091, S_T092, S_T093, S_T094, S_T095, S_T096, S_T097, S_T098, S_T099, S_T100,
		S_T101, S_T102, S_T103, S_T104, S_T105, S_T106, S_T107, S_T108, S_T109, S_T110,
		S_T111, S_T112, S_T113, S_T114, S_T115, S_T116, S_T117, S_T118, S_T119, S_T120,
		S_T121, S_T122, S_T123, S_T124, S_T125, S_T126, S_T127, S_T128, S_T129, S_T130,
		S_T131, S_T132, S_T133, S_T134, S_T135, S_T136, S_T137, S_T138, S_T139, S_T140,
		S_T141, S_T142, S_T143, S_T144, S_T145, S_T146, S_T147, S_T148, S_T149, S_T150,
		S_T151, S_T152, S_T153, S_T154, S_T155, S_T156, S_T157, S_T158, S_T159, S_T160
	};

	enum Ending_Value {
		E_Amb = 0, E_Aff, E_N, E_V, E_Agt, E_Absl, E_Thm, E_Gen, E_Cor, E_Plc,
		E_Src, E_Des, E_Inst, E_Cap, E_Cpn, E_Dur, E_Pst, E_Prs, E_Fut, E_Dcl,
		E_Whq, E_Impr, E_Ppo, E_Excl, E_Cau, E_Pss, E_Pol, E_Neg, E_Con, E_Ord,
		E_CCn, E_Cvr, E_Cnd, E_Itnd, E_Res, E_Thou, E_Rsu, E_Selc, E_Quo, E_Ppr,
		E_Lik, E_Fin, E_Unc, E_Giv, E_Re, E_Hon
	};

	static char* Stem_Field_Name[] = {
		"Proto" ,"Amb"  ,"Lex"   ,"Aff"  ,"N"   ,"Pro"  ,"Num"  ,"V"   ,"Act"  ,"Sta",
		"Adv"   ,"IC"   ,"Numb"  ,"UnC"  ,"Prp" ,"Abt"  ,"Conc" ,"Ani" ,"Hum"  ,"Aml",
		"Hon"   ,"Pred" ,"Whq"   ,"Per"  ,"Ref" ,"Unt"  ,"Con"  ,"Neg" ,"Cau"  ,"Pss",
		"Mov"   ,"Awd"  ,"Psy"   ,"Hop"  ,"Cog" ,"Fnc"  ,"Spe"  ,"Sen" ,"Rel"  ,"Cmp",
		"Scmp"  ,"Cho"  ,"Sit"   ,"Dur"  ,"Prt" ,"Bod"  ,"Nat"  ,"Atf" ,"Veh"  ,"Fod",
		"Cur"   ,"Plc"  ,"Lnd"   ,"Aplc" ,"Org" ,"Rlg"  ,"Atr"  ,"Spc" ,"Tm"   ,"Ntu",
		"App"   ,"Cls"  ,"Fom"   ,"Qnt"  ,"Lir" ,"Pir"  ,"Rir"  ,"Tir" ,"Sir"  ,"Uir",
		"Ruir"  ,"Hir"  ,"Ambid" ,"MnCat",

		"T001", "T002", "T003", "T004", "T005", "T006", "T007", "T008", "T009", "T010",
		"T011", "T012", "T013", "T014", "T015", "T016", "T017", "T018", "T019", "T020",
		"T021", "T022", "T023", "T024", "T025", "T026", "T027", "T028", "T029", "T030",
		"T031", "T032", "T033", "T034", "T035", "T036", "T037", "T038", "T039", "T040",
		"T041", "T042", "T043", "T044", "T045", "T046", "T047", "T048", "T049", "T050",
		"T051", "T052", "T053", "T054", "T055", "T056", "T057", "T058", "T059", "T060",
		"T061", "T062", "T063", "T064", "T065", "T066", "T067", "T068", "T069", "T070",
		"T071", "T072", "T073", "T074", "T075", "T076", "T077", "T078", "T079", "T080",
		"T081", "T082", "T083", "T084", "T085", "T086", "T087", "T088", "T089", "T090",
		"T091", "T092", "T093", "T094", "T095", "T096", "T097", "T098", "T099", "T100",
		"T101", "T102", "T103", "T104", "T105", "T106", "T107", "T108", "T109", "T110",
		"T111", "T112", "T113", "T114", "T115", "T116", "T117", "T118", "T119", "T120",
		"T121", "T122", "T123", "T124", "T125", "T126", "T127", "T128", "T129", "T130",
		"T131", "T132", "T133", "T134", "T135", "T136", "T137", "T138", "T139", "T140",
		"T141", "T142", "T143", "T144", "T145", "T146", "T147", "T148", "T149", "T150",
		"T151", "T152", "T153", "T154", "T155", "T156", "T157", "T158", "T159", "T160"

	};
	/* 총74개 + 160 -> 234 */

	static char* End_Field_Name[] = {
		"Amb"  ,"Aff"  ,"N"    ,"V"    ,"Agt"  ,"Absl"  ,"Thm"   ,"Gen"  ,"Cor"  ,"Plc",
		"Src"  ,"Des"  ,"Inst" ,"Cap"  ,"Cpn"  ,"Dur"   ,"Pst"   ,"Prs"  ,"Fut"  ,"Dcl",
		"Whq"  ,"Impr" ,"Ppo"  ,"Excl" ,"Cau"  ,"Pss"   ,"Pol"   ,"Neg"  ,"Con"  ,"Ord",
		"CCn"  ,"Cvr"  ,"Cnd"  ,"Itnd" ,"Res"  ,"Thou"  ,"Rsu"   ,"Selc"  ,"Quo" ,"Ppr",
		"Lik"  ,"Fin"  ,"UnC"  ,"Giv"  ,"Re"   ,"Hon"
	};

	/* 총47개 */
#define STEM_FEATURE_COUNT 234
#define END_FEATURE_COUNT 47

	/* 형태소 nKind */
#define KOREAN_CHAR 0
#define CHINESE_CHAR 1
#define JAPAN_CHAR 2
#define ENGLISH_CHAR 4
#define NUMBER_CHAR 5
#define ECT_CHAR 6


#define SYNLIST_COUNT 54
#define MAX_PROPOSITION_INDEX	SYNLIST_COUNT

	enum _pCategory {
		conj,					// 0
		voc,					// 1
		padv,					// 2
		subj,					// 3
		obj,					// 4
		comp,					// 5
		ladv,					// 6
		tadv,					// 7
		madv,					// 8
		etcadv,				// 9
		pred,					// 10
		aux,					// 11
		hyperi,				// 12
		topicadv,			// 13 Added by KKH 2003-11-03
		purposeadv,			// 14 Added by KKH 2003-11-03
		amountadv,			// 15 Added by KKH 2003-11-05
		instrument,			// 16 Added by KKH 2003-11-05
		dative,				// 17 Added by KKH 2003-11-05
		comitative,			// 18 Added by KKH 2003-11-05
		theme,				// 19 Added by KKH 2003-11-19
		agent,				// 20 Added by KKH 2003-11-19

		Ns,					// 21
		Np,					// 22
		Ne,					// 23
		Ss,					// 24
		Se,					// 25
		Pe,					// 26
		Ps,					// 27
		Aux,					// 28
		Qor,					// 29
		S,						// 30
		Hn,					// 31
		Rc,					// 32
		Head,					// 33
		Mod,					// 34
		I,						// 35
		Qnot,					// 36
		G,						// 37
		Finished,			// 38
		SFinished,			// 39
		DES,					// 40
		BothMask,			// 41
		OldRecover,			// 42
		PNG,					// 43 Added by KKH 2003-11-05
		Topic,				// 44 Added by KKH 2003-11-03
		Purpose,				// 45 Added by KKH 2003-11-03
		AdvP,					// 46 Added by KKH 2003-11-03
		GPC,					// 47 Added by KKH 2003-11-05
		RUnc,					// 48 Added by KKH 2003-11-12
		Unc,					// 49 Added by KKH 2003-11-12
		DFin,					// 50 Added by KKH 2003-11-17
		QFin,					// 51 Added by KKH 2003-11-20
		Qdel					// 52 Added by KKH 2003-11-24
	};

	static char* pCategStr[] = {
		"conj",				// 0
		"voc",				// 1
		"padv",				// 2
		"subj",				// 3
		"obj",				// 4
		"comp",				// 5
		"ladv",				// 6
		"tadv",				// 7
		"madv",				// 8
		"etcadv",			// 9
		"pred",				// 10
		"aux",				// 11
		"hyperi",			// 12
		"topicadv",			// 13 Added by KKH 2003-11-03
		"purposeadv",		// 14 Added by KKH 2003-11-03
		"amountadv",		// 15 Added by KKH 2003-11-05
		"instrument",		// 16 Added by KKH 2003-11-05
		"dative",			// 17 Added by KKH 2003-11-05
		"comitative",		// 18 Added by KKH 2003-11-05
		"theme",				// 19 Added by KKH 2003-11-19
		"agent",				// 20 Added by KKH 2003-11-19

		"Ns",					// 21
		"Np",					// 22
		"Ne",					// 23
		"Ss",					// 24
		"Se",					// 25
		"Pe",
		"Ps",
		"Aux",
		"Qor",
		"S",
		"Hn",
		"Rc",
		"Head",
		"Mod",
		"I",
		"Qnot",
		"G",
		"Finished",
		"SFinished",
		"DES",
		"BothMaks",
		"OldRecover",
		"PNG",			// Added by KKH 2003-11-05
		"Topic",			// Added by KKH 2003-11-03
		"Purpose",		// Added by KKH 2003-11-03
		"AdvP",			// Added by KKH 2003-11-03
		"GPC",			// Added by KKH 2003-11-05
		"RUnc",			// Added by KKH 2003-11-12
		"Unc",			// Added by KKH 2003-11-12
		"DFin",			// Added by KKH 2003-11-17 - 이 자질은 문장 분할시 이미 검사한 어절에 부여하여 다시 똑같은 조건으로 검사하지 않게 하기 위한 자질이다.
		"QFin",			// Added by KKH 2003-11-20 - 이 자질은 문장 분할시 이미 검사한 인용문에 대해서 다시 똑같은 조건으로 검사하지 않게 하기 위한 자질이다.
		"Qdel"			// Added by KKH 2003-11-24 - 숙어 처리를 위해 신설한 자질 ( Rule ID 59 )
	};

	typedef enum _pCategory pCategory;

	enum _mField {
		Hon,
		Prs,
		Pst,
		Fut,
		Ig,
		Pp,
		Neg,
		Dcl,
		Whq,
		Yq,
		Impr,
		Ppo,
		Excl,
		Re,
		PnG,
		Reporting
	};
	typedef enum _mField mField;


#define MAX_MODALITY_INDEX Reporting + 1

#ifdef __cplusplus
}
#endif

#endif
