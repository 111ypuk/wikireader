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

#include "jsp_kernel.h"
#include "itron.h"
#include "stdio.h"
#include "string.h"

#include "task.h"
#include "cyclic.h"
#include "mailbox.h"
#include "semaphore.h"
#include "wait.h"
#include "eventflag.h"
#include "dataqueue.h"
#include "mempfix.h"

#define TPS_BUFSIZE	2048		    /* ���b�Z�[�W�o�b�t�@�T�C�Y     */
					    /* print buffer size (0x0800)   */
#define TPS_EMPTY_MSG	"Warning: Stop by print buffer full."

#define TPS_OK		0		    /* �G���[�R�[�h */
#define TPS_EXT_PARAM	1
#define TPS_ERROR	-1		    /* error code   */
#define TPS_NO_OBJECT	-2

#define TPS_TSKSTS_RUNDMT   1		    /* �^�X�N��ԃt���O */
#define TPS_TSKSTS_SUSPEND  2		    /* Task status flag */

#define TPS_STSID_SYS	0		    /* ��Ԏ擾�֐��p�t���O	    */
#define TPS_STSID_TSK	1		    /* Flags for status reference   */
					    /* function */

#include "kernel_objs.h"
extern SEMCB semcb_table[TNUM_SEMID];
extern FLGCB flgcb_table[TNUM_FLGID];
extern DTQCB dtqcb_table[TNUM_DTQID];
extern MBXCB mbxcb_table[TNUM_MBXID];
extern MPFCB mpfcb_table[TNUM_MPFID];
extern CYCCB cyccb_table[TNUM_CYCID];

static	INT tps_StartNum, tps_EndNum;	    /* ���\���͈͂�ێ� */
static	INT tps_TaskFlag;		    /* The start/end number of the  */
					    /* object which displays infor- */
					    /* tion			    */

typedef struct {			    /* ���b�Z�[�W�o�b�t�@�\����     */
	INT     iSize;			    /* structure of sprintf buffer  */
	B	    chBuff[TPS_BUFSIZE + 3];
} tps_DebugMsg_t;
static	tps_DebugMsg_t	tps_tmpbuf;

/*
 *  �󂫃o�b�t�@�T�C�Y�̃`�F�b�N
 *  Check free size of tps_tmpbuf
 */
/************************************************************************
 *  chk_prtbuf
 *	Type	 : INT
 *	Ret val  : Error code
 *		     0 ... Normal exit
 *		    -1 ... Parameter error
 *	Argument : INT	 iChkSize ... Requisite size
 *	Function : Check free buffer size
 ************************************************************************/
static INT chk_prtbuf(INT iChkSize)
{
	INT     iCnt;

	if(iChkSize > (TPS_BUFSIZE - tps_tmpbuf.iSize)){
						    /* �x�����b�Z�[�W�����܂�Ȃ��ꍇ */
						    /* when warning message doesn't fit*/
						    /* buffer			      */
		if(tps_tmpbuf.iSize + sizeof(TPS_EMPTY_MSG) + 2 + 1 > TPS_BUFSIZE){
			tps_tmpbuf.iSize = TPS_BUFSIZE - sizeof(TPS_EMPTY_MSG) - 2 - 1;
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%s\n%c",
				TPS_EMPTY_MSG, '\0');

		tps_tmpbuf.iSize += iCnt;

		return TPS_ERROR;
	}
	return TPS_OK;
}

/*
 *  ���o�͔͈͂̐ݒ�
 *  Set up the range which display information.
 */
/************************************************************************
 *  chk_param
 *	Type	 : static INT
 *	Ret val  : Return code
 *		    -1 ... Parameter Error
 *		     0 ... all display
 *		     1 ... valid parameters specified
 *	Argument : INT iStart  ... start displaying number.
 *		   INT iEnd    ... terminate displaying number.
 *		   INT iMaxNum ... Max ID number.
 *	Function : Get current IL from PSR.
 ************************************************************************/
