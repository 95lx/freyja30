/* SYSMGR.H -- HP95LX System Manager Definitions */

/* The following are excerpts from files Copyright 1990 by
Hewlett-Packard and Copyright 1990 by Lotus Development Corporation. */

/* ============================================================ */
/* #include "edit.h" */

typedef struct {
	char    far *m_buffer;    /*  user-supplied edit buffer  */
	int     m_length;         /*  length of buffer  */
	int     m_pos;            /*  current cursor position  */
	int     m_row, m_col;     /*  location of edit area  */
	int     m_nrows, m_ncols; /*  dimensions of edit area  */
	int	m_yoff,m_disprows;/*  current top row, number of rows displayable */
	char    m_ccol;           /*  cursor column  */
	char    m_modified;       /*  1 -> buffer has been modified  */
        char    m_xoff;           /*  1st disp. col (for ticker fields only)*/
        char    wrapflag;         /*  1 -> wordwrap enabled,-1 ticker field */
	int     far *m_line;      /*  pointer to array used for line starts */
				  /*     must be at least m_nrows+1 long */
	char 	markon;		  /* currenty marking? */
	char	spec_flag;
	int	markst;		  /* offset of start of mark, inclusive */
	int	markend;	  /* offset of end of mark, inclusive */
        int     m_dispcols;       /*  displayable columns */
} MDITDATA;


typedef struct {
	int edit_length; 	/* current length of the edit buffer */
	char first_time;      	/* flag for special processing on first char */
	char spec_flags;	/* bit 0 is tab handling */
     	int prompt_window;      /* whether this belongs to the prompt window */
	char far *message_line; /* the top line message for prompt */
	int message_line_length;/* length of same */
	char far *prompt_line;  /* second line of prompt window */
	int prompt_line_length; /* length of same */
	char edit_buffer[80];   /* work space for the buffer */
	int  line_array[2];	/* line array needed by mdit */
	MDITDATA mdit;          /* multi-line edit structure */
	int e_dispcols;
} EDITDATA;

/* ============================================================ */
/* #include "event.h" */

enum event_kind {
	E_NONE,		/* no events available */
	E_KEY,		/* keystroke available */
	E_BREAK,	/* CNTRL_BREAK encountered */
	E_ACTIV,	/* reactivation event, always follows E_DEACT */
	E_DEACT,	/* about to sleep, next event request is delayed */
	E_TERM,		/* forced closure of application */
	E_BRIDGE,	/* 123 bridge service request, only ret. to 123 */
	E_GROW,	 	/* request to 123 to grow */
	E_SHRINK,	/* request to 123 to shrink */
	E_ALARM_EXP,	/* application's alarm has expired */
	E_ALARM_DAY,	/* daily chance to set an alarm */
	E_TIMECHANGE	/* system date or time has been changed */
};

typedef struct {
	enum event_kind kind;
	unsigned int data;	/* ASCII (CP 850) translation of keystroke */
	unsigned char scan;	/* scan code from BIOS */
	unsigned char shifts;	/* shifts register, when function returns */
				/* not necessarily when key struck */
	unsigned int lics;	/* LICS translation of keystroke */
	unsigned char fkey_num;	/* function key number for 123 only */
	void far * bridge;	/* pointer to 123 bridge data structure */
} EVENT;


/* ============================================================ */
/* #include "fmenu.h" */

/* define return codes */

#define	RET_BADFILE		-5
#define	RET_BADDIR		-4
#define	RET_BADDRIVE	-3
#define	RET_UNKNOWN		-2
#define	RET_BAD			-1
#define	RET_OK			0
#define	RET_REDISPLAY	1
#define	RET_ACCEPT		2
#define	RET_ABORT		3

/* file info structures created in FM_BUFFER */
#pragma pack(1)
typedef	struct {
	char fi_attr;				/*  file attribute  */
	int fi_time;				/*  time modified  */
     	int fi_date;				/*  date modified  */
	long fi_size;  				/*  file length  */
     	char fi_name[13];			/*  file name  */
} FILEINFO;                                  

/* define the menu structure */
typedef struct {
	char far *fm_path;		/* base directory name C:\DATA\ */
	char far *fm_pattern;		/* file pattern, e.g.  *.WK1    */
	FILEINFO far *fm_buffer;	/* workspace  for file list (hold finfos) */
	int fm_buf_size;		/* size of the buffer in bytes */
     	int fm_startline,fm_startcol;	/* starting row,col */
	int fm_numlines, fm_numcols;	/* number of lines and columns */
	int fm_filesperline;		/* nuumber of files displayed across */
/* -- Initted in service -- */
	int fm_firstedit;			/* 0 if first edit char, else multi line */
	int fm_filesinbuf;		/* number of files in list */
	int fm_maxfinbuf;		/* maximum number of files buffer holds */
	int fm_topfile;			/* file at top of list */
	int fm_curselect;		/* index of file to highlight */
	int fm_oldselect;		/* index of file to un-highlight */
	int fm_focus;			/* 1 = fmenu, 2 = edit */
} FMENU;


