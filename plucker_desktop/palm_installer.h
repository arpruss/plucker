//----------------------------------------------------------------------------------------
/*!
    \file       palm_installer.h        
    \modified                            
    \copyright  (c) Robert O'Connor ( www.oconnor.md )    
    \licence    GPL
    \brief      Includes a correct platform-specific include file for palm_installer class    
    \author     Robert O'Connor
    \date       2002/10/20    
 */  
// RCS-ID:      $Id: palm_installer.h,v 1.7 2007/06/29 00:58:54 robertoconnor Exp $
//----------------------------------------------------------------------------------------

#ifndef _PALM_INSTALLER_H_
#define _PALM_INSTALLER_H_

#ifdef __WXMSW__ 
    #include "palm_installer_msw.h"
#endif

#ifdef __WXMAC__
	#include "palm_installer_mac.h"
#endif

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
    #include "palm_installer_gtk.h"
#endif

#endif  //PALM_INSTALLER_H

