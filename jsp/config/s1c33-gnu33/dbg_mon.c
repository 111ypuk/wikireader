/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
 * 
 *  ��L���쌠�҂́C�ȉ��� (1)�`(4) �̏������CFree Software Foundation 
 *  �ɂ���Č��\����Ă��� GNU General Public License �� Version 2 �ɋL
 *  �q����Ă�������𖞂����ꍇ�Ɍ���C�{�\�t�g�E�F�A�i�{�\�t�g�E�F�A
 *  �����ς������̂��܂ށD�ȉ������j���g�p�E�����E���ρE�Ĕz�z�i�ȉ��C
 *  ���p�ƌĂԁj���邱�Ƃ𖳏��ŋ�������D
 *  (1) �{�\�t�g�E�F�A���\�[�X�R�[�h�̌`�ŗ��p����ꍇ�ɂ́C��L�̒���
 *      ���\���C���̗��p��������щ��L�̖��ۏ؋K�肪�C���̂܂܂̌`�Ń\�[
 *      �X�R�[�h���Ɋ܂܂�Ă��邱�ƁD
 *  (2) �{�\�t�g�E�F�A���C���C�u�����`���ȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł���`�ōĔz�z����ꍇ�ɂ́C�Ĕz�z�ɔ����h�L�������g�i���p
 *      �҃}�j���A���Ȃǁj�ɁC��L�̒��쌠�\���C���̗��p��������щ��L
 *      �̖��ۏ؋K����f�ڂ��邱�ƁD
 *  (3) �{�\�t�g�E�F�A���C�@��ɑg�ݍ��ނȂǁC���̃\�t�g�E�F�A�J���Ɏg
 *      �p�ł��Ȃ��`�ōĔz�z����ꍇ�ɂ́C���̂����ꂩ�̏����𖞂�����
 *      �ƁD
 *    (a) �Ĕz�z�ɔ����h�L�������g�i���p�҃}�j���A���Ȃǁj�ɁC��L�̒�
 *        �쌠�\���C���̗��p��������щ��L�̖��ۏ؋K����f�ڂ��邱�ƁD
 *    (b) �Ĕz�z�̌`�Ԃ��C�ʂɒ�߂���@�ɂ���āCTOPPERS�v���W�F�N�g��
 *        �񍐂��邱�ƁD
 *  (4) �{�\�t�g�E�F�A�̗��p�ɂ�蒼�ړI�܂��͊ԐړI�ɐ����邢���Ȃ鑹
 *      �Q������C��L���쌠�҂����TOPPERS�v���W�F�N�g��Ɛӂ��邱�ƁD
 * 
 *  �{�\�t�g�E�F�A�́C���ۏ؂Œ񋟂���Ă�����̂ł���D��L���쌠�҂�
 *  ���TOPPERS�v���W�F�N�g�́C�{�\�t�g�E�F�A�Ɋւ��āC���̓K�p�\����
 *  �܂߂āC�����Ȃ�ۏ؂��s��Ȃ��D�܂��C�{�\�t�g�E�F�A�̗��p�ɂ�蒼
 *  �ړI�܂��͊ԐړI�ɐ����������Ȃ鑹�Q�Ɋւ��Ă��C���̐ӔC�𕉂�Ȃ��D
 * 
 */

#include "stdio.h"
#include "string.h"

#include "jsp_kernel.h"
#include "sil.h"

#include "task.h"
#include "cyclic.h"
#include "mailbox.h"
#include "semaphore.h"
#include "wait.h"
#include "eventflag.h"
#include "dataqueue.h"
#include "mempfix.h"
#include "interrupt.h"
#include "exception.h"

#include "dbg_mon.h"
#include "kernel_objs.h"

/*
 *  �e�[�u������
 *  Generates tables
 */

const	TPS_OSIDENT_INFO	_kernel_OSIdent =
	{"TOPPERS/JSP\0\0\0\0\0\0\0\0",
	 "1.4.2\0\0\0\0"};

const	TPS_OBJINFO_TABLE	_kernel_tps_ObjInfoTable[22] = {
	/* ��{�T�C�Y			������		�z�u�A�h���X	*/
	/* Unit size			Number		Address		*/
	{sizeof(tps_IntNestCnt),	1,		(void *)&tps_IntNestCnt},
	{sizeof(enadsp),		1,		(void *)&enadsp},
	{sizeof(runtsk),		1,		(void *)&runtsk},
	{sizeof(schedtsk),		1,		(void *)&schedtsk},
	{sizeof(current_time),		1,		(void *)&current_time},
	{sizeof(ready_queue[0]),	TNUM_TPRI,	(void *)ready_queue},
	{sizeof(TCB),			TNUM_TSKID,	(void *)tcb_table},
	{sizeof(TINIB),			TNUM_TSKID,	(void *)tinib_table},
	{sizeof(CYCCB),			TNUM_CYCID,	(void *)cyccb_table},
	{sizeof(CYCINIB),		TNUM_CYCID,	(void *)cycinib_table},
	{sizeof(INHINIB),		TNUM_INHNO,	(void *)inhinib_table},
	{sizeof(EXCINIB),		TNUM_EXCNO,	(void *)excinib_table},
	{sizeof(MBXCB),			TNUM_MBXID,	(void *)mbxcb_table},
	{sizeof(MBXINIB),		TNUM_MBXID,	(void *)mbxinib_table},
	{sizeof(SEMCB),			TNUM_SEMID,	(void *)semcb_table},
	{sizeof(SEMINIB),		TNUM_SEMID,	(void *)seminib_table},
	{sizeof(FLGCB),			TNUM_FLGID,	(void *)flgcb_table},
	{sizeof(FLGINIB),		TNUM_FLGID,	(void *)flginib_table},
	{sizeof(DTQCB),			TNUM_DTQID,	(void *)dtqcb_table},
	{sizeof(DTQINIB),		TNUM_DTQID,	(void *)dtqinib_table},
	{sizeof(MPFCB),			TNUM_MPFID,	(void *)mpfcb_table},
	{sizeof(MPFINIB),		TNUM_MPFID,	(void *)mpfinib_table}
};

const	unsigned short		_kernel_tps_PropertyTable[6] = {
	TMIN_TPRI,						/* �ŏ��D��x			*/
								/* Minimum priority		*/
	TMAX_TPRI,						/* �ő�D��x			*/
								/* Maximum priority		*/
	TIC_NUME,						/* �^�C���e�B�b�N����1		*/
								/* Time tick cycle 1		*/
	TIC_DENO,						/* �^�C���e�B�b�N����2		*/
								/* Time tick cycle 2		*/
	1,							/* �V�X�e�������X�V����		*/
								/* The cycle renewing system clock */
	TPS_CPULOCK_LEV						/* CPU���b�N���荞�݃��x��	*/
								/* Interruption level as CPU lock  */
};


TPS_DISPATCH_INFO	_kernel_tps_DispatchInfo;		/* �f�B�X�p�b�`���O�L�^�̈�	*/
								/* Dispatch log recording area	*/
