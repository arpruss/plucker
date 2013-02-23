/*
 * $Id: axxpacimp.c,v 1.6 2003/09/24 13:06:43 prussar Exp $
 *
 * Viewer - a part of Plucker, the free off-line HTML viewer for PalmOS
 * Copyright (c) 1998-2001, Mark Ian Lillywhite and Michael Nordstrom
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#include "config.h"
#include "axxpacimp.h"

#include "util.h"
  
/***********************************************************************
 *
 *      Local functions
 *
 ***********************************************************************/
static Int16 GetFilePos(axxPacFD FileDesc) AXXPAC_SECTION;
static Int16 RemoveFileRec(FileRef fileRef) AXXPAC_SECTION;
static void LoadOffsets(axxPacFD) AXXPAC_SECTION;
static Int16 LoadAxxPac(void) AXXPAC_SECTION;
static Int16 CreateFileRef() AXXPAC_SECTION;
static Int32 GetFileProperties (FileRef fileRef, file_rec_t* axxFileRec) 
    AXXPAC_SECTION;
static Err WriteDataToFile(const void* dataP, UInt32* sizeP, void* fdP) 
    AXXPAC_SECTION;
static Err ReadDataFromFile(void* dataP, UInt32* sizeP, void* fdP) 
    AXXPAC_SECTION;
static Boolean DeleteProc(const char* nameP, UInt16 version, UInt16 cardNo,
            LocalID dbID, void* userDataP) AXXPAC_SECTION;
static Int32 UpdateFileDesc(FileRef fileRef, axxPacFD FileDesc) AXXPAC_SECTION;
/***********************************************************************
 *
 *      Private variables
 *
 ***********************************************************************/
static MemHandle axxFileListHandle;
static MemHandle moff;
static UInt16 LibRef;
static UInt16 nrec;
static Int16 axxPacStatus       = AXXPAC_STATUS_UNKNOWN;
static Int16 axxFLSize          = 0;
static Int16 currFileDesc    = -1; 
static Char label[32]           ="";



Boolean IsAxxPacPresent(void)
{
    return (axxPacStatus == AXXPAC_PRESENT);
}



/* try to load the AxxPac library to check if AxxPac is present */
Int16 LoadAxxPac(void)
{
    Err err;

    /* try to open already installed axxPac driver lib */
    err = SysLibFind(AXXPAC_LIB_NAME, &LibRef);

    /* didn't work, try to load it */
    if ( err != errNone )
        err = SysLibLoad('libr', AXXPAC_LIB_CREATOR, &LibRef);

    if ( err != errNone ) {
        /* ERROR: unable to access library */
        axxPacStatus = AXXPAC_NOT_PRESENT;
    }
    else {
        /* try to open lib */
        if ( axxPacLibOpen(LibRef) != 0 ) {
            /* ERROR: unable to open library */
            axxPacStatus = AXXPAC_NOT_PRESENT;
        }
        else {
            axxPacStatus = AXXPAC_PRESENT;
        }
    }
    return axxPacStatus;
}



Err  InitializeAxxPac(void)
{
    if ( axxPacStatus == AXXPAC_STATUS_UNKNOWN ) {
        LoadAxxPac();
    }
    if ( axxPacStatus == AXXPAC_PRESENT )
        return errNone;
    else
        return ftrErrNoSuchFeature;
}
 

/* close AxxPac library and remove it if necessary */
void TeardownAxxPac(void)
{
    UInt16 numapps;
    Err err;

    if ( axxPacStatus == AXXPAC_PRESENT) {
        err = axxPacLibClose(LibRef, &numapps);
        if ( ( err == errNone ) && ( numapps == 0 ) )
            SysLibRemove( LibRef );
        axxPacStatus=AXXPAC_STATUS_UNKNOWN;
    }
}


/* Wrappers to VFS functions */

Err vfsVolumeEnumerate(UInt16 *volRefNumP, UInt32 *volIteratorP)
{
    *volRefNumP = 1;
    if ( volIteratorP != NULL ) {
        if ( *volIteratorP == vfsIteratorStart ) {
            *volRefNumP = LibRef;
            *volIteratorP = vfsIteratorStop;
        }
    }
    return errNone;
}



