/******************************************************************************
    
	This file is part of CSources, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published 
	by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#if !defined(__DBASE_H__)
#define __DBASE_H__

#include "CSOURCES.H"
#include "LISTE.H"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 4)
typedef struct _tagdbaseheadnet {	/* Kopfstruktur: dBase IV			*/
	byte art;		                /* 3 = ohne Memo-Datei, 131 mit Memo 	*/
    byte year;	 	                /* Datum der letzten Änderung JJ MM TT	*/
	byte month;
	byte day;
	dword records;		            /* Anzahl der Sätze 			*/
	word headlen;		            /* Länge des Headers 			*/
	word recordlen;	                /* Länge der Datensätze 		*/
	byte reserve2[2];
	byte acttrans;		            /* nicht beendete Transaktionen	    IV	*/
	byte cryptflag;		            /* Flag für Verschlüsselung	    IV	*/
	byte reserve1[3];	            
	byte IdentityMarker;            /* 1 Identity vorhanden, 0 nicht */
	dword IdentityFieldNo;
	dword IdentityValue;
	byte MDXmarker;		            /* 1 MDX-Datei ist vorhanden
				                       0 kein Multikey-Index	    IV	*/
	byte reserve[3];	            /* reserviert 				*/
} dbaseheadnet;	                    /* 32 Bytes */
#define szdbaseheadnet sizeof(dbaseheadnet)

typedef struct _tagdbasefield {	/* Feld-Bezeichner  dBase IV 	*/
	byte fname[11];		        /* Feldname 			*/
	byte ftyp;		            /* Feldtyp: 'C' Character, 'M' Memo, 'N' Numeric, 'F' Float, 'I' Identity, 'L' Logical, 'D' Date, 'T' Time */
	dword fadr;		            /* Feldadresse 			*/
	byte flen;		            /* Feldlänge 			*/
	byte fdec;		            /* Dezimalstellen für Feldtyp 'N' 	*/
	byte netYLow;               /* reserviert für Netzwerk	*/
	byte netYHigh;	            /* reserviert für Netzwerk	*/
	byte wordID;		        /* ID für Arbeitsbereich	*/
	byte netXLow;	            /* reserviert für Netzwerk	*/
	byte netXHigh;	            /* reserviert für Netzwerk	*/
	byte setfield;		        /* Kennzeichnung für SET FIELDS */
	byte reserve[8];	        /* reserviert 			*/
} dbasefield;                   /* 32 bytes */
#define szdbasefield sizeof(dbasefield)

typedef struct _tagdbaseindexheader {
	dword art;                   // MDX
	dword headersize;            // Größe Indexheader
	dword indexcnt;              // Anzahl Indices
	byte reserve[20];
} dbaseindexheader;
#define szdbaseindexheader sizeof(dbaseindexheader)

typedef struct _tagdbaseindexsubheader {
	sqword rootNode;             // filepos root
	dword fieldcnt;              // key field cnt
	byte reserve[20];
} dbaseindexsubheader;
#define szdbaseindexsubheader sizeof(dbaseindexsubheader)
#pragma pack(pop)

typedef struct _tagdbaseindex {
	Pointer index;               // Btree
	dword fieldcnt;              // field cnt key
	dbasefield* fields;          // key fields
	Pointer keyData;             // key data
	LSearchResultType curRecord; // begin .. next current Record
} dbaseindex;
#define szdbaseindex sizeof(dbaseindex)

typedef struct _tagdbasefile {
    // dbf file info
	int fd;
	dbaseheadnet* head;
	dword fieldcnt;
	dbasefield* fields;
    Pointer data;
    Pointer backup;
    dword isBatch;
    // dbt file info
	int memofd;
	dword memopointer;
	Pointer memodatachunk;
    // mdx file info
	int indexfd;
	dword indexcnt;
	dbaseindex** index;
} dbasefile;
#define szdbasefile sizeof(dbasefile)

CSOURCES_API dbasefile* __stdcall dbasefile_init(word fieldcnt, word recordlen);
CSOURCES_API void __stdcall dbasefile_initfield(dbasefile* fileinfo, dword fieldno, CConstPointer fieldname, byte fieldtype, byte fieldlen, byte fielddec);
CSOURCES_API bool __stdcall dbasefile_hasmemo(dbasefile* fileinfo);
CSOURCES_API bool __stdcall dbasefile_hasindex(dbasefile* fileinfo);