TPS_KOBJACCESS_INFO	_kernel_tps_KObjAccessInfo;		/* �J�[�l���I�u�W�F�N�g�A�N�Z�X	*/
								/* ���O�L�^�̈�			*/
								/* Kernel object access log	*/
								/* Recording area		*/
void 			*_kernel_tps_SymStack[15 + 7];		/* ���d���荞�ݒǐ՗p�V���{���X�^�b�N */
								/* Symbol stack to trace multi-	*/
								/* ple interruption		*/
void			*_kernel_tps_CycHdrTorch;		/* �����n���h���N���t���O	*/
								/* Cyclic handler start flag	*/
static	unsigned char	_kernel_tps_PrevCtxType;		/* �O�o�f�B�X�p�b�`���O���	*/
static	unsigned short	_kernel_tps_PrevCtxId;			/* Dispatch log information	*/
								/* recorded previosly		*/


/************************************************************************
 *  tpsInitMonitor
 *	Type	 : void
 *	Ret val  : none
 *	Argument : VP_INT exinf ... extension parameter.
 *	Function : Initialize any variables of Status moniter module.
 ************************************************************************/
void tpsInitMonitor(VP_INT pExInfo)
{
								/* ���O�L�^�̈搧���񏉊���	*/
								/* Initializes Log recording	*/
								/* control area			*/
	_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount = TPS_MAX_DISPLOG;
	if(0x00010000 < _kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount){
		_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount = 0x00010000;
	}
	_kernel_tps_DispatchInfo.stRecCtrl.usRefPoint    = 0;	/* O.W.���t���O�̏�������	*/
	_kernel_tps_DispatchInfo.stRecCtrl.usRecPoint    = 0;	/* IDE�̐ӔC�ōs��		*/
	_kernel_tps_DispatchInfo.stRecCtrl.bOWState      = 0;	/* O.W. Enable flag is initia-	*/
								/* lized by IDE.		*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount = TPS_MAX_KOBJLOG;
	if(0x00010000 < _kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount){
		_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount = 0x00010000;
	}
	_kernel_tps_KObjAccessInfo.stRecCtrl.usRefPoint    = 0;	/* O.W.���t���O�̏�������	*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.usRecPoint    = 0;	/* IDE�̐ӔC�ōs��		*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.bOWState      = 0;	/* O.W. Enable flag is initia-	*/
							       	/* lized by IDE.		*/
	_kernel_tps_CycHdrTorch = NULL;
	_kernel_tps_PrevCtxType = 0x00;
	_kernel_tps_PrevCtxId   = 0x0000;
	return;
}