static INT chk_param(INT iStart, INT iEnd, INT iMaxNum)
{
	if(iMaxNum == 0){
		return TPS_NO_OBJECT;		    /* �������I�u�W�F�N�g�̎w��     */
	}					    /* Specified ungenerated object */

	if((iStart < 0) || (iEnd > iMaxNum)){
		return TPS_ERROR;		    /* �ُ�p�����[�^	 */
	}					    /* illegal parameter */

	if(iStart == 0){
		if(iEnd == 0){			    /* �S���\������		 */
			tps_StartNum = 1;		    /* all display (1 -> maxnum) */
			tps_EndNum = iMaxNum;
			return TPS_OK;
		}
		else{
			return TPS_ERROR;
		}
	}

	if(iEnd > iStart){
		if(iEnd > iMaxNum){
			tps_StartNum = iStart;	    /* �J�n�l����ő�l�܂� */
			tps_EndNum = iMaxNum;	    /* iStart -> iMaxNum    */
		}else{
			tps_StartNum = iStart;	    /* �J�n�l����I���l�܂� */
			tps_EndNum = iEnd;		    /* iStart -> iEnd	    */
		}
	}else{
		if(iStart > iMaxNum){
			return TPS_ERROR;
		}
		tps_StartNum = iStart;		    /* �J�n�l�̒P�ƕ\��     */
		tps_EndNum = iStart;		    /* iStart only	    */
	}
	return TPS_EXT_PARAM;
}

/*
 *  �L���[�C���O��񃁃b�Z�[�W�쐬����
 *  Make queuing information
 */
/************************************************************************
 *  print_queue
 *	Type	 : static void
 *	Ret val  : None
 *	Argument : QUEUE *pMember ... the table registering queue top/tail
 *				      address
 *	Function : Make queuing list message.
 ************************************************************************/