CSOURCES_API errno_t __stdcall dbasefile_create(CConstPointer filename, dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbasefile_open(CConstPointer filename, dbasefile** fileinfo);
CSOURCES_API errno_t __stdcall dbasefile_read(dbasefile* fileinfo, sqword recno);
CSOURCES_API errno_t __stdcall dbasefile_write(dbasefile* fileinfo, sqword recno);
CSOURCES_API errno_t __stdcall dbasefile_delete(dbasefile* fileinfo, sqword recno);
CSOURCES_API errno_t __stdcall dbasefile_append(dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbasefile_close(dbasefile* fileinfo);

CSOURCES_API void __stdcall dbasefile_beginbatch(dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbasefile_endbatch(dbasefile* fileinfo);

CSOURCES_API void __stdcall dbasefile_cleardata(dbasefile* fileinfo);
CSOURCES_API bool __stdcall dbasefile_isdeleted(dbasefile* fileinfo);
CSOURCES_API sqword __stdcall dbasefile_getrecordcnt(dbasefile* fileinfo);
CSOURCES_API dword __stdcall dbasefile_getfieldcnt(dbasefile* fileinfo);
CSOURCES_API int __stdcall dbasefile_getfieldno(dbasefile* fileinfo, CConstPointer fieldname);
CSOURCES_API CPointer __stdcall dbasefile_getfieldname(dbasefile* fileinfo, dword fieldno);
CSOURCES_API void __stdcall dbasefile_getfieldinfo(dbasefile* fileinfo, dword fieldno, byte* fieldtype, byte* fieldlen, byte* fielddec);
CSOURCES_API Pointer __stdcall dbasefile_getfielddata(dbasefile* fileinfo, dword fieldno);
CSOURCES_API bool __stdcall dbasefile_getfielddata_bool(dbasefile* fileinfo, dword fieldno);
CSOURCES_API sdword __stdcall dbasefile_getfielddata_long(dbasefile* fileinfo, dword fieldno);
CSOURCES_API double __stdcall dbasefile_getfielddata_double(dbasefile* fileinfo, dword fieldno);
CSOURCES_API CPointer __stdcall dbasefile_getfielddata_string(dbasefile* fileinfo, dword fieldno);
CSOURCES_API bool _stdcall dbasefile_cmpfielddata(dbasefile* fileinfo, dword fieldno);
CSOURCES_API void __stdcall dbasefile_setfielddata(dbasefile* fileinfo, dword fieldno, Pointer data);
CSOURCES_API void __stdcall dbasefile_setfielddata_bool(dbasefile* fileinfo, dword fieldno, bool data);
CSOURCES_API void __stdcall dbasefile_setfielddata_long(dbasefile* fileinfo, dword fieldno, sdword data);
CSOURCES_API void __stdcall dbasefile_setfielddata_double(dbasefile* fileinfo, dword fieldno, double data);
CSOURCES_API void __stdcall dbasefile_setfielddata_string(dbasefile* fileinfo, dword fieldno, CPointer data, dword datalen);

#define DBASEMEMO_CHUNKSIZE 512L
CSOURCES_API errno_t __stdcall dbasememo_create(CConstPointer filename, dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbasememo_open(CConstPointer filename, dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbasememo_readchunk(dbasefile* fileinfo, sqword recno);
CSOURCES_API errno_t __stdcall dbasememo_writechunk(dbasefile* fileinfo, sqword recno);
CSOURCES_API errno_t __stdcall dbasememo_appendchunk(dbasefile* fileinfo, sqword* recno);

CSOURCES_API Pointer __stdcall dbasememo_getdata(dbasefile* fileinfo);
CSOURCES_API void __stdcall dbasememo_setdata(dbasefile* fileinfo, Pointer data);

CSOURCES_API errno_t __stdcall dbaseindex_init(dbasefile* fileinfo, dword indexcnt);
CSOURCES_API errno_t __stdcall dbaseindex_initfields(dbasefile* fileinfo, dword index, dword fieldcnt);
CSOURCES_API void __stdcall dbaseindex_initfield(dbasefile* fileinfo, dword index, dword field, dword fieldno, byte fieldlen);

CSOURCES_API dword __stdcall dbaseindex_getindexcnt(dbasefile* fileinfo);
CSOURCES_API dword __stdcall dbaseindex_getindexfieldcnt(dbasefile* fileinfo, dword index);
CSOURCES_API void __stdcall dbaseindex_getindexfieldinfo(dbasefile* fileinfo, dword index, dword field, dword* fieldno, byte* fieldlen);

CSOURCES_API errno_t __stdcall dbaseindex_create(CConstPointer filename, dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbaseindex_open(CConstPointer filename, dbasefile* fileinfo);
CSOURCES_API errno_t __stdcall dbaseindex_insertkey(dbasefile* fileinfo, dword index, sqword recno);
CSOURCES_API errno_t __stdcall dbaseindex_removekey(dbasefile* fileinfo, dword index, sqword recno);

CSOURCES_API errno_t __stdcall dbaseindex_getRecno(dbasefile* fileinfo, dword index, sqword* recno);
CSOURCES_API errno_t __stdcall dbaseindex_getSearchedRecno(dbasefile* fileinfo, dword index, sqword* recno);
CSOURCES_API errno_t __stdcall dbaseindex_getFirstRecno(dbasefile* fileinfo, dword index, sqword* recno);
CSOURCES_API errno_t __stdcall dbaseindex_getNextRecno(dbasefile* fileinfo, dword index, sqword* recno);

#ifdef __cplusplus
}
#endif

/* end dbase.h
*/
#endif
