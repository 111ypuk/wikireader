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

/*
 *  Tab Size : 8
 */

#ifndef	_DBG_MON_H_
#define	_DBG_MON_H_

/*
 *  ��`
 *  definitions
 */

#define		TPS_E_OK			(0x00)
#define		TPS_E_NG			(0x01)

#define		TPS_FLAG_OFF			(0x00)
#define		TPS_FLAG_ON			(0x01)

#define		TPS_SIZE_LOGCTL			(0x0c)
#define		TPS_OSET_MAXRECORD		(0x00)
#define		TPS_OSET_RDSTART		(0x04)
#define		TPS_OSET_WTSTART		(0x06)
#define		TPS_OSET_OWCTRL			(0x08)
#define		TPS_OSET_OWSTATE		(0x09)

#define		TPS_SIZE_DISPLOG		(0x08)
#define		TPS_OSET_DIPS_SYSCLK		(0x00)
#define		TPS_OSET_DISP_CTXTYPE		(0x04)
#define		TPS_OSET_DISP_CTXID		(0x6)

#define		TPS_SIZE_KOBJLOG		(0x10)
#define		TPS_OSET_KOBJ_SYSCLK		(0x00)
#define		TPS_OSET_KOBJ_OBJTYPE		(0x04)
#define		TPS_OSET_KOBJ_ACCTYPE		(0x05)
#define		TPS_OSET_KOBJ_OBJID		(0x06)
#define		TPS_OSET_KOBJ_RESULT		(0x08)
#define		TPS_OSET_KOBJ_CTXTYPE		(0x09)
#define		TPS_OSET_KOBJ_CTXID		(0x0a)
#define		TPS_OSET_KOBJ_DATAID		(0x0c)

#define		TPS_CTXTYPE_TASK		(0x00)
#define		TPS_CTXTYPE_CYCHDR		(0x01)
#define		TPS_CTXTYPE_INTHDR		(0x02)
#define		TPS_CTXTYPE_EXCHDR		(0x03)
#define		TPS_CTXTYPE_IDLE		(0x10)

#define		TPS_OBJTYPE_FLAG		(0x01)
#define		TPS_OBJTYPE_MBOX		(0x03)

#define		TPS_ACCTYPE_SND			(0x00)
#define		TPS_ACCTYPE_RCV			(0x01)
#define		TPS_ACCTYPE_OTHER		(0x02)

#define		TPS_MAX_LOGTYPE			(0x02)
#define		TPS_LOGTYPE_DISP		(0x00)
#define		TPS_LOGTYPE_KOBJ		(0x01)

#define		TPS_MAX_DISPLOG			(4096)
#define		TPS_MAX_KOBJLOG			(4096)

#define		TPS_C_TIMER_HANDLER		(timer_handler_entry)


#ifndef	_MACRO_ONLY

#include "itron.h"

/*
 *  �}�N��
 *  macro
 */
#define		TPS_M_4PARS2BCD(bP1, bP2, bP3, bP4)	((unsigned long)(\
							 ((unsigned long)bP1 << 24) |\
							 ((unsigned long)bP2 << 16) |\
							 ((unsigned long)bP3 <<  8) |\
							 ((unsigned long)bP4)))
/*
 *  �\����
 *  Structures
 */
typedef	struct {
	unsigned long	ulMaxRecCount;			/* �ő�L�^����			*/
							/* Maximum record count		*/
	unsigned short	usRefPoint;			/* ���O�擾�J�n�ʒu		*/
							/* Oldest log position		*/
	unsigned short	usRecPoint;			/* ���O�L�^�ʒu			*/
							/* Recording position		*/
	unsigned char	bOWEnable;			/* �I�[�o�[���C�g���t���O	*/
							/* OverWrite Enable flag	*/
	unsigned char	bOWState;			/* �I�[�o�[���C�g�����t���O	*/
							/* OverWrite state flag		*/
	unsigned char	bRsv[2];			/* Reserved */
} TPS_LOGREC_CTRL;


