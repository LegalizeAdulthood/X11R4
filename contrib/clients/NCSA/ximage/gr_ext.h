/*
 *	File:		gr_ext.h
 *	Contents:	Header file containing all the externs
 *				of the graphics module.
 */

/*
 * Local externs
 */

extern void		gr_BossDisplay();
extern void		gr_BossSetAxes();
extern void		gr_BossSetArb();
extern void		gr_CloseBossLevel();
extern void		gr_CloseCubeLevel();
extern void		gr_CloseDsplLevel();
extern void		gr_CloseTileLevel();
extern void		gr_CubeDisplayAuto();
extern void		gr_CubeDisplaySingle();
extern void		gr_CubeDisplaySelected();
extern void		gr_CubeFlipVert();
extern void		gr_CubeFlipHoriz();
extern void		gr_CubeRotLeft();
extern void		gr_CubeRotRight();
extern void		gr_CubeSetFrame();
extern void		gr_CubeSetInterp();
extern void		gr_CubeSetNonInterp();
extern void		gr_CubeSetXY();
extern void		gr_CubeSetXZ();
extern void		gr_CubeSetYZ();
extern void		gr_DsplListHScroll();
extern void		gr_DsplListVScroll();
extern void		gr_DsplSetVal();
extern void		gr_DsplSliderSelected();
extern void		gr_FileDirAccept();
extern void		gr_FileCheck();
extern void		gr_FileLoad();
extern void		gr_ImageSelect();
extern void		gr_PalletLoad();
extern void		gr_Terminate();
extern void		gr_TileSelect();
extern A_BossWind_t *gr_InitBossLevel();
extern A_BossWind_t *gr_MallocBoss();
extern A_CubeWind_t *gr_InitCubeLevel();
extern A_CubeWind_t *gr_MallocCube();
extern A_DsplWind_t *gr_InitDsplLevel();
extern A_DsplWind_t *gr_MallocDspl();
extern A_TileWind_t *gr_InitTileLevel();
extern A_TileWind_t *gr_MallocTile();
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
extern char		*gr_DialogGetValue();
extern char		*gr_ListgetStruct();
extern char		*gr_TextGetStr();

extern int		gr_CubegetCurIncr();
extern int		gr_CubegetCurPlane();
extern int		gr_CubegetCurScale();
extern int		gr_CubegetCurNumPlanes();
extern short	gr_CubegetCurTile();

/*
 * Inter-module externs
 */

extern char		**td_FileDirStrings();
extern char		*td_CurrentDate();
extern char		*td_getDirName();
extern char		*td_getLogFileName();
extern char		*td_getToolName();
extern char		*td_HdfgetStats();
extern char		*td_HdfgetPixData();
extern char		**td_HdfgetHScale();
extern char		**td_HdfgetVScale();
extern char		**td_HdfgetPlaneData();
extern int		td_HdfgetScale();
extern int		td_HdfgetDim();
extern int		td_HdfCheck();
extern int		td_HdfLoad();
extern double	td_getVersion();