static INT print_queue(QUEUE* pMember)
{
	QUEUE* pTskcb;
	INT iFlag, iLoop, iCnt;


	pTskcb = pMember->next;				/* �擪�^�X�N���擾     */
	iFlag = 0;					/* get Top task-address */

	while(pTskcb != pMember){			/* �^�X�N������Ό������� */
		iLoop = 0;				/* Search task when Top   */
							/* address is not pMember */
		while((QUEUE*)&tcb_table[iLoop] != pTskcb){
			iLoop++;
			if(iLoop == tmax_tskid){
				break;			/* ������Ȃ���ΏI������ */
			}				/* not found(illegal)	*/ 
		}

		if(chk_prtbuf((iFlag) ? 10 : 7)){	/* �o�b�t�@�e�ʂ��`�F�b�N */
			return TPS_ERROR;		/* free buffer size check */
		}

		if(iFlag == 0){ 			/* �擪�^�X�N�̕\��  */
							/* display head task */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"task%d", iLoop + 1);
			tps_tmpbuf.iSize += iCnt;
			iFlag = 1;
		}else{					/* ��擪�^�X�N�̕\��    */
							/* display continue task */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"->task%d", iLoop + 1);
			tps_tmpbuf.iSize += iCnt;
		}
		pTskcb = pTskcb->next;			/* ���̃^�X�N���擾      */
	}						/* get next task-address */

	if(iFlag == 0){					/* �L���[�C���O�^�X�N�Ȃ�*/
							/* Queuing no objects    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "-");
		tps_tmpbuf.iSize += iCnt;
	}

	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"\n");
	tps_tmpbuf.iSize += iCnt;

	return TPS_OK;
}

/*
 *  �V�X�e��/�^�X�N��Ԃ̎擾
 *  Refer system/task status.
 */
/************************************************************************
 *  print_status
 *	Type	 : static void
 *	Ret val  : None
 *	Argument : INT	iStsId	  ... target type
 *		   TCB* pstTcb	  ... Task contorl block address
 *		   B*	chStsBuff ... Buffer address
 *	Function : Make status message of system or specified task.
 ************************************************************************/
static void print_status(INT iStsId, TCB* pstTcb, B *chStsBuff)
{
	ER	iRetVal;

	*chStsBuff = '\0';				/* �V�X�e����Ԃ̎擾  */
	if(iStsId == TPS_STSID_SYS){			/* Check System Status */
		if(enadsp == FALSE){
			strcat(chStsBuff, " DDSP");
		}

		iRetVal = sense_lock();
		if(iRetVal == TRUE){
			strcat(chStsBuff, " LOC");
		}

		if(!tps_IntNestCnt){
			strcat(chStsBuff, " TSK");
		}
		else{
			strcat(chStsBuff, " INDP");
		}

		return;
	}

	tps_TaskFlag = 0;
	if(pstTcb == runtsk){				/* �^�X�N��Ԃ̎擾  */
		sprintf(chStsBuff, "RUN");		/* Check task Status */
		tps_TaskFlag = TPS_TSKSTS_RUNDMT;
	}
	else if(TSTAT_DORMANT(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "DMT");
		tps_TaskFlag = TPS_TSKSTS_RUNDMT;
	}
	else if(TSTAT_RUNNABLE(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "RDY");
	}
	else if(TSTAT_WAITING(pstTcb->tstat) == TRUE){
		if(TSTAT_SUSPENDED(pstTcb->tstat) == TRUE){
			sprintf(chStsBuff, "WAS");
			tps_TaskFlag = TPS_TSKSTS_SUSPEND;
		}
		else{
			sprintf(chStsBuff, "WAI");
		}
	}
	else if(TSTAT_SUSPENDED(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "SUS");
		tps_TaskFlag = TPS_TSKSTS_SUSPEND;
	}

	return;
}

/*
 *  �V�X�e����Ԃ̕\��
 *  Display system status.
 */
/************************************************************************
 *  iprint_mng
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iParam1 ... Reserved(Don't used)
 *		   INT	iParam2 ... Reserved(Don't used)
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make status message of system.
 ************************************************************************/
VP iprint_mng(INT iParam1, INT iParam2, INT iParam3)
{
	INT iCnt;
	B	chCurSts[25];

	tps_tmpbuf.iSize = 0;
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* �^�C�g���̕\�� */
			    "System management information\n");	/* print title   */
	tps_tmpbuf.iSize += iCnt;

							/* �V�X�e����Ԃ̕\��*/
	print_status(TPS_STSID_SYS, NULL, chCurSts);    /* print system status */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
			    "System information        (%08xh) :%s\n",
			    (UINT)&enadsp, chCurSts);
	tps_tmpbuf.iSize += iCnt;

							/* �^�X�NID�̕\��        */
							/* print running task ID */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, 
			    "Current running task ID   (%08xh) : ",
			    (UINT)&runtsk);
	tps_tmpbuf.iSize += iCnt;
	if(runtsk != NULL){ 	    
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%03d\n",
				TSKID(runtsk));
	}
	else{
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"-\n");
	}
	tps_tmpbuf.iSize += iCnt;

								/* ���^�X�NID�̕\��   */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* print next task ID */
			    "Next task ID              (%08xh) : ",
			    (UINT)&schedtsk);
	tps_tmpbuf.iSize += iCnt;
	if(runtsk != NULL){ 	    
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%03d\n",
				TSKID(schedtsk));
	}
	else{
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"-\n");
	}
	tps_tmpbuf.iSize += iCnt;

								/* �V�X�e�����Ԃ�\�� */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* print current time */
			    "Current time              (%08xh) : %08x\n",
			    (UINT)&current_time, current_time);
	tps_tmpbuf.iSize += iCnt;

							/* �����݃l�X�g�J�E���g�̕\��    */
							/* print Nesting interrupt count */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
			    "Interrupt nesting count   (%08xh) : %03d\n",
			    (UINT)&tps_IntNestCnt, tps_IntNestCnt);
	tps_tmpbuf.iSize += iCnt;

	tps_tmpbuf.iSize++; 				/* �k�������������Z */
							/* ++ is null	*/
	return (VP)&tps_tmpbuf;
}