/************************************************************************
 *  tpsChkRecAreaFull
 *	Type	 : int
 *	Ret val  : TPS_E_OK ... It is possible to record log.
 *	           TPS_E_NG ... It is not possible to record log.
 *	Argument : TPS_LOGREC_CTRL *pstCtrl ... Pointer to control area.
 *	Function : If O.W. is not followed, and all area is recorded, 
 *                 This function notices that it is not possible to record log.
 ************************************************************************/
int
tpsChkRecAreaFull(TPS_LOGREC_CTRL *pstCtrl, int iLogType)
{
	unsigned short	usRecPoint, usRefPoint;
	int		iRetVal;

	if(!(pstCtrl->ulMaxRecCount)){				/* ���O�̋L�^�������Ă��Ȃ�	*/
		return	TPS_E_NG;				/* Recording is disallowed	*/
	}

	iRetVal = TPS_E_OK;					/* ���O�̋L�^��������Ă���	*/
	usRecPoint = pstCtrl->usRecPoint;			/* Recorded normally		*/
	usRefPoint = pstCtrl->usRefPoint;
	if(TPS_FLAG_OFF != pstCtrl->bOWState){
		if(TPS_FLAG_OFF == pstCtrl->bOWEnable){		/* ���g�p���R�[�h�Ȃ�		*/
			iRetVal = TPS_E_NG;			/* All records are used		*/
		}
	}

	return iRetVal;
}


/************************************************************************
 *  tpsSearchCtxId
 *	Type	 : unsigned short
 *	Ret val  : ID or Handler number.
 *                 0xffff(-1) means that recording is not done.
 *	Argument : unsigned char *bCtxType ... Context type
 *		   void *pCBoxAddr         ... Control Box or Initalize 
 *		                              infomation address
 *	Function : This function specifies Task/Cyclic Handler ID or
 *		   Interrupt/Exception handler number.
 *		   If "TPS_CTXTYPE_INTHDR" is specified, this function
 *		   searches in Int-handler information table first.
 *		   But If there is not corresponding information, it searches
 *		   in Exc-handler information table next, when there is
 *		   the information, this function renews specified type with
 *		   "TPS_CTXTYPE_EXCHDR". 
 ************************************************************************/