/* ============================================================ */
/*#include "menu.h"*/

#define   MAX_KWDS    20
#define   MAX_MENU     3
#define   MAX_MWID     80

typedef struct {
     char menu_text [MAX_MENU][MAX_MWID];
     int menu_count;
     int menu_highlight;
     int menu_tprompt;
     char menu_line   [MAX_KWDS];
     char menu_offset [MAX_KWDS];
     char menu_length [MAX_KWDS];
     char menu_letter [MAX_KWDS];
    unsigned menu_prompt[MAX_KWDS];
     } MENUDATA;


/* ============================================================ */
/*#include "settings.h"*/
#define MAX_MACRO_LEN	256

#define NAMEBUFLEN	30
#define TITLEBUFLEN	30
#define COMPANYBUFLEN	28

/*  settings */
typedef struct {
	int	Country;	/* */
	int	Volume; 	/* 1 speaker volume level 0ffh for off and 0-3 */
	int	Contrast;	/* 2 contrast level 1-5 */
	int	WeekStart;	/* 3 week starts SUNDAY or Monday */
	int	Punctuation;	/* 4 Punctuation format */
	int 	Language;	/* two character code for current language */
	int	DateFormat;	/* 6 current date format */
	int	TimeFormat;	/* 7 current time format */
	int	Sort;		/* 8 collating sequence */
	char	PicFile[80];	/* picture file name */
	char	Name_Buff[NAMEBUFLEN];	
	char	Title_Buff[TITLEBUFLEN]; 
	char	Company_Buff[COMPANYBUFLEN]; 
	int 	LanguageCount;
	int	LanguagesAvail[6];	/* MAX_LANG */
	char	LanguageMenu[66];	

	char	DateSeparator[2]; /* date separator  can be /-. */
	char	TimeSeparator[2]; /* time separator can be :., or h (for hms) */
	char	DateOrder;	/* */
	char	Time24;		/* use 24 hour time? */

	char	Currency[16];   /* currency string */
	int	CurrencyFix;   /* prefix or sufix */

	int	Keyboard;
	int	PrBaud; 	/* printer baud rate */
	int	PrDriver;	/* epson, hp, etc, by code */
	int 	PrInterface;	/* com 1, etc  */
	int	SystemRupt;		/* sysmgr interupt number - 60 default */
	int	CodePage;		/* for PC based - 437 or 850 based */
	int	ExitKeyCode;	/* for PC based - active exit key */
	int	MenuKeyCode;	/* for PC based - active menu key */
	int	CharKeyCode;	/* for PC based - active CHAR key toggle */
	int	Alarm[6];
} SETTINGS;


typedef struct {
	char	MacroFK;	   /* MacroFunctionKey Number 1-10 0 for not used */
        unsigned char MacroSize;   /* elements in the macro */
        char    CommentString[38]; /* MacroFunctionKey's Comment line */
	unsigned int MacroBody[MAX_MACRO_LEN];
} MACROHD;


/* ============================================================ */
/* include "task.h" from HP100LX kit */

#ifdef	 MOTOWN
#define TASKCOUNT	        24
#define MaxAppsFileSize	    1024
#define MaxRAMApps	        TASKCOUNT
#else
#define TASKCOUNT	        30
#define MaxAppsFileSize	    512
#define MaxRAMApps	        10
#endif


#define	ACC_NAME_LEN	12		/* sufficient to store filename */

typedef unsigned char UChar;
typedef unsigned int  UInt;


struct task {
	unsigned  t_sp;			    /* offset made low, mdb     */
	unsigned  t_ss;
	unsigned  t_off_image;		/* offset made low, mdb     */
	unsigned  t_seg_image;		
	unsigned  t_ds;
	unsigned  t_memseg;		    /* base of allocation block */
	unsigned  t_hotkey;		    /* copied from IMAGEVEC     */

    unsigned  t_phypage[2];     /* IMAGEVEC.wAS_PhyPage     */
    unsigned  t_logicalpage[2]; /* IMAGEVEC.wRS_LogicalPage */
    unsigned  t_pagecount[2];   /* IMAGEVEC.wRS_PageCount   */
	char	  t_chipsel[2];
	char	  t_state;
	char	  t_is123;
	unsigned  t_rsrc_seg;
	char	  t_nowait;
	char	  t_sys_ver;
	char	  t_extname[12];
	char      t_lock_state;
	char	  t_spare[3];
	unsigned  t_far_size;
	unsigned  t_far_off;
	unsigned  t_far_rsvrd;
	char	  t_sp_status;
	char	  t_sp_link;
	unsigned  t_ic_u_loc;
	unsigned  t_ic_o_loc;


};