extern Err vfsVolumeGetLabel(UInt16 volRefNum, Char *labelP, UInt16 bufLen)
{
    axxPacCardInfo cardinfo;
    Int32 ret;

    /* If the label was already retrieved, return it directly */
    if ( StrLen( label ) != 0 ) {
        StrNCopy( labelP, label, min( bufLen, 32 ) );
        return errNone;
    }

    ret = axxPacGetCardInfo(LibRef, &cardinfo);

    if ( ret == 0 ) {
        StrNCopy(labelP, cardinfo.vendor_name, min(bufLen, 16));
        StrNCat (labelP, cardinfo.device_name, bufLen) ;
        /* Keep the value for faster future accesses */
        StrNCat (label, labelP, min(bufLen, 32));
        return errNone;
    }
    else {
        return vfsErrVolumeBadRef;  
    }
}



Err vfsVolumeInfo(UInt16 volRefNum, VolumeInfoType *volInfoP)
{
    volInfoP->attributes = 0;
    volInfoP->fsType = vfsFilesystemType_VFAT;
    return errNone;
}



Err vfsFileOpen(UInt16 volRefNum, const Char *pathNameP, UInt16 openMode,
        FileRef *fileRefP)
{
    axxPacFD FileDesc = 0;                       
    Int16 fileRef;
    axxPacMode mode = 0;
    Char* strErr;
    Err ret;
    axxPacStatType stat;
    file_rec_t* axxFileList;

    ret = axxPacStat( volRefNum, (Char*)pathNameP, &stat );
    if ( ret != 0 ) {
      return vfsErrFileNotFound;
    }
    if ( ! ( stat.attrib & FA_SUBDIR ) ) {
        /* only files can be opened in the AxxPac */
        if ( openMode & vfsModeWrite )
            mode = O_RdWr;
        else if ( openMode & vfsModeRead )
            mode = O_RdOnly;
        else if ( openMode & vfsModeCreate )
            mode = O_Creat;
        else
            return vfsErrFilePermissionDenied;

        FileDesc = axxPacOpen( volRefNum, (Char*) pathNameP, mode );
        if ( FileDesc < 0 ) {
            strErr= axxPacStrError( volRefNum );
            /* errors need to be mapped to VFS errors */
            if ( strErr[3]=='s' ) 
                return vfsErrFileNotFound;
            return expErrNotOpen;
        }
        /* create the File Ref that will be identify the file even if  */
        /* the axxPac closes the file by going to sleep mode */
        fileRef = CreateFileRef();
    }
    else {
      /* folders will just have a virtual file descriptor */
        FileDesc = CreateFileRef();
        fileRef  = FileDesc;
    }

    if ( axxFLSize == 0 ) {
        axxFileListHandle = MemHandleNew( sizeof( file_rec_t ) );
    }
    else {
        ret = MemHandleResize( axxFileListHandle, sizeof( file_rec_t ) *
                                                 ( axxFLSize + 1 ) );
        if ( ret != 0 )
            goto ERROR_FO;
    }
    axxFileList = MemHandleLock(axxFileListHandle);
    /* some Axxpac functions require the path, others require the FileDesc,
       store the FileDesc, attrib and path in a table */
    StrCopy (axxFileList[axxFLSize].name, pathNameP);
    axxFileList[axxFLSize].fd = FileDesc;
    axxFileList[axxFLSize].attrib = stat.attrib;
    axxFileList[axxFLSize].size = stat.size;
    axxFileList[axxFLSize].position=0;
    axxFileList[axxFLSize].mode=mode;
    axxFileList[axxFLSize].fileRef=fileRef;
    axxFLSize++; 
    ret = MemHandleUnlock(axxFileListHandle);
    *fileRefP = (FileRef)fileRef;  

    return errNone ;

ERROR_FO:
  axxPacClose(volRefNum, FileDesc);
  return vfsErrFilePermissionDenied;
}



Err vfsFileClose (FileRef fileRef)
{
    Err err = errNone;
    file_rec_t axxFileRec;
    
    if ( GetFileProperties ( fileRef, &axxFileRec ) != errNone )
               return vfsErrFileBadRef;
    if ( ! ( axxFileRec.attrib & FA_SUBDIR ) ) {
        /* only files were actually opened */
        err = axxPacClose( LibRef, axxFileRec.fd );
    }
    if ( ( err == errNone ) || ( err==AXXPAC_ERR_FILE_CLOSED ) ) {
       /* Note: if it is a folder err will also be errNone since
        it was initialized to errNone */
       RemoveFileRec( fileRef );
       return errNone ;
    }
    else {
        return expErrNotOpen;
    }
}