unsigned short
tpsSearchCtxId(unsigned char *pbCtxType, void *pCBoxAddr)
{
	unsigned short	usRecId, usTemp;

	switch (*pbCtxType){
		case TPS_CTXTYPE_INTHDR:
			if(TPS_C_TIMER_HANDLER == pCBoxAddr){
				usRecId = (unsigned short)-1;
				break;
			}

			usTemp = TNUM_INHNO;			/* �ϐ��Ɋi�[���Ĕ�r����̂�	*/
			usRecId = 0;				/* �I�u�W�F�N�g�������̏ꍇ��	*/
								/* ���[�j���O���������邽��	*/
								/* usTemp is used to restrain	*/
			while((usTemp > usRecId) &&		/* Warning.			*/
			      ((FP)pCBoxAddr != _kernel_inhinib_table[usRecId].inthdr)){
				usRecId++;
			}
			if(usTemp > usRecId){
				usRecId++;
				break;
			}
			/* Search Exception handler information table */
			*pbCtxType = TPS_CTXTYPE_EXCHDR;
			usTemp = TNUM_EXCNO;
			usRecId = 0;
			while((usTemp > usRecId) &&
			      (pCBoxAddr != (void *)_kernel_excinib_table[usRecId].exchdr)){
				usRecId++;
			}
			if(usTemp > usRecId){
				usRecId++;
				break;
			}

			usRecId = (unsigned short)-1 ;		/* �}�N��"INT_ENTRY" or "EXC_ENTRY"	*/
			break;					/* �ɂ��t�����ꂽ�o�������������	*/
								/* �Ăяo����邽�߁A�Y���Ȃ��Ƃ���	*/
								/* �P�[�X�͎��ۂɂ͑��݂��Ȃ�		*/
		case TPS_CTXTYPE_CYCHDR:			/* Usually, this route is not proccessed*/
			usRecId = (unsigned short)((((CYCCB*)pCBoxAddr) - cyccb_table) + TMIN_CYCID);
			break;
		case TPS_CTXTYPE_TASK:
			usRecId = (unsigned short)TSKID((TCB *)pCBoxAddr);
			break;
		default:
			usRecId = 0;				/* Idle���[�`��		*/
			break;					/* Idle routine		*/
	}

	return usRecId;
}


/************************************************************************
 *  tpsRecDispatchLog
 *	Type	 : void
 *	Ret val  : none
 *	Argument : unsigned char bCtxType  ... The context type which shifted
 *	                                       to running state.
 *		   void *pCBAddr ... Normally, Address of a control box of the context
 *				     which gets the running right is specified.
 *			If Int/Exc handler is started, Entry point of the
 *			handler(XXX_entry) is specified.
 *	Function : Records Dispath log with specified parameters.
 ************************************************************************/
void
tpsRecDispatchLog(unsigned char bCtxType, void *pCBAddr)
{
	volatile unsigned short	*pusRecPoint;
	unsigned short		usRecId;
	int			iRetVal;

	iRetVal = tpsChkRecAreaFull((TPS_LOGREC_CTRL *)&_kernel_tps_DispatchInfo,
				    TPS_LOGTYPE_DISP);
	if(TPS_E_NG == iRetVal){				/* �I�[�o�[���C�g�֎~��Ԃł�	*/
		return;						/* �󂫃��R�[�h�Ȃ�		*/
	}							/* Log recording is impossible	*/

	pusRecPoint = &(_kernel_tps_DispatchInfo.stRecCtrl.usRecPoint);
	usRecId = tpsSearchCtxId(&bCtxType, pCBAddr);		/* �^�X�N/�����n���h��ID or	*/
								/* ���荞�݃n���h���ԍ������	*/
								/* Specifies calling context	*/
	if(0xffff == usRecId){
		return;						/* �^�C�}�n���h���ւ̃f�B�X�p�b�`   */
	}							/* Dispatches to timer handler	*/

	if((bCtxType == _kernel_tps_PrevCtxType) &&		/* ���O�ɋL�^�����R���e�L�X�g��	*/
	   (usRecId  == _kernel_tps_PrevCtxId)){		/* ����̏ꍇ�͋L�^���Ȃ�	*/
		return;						/* �i�^�C�}�n���h���̒P�Ǝ��s�j	*/
	}							/* Dispatches from timer handler*/

								/* �f�B�X�p�b�`���O��CPU���b�N��� or	*/
								/* ���荞�݋֎~��ԂŌĂяo�����	*/
								/* The condition when this function	*/
								/* is called is "CPU lock".		*/
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].ulRecTime = current_time;
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].bCtxType  = bCtxType;
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].usCtxId   = usRecId;

	_kernel_tps_PrevCtxType = bCtxType;
	_kernel_tps_PrevCtxId   = usRecId;

	*pusRecPoint += 1;
	if((_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount & 0x0000ffff) <= *pusRecPoint){
		*pusRecPoint = 0;
	}
	if(*pusRecPoint == _kernel_tps_DispatchInfo.stRecCtrl.usRefPoint){
		_kernel_tps_DispatchInfo.stRecCtrl.bOWState = 0x01;
	}

	return;
}