struct rsrc_entry {
	char re_name[13];
	char re_pad;
	unsigned long re_romloc;
};


#ifdef XBS
  #ifdef EMUL
  #define ImagevecSeg  0x9000
  #else
  #define ImagevecSeg  0xE000
  #endif
#else
  #ifdef EMUL
  #define ImagevecSeg  0x9000
  #else
  #define ImagevecSeg  0xF000
  #endif
#endif

#define VIDEOGRSEG   0xb800
#define VIDEOGROFF   0x0000
#define VIDEOGRLINELEN 0x50
#define VIDEOBASE    ((void far *)0xB8000000)

#define ImagevecOff0 0x7

/* ============================================================ */
/* #include "headers/time.h" */
typedef struct {
	int	dt_year;
	char	dt_month;
	char	dt_date;
	char	dt_day;
	char	dt_hour;
	char	dt_minute;
	char	dt_second;
	char	dt_hundreth;
} DTM;


/* ============================================================ */
/* #include "interfac.h" */

   #define  CLASS_BASE     0x0
   #define  FNCT_BASE      0x0

#define SC_EVENT	1
#define SC_MENU 	2
#define SC_SCREEN	3
#define SC_EDIT 	4
#define SC_FILE 	5
#define SC_PM		6
#define SC_CB		7
#define SC_SOUND	8
#define SC_MM           (CLASS_BASE + 9)
#define SC_DTIME	11
#define SC_MISC 	15
#define SC_VIDVIR	16	

#define F_M_EVENT	(SC_EVENT * 256) + 0
#define m_event(a)	\
	c_service(F_M_EVENT,(void far *)(a) )

#define F_M_NEVENT    (SC_EVENT * 256) + 1
#define m_nevent(a)	\
	c_service(F_M_NEVENT,(void far *)(a))

#define F_M_NO_FINI	(SC_EVENT * 256) + 7
#define m_no_fini(a)	\
	c_service(F_M_NO_FINI,(void far *)(a) )



#define F_MENU_SETUP	(SC_MENU * 256) + 0
#define menu_setup(a,b,c,d,e,f,g)	\
	c_service(F_MENU_SETUP,(void far *)(a),(void far *)(b), \
		(c),(d),(void far *)(e),(f),(void far *)(g) )

#define F_MENU_DIS	(SC_MENU * 256) + 1
#define menu_dis(a)	\
	c_service(F_MENU_DIS,(void far *)(a))

#define F_MENU_ON	(SC_MENU * 256) + 2
#define menu_on(a)	\
	c_service(F_MENU_ON,(void far *)(a))

#define F_MENU_OFF	(SC_MENU * 256) + 3
#define menu_off(a)	\
	c_service(F_MENU_OFF,(void far *)(a))

#define F_MENU_KEY	(SC_MENU * 256) + 4
#define menu_key(a,b,c) \
	c_service(F_MENU_KEY,(void far *)(a),(b),(void far *)(c))

#define F_FMENU_INIT	  (SC_MENU * 256) + 5
#define fmenu_init(a,b,c,d,e)	  \
	c_service(F_FMENU_INIT,(void far *)(a),(void far *)(b),(char far *)(c),(d),(e))

#define F_FMENU_DIS	 (SC_MENU * 256) + 6
#define fmenu_dis(a,b)	  \
	c_service(F_FMENU_DIS,(void far *)(a),(void far *)(b))

#define F_FMENU_KEY	 (SC_MENU * 256) + 7
#define fmenu_key(a,b,c) \
	c_service(F_FMENU_KEY,(void far *)(a),(void far *)(b),(c))

#define F_FMENU_OFF	  (SC_MENU * 256) + 8
#define fmenu_off(a,b)	 \
	c_service(F_FMENU_OFF,(void far *)(a),(void far *)(a))



#define F_M_DISP	(SC_SCREEN * 256) + 0
#define m_disp(a,b,c,d,e,f)		\
	c_service(F_M_DISP,a,b,(void far *)(c),d,e)

#define F_M_CLEAR	(SC_SCREEN * 256) + 1
#define m_clear(a,b,c,d)	     \
	c_service(F_M_CLEAR,a,b,c,d)



#define F_EDIT_TOP	(SC_EDIT * 256) + 1
#define edit_top(a,b,c,d,e,f,g,h)	   \
	c_service(F_EDIT_TOP,(void far *)(a),(void far *)(b),c,d,(void far *)(e), \
		   f,(void far *)(g),h)

