/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_ext.h
 *	Contents:	Header file containing all the externs
 *				of the graphics module.
 */

/*
 * Local externs
 */

extern void		gr_SaveAni();
extern void		gr_SaveDicer();
extern void		gr_SaveDiskAAni();
extern void		gr_SaveDiskPAni();
extern void		gr_SaveDspl();
extern void		gr_SaveIso();
extern void		gr_SaveProc();
extern void		gr_SaveTile();
extern void		gr_SaveView();
extern void		gr_SaveVbuff();
extern void		gr_AniSetAutoRev();
extern void		gr_AniSetAutoRep();
extern void		gr_AniSetAutoOff();
extern void		gr_AniExpose();
extern void		gr_AniFrameSet();
extern void		gr_AniSkipSet();
extern void		gr_AniFrameSliderSel();
extern void		gr_AniFrameSliderMov();
extern void		gr_AniFrameSliderRel();
extern void		gr_AniSpeedFast();
extern void		gr_AniSpeedSlow();
extern void		gr_AniSpeedSliderSel();
extern void		gr_AniSpeedSliderMov();
extern void		gr_AniSpeedSliderRel();
extern void		gr_AniLoad();
extern void		gr_AniPlayStop();
extern void		gr_AniPlayForward();
extern void		gr_AniPlayBackward();
extern void		gr_AniStepForward();
extern void		gr_AniStepBackward();
extern void		gr_ArbAniOpen();
extern void		gr_ArbCubeExpose();
extern void		gr_ArbDepthExpose();
extern void		gr_ArbSetInterp();
extern void		gr_ArbSetNonInterp();
extern void		gr_ArbUseDisk();
extern void		gr_ArbUseMemory();
extern void		gr_ArbUsePixmap();
extern void		gr_ArbUseXImage();
extern void		gr_ArbViewOpen();
extern void		gr_ArbXSliderSel();
extern void		gr_ArbXSliderMov();
extern void		gr_ArbXSliderRel();
extern void		gr_ArbYSliderSel();
extern void		gr_ArbYSliderMov();
extern void		gr_ArbYSliderRel();
extern void		gr_ArbZSliderSel();
extern void		gr_ArbZSliderMov();
extern void		gr_ArbZSliderRel();
extern void		gr_BossDisplay();
extern void		gr_BossSetAxes();
extern void		gr_BossSetArb();
extern void		gr_BossSetDicer();
extern void		gr_BossSetIso();
extern void		gr_CloseAniLevel();
extern void		gr_CloseArbLevel();
extern void		gr_CloseBossLevel();
extern void		gr_CloseCubeLevel();
extern void		gr_CloseDiceLevel();
extern void		gr_CloseDsplLevel();
extern void		gr_CloseHistLevel();
extern void		gr_CloseIsoLevel();
extern void		gr_CloseFileLevel();
extern void		gr_CloseSubsLevel();
extern void		gr_CloseTileLevel();
extern void		gr_CloseViewLevel();
extern void		gr_CubeAnimate();
extern void		gr_CubeDisplayAuto();
extern void		gr_CubeDisplaySingle();
extern void		gr_CubeDisplayUnfolded();
extern void		gr_CubeDrawExpose();
extern void		gr_CubeFlipVert();
extern void		gr_CubeFlipHoriz();
extern void		gr_CubeOpenSubs();
extern void		gr_CubeRotLeft();
extern void		gr_CubeRotRight();
extern void		gr_CubeSetIsStep();
extern void		gr_CubeSetIsCont();
extern void		gr_CubeSetInterp();
extern void		gr_CubeSetNonInterp();
extern void		gr_CubeSet2D();
extern void		gr_CubeSet3D();
extern void		gr_CubeSetXY();
extern void		gr_CubeSetXZ();
extern void		gr_CubeSetYZ();
extern void		gr_CubeUseDisk();
extern void		gr_CubeUseMemory();
extern void		gr_CubeUsePixmap();
extern void		gr_CubeUseXImage();
extern void		gr_DiceExpose();
extern void		gr_DiceErase();
extern void		gr_DiceView();
extern void		gr_DiceSetDicerOn();
extern void		gr_DiceSetDicerOff();
extern void		gr_DiceSetPlanes();
extern void		gr_DiceShadeDark();
extern void		gr_DiceShadeLight();
extern void		gr_DiceShadeSel();
extern void		gr_DiceShadeMov();
extern void		gr_DiceShadeRel();
extern void		gr_DiceSetX();
extern void		gr_DiceSetY();
extern void		gr_DiceSetZ();
extern void		gr_DiceXSliderSel();
extern void		gr_DiceXSliderMov();
extern void		gr_DiceXSliderRel();
extern void		gr_DiceYSliderSel();
extern void		gr_DiceYSliderMov();
extern void		gr_DiceYSliderRel();
extern void		gr_DiceZSliderSel();
extern void		gr_DiceZSliderMov();
extern void		gr_DiceZSliderRel();
extern void		gr_DsplHistogram();
extern void		gr_DsplListHScroll();
extern void		gr_DsplListVScroll();
extern void		gr_DsplListSelect();
extern void		gr_DsplImageSelect();
extern void		gr_DsplSetSynch();
extern void		gr_DsplUnSetSynch();
extern void		gr_DsplSliderSelected();
extern void		gr_FileDirReturn();
extern void		gr_FileSaveReturn();
extern void		gr_FileSetLoadAll();
extern void		gr_FileSetLoadSingle();
extern void		gr_FileSetGlobal();
extern void		gr_FileSetLocal();
extern void		gr_FileSetSeparate();
extern void		gr_FileSetSingle();
extern void		gr_FileUseHDF();
extern void		gr_FileUseDisk();
extern void		gr_FileUseMemory();
extern void		gr_FileUseMiriad();
extern void		gr_FileUsePixmap();
extern void		gr_FileUseXImage();
extern void		gr_FileDirAccept();
extern void		gr_FileDirUp();
extern void		gr_FileDirDown();
extern void		gr_FileCheck();
extern void		gr_FileLoad();
extern void		gr_FileSave();
extern void		gr_HistExpose();
extern void		gr_IsoExpose();
extern void		gr_IsoSetXY();
extern void		gr_IsoSetXZ();
extern void		gr_IsoSetYZ();
extern void		gr_IsoView();
extern void		gr_LoadPAL();
extern void		gr_LoadPALSplit();
extern void		gr_LoadRASTER();
extern void		gr_LoadSDS();
extern void		gr_PalletLoad();
extern void		gr_PalletLoadSplit();
extern void		gr_Terminate();
extern void		gr_SubsExpose();
extern void		gr_SubsIDSliderSel();
extern void		gr_SubsIDSliderMov();
extern void		gr_SubsIDSliderRel();
extern void		gr_SubsReset();
extern void		gr_SubsSetID();
extern void		gr_SubsStart();
extern void		gr_TileSelect();
extern A_AniWind_t  *gr_InitAniLevel();
extern A_AniWind_t  *gr_InitAniLevel2();
extern A_AniWind_t  *gr_InitAniLevel3();
extern A_ArbWind_t  *gr_InitArbLevel();
extern A_BossWind_t *gr_InitBossLevel();
extern A_Box_t		*gr_CreateBox();
extern A_BoxEdge_t	*gr_CreateENode();
extern A_BoxVert_t	*gr_CreateVNode();
extern A_BoxEdge_t	*gr_CreateEList();
extern A_BoxVert_t	*gr_CreateVList();
extern A_CubeWind_t *gr_InitCubeLevel();
extern A_DiceWind_t *gr_InitDiceLevel();
extern A_DsplWind_t *gr_InitDsplLevel();
extern A_HistWind_t *gr_InitHistLevel();
extern A_IsoWind_t	*gr_InitIsoLevel();
extern A_FileWind_t *gr_InitFileLevel();
extern A_SubsWind_t *gr_InitSubsLevel();
extern A_TileWind_t *gr_InitTileLevel();
extern A_ViewWind_t *gr_InitViewLevel();
extern Widget	gr_DialogGetTextWind();
extern Widget	gr_MakeBox();
extern Widget	gr_MakeButton();
extern Widget	gr_MakeBulletin();
extern Widget	gr_MakeDialog();
extern Widget	gr_MakeImage();
extern Widget	gr_MakeImageStatic();
extern Widget	gr_MakeList();
extern Widget	gr_MakeListSimple();
extern Widget	gr_MakeListStatic();
extern Widget	gr_MakeRowColumn();
extern Widget	gr_MakeSlider();
extern Widget	gr_MakeText();
extern Widget	gr_MakeTitleBar();
extern Widget	gr_MakeToggle();
extern Widget	gr_MakeVPort();
extern Widget	gr_MakeWindow();
extern Widget	gr_MakeWorkSpace();
extern XImage	*gr_ImageCreate();
extern Pixmap	gr_PixmapCreate();
extern char		*gr_DialogGetValue();
extern char		*gr_ListgetStruct();
extern char		*gr_TextGetStr();
extern char		*gr_ViewgetData();

