#define GPDCON     (*((volatile unsigned long *)0x7F008060))
#define GPDDAT     (*((volatile unsigned long *)0x7F008064))


#define GPNCON     (*((volatile unsigned long *)0x7F008830))
#define GPNDAT     (*((volatile unsigned long *)0x7F008834))

#define EINT0CON0  (*((volatile unsigned long *)0x7F008900))
#define EINT0MASK  (*((volatile unsigned long *)0x7F008920))

#define EINT0PEND  (*((volatile unsigned long *)0x7F008924))

#define PRIORITY    (*((volatile unsigned long *)0x7F008280))
#define SERVICE     (*((volatile unsigned long *)0x7F008284))
#define SERVICEPEND (*((volatile unsigned long *)0x7F008288))


#define VIC0IRQSTATUS  (*((volatile unsigned long *)0x71200000))
#define VIC0FIQSTATUS  (*((volatile unsigned long *)0x71200004))
#define VIC0RAWINTR    (*((volatile unsigned long *)0x71200008))
#define VIC0INTSELECT  (*((volatile unsigned long *)0x7120000c))
#define VIC0INTENABLE  (*((volatile unsigned long *)0x71200010))
#define VIC0INTENCLEAR (*((volatile unsigned long *)0x71200014))
#define VIC0PROTECTION (*((volatile unsigned long *)0x71200020))
#define VIC0SWPRIORITYMASK (*((volatile unsigned long *)0x71200024))
#define VIC0PRIORITYDAISY  (*((volatile unsigned long *)0x71200028))

#define VIC0VECTADDR0      (*((volatile unsigned long *)0x71200100))
#define VIC0VECTADDR1      (*((volatile unsigned long *)0x71200104))
#define VIC0VECTADDR25      (*((volatile unsigned long *)0x71200164))
#define VIC0ADDRESS        (*((volatile unsigned long *)0x71200f00))

#define VIC1IRQSTATUS  (*((volatile unsigned long *)0x71300000))
#define VIC1VECTADDR4      (*((volatile unsigned long *)0x71300110))
#define VIC1VECTADDR5      (*((volatile unsigned long *)0x71300114))
#define VIC1VECTADDR30      (*((volatile unsigned long *)0x71300178))
#define VIC1VECTADDR31      (*((volatile unsigned long *)0x7130017c))
#define VIC1INTENABLE  (*((volatile unsigned long *)0x71300010))
#define VIC1ADDRESS        (*((volatile unsigned long *)0x71300f00))


#define GPECON  (*((volatile unsigned long *)0x7F008080))
#define GPEDAT  (*((volatile unsigned long *)0x7F008084))
#define GPFCON  (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))
#define GPICON  (*((volatile unsigned long *)0x7F008100))
#define GPJCON  (*((volatile unsigned long *)0x7F008120))

/* display controller */
#define MIFPCON     (*((volatile unsigned long *)0x7410800C))
#define SPCON         (*((volatile unsigned long *)0x7F0081A0))
#define VIDCON0      (*((volatile unsigned long *)0x77100000))
#define VIDCON1      (*((volatile unsigned long *)0x77100004))
#define VIDTCON0     (*((volatile unsigned long *)0x77100010))
#define VIDTCON1     (*((volatile unsigned long *)0x77100014))
#define VIDTCON2     (*((volatile unsigned long *)0x77100018))
#define WINCON0      (*((volatile unsigned long *)0x77100020))
#define VIDOSD0A      (*((volatile unsigned long *)0x77100040))
#define VIDOSD0B      (*((volatile unsigned long *)0x77100044))
#define VIDOSD0C      (*((volatile unsigned long *)0x77100048))
#define VIDW00ADD0B0      (*((volatile unsigned long *)0x771000A0))
#define VIDW00ADD1B0      (*((volatile unsigned long *)0x771000D0))
#define VIDW00ADD2      (*((volatile unsigned long *)0x77100100))

#define WPALCON      (*((volatile unsigned long *)0x771001A0))
#define WIN0_PALENTRY0      0x77100400