/*
 *  ���f�B�L���[�̕\��
 *  Display ready queue.
 */
/************************************************************************
 *  iprint_rdy
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start priority
 *		   INT	iEnd	... End priority
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
VP iprint_rdy(INT iStart, INT iEnd, INT iParam3)
{
	INT iRetVal, iLoop, iCnt;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, TMAX_TPRI);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

							    /* �^�C�g����\��	*/
							    /* print title	*/
	iCnt = sprintf(tps_tmpbuf.chBuff,"Ready queue\nPRI  ADDRESS    QUEUE\n");
	tps_tmpbuf.iSize = iCnt;

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(15) != TPS_OK){		/* �o�b�t�@�e�ʂ��`�F�b�N */
			break;				/* free buffer size check */
		}

							/* �D��x�ƃA�h���X��\��     */
							/* print priority No, address */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%-3d  %08xh  ",
				iLoop, (UINT)&ready_queue[iLoop-1]);
		tps_tmpbuf.iSize += iCnt;

							/* �L���[���b�Z�[�W���쐬���� */
							/* print queue list */
		print_queue((QUEUE *)&ready_queue[iLoop-1]);
	}

	tps_tmpbuf.iSize ++;				/* �k�������������Z */
	return (VP)&tps_tmpbuf;				/* ++ is null	*/
}

/*
 *  �^�X�N������̕\��
 *  Display task control block.
 */
/************************************************************************
 *  iprint_tsk
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start task ID
 *		   INT	iEnd	... End task ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
int *iprint_tsk(INT iStart, INT iEnd, INT iParam3)
{
	TCB* pTskcb;
	INT iRetVal, iLoop, iCnt;
	B	chStsBuff[5];

	
	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_tskid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == E_OK){				/* �S���\������ */
							/* all display  */
		iCnt =	sprintf(tps_tmpbuf.chBuff, "Task overview\n");
		iCnt += sprintf(tps_tmpbuf.chBuff + iCnt,
				"TID  PC         SP         STA\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(32) != TPS_OK){	/* �o�b�t�@�e�ʂ��`�F�b�N */
				break;			/* free buffer size       */
			}

			pTskcb = (TCB*)&tcb_table[iLoop-1];
			print_status(TPS_STSID_TSK, pTskcb, chStsBuff);
					    
			if(tps_TaskFlag == 1){		/* �^�X�N���x�~, ���s��Ԃ̏ꍇ */
							/* case DMT,RUN 		*/
				iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%-3d  --------   --------   %3s\n",
					iLoop, chStsBuff);
				tps_tmpbuf.iSize += iCnt;
			}else{
				iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%-3d  %08xh  %08xh  %3s\n",
					iLoop, (UINT)pTskcb->tskctxb.pc,
					(UINT)pTskcb->tskctxb.sp, chStsBuff);
				tps_tmpbuf.iSize += iCnt;
			}
		}
		tps_tmpbuf.iSize++;			/* �k�������������Z */
							/* ++ is nul	    */
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(12) != TPS_OK){
			break;
		}
							/* �^�X�NID��\��	*/
							/* print task ID	*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"TSKID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		pTskcb = (TCB*)&tcb_table[iLoop-1];
		if(chk_prtbuf(32) != E_OK){
			break;
		}
		print_status(TPS_STSID_TSK, pTskcb, chStsBuff);
							/* �^�X�N����u���b�N�A�h���X��\�� */
							/* print taskcb address 	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Taskcb addr        : %08xh\n",
				(UINT)pTskcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* ���݂�PC��\��   */
							/* print current PC */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"PC                 : ");
		tps_tmpbuf.iSize += iCnt;
		if(tps_TaskFlag == 1){			/* �^�X�N���x�~, ���s��Ԃ̏ꍇ */
							/* case DMT, RUN	        */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "--------\n");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    (UINT)pTskcb->tskctxb.pc);
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							    /* PC�����l��\������ */
							    /* print initial PC   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial PC         : %08xh\n",
				(UINT)pTskcb->tinib->task);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* ���݂�SP��\��   */
							/* print current SP */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"SP                 : ");
		tps_tmpbuf.iSize += iCnt;
		if(tps_TaskFlag == 1){			/* �^�X�N���x�~, ���s��Ԃ̏ꍇ */
							/* case DMT, RUN	        */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "--------\n");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    (UINT)pTskcb->tskctxb.sp);
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* SP�����l��\������ */
							/* print initial SP   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial SP         : %08xh\n",
				(UINT)pTskcb->tinib->stk + pTskcb->tinib->stksz);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(25) != E_OK){
			break;
		}
							/* ���݂̗D��x��\������ */
							/* print current priority */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Priority           : %-2d\n",
				pTskcb->priority + TMIN_TPRI);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(25) != E_OK){
			break;
		}
							/* �D��x�̏����l��\������ */
							/* print initial priority   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial priority   : %-2d\n",
				pTskcb->tinib->ipriority + TMIN_TPRI);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(26) != E_OK){
			break;
		}
							/* �^�X�N��Ԃ�\������ */
							/* print task status    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Task status        : %3s\n",
				chStsBuff);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){
			break;
		}
							/* �N���v������\������      */
							/* print Start request count */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Start Req.   count : %d\n",
				pTskcb->actcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){				
			break;
		}
							/* �N���v������\������       */
							/* print wake up request count*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"WUP Req.     count : %d\n",
				pTskcb->wupcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){
			break;
		}
							/* �T�X�y���h�v������\������ */
							/* print Suspend request count*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Suspend Req. count : %d\n\n",
				(tps_TaskFlag == TPS_TSKSTS_SUSPEND) ? 1 : 0);
		tps_tmpbuf.iSize += iCnt;
	}

	tps_tmpbuf.iSize++;
	return((VP)&tps_tmpbuf);
}

/*
 *  �����n���h��������̕\��
 *  Display cyclic handler control block.
 */