Err vfsFileDBInfo(FileRef ref, Char *nameP, UInt16 *attributesP,
        UInt16 *versionP, UInt32 *crDateP, UInt32 *modDateP,
        UInt32 *bckUpDateP, UInt32 *modNumP, MemHandle *appInfoHP,
        MemHandle *sortInfoHP, UInt32 *typeP, UInt32 *creatorP,
        UInt16 *numRecordsP)
{
    DatabaseHdrType DBHdr;
    file_rec_t axxFileRec;
    
    if (GetFileProperties (ref,&axxFileRec)!=errNone)
               return vfsErrBadData;    
    if (axxFileRec.attrib & FA_SUBDIR) {
        return vfsErrBadData;
    }
    if (axxPacRead(LibRef, axxFileRec.fd, &DBHdr, sizeof(DatabaseHdrType)) !=
        sizeof(DatabaseHdrType)) {
        return vfsErrBadData;
    }
    if (nameP != NULL)
        StrCopy(nameP, DBHdr.name);

    if (attributesP != NULL)
        *attributesP = DBHdr.attributes;

    if (versionP != NULL)
        *versionP=DBHdr.version;

    if (crDateP != NULL)
        *crDateP = DBHdr.creationDate;

    if (modDateP != NULL)
        *modDateP = DBHdr.modificationDate;

    if (bckUpDateP != NULL)
        *bckUpDateP = DBHdr.lastBackupDate;

    if (modNumP != NULL)
        *modNumP = DBHdr.modificationNumber;

    /* assumption: appInfoHP is NULL */
     ASSERT_MSG("AIM5", appInfoHP == NULL);
    /* assumption: sortInfoHP is NULL */
    ASSERT_MSG("AIM5", sortInfoHP == NULL);

    if (typeP != NULL)
        *typeP = DBHdr.type;

    if (creatorP != NULL)
        *creatorP = DBHdr.creator;

    if (numRecordsP != NULL)
        *numRecordsP = DBHdr.recordList.numRecords;
    
    return errNone;
}



/* This function maps to two in the AxxPac API:
   axxPacFindFirst and axxPacFindNext */
Err vfsDirEntryEnumerate(FileRef dirRef, UInt32 *dirEntryIteratorP, 
        FileInfoType *infoP)
{
    static axxPacFileInfo axxFileInfo;
    file_rec_t axxFileRec;
    Char symb[2] = "*\0";
    Err ret;
  
    if (*dirEntryIteratorP == vfsIteratorStart) {
        if (GetFileProperties (dirRef,&axxFileRec)!=errNone)
               return vfsErrFileBadRef;
        /* Use the wildcar '*' after the dir name to use as a pattern to
           search */
        StrCat(axxFileRec.name, symb);

        /* assumption: there is only one directory enumerated at a time */
        ret = axxPacFindFirst(LibRef, axxFileRec.name, &axxFileInfo,
                  FIND_FILES_ONLY);
        if (ret == 0) {
            infoP->attributes = axxFileInfo.attrib ;
            /* nameP will store the file name only if it is not null */
            if (infoP->nameP != NULL)
                StrNCopy(infoP->nameP, axxFileInfo.name,
                    min(256, infoP->nameBufLen)); 
        }
        else {
            if (0 < ret) {
                *dirEntryIteratorP = vfsIteratorStop;
                return expErrEnumerationEmpty;
            }
            else {
                /* error should be mapped but... */
                return vfsErrFileBadRef;
            }
        }
        ret = axxPacFindNext (LibRef, &axxFileInfo);
        if (0 < ret)
            *dirEntryIteratorP = vfsIteratorStop;
        else if (ret == 0)
            *dirEntryIteratorP = (Int32)&axxFileInfo;

        return errNone;
    }
    else {
        /* assumption: a call with vfsIteratorStart has been made before */
        infoP->attributes = axxFileInfo.attrib ;
        /* nameP will store the file name only if it is not null */
        if (infoP->nameP != NULL)
            StrNCopy(infoP->nameP, axxFileInfo.name,
                min(infoP->nameBufLen, 256));
        ret=axxPacFindNext (LibRef, &axxFileInfo);
        if (0 < ret)
            *dirEntryIteratorP = vfsIteratorStop;
        else if ( ret == 0 )
            *dirEntryIteratorP = (Int32)&axxFileInfo;

        return errNone;
    }
}