typedef	struct {
	SYSTIM		ulRecTime;			/* ���O�L�^����			*/
							/* Recorded time		*/
	unsigned char	bCtxType;			/* �������			*/
							/* Context type			*/
	unsigned char	bRsv;				/* Reserved			*/
	unsigned short	usCtxId;			/* ���s��ԂɈڍs����������ID	*/
							/* Context ID			*/
} TPS_DISPLOG;


typedef	struct {
	SYSTIM		ulRecTime;			/* ���O�L�^����			*/
							/* Recorded time		*/
	unsigned char	bObjType;			/* �J�[�l���I�u�W�F�N�g���	*/
							/* Kernel object type		*/
	unsigned char	bAccessType;			/* �A�N�Z�X���			*/
							/* Access type			*/
	unsigned short	usObjectId;			/* �A�N�Z�X�����������J�[�l���I�u�W�F�N�g��ID */
							/* ID of accessed kernel object	*/
	unsigned char	bResult;			/* ��������			*/
							/* Result			*/
	unsigned char	bCtxType;			/* �A�N�Z�X���������̎��	*/
							/* Context type			*/
	unsigned short	usCtxId;			/* �A�N�Z�X����������ID		*/
							/* Context ID			*/
	unsigned long	ulDataId;			/* �f�[�^ID			*/
							/* Data ID			*/
} TPS_KOBJLOG;


typedef	struct {					/* �f�B�X�p�b�`���O�L�^�̈�		*/
							/* Recording area for dispatch log	*/
	TPS_LOGREC_CTRL	stRecCtrl;			/* ���O�L�^�̈搧����			*/
							/* Control information			*/
	TPS_DISPLOG	stDispLog[TPS_MAX_DISPLOG];	/* �f�B�X�p�b�`���O			*/
} TPS_DISPATCH_INFO;					/* Log buffer				*/


typedef	struct {					/* �J�[�l���I�u�W�F�N�g�L�^�̈�		*/
							/* Recording area for kernel object	*/
							/* access log				*/
	TPS_LOGREC_CTRL	stRecCtrl;			/* ���O�L�^�̈搧����			*/
							/* Control information			*/
	TPS_KOBJLOG	stKObjLog[TPS_MAX_KOBJLOG];	/* �J�[�l���I�u�W�F�N�g�A�N�Z�X���O	*/
} TPS_KOBJACCESS_INFO;					/* Log buffer				*/


typedef	struct {					/* OS��ʊi�[�e�[�u��			*/
							/* OS identity information table	*/
	char	chOSName[20];				/* OS����				*/
							/* OS name				*/
	char	chOSVersion[10];			/* �o�[�W�������			*/
							/* OS version*/
} TPS_OSIDENT_INFO;

typedef	struct {
	unsigned short	usUnitSize;			/* �I�u�W�F�N�g����{�T�C�Y	*/
							/* Unit size of target		*/
	unsigned short	usObjCnt;			/* �I�u�W�F�N�g������		*/
							/* Generated count		*/
	void		*pObjInfoAddr;			/* �I�u�W�F�N�g���z�u�A�h���X	*/
} TPS_OBJINFO_TABLE;					/* assigned address		*/


/*
 *  �v���g�^�C�v�錾
 *  Proto-type declarations
 */
extern	void	tpsInitMonitor(VP_INT);
extern	void	tpsRecDispatchLog(unsigned char, void *);
extern	void	tpsRecKObjAccessLog(unsigned short, void *, unsigned long, unsigned long);

extern	void	timer_handler_entry(void);

/*
 *  �O���ϐ�
 *  variables for external
 */
extern	void	*_kernel_tps_SymStack[];
extern	void	*_kernel_tps_CycHdrTorch;


#endif	/* _MACRO_ONLY */

#endif	/* _DBG_MON_H_ */