/************************************************************************
 *  iprint_cyc
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler No.
 *		   INT	iEnd	... End handler No.
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
VP iprint_cyc(INT iStart, INT iEnd, INT Param3)
{
	CYCCB*  pHndrcb;
	INT     iRetVal, iLoop, iCnt;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_cycid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Cyclic handler overview\n");
		tps_tmpbuf.iSize = iCnt;
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"NO   ACT    PC         CYCLETIME  ENTRYTIME\n");
		tps_tmpbuf.iSize += iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(45) != TPS_OK){
				break;
			}
							/* �n���h����Ԃ�\������ */
							/* print handler status   */
			pHndrcb = (CYCCB*)&cyccb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %-3s    %08xh  %08xh  %08xh\n",
				    iLoop, ((pHndrcb->cycsta == TRUE) ? "ON" : "OFF"),
				    (UINT)pHndrcb->cycinib->cychdr,
				    pHndrcb->cycinib->cyctim, pHndrcb->evttim);
			tps_tmpbuf.iSize += iCnt;
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(26) != TPS_OK){
			break;
		}
							/* �n���h���ԍ���\������ */
							/* print handler No.      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cyclic handler No. = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/*�����n���h������u���b�N�A�h���X��\������ */
							/* print cyclic handler cb address	     */
		pHndrcb = (CYCCB*)&cyccb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cyccb addr       : %08xh\n",
				(UINT)pHndrcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* ������Ԃ�\������ */
							/* print activation   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Activation       : %-3s\n",
				(pHndrcb->cycsta == TRUE) ? "ON" : "OFF");
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* �g�����i�[�̈��\������ 		  */
							/* print address stored expand information*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Expand Info addr : %08xh\n",
				(UINT)pHndrcb->cycinib->exinf);
		tps_tmpbuf.iSize += iCnt;
		
		if(chk_prtbuf(30) != TPS_OK){			    
			break;
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, /* PC��\������ */
				"PC               : %08xh\n",	     /* print PC     */
				(UINT)pHndrcb->cycinib->cychdr);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* �ʑ����Ԃ�\������ */
							/* print phase time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Phase time       : %08xh\n",
				pHndrcb->cycinib->cycphs);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* �������Ԃ�\������ */
							/* print cycle time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cycle time       : %08xh\n",
				pHndrcb->cycinib->cyctim);
		tps_tmpbuf.iSize += iCnt;
		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* �N��������\������ */
							/* print entry time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Entry time       : %08xh\n\n",
				pHndrcb->evttim);
		tps_tmpbuf.iSize += iCnt;
	}

	tps_tmpbuf.iSize ++;
	return (VP)&tps_tmpbuf;
}

/*
 *  ���[���{�b�N�X������̕\��
 *  Display mail box control block.
 */