Err vfsFileSize
        (
        FileRef fileRef, 
        UInt32 *fileSizeP
        )
{
    file_rec_t axxFileRec;
    
    if (GetFileProperties (fileRef,&axxFileRec)!=errNone)
               return vfsErrFileBadRef;
    *fileSizeP=axxFileRec.size;
    return errNone;   
}



Err vfsFileGetAttributes
    (
    FileRef fileRef, 
    UInt32 *attributesP
    )
{
    file_rec_t axxFileRec;

    if (GetFileProperties (fileRef,&axxFileRec)!=errNone)
               return vfsErrFileBadRef;
    *attributesP =axxFileRec.attrib;
    return errNone;
}



Err vfsFileDBGetRecord
    (
    FileRef ref,
    UInt16 recIndex,
    MemHandle *recHP,
    UInt8 *recAttrP,
    UInt32 *uniqueIDP
    )
{
    UInt32 offset;
    UInt32 length;
    MemPtr mp;
    Char* buf;
    file_rec_t axxFileRec;
    Err err;

    if (GetFileProperties (ref,&axxFileRec)!=errNone)
               return dmErrNotRecordDB;

    if (currFileDesc != axxFileRec.fd)
        LoadOffsets(axxFileRec.fd);

    /*Get the record byte position withing the file */
    mp = MemHandleLock(moff);
   ASSERT_MSG("AIM0", mp != 0);

    offset = ((UInt32*)mp)[recIndex * 2];
    length = recIndex < nrec ? ((UInt32*)mp)[recIndex * 2 + 2] : 
                 axxFileRec.size;
    MemHandleUnlock(moff);
    if ( nrec < recIndex) {
        return dmErrIndexOutOfRange;
    }
     length -= offset;
    err=axxPacSeek(LibRef, axxFileRec.fd, offset, SEEK_SET);
    if (err==AXXPAC_ERR_FILE_CLOSED) {
         axxPacFD FileDesc;
        /*This happens when the axxPac has entered into sleep mode */
        FileDesc = axxPacOpen(LibRef, axxFileRec.name,axxFileRec.mode);
        if (FileDesc<0)
              return dmErrNotRecordDB;
        /*The axxPac could have assigned a FileDesc different from before */
        if (FileDesc!=axxFileRec.fd) {
                UpdateFileDesc (ref,FileDesc);
                axxFileRec.fd=FileDesc;
        }
        err=axxPacSeek(LibRef, FileDesc, offset, SEEK_SET);
    }
    *recHP = MemHandleNew(length);
    buf = MemHandleLock(*recHP);
    axxPacRead (LibRef, axxFileRec.fd, buf, length);
    MemHandleUnlock(*recHP);
    return errNone;
}



Err vfsFileWrite
        (
        FileRef fileRef, 
        UInt32 numBytes,  
        void *dataP, 
        UInt32 *numBytesWrittenP
        )
{
    Int32 ret;
    file_rec_t axxFileRec;

    if (GetFileProperties (fileRef,&axxFileRec)!=errNone)
               return expErrNotOpen;

    ret = axxPacWrite(LibRef, axxFileRec.fd, dataP, (Int32)numBytes);
    if (ret < 0)
        return expErrNotOpen;

    if (numBytesWrittenP != NULL)
        *numBytesWrittenP = ret;

    return errNone;
}



Err vfsFileRename (UInt16 volRefNum,  Char *pathNameP,  Char *newNameP)
{
    Int32 ret;
    Char newName[256];
    Int16 i;
    Int16 srclen;

    i = srclen = StrLen(pathNameP);
    /* in the AxxPac the Rename function can change the path. The new
       name must contain the original path */
    while ((1 < i) && (pathNameP[i - 1] != '/'))
        i--;
    StrNCopy(newName, pathNameP, i);
    newName[i] = chrNull;
    StrCat(newName, newNameP);
    ret = axxPacRename(LibRef, pathNameP, newName);
    if (ret < 0)
        return expErrNotOpen;
    else
        return errNone;
}



Err vfsDirCreate(UInt16 volRefNum, Char *dirNameP)
{
    Err err;

    err = axxPacNewDir(LibRef, dirNameP);
    if (err != errNone)
        return vfsErrBadName;
    else
        return err;
}