#define ULCON0     (*((volatile unsigned long *)0x7F005000))
#define UCON0      (*((volatile unsigned long *)0x7F005004))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define UMCON0     (*((volatile unsigned long *)0x7F00500C))
#define UTRSTAT0   (*((volatile unsigned long *)0x7F005010))
#define UFSTAT0    (*((volatile unsigned long *)0x7F005018))
#define UTXH0      (*((volatile unsigned char *)0x7F005020))
#define URXH0      (*((volatile unsigned char *)0x7F005024))
#define UBRDIV0    (*((volatile unsigned short *)0x7F005028))
#define UDIVSLOT0  (*((volatile unsigned short *)0x7F00502C))
#define UINTP0      (*((volatile unsigned long *)0x7F005030))
 

#define UINTM0    (*((volatile unsigned long *)0x7F005038))


#define GPACON     (*((volatile unsigned short *)0x7F008000))

#define ADCCON           (*((volatile unsigned long*)0x7E00B000))									
#define ADCTSC           (*((volatile unsigned long*)0x7E00B004))									
#define ADCDLY           (*((volatile unsigned long*)0x7E00B008)) 									
#define ADCDAT0          (*((volatile unsigned long*)0x7E00B00C))
#define ADCDAT1          (*((volatile unsigned long*)0x7E00B010))
#define ADCUPDN          (*((volatile unsigned long*)0x7E00B014))
#define ADCCLRINT        (*((volatile unsigned long*)0x7E00B018)) 
#define Reserved         (*((volatile unsigned long*)0x7E00B01C))
#define ADCCLRINTPNDNUP  (*((volatile unsigned long*)0x7E00B020))

#define GPFCON  (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))

#define TCFG0    (*((volatile unsigned long *)0x7F006000))
#define TCFG1    (*((volatile unsigned long *)0x7F006004))
#define TCNTB0    (*((volatile unsigned long *)0x7F00600C))
#define TCMPB0    (*((volatile unsigned long *)0x7F006010))
#define TCON       (*((volatile unsigned long *)0x7F006008))
#define TCNTB2       (*((volatile unsigned long *)0x7F006024))
#define TINT_CSTAT       (*((volatile unsigned long *)0x7F006044))

/* AC97 */
#define AC_GLBCTRL    (*((volatile unsigned long*)0x7F001000))
#define AC_GLBSTAT    (*((volatile unsigned long*)0x7F001004))
#define AC_CODEC_CMD  (*((volatile unsigned long*)0x7F001008))  
#define AC_CODEC_STAT (*((volatile unsigned long*)0x7F00100C))  
#define AC_PCMADDR    (*((volatile unsigned long*)0x7F001010))
#define AC_MICADDR    (*((volatile unsigned long*)0x7F001014))
#define AC_PCMDATA    (*((volatile unsigned long*)0x7F001018))
#define AC_MICDATA    (*((volatile unsigned long*)0x7F00101C))


/* DMA */
#define SDMA_SEL                 (*((volatile unsigned long *)0x7E00F110))

#define SDMAC1_BASE              (*((volatile unsigned long *)0x75100000))
#define DMAC1Configuration       (*((volatile unsigned long *)(0x75100000 + 0x030)))
#define DMAC1Sync                (*((volatile unsigned long *)(0x75100000 + 0x034)))
#define DMAC1C0SrcAddr           (*((volatile unsigned long *)(0x75100000 + 0x100))) 
#define DMAC1C0DestAddr          (*((volatile unsigned long *)(0x75100000 + 0x104))) 
#define DMAC1C0LLI               (*((volatile unsigned long *)(0x75100000 + 0x108))) 
#define DMAC1C0Control0          (*((volatile unsigned long *)(0x75100000 + 0x10C))) 
#define DMAC1C0Control1          (*((volatile unsigned long *)(0x75100000 + 0x110))) 
#define DMAC1C0Configuration     (*((volatile unsigned long *)(0x75100000 + 0x114))) 
#define DMAC1C0ConfigurationExp  (*((volatile unsigned long *)(0x75100000 + 0x118))) 

/* SROM Controller */
#define SROM_BW       (*((volatile unsigned long *)0x70000000)) 
#define SROM_BC1      (*((volatile unsigned long *)0x70000008))