/************************************************************************
 *  iprint_mbx
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified mail box.
 ************************************************************************/
VP iprint_mbx(INT iStart, INT iEnd, INT iParam3)
{
	MBXCB*  pMlbxcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    chAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_mbxid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Mail box overview\nID   MSG  QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(11) != TPS_OK){
				break;
			}
			pMlbxcb = (MBXCB*)&mbxcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %3s  ",
				    iLoop, (pMlbxcb->head == NULL) ? " - " : " * ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pMlbxcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(20) != TPS_OK){
			break;
		}
							/* ���[���{�b�N�XID��\������ */
							/* print mail box ID	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mail box ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* ���[���{�b�N�X����u���b�N�A�h���X */
							/* ��\������			      */
							/* print control box address	      */
		pMlbxcb = (MBXCB *)&mbxcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mbxcb addr         : %08xh\n",
				(UINT)pMlbxcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* ���[���{�b�N�X������\������ */
							/* print mail box attribute     */
		if(pMlbxcb->mbxinib->mbxatr == (TA_TFIFO | TA_MFIFO)){
			chAtrMsg = "TA_TFIFO | TA_MFIFO";
		}
		else if(pMlbxcb->mbxinib->mbxatr == (TA_TFIFO | TA_MPRI)){
			chAtrMsg = "TA_TFIFO | TA_MPRI";
		}
		else if(pMlbxcb->mbxinib->mbxatr == (TA_TPRI | TA_MFIFO)){
			chAtrMsg = "TA_TPRI | TA_MFIFO";
		}
		else{
			chAtrMsg = "TA_TPRI | TA_MPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mail box attribute : %s\n",
				chAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* �ő�D��x��\������ */
							/* print Max priority   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Max mail priority  : %08xh\n",
				pMlbxcb->mbxinib->maxmpri);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(28) != TPS_OK){
			break;
		}
							/* ���[���̗L����\������ */
							/* print mail exist/empty */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Message list       : %s\n",
				(pMlbxcb->head == NULL) ? "Empty" : "Exist");
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* ��M�҂��L���[��\������ */
							/* Print waitting queue     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Rcv queue          : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pMlbxcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  �Z�}�t�H������̕\��
 *  Display semaphore control block.
 */
/************************************************************************
 *  iprint_sem
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified semaphore.
 ************************************************************************/
VP iprint_sem(INT iStart, INT iEnd, INT iParam3)
{
	SEMCB*  pSemcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_semid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Semaphore overview\nID   MAX  CNT  QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
			pSemcb = (SEMCB*)&semcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %-3d  %-3d  ",
				    iLoop, pSemcb->seminib->maxsem, pSemcb->semcnt);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pSemcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(20) != TPS_OK){		    
			break;
		}
							/* �Z�}�t�HID��\������ */
							/* print mail box ID    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* �Z�}�t�H����u���b�N�A�h���X��\������ */
							/* print control box address		  */
		pSemcb = (SEMCB*)&semcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semcb addr          : %08xh\n",
				(UINT)pSemcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* �Z�}�t�H������\������    */
							/* print semaphore attribute */
		if(pSemcb->seminib->sematr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore attribute : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* �Z�}�t�H�J�E���g�ő�l��\������ */
							/* print Max count		    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Max semaphore count : %08xh\n",
				pSemcb->seminib->maxsem);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
}							/* ���݂̃Z�}�t�H�J�E���g��\������ */
							/* print current semaphore count    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore count     : %08xh\n",
				pSemcb->semcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* �擾�҂��^�X�N�̃L���[��\������ */
							/* print Waitting task queue	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait queue          : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pSemcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  �C�x���g�t���O������̕\��
 *  Display event flag control block.
 */
/************************************************************************
 *  iprint_flg
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified event flag.
 ************************************************************************/
VP iprint_flg(INT iStart, INT iEnd, INT iParam3)
{
	FLGCB	*pFlgcb;
	TCB 	*pTskcb;
	WINFO_FLG	*pWaitFlgInfo;
	INT     iRetVal, iLoop, iCnt;
	B	    chAtrMsg[30];

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_flgid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Event flag overview\nID   QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(6) != TPS_OK){
				break;
			}
			pFlgcb = (FLGCB*)&flgcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  ", iLoop);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pFlgcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* �C�x���g�t���OID��\������ */
							/* print event flag ID	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Event flag ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* �C�x���g�t���O����u���b�N */
							/* �A�h���X��\������	      */
							/* print control box address  */
		pFlgcb = (FLGCB*)&flgcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Flgcb addr           : %08xh\n",
				(UINT)pFlgcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(58) != TPS_OK){
			break;
		}
							/* �C�x���g�t���O������\������ */
							/* print event flag attribute   */
		chAtrMsg[0] = '\0';
							/* �^�X�N�D��x�ɂ��L���[�C���O */
							/* queuing by task priority       */
		if(pFlgcb->flginib->flgatr & TA_TPRI){
			strcat(chAtrMsg, "TA_TPRI");
		}
		else{
			strcat(chAtrMsg, "TA_TFIFO");	/* FIFO�ł̃L���[�C���O */
		}					/* queuing by FIFO	*/

		if(pFlgcb->flginib->flgatr & 0x02){
			strcat(chAtrMsg, " | TA_WMUL");
		}
		else{
			strcat(chAtrMsg, " | TA_WSGL");
		}
							/* �p�^�[����v��t���O�̃N���A */
							/* cleaing flag pattern	        */
		if(pFlgcb->flginib->flgatr & TA_CLR){
			strcat(chAtrMsg, " | TA_CLR");	
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Event flag attribute : %s\n",
				chAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* ���݂̃t���O�p�^�[����\������ */
							/* print current flag pattern     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Flag pattern         : %08xh\n",
				pFlgcb->flgptn);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* �҂��^�X�N�̃t���O�p�^�[��	      */
							/* print wait flag pattern of top task*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait flag pattern    : ");
		tps_tmpbuf.iSize += iCnt;
		
		pTskcb = (TCB *)pFlgcb->wait_queue.next;
		pWaitFlgInfo = (WINFO_FLG*)pTskcb->winfo;
		if(pTskcb != (TCB *)&pFlgcb->wait_queue){
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    pWaitFlgInfo->waiptn);
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "-\n");
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* �҂����[�h�̕\�� */
							/* print wait mode  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"Wait mode            : ");
		tps_tmpbuf.iSize += iCnt;
		if(pTskcb != (TCB *)&pFlgcb->wait_queue){
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%s\n", (pWaitFlgInfo->wfmode == TWF_ORW) ? "TWF_ORW" : "TWF_ANDW");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"-\n");
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* �擾�҂��^�X�N�̃L���[��\������ */
							/* print Waitting task queue	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait queue           : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pFlgcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  �f�[�^�L���[������̕\��
 *  Display data queue control block.
 */
/************************************************************************
 *  iprint_dtq
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified data queue.
 ************************************************************************/
VP iprint_dtq(INT iStart, INT iEnd, INT iParam3)
{
	DTQCB*  pDtqcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_dtqid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Data queue overview\nID   MSG       QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
							/* ��M��Ԃ�\������ */
							/* print recieve status */
			pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %s  RCV  ",
				    iLoop, (pDtqcb->count == 0) ? " - " : " * ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)&pDtqcb->rwait_queue);

			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
							/* ���M��Ԃ�\������ */
							/* print send status  */
			pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "          SND  ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)&pDtqcb->swait_queue);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* �f�[�^�L���[ID��\������ */
							/* print Data queue ID	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* �f�[�^�L���[����u���b�N */
							/* �A�h���X��\������	    */
							/* print control box address*/
		pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Dtqcb addr                  : %08xh\n",
				(UINT)pDtqcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(40) != TPS_OK){
			break;
		}
							/* �f�[�^�L���[������\������ */
							/* print data queue attribute */
		if(pDtqcb->dtqinib->dtqatr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue attribute        : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* �L���[�̈�̐擪�A�h���X��\������   */
							/* print top address of data queue area */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue area top address : %08xh\n",
				(UINT)pDtqcb->dtqinib->dtq);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* �f�[�^�L�^�ʒu��\������      */
							/* print current offset from top */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Top data offset             : %08xh\n",
				pDtqcb->head);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* �L���[�C���O�f�[�^����\�����邷�� */
							/* print queuing data count	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Queuing data count          : %08xh\n",
				pDtqcb->count);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* ��M�҂��^�X�N�L���[��\������     */
							/* print Waitting task queue(recieve) */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Rcv task queue              : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)&pDtqcb->rwait_queue);
		if(iRetVal != TPS_OK){
			break;
		}

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* ���M�҂��^�X�N�L���[��\������  */
							/* print Waitting task queue(send) */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Snd task queue              : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)&pDtqcb->swait_queue);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  �Œ蒷�������v�[��������̕\��
 *  Display fixed memory pool control block.
 */
/************************************************************************
 *  iprint_mpf
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified memory pool.
 ************************************************************************/
VP iprint_mpf(INT iStart, INT iEnd, INT iParam3)
{
	MPFCB*  pMpfcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_mpfid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* �S���\������ */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Memory pool overview\nID   QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(6) != TPS_OK){
			break;
			}
			pMpfcb = (MPFCB*)&mpfcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  ",
				    iLoop);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pMpfcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	tps_tmpbuf.iSize = 0;
	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* �������v�[��ID��\������ */
							/* print memory pool ID     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* �������v�[������u���b�N */
							/* �A�h���X��\������	    */
							/* print control box address*/
		pMpfcb = (MPFCB*)&mpfcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mpfcb addr                      : %08xh\n",
				(UINT)pMpfcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(44) != TPS_OK){
			break;
		}
							/* �������v�[��������\������ */
							/* print data queue attribute */
		if(pMpfcb->mpfinib->mpfatr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool attribute           : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* �u���b�N�T�C�Y��\������ */
							/* print memory block size  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory block size               : %08xh\n",
				pMpfcb->mpfinib->blksz);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* �������v�[���̐擪�A�h���X��\������ */
							/* print top address of memory pool     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool area top address    : %08xh\n",
				(UINT)pMpfcb->mpfinib->mpf);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* �������v�[���̖����A�h���X��\������ */
							/* print tail address of memory pool    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool area tail address   : %08xh\n",
				(UINT)pMpfcb->mpfinib->limit);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* ���g�p�̈�̐擪�A�h���X��\������ */
							/* print top area of unused area      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Unused area top address         : %08xh\n",
				(UINT)pMpfcb->unused);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(48) != TPS_OK){				
			break;
		}
							/* �ԋp�u���b�N���X�g�g�b�v�̃u���b�N    */
							/* �A�h���X��\������		     */
							/* print top address released block list */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Released block list top address : %08xh\n",
				(UINT)pMpfcb->freelist);
		tps_tmpbuf.iSize += iCnt;

							/* �擾�҂��^�X�N�̃L���[��\������ */
							/* print Waitting task queue(send)  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait task queue                 : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)pMpfcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}

	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