Err vfsExportDatabaseToFile (UInt16 volRefNum, Char *pathNameP,
                             UInt16 cardNo, LocalID dbID)
{
    axxPacFD fd;
    Err ret;
    Err (*ExgDBWriteProcPtr) (const void* dataP, UInt32* sizeP, 
            void* userDataP);

    if ((fd = axxPacOpen(LibRef, pathNameP, O_Creat)) < 0)
        return vfsErrBadName;

    ExgDBWriteProcPtr = &WriteDataToFile;

    ret = ExgDBWrite(WriteDataToFile, &fd, NULL, dbID, cardNo); 
    ret = axxPacClose(LibRef, fd);
    if (0 < ret)
        return expErrNotEnoughPower;
    else
        return ret;
}



Err vfsImportDatabaseFromFile(UInt16 volRefNum, Char *pathNameP,
        UInt16 *cardNoP, LocalID *dbIDP)
{
    axxPacFD fd;                      
    Err ret;
    Boolean needReset;
    Char nameP[32];
  
    if ((fd = axxPacOpen(LibRef, pathNameP, O_RdOnly)) < 0)
        return vfsErrBadName;

    *cardNoP = 0;
    ret = ExgDBRead(&ReadDataFromFile,DeleteProc, &fd, dbIDP, *cardNoP,
            &needReset, true);
    ret = axxPacClose(LibRef, fd);
    if (ret < 0)
        return expErrNotEnoughPower;
    if (ret == dmErrAlreadyExists) {
        ret = vfsFileDBInfo(fd, nameP, NULL,NULL, NULL, NULL, NULL, NULL,
                NULL, NULL, NULL,NULL, NULL);
        *dbIDP = DmFindDatabase(0, nameP);
        *cardNoP = 0;
    }
    return ret; 
}



Err vfsFileDelete(UInt16 volRefNum, Char *pathNameP)
{
    Err err;

    err = axxPacDelete(LibRef, pathNameP);
    if (errNone <= err)
        return errNone;
    else
        return vfsErrFileNotFound;
}



Err vfsFileRead (FileRef fileRef, UInt32 numBytes, void *bufP,
        UInt32 *numBytesReadP)
{
    Int32 ret;
    file_rec_t axxFileRec;
  
    if (GetFileProperties (fileRef,&axxFileRec)!=errNone)
               return expErrNotOpen;

    ret = axxPacRead(LibRef, axxFileRec.fd, bufP, (Int32)numBytes);
    if (ret < 0)
        return expErrNotOpen;

    *numBytesReadP = ret;
    if (ret == 0)
        return vfsErrFileEOF;
    else
        return errNone;
}



Err vfsFileEOF (FileRef fileRef)
{
    Int32 ret;
    Char buf;
    file_rec_t axxFileRec;
  
    if (GetFileProperties (fileRef,&axxFileRec)!=errNone)
               return expErrNotOpen;
    /* read one char to see if it is the end of the file */
    ret = axxPacRead(LibRef, axxFileRec.fd, &buf, 1);
    if (ret < 0)
        return expErrNotOpen;
    if (ret == 0)
        return vfsErrFileEOF;
    ret = axxPacSeek(LibRef, axxFileRec.fd, -1, SEEK_CUR);
    return errNone;
}



/***********************************************
 *             Internal functions              *
 ***********************************************/
 
 /*Returns the File record associated to an vfs fileRef */
Int32 GetFileProperties (FileRef fileRef, file_rec_t* axxFileRec)
{
    Int16 i;
    file_rec_t* axxFileList;

    i = GetFilePos((Int16)fileRef) ;
    if (i == -1)
        return expErrNotOpen;    
    axxFileList = MemHandleLock(axxFileListHandle);
    MemMove (axxFileRec, &axxFileList[i],sizeof(file_rec_t));    
    MemHandleUnlock(axxFileListHandle);
    return errNone;
}

/* Updates the axxPac FileDescriptor associated to a specific vfs fileRef */
Int32 UpdateFileDesc(FileRef fileRef, axxPacFD FileDesc)
{
    Int16 i;
    Err ret;
    file_rec_t* axxFileList;
  
    axxFileList = MemHandleLock(axxFileListHandle);
    for (i=0; i < axxFLSize; i++) {
        if (axxFileList[i].fileRef == fileRef) {
            axxFileList[i].fd=FileDesc;
            ret = MemHandleUnlock(axxFileListHandle);
            return errNone;
        }
    }
    ret = MemHandleUnlock(axxFileListHandle);
    return -1;
}