/************************************************************************
 *  tpsRecKObjAccessLog
 *	Type	 : void
 *	Ret val  : none
 *	Argument : unsigned short usObjId ... target object.
 *		   void *pCtxInfo ... ID of the context which calls system call.
 *		   unsigned long  ulDataId ... Data ID
 *		   unsigned long  ulTypesBcd ... includes "kernel object type",
 *				                 "access type", "result" and
 *				                 "context type".
 *				  Assign is in the following.
 *				     Bit  0 -  7 : Kernel object type
 *				     Bit  8 - 15 : Access type
 *				     Bit 16 - 23 : Result
 *				     Bit 24 - 31 : Context type
 *		
 *	Function : Records Kernel object access log with specified parameters.
 ************************************************************************/
void
tpsRecKObjAccessLog(unsigned short usObjId, void *pCtxInfo, unsigned long ulDataId, unsigned long ulTypesBcd)
{
	unsigned char	bKObjType, bAccessType, bResult, bCtxType;
	volatile unsigned short	*pusRecPoint;
	unsigned short		usCtxId;
	int			iRetVal;

	iRetVal = tpsChkRecAreaFull((TPS_LOGREC_CTRL *)&_kernel_tps_KObjAccessInfo,
				    TPS_LOGTYPE_KOBJ);		/* ���O�L�^�̉ۂ��`�F�b�N	*/
	if(TPS_E_NG == iRetVal){				/* Checking log area capacity	*/
		return;
	}

	pusRecPoint = &(_kernel_tps_KObjAccessInfo.stRecCtrl.usRecPoint);
	bKObjType   = (unsigned char)(ulTypesBcd         & 0x000000ff);
	bAccessType = (unsigned char)((ulTypesBcd >>  8) & 0x000000ff);
	bResult     = (unsigned char)((ulTypesBcd >> 16) & 0x000000ff);
	bCtxType    = (unsigned char)((ulTypesBcd >> 24) & 0x000000ff);

	usCtxId = tpsSearchCtxId(&bCtxType, pCtxInfo);		/* �^�X�N/�����n���h��ID or	*/
	if(!usCtxId || (0xffff == usCtxId)){			/* ���荞�݃n���h���ԍ������	*/
		return;						/* Specify Calling context	*/
	}							/* �z��O�̃R���e�L�X�g���	*/
								/* Not supporting context	*/
	if(TPS_OBJTYPE_MBOX != bKObjType &&
	   TPS_OBJTYPE_FLAG != bKObjType){			/* �T�|�[�g�O�̃I�u�W�F�N�g���	*/
		return;						/* Not supporting object type	*/
	}

	{
		SIL_PRE_LOC;					/* (���d)���荞�݂��֎~���邽��	*/
		SIL_LOC_INT();					/* IE�𑀍삷��			*/
								/* Prohibits interruption	*/
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].ulRecTime   = current_time;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bObjType    = bKObjType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bAccessType = bAccessType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].usObjectId  = usObjId;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bResult     = bResult;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bCtxType    = bCtxType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].usCtxId     = usCtxId;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].ulDataId    = ulDataId;

		*pusRecPoint += 1;
		if((_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount & 0x0000ffff) <= *pusRecPoint){
			*pusRecPoint = 0;
		}
		if(*pusRecPoint == _kernel_tps_KObjAccessInfo.stRecCtrl.usRefPoint){
			_kernel_tps_KObjAccessInfo.stRecCtrl.bOWState = 0x01;
		}
		SIL_UNL_INT();					/* ���荞�݋֎~����������	*/
	}							/* Allows interruption		*/

	return;
}