#define F_EDIT_DIS	(SC_EDIT * 256) + 2
#define edit_dis(a)			\
	c_service(F_EDIT_DIS,(void far *)(a))

#define F_EDIT_KEY	(SC_EDIT * 256) + 3
#define edit_key(a,b,c) 		\
	c_service(F_EDIT_KEY,(void far *)(a),b,(void far *)(c))



#define F_M_GET_SYSDIR	(SC_FILE * 256) + 22
#define m_get_sysdir(a) 	      \
	c_service(F_M_GET_SYSDIR,(void far *)(a))



#define F_M_INIT	(SC_PM * 256) + 0
#define m_init()	c_service(F_M_INIT)

#define F_M_FINI	(SC_PM * 256) + 1
#define m_fini()	c_service(F_M_FINI)

#define F_M_LOCK	(SC_PM * 256) + 2
#define m_lock()	c_service(F_M_LOCK)

#define F_M_UNLOCK	(SC_PM * 256) + 3
#define m_unlock()	c_service(F_M_UNLOCK)

#define F_M_SPAWN	(SC_PM * 256) + 4
#define m_spawn(a,b,c,d)	\
	c_service(F_M_SPAWN,(void far *)(a),b,c, (void far *)(d))



#define F_M_OPEN_CB	(SC_CB * 256) + 0
#define m_open_cb()		\
	c_service(F_M_OPEN_CB)

#define F_M_CLOSE_CB	(SC_CB * 256) + 1
#define m_close_cb()		\
	c_service(F_M_CLOSE_CB)

#define F_M_RESET_CB	(SC_CB * 256) + 2
#define m_reset_cb(a)		\
	c_service(F_M_RESET_CB,(void far *)(a))

#define F_M_NEW_REP	(SC_CB * 256) + 4
#define m_new_rep(a)		 \
	c_service(F_M_NEW_REP,(void far *)(a))

#define F_M_FINI_REP	(SC_CB * 256) + 5
#define m_fini_rep()		\
	c_service(F_M_FINI_REP)

#define F_M_REP_INDEX	(SC_CB * 256) + 7
#define m_rep_index(a,b,c)	\
	c_service(F_M_REP_INDEX,(void far *)(a),(void far *)(b),(void far *)(c))

#define F_M_CB_WRITE	(SC_CB * 256) + 8
#define m_cb_write(a,b) 	\
	c_service(F_M_CB_WRITE,(void far *)(a),b)

#define F_M_CB_READ	(SC_CB * 256) + 9
#define m_cb_read(a,b,c,d)	\
	c_service(F_M_CB_READ,a,b,(void far *)(c),d)



#define F_M_THUD	(SC_SOUND * 256) + 1
#define m_thud()	c_service(F_M_THUD)



#define F_M_ALLOC_LARGE (SC_MM * 256) + 3 + FNCT_BASE
#define m_alloc_large(a)                \
	c_service(F_M_ALLOC_LARGE,a)

#define F_M_FREE_LARGE  (SC_MM * 256) + 4 + FNCT_BASE
#define m_free_large(a)         \
	c_service(F_M_FREE_LARGE,a)

#define F_M_GET_TCB            (SC_MM * 256) + 12 + FNCT_BASE
#define m_get_TCB()             \
	c_service_far_ptr(F_M_GET_TCB)



#define F_M_GETDTM	(SC_DTIME * 256) + 1
#define m_getdtm(a)	      \
	c_service(F_M_GETDTM,(void far *)(a))

#define F_M_TELLTIME	 (SC_DTIME * 256) + 8
#define m_telltime(a,b,c)      \
	c_service(F_M_TELLTIME,a,b,c)

#define F_M_GET_SETTINGS (SC_DTIME *256) + 9
#define m_get_settings(a,b)      \
	c_service(F_M_GET_SETTINGS, (void far *)(a),(void far *)(b))

#define F_M_POSTTIME (SC_DTIME * 256) + 20
#define m_posttime()      \
        c_service(F_M_POSTTIME)



#define F_MESSAGE	(SC_MISC * 256) + 3
#define message(a,b,c,d)	       \
	c_service(F_MESSAGE,(void far *)(a),b,(void far *)(c),d)

#define F_MSG_OFF	(SC_MISC * 256) + 4
#define msg_off()		\
	c_service(F_MSG_OFF)

#define F_M_GET_TCB_SIZE (SC_MISC * 256) + 19 + FNCT_BASE
#define m_get_TCB_size()	    \
	c_service(F_M_GET_TCB_SIZE )



#define	F_M_DIRTY_SYNC		((SC_VIDVIR * 256) + 5)
#define	m_dirty_sync()			\
   c_service(F_M_DIRTY_SYNC)



/* end of SYSMGR.H -- HP95LX System Manager Definitions */