/*Generates an internal FileRef that will identify the file opened even if the
    axxPac enters in sleep mode and closes it*/
Int16 CreateFileRef()
{
    Int16 i;
    Err ret;
    file_rec_t* axxFileList;
    /* assumption: the FileList matrix will not have more than 16 items (1-16) */
    Int16 idMap=0;
     
    if (axxFLSize==0) return 1;
    axxFileList = MemHandleLock(axxFileListHandle);
    /*activate the bit at the position of each FileRef */
    for (i=0; i < axxFLSize; i++) {
        idMap=idMap|(1<<(axxFileList[i].fileRef-1));
    }
    ret = MemHandleUnlock(axxFileListHandle);
    /*check which is the first bit inactive */
    for (i=0; i < axxFLSize; i++) {
        if ((idMap & (1<<i))==0) return i+1;/*fileRef should not be 0,  */
                                        /*since VFS fileRefs cannot be 0 */
    }
    return i+2;
}

/* callback function of ExgDBRead it just returns false in order not
   to overwrite any DB */
Boolean DeleteProc (const char* nameP, UInt16 version, UInt16 cardNo,
            LocalID dbID, void* userDataP)
{
    *(axxPacFD*)userDataP = dbID;
    return false;        
}



/* callback function of ExgDBRead it does the actual reading of the
   database from the axxPac */
Err ReadDataFromFile (void* dataP, UInt32* sizeP, void* fdP)
{
    if ((*sizeP =
         axxPacRead(LibRef, *(axxPacFD*)fdP, (Char*)dataP, *sizeP)) < 0) 
        return expErrNotEnoughPower;
    else
        return errNone;
}



/* callback function of ExgDBWrite it does the actual writing of the
   database into the file */
Err WriteDataToFile (const void* dataP, UInt32* sizeP, void * fdP)
{
    if (axxPacWrite(LibRef, *(axxPacFD*)fdP, (Char*)dataP, *sizeP) < 0) 
        return expErrNotEnoughPower;
    else
        return errNone;
}



/* stores the table of offsets for all records of the current file for
   faster access.  */
static void LoadOffsets(axxPacFD fd)
{
    MemPtr mp;

    /* assumption: there is just one file being read */
    ASSERT_MSG("AIM6", currFileDesc == -1);

    axxPacSeek(LibRef, fd, 76, SEEK_SET);
    axxPacRead(LibRef, fd, &nrec, 2);
    moff = MemHandleNew(nrec * 8L);
    ASSERT_MSG("AIM3", moff != 0);

    mp = MemHandleLock(moff);
    ASSERT_MSG("AIM4", mp != 0);

    axxPacSeek(LibRef, fd, 78, SEEK_SET);
    axxPacRead(LibRef, fd, mp, nrec * 8L);
    MemHandleUnlock(moff);
    currFileDesc = fd;
    nrec--;
}



/* returns the position within the internal table of a specific File
   already open */
static Int16 GetFilePos(Int16 fr)
{
    Int16 i;
    Err ret;
    file_rec_t* axxFileList;
  
    axxFileList = MemHandleLock(axxFileListHandle);
    for (i=0; i < axxFLSize; i++) {
        if (axxFileList[i].fileRef == fr) {
            ret = MemHandleUnlock(axxFileListHandle);
            return i;
        }
    }
    ret = MemHandleUnlock(axxFileListHandle);
    return -1;
}


/* remove an item from the list and move te rest one position up */
static Int16 RemoveFileRec(FileRef fr)
{
    Int16 i;
    Err ret;
    file_rec_t* axxFileList;
    axxPacFD FileDesc;

    axxFileList = MemHandleLock(axxFileListHandle);
    i = GetFilePos((Int16)fr);
    if (i == -1)
        return -1;
    
    FileDesc=axxFileList[i].fd;
    for (; i < axxFLSize - 1; i++) {
        MemMove (&axxFileList[i], &axxFileList[i + 1],sizeof(axxFileList));
    }    
    axxFLSize--;
    ret = MemHandleResize(axxFileListHandle, sizeof(file_rec_t) * axxFLSize);
    ret = MemHandleUnlock(axxFileListHandle);
    /* Check if the current file is the one removed from the list */
    if ((currFileDesc != -1) && ( FileDesc== currFileDesc)) {
        /* the cache of offsets is no longer valid if this file
           was closed */
        currFileDesc = -1;
        MemHandleFree(moff);
    }
    return 0;
}