extern int		gr_ArbgetCurScale();
extern int		gr_ArbgetCurNumFrames();
extern double	gr_ArbgetCurZPercent();
extern int		gr_CubegetCurIncr();
extern int		gr_CubegetCurPlane();
extern int		gr_CubegetCurScale();
extern int		gr_CubegetCurNumPlanes();
extern int		gr_SliderGetValue();
extern Visual	*gr_GetVisual();

/*
 * Inter-module externs
 */

extern int		td_FileIsDir();
extern int		td_FileIsHdf();
extern int		td_FileExist();
extern char		**td_FileDirStrings();
extern char		*td_CurrentDate();
extern char		*td_getDirName();
extern char		*td_getLogFileName();
extern char		*td_getToolName();
extern char		*td_HdfgetRasData();
extern char		*td_HdfgetRaster();
extern char		*td_HdfgetStats();
extern char		*td_HdfgetPixData();
extern char		**td_HdfgetHScale();
extern char		**td_HdfgetVScale();
extern char		**td_HdfgetPlaneData();
extern char		td_HdfConvertToPix();
extern char		td_HdfConvertToSplit();
extern int		td_HdfR8restart();
extern int		td_HdfPalrestart();
extern int		td_HdfSDrestart();
extern int		td_HdfgetDim();
extern int		td_HdfCheck();
extern int		td_HdfLoad();
extern int		td_HdfgetRasDims();
extern int		td_HdfAddImage();
extern int		td_HdfPutImage();
extern int		td_MiriadLoad();
extern double	td_getVersion();
extern char		*td_Malloc();
extern char		*td_Malloc1D();
extern char		**td_Malloc2D();
extern char		**td_Malloc2DChar();
extern int		**td_Malloc2DInt();
extern A_VRect_t	**td_Malloc2DVRect();
extern float32	***td_Malloc3Dfloat32();
