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
#include "CS_PCH.H"
#include "DBASE.H"
#include "DBASENDX.H"
#include "STRUTIL.H"
#include "UTLPTR.H"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#ifdef OK_SYS_WINDOWS
	#include <share.h>
#endif
#ifdef OK_SYS_UNIX
	#include <unistd.h>
	#define _write write
	#define _read read
	#define _close close
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#ifdef OK_SYS_UNIX
static errno_t chsize( int handle, long size )
{
	Pointer buf;
	int cnt;
	off_t fpos;
	
	fpos = lseek( handle, 0, SEEK_END );
	if ( 0 > fpos )
		return errno;
	buf = TFalloc(size - fpos);
	if ( PtrCheck(buf) )
		return ENOMEM;
	cnt = write(handle, buf, size - fpos);
	if ( 0 > cnt )
		return errno;
	if ( cnt != (size - fpos) )
		return EIO;
	TFfree(buf);
	return 0;
}
#endif

static errno_t __stdcall dbasefile_updateheader(dbasefile* fileinfo, bool updatefields)
{
	byte buf = 0x0D;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
    __time64_t long_time;
    struct tm* newtime;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	time_t long_time;
	struct tm* newtime;
	off_t fpos;
#endif
#endif
#ifdef OK_COMP_MSC
    __time64_t long_time;
    struct tm newtime;
    errno_t err;
	sqword fpos;
#endif
	int cnt;
	dword sz;

	if ( fileinfo->isBatch )
		return 0;
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	_time64( &long_time );
    newtime = _localtime64( &long_time );
    if ( PtrCheck(newtime) )
        return errno;
	fileinfo->head->year = newtime->tm_year - 100;
	fileinfo->head->month = newtime->tm_mon + 1;
	fileinfo->head->day = newtime->tm_mday;
	fpos = _lseeki64( fileinfo->fd, 0LL, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
    time( &long_time );
    newtime = localtime( &long_time );
    if ( PtrCheck(newtime) )
        return errno;
	fileinfo->head->year = newtime->tm_year - 100;
	fileinfo->head->month = newtime->tm_mon + 1;
	fileinfo->head->day = newtime->tm_mday;
	fpos = lseek( fileinfo->fd, 0, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	_time64(&long_time);
    err = _localtime64_s( &newtime, &long_time ); 
    if (err)
		return err;
	fileinfo->head->year = newtime.tm_year - 100;
	fileinfo->head->month = newtime.tm_mon + 1;
	fileinfo->head->day = newtime.tm_mday;
	fpos = _lseeki64( fileinfo->fd, 0LL, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != 0 )
		return EIO;
	cnt = _write(fileinfo->fd, fileinfo->head, szdbaseheadnet);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szdbaseheadnet )
		return EIO;
	if ( updatefields )
	{
		sz = fileinfo->fieldcnt * szdbasefield;
		cnt = _write(fileinfo->fd, fileinfo->fields, sz);
		if ( cnt < 0 )
			return errno;
		if ( cnt < Cast(int, sz) )
			return EIO;
		cnt = _write(fileinfo->fd, &buf, 1);
		if ( cnt < 0 )
			return errno;
		if ( cnt < 1 )
			return EIO;
	}
	return 0;
}

static void __stdcall dbasefile_initfieldbuffer(dbasefile* fileinfo)
{
	dword fieldno;
	dbasefield* p;
	dword bufadr;

	fieldno = 0;
	p = fileinfo->fields;
	bufadr = 1;
	while ( fieldno < fileinfo->fieldcnt )
	{
		p->fadr = bufadr;
		bufadr += p->flen;
		++fieldno;
		p = CastAnyPtr(dbasefield, _l_ptradd(p, sizeof(dbasefield)));
	}
}

dbasefile* __stdcall dbasefile_init(word fieldcnt, word recordlen)
{
	dbasefile* p1 = CastAnyPtr(dbasefile, TFalloc(szdbasefile));

	if ( PtrCheck(p1) )
		return NULL;
	p1->memofd = -1;
	p1->indexfd = -1;
	p1->head = CastAnyPtr(dbaseheadnet, TFalloc(szdbaseheadnet));
	if ( PtrCheck(p1->head) )
		return NULL;
	p1->fieldcnt = fieldcnt;
	p1->fields = CastAnyPtr(dbasefield, TFalloc(fieldcnt * szdbasefield));
	if ( PtrCheck(p1->fields) )
		return NULL;
	p1->head->recordlen = recordlen + 1;
    p1->data = TFalloc(p1->head->recordlen);
    if ( PtrCheck(p1->data) )
        return NULL;
    p1->backup = TFalloc(p1->head->recordlen);
    if ( PtrCheck(p1->backup) )
        return NULL;
    p1->head->headlen = szdbaseheadnet + (fieldcnt * szdbasefield) + 1;
	p1->head->art = 0x03;
	return p1;
}

void __stdcall dbasefile_initfield(dbasefile* fileinfo, dword fieldno, CConstPointer fieldname, byte fieldtype, byte fieldlen, byte fielddec)
{
	int cnt;
	byte* fn;
	dbasefield* p;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	fn = p->fname;
	cnt = 0;
	while ( (*fieldname != 0) && (cnt < 10) )
	{
		*fn++ = Castbyte(*fieldname++);
		++cnt;
	}
	while ( cnt < 11 )
	{
		*fn++ = 0;
		++cnt;
	}
	p->ftyp = fieldtype;
	p->flen = fieldlen;
	p->fdec = fielddec;
	if ( (p->ftyp == 'M') && (fileinfo->head->art == 0x03) )
		fileinfo->head->art = 0x83;
	if ( (p->ftyp == 'I') && (fileinfo->head->IdentityMarker == 0) )
	{
		fileinfo->head->IdentityMarker = 1;
		fileinfo->head->IdentityFieldNo = fieldno;
		fileinfo->head->IdentityValue = 0;
	}
}

bool __stdcall dbasefile_hasmemo(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	return TestBit(fileinfo->head->art,0x80)?true:false;
}

bool __stdcall dbasefile_hasindex(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	return (fileinfo->head->MDXmarker != 0)?true:false;
}

errno_t __stdcall dbasefile_create(CConstPointer filename, dbasefile* fileinfo)
{
	errno_t result;
	byte buf = 0x1A;
	int cnt;

	assert(fileinfo != NULL);
	dbasefile_initfieldbuffer(fileinfo);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fileinfo->fd = _tsopen(filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
        if ( fileinfo->fd < 0 )
            return errno;
	result = _chsize( fileinfo->fd, (long)(fileinfo->head->headlen + 1) );
        if ( result < 0 )
            return errno;
#endif
#ifdef OK_SYS_UNIX
	fileinfo->fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( fileinfo->fd < 0 )
        return errno;
	result = chsize( fileinfo->fd, fileinfo->head->headlen + 1 );
	if ( result )
		return result;
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&(fileinfo->fd), filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
	result = _chsize_s( fileinfo->fd, fileinfo->head->headlen + 1 );
	if ( result )
		return result;
#endif
	result = dbasefile_updateheader(fileinfo, true);
	if ( result )
		return result;
	cnt = _write(fileinfo->fd, &buf, 1);
	if ( 0 > cnt )
		return errno;
	if ( cnt != 1 )
		return EIO;
#ifdef OK_SYS_WINDOWS		
	if ( 0 > _commit( fileinfo->fd ) )
		return errno;
#endif
	return 0;
}

errno_t __stdcall dbasefile_open(CConstPointer filename, dbasefile** fileinfo)
{
#ifdef OK_COMP_MSC
	errno_t result;
#endif
	int fd;
	dbaseheadnet head;
	int cnt;
	int cnt1;
	dbasefile* info;

	assert(fileinfo != NULL);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fd = _tsopen(filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( fd < 0 )
		return errno;
#endif
#ifdef OK_SYS_UNIX
	fd = open(filename, _O_BINARY | _O_RDWR);
    if ( fd < 0 )
		return errno;
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&fd, filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
#endif
	cnt = _read(fd, &head, szdbaseheadnet);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szdbaseheadnet )
		return EIO;
	if ( (head.art != 0x03) && (head.art != 0x83) )
		return EINVAL;
	cnt = head.headlen - szdbaseheadnet - 1;
	if ( cnt % szdbasefield )
		return EINVAL;
	info = dbasefile_init(Castword(cnt / szdbasefield), head.recordlen - 1);
	if ( info == NULL )
		return ENOMEM;
	*fileinfo = info;
	info->fd = fd;
	s_memcpy_s(info->head, szdbaseheadnet, &head, szdbaseheadnet);
	cnt1 = _read(info->fd, info->fields, cnt);
	if ( cnt1 < 0 )
		return errno;
	if ( cnt1 < cnt )
		return EIO;
	dbasefile_initfieldbuffer(info);
	return 0;
}

errno_t __stdcall dbasefile_read(dbasefile* fileinfo, sqword recno)
{
	int cnt;
#ifdef OK_SYS_WINDOWS	
	sqword fpos;
	sqword fpos1;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
	off_t fpos1;
#endif

	assert(fileinfo != NULL);
	assert(recno < fileinfo->head->records);
	assert(fileinfo->fd >= 0);
#ifdef OK_SYS_WINDOWS	
	fpos1 = Castsqword(fileinfo->head->headlen) +
		(recno * Castsqword(fileinfo->head->recordlen));
	fpos = _lseeki64( fileinfo->fd, fpos1, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fpos1 = Cast(off_t, fileinfo->head->headlen) + 
		(Cast(off_t, recno) * Cast(off_t, fileinfo->head->recordlen));
	fpos = lseek( fileinfo->fd, fpos1, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != fpos1 )
		return EIO;
	cnt = _read(fileinfo->fd, fileinfo->data, fileinfo->head->recordlen);
	if ( cnt < 0 )
		return errno;
	if ( cnt < fileinfo->head->recordlen )
		return EIO;
    s_memcpy_s(fileinfo->backup, fileinfo->head->recordlen, fileinfo->data, fileinfo->head->recordlen);
	return 0;
}

errno_t __stdcall dbasefile_write(dbasefile* fileinfo, sqword recno)
{
	int cnt;
	errno_t err;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
	sqword fpos1;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
	off_t fpos1;
#endif
	BPointer bp;

	assert(fileinfo != NULL);
	assert(recno < fileinfo->head->records);
	assert(fileinfo->fd >= 0);
#ifdef OK_SYS_WINDOWS
	fpos1 = Castsqword(fileinfo->head->headlen) +
		(recno * Castsqword(fileinfo->head->recordlen));
	fpos = _lseeki64( fileinfo->fd, fpos1, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fpos1 = Cast(off_t, fileinfo->head->headlen) + 
		(Cast(off_t, recno) * Cast(off_t, fileinfo->head->recordlen));
	fpos = lseek( fileinfo->fd, fpos1, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != fpos1 )
		return EIO;
	bp = CastAny(BPointer, fileinfo->data);
	*bp = ' ';
	cnt = _write(fileinfo->fd, fileinfo->data, fileinfo->head->recordlen);
	if ( cnt < 0 )
		return errno;
	if ( cnt < fileinfo->head->recordlen )
		return EIO;
	err = dbasefile_updateheader(fileinfo, false);
	if ( err )
		return err;
#ifdef OK_SYS_WINDOWS
	if (!(fileinfo->isBatch))
	{
		if ( 0 > _commit( fileinfo->fd ) )
			return errno;
	}
#endif
	return 0;
}

errno_t __stdcall dbasefile_delete(dbasefile* fileinfo, sqword recno)
{
	int cnt;
	errno_t err;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
	sqword fpos1;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
	off_t fpos1;
#endif
	BPointer bp;

	assert(fileinfo != NULL);
	assert(recno < fileinfo->head->records);
	assert(fileinfo->fd >= 0);
#ifdef OK_SYS_WINDOWS
	fpos1 = Castsqword(fileinfo->head->headlen) +
		(recno * Castsqword(fileinfo->head->recordlen));
	fpos = _lseeki64( fileinfo->fd, fpos1, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fpos1 = Cast(off_t, fileinfo->head->headlen) + 
		(Cast(off_t, recno) * Cast(off_t, fileinfo->head->recordlen));
	fpos = lseek( fileinfo->fd, fpos1, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != fpos1 )
		return EIO;
	bp = CastAny(BPointer, fileinfo->data);
	*bp = '*';
	cnt = _write(fileinfo->fd, fileinfo->data, fileinfo->head->recordlen);
	if ( cnt < 0 )
		return errno;
	if ( cnt < fileinfo->head->recordlen )
		return EIO;
	err = dbasefile_updateheader(fileinfo, false);
	if ( err )
		return err;
#ifdef OK_SYS_WINDOWS
	if (!(fileinfo->isBatch))
	{
		if ( 0 > _commit( fileinfo->fd ) )
			return errno;
	}
#endif
	return 0;
}

errno_t __stdcall dbasefile_append(dbasefile* fileinfo)
{
	byte buf = 0x1A;
	errno_t err;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
	sqword fpos1;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
	off_t fpos1;
#endif
	BPointer bp;

	assert(fileinfo != NULL);
	assert(fileinfo->fd >= 0);
	if ( fileinfo->head->IdentityMarker )
	{
		dbasefield* IdentityField = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fileinfo->head->IdentityFieldNo * szdbasefield));
		char buf[20];

		++(fileinfo->head->IdentityValue);
#ifdef OK_COMP_MSC
		sprintf_s(buf, 20, "%*.*ld", IdentityField->flen, IdentityField->flen, fileinfo->head->IdentityValue);
#endif
#ifdef OK_COMP_GNUC
		sprintf(buf, "%*.*ld", IdentityField->flen, IdentityField->flen, fileinfo->head->IdentityValue);
#endif
		s_memcpy_s(_l_ptradd(fileinfo->data, IdentityField->fadr), IdentityField->flen, buf, IdentityField->flen);
	}
	++(fileinfo->head->records);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fpos1 = Cast(sqword, fileinfo->head->headlen) + 
		(Cast(sqword, fileinfo->head->records) * Cast(sqword, fileinfo->head->recordlen));
	err = _chsize( fileinfo->fd, (long)(fpos1) );
	if ( err < 0 )
		return errno;
	fpos1 -= Cast(sqword, fileinfo->head->recordlen);
	fpos = _lseeki64( fileinfo->fd, fpos1, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fpos1 = Cast(off_t, fileinfo->head->headlen) + 
		(Cast(off_t, fileinfo->head->records) * Cast(off_t, fileinfo->head->recordlen));
	err = chsize( fileinfo->fd, fpos1 );
	if ( err )
		return err;
	fpos1 -= Cast(off_t, fileinfo->head->recordlen);
	fpos = lseek( fileinfo->fd, fpos1, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	fpos1 = Cast(sqword, fileinfo->head->headlen) +
		(Cast(sqword, fileinfo->head->records) * Cast(sqword, fileinfo->head->recordlen));
	err = _chsize_s( fileinfo->fd, fpos1 );
	if ( err )
		return err;
	fpos1 -= Cast(sqword, fileinfo->head->recordlen);
	fpos = _lseeki64( fileinfo->fd, fpos1, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != fpos1 )
		return EIO;
	bp = CastAny(BPointer, fileinfo->data);
	*bp = ' ';
	cnt = _write(fileinfo->fd, fileinfo->data, fileinfo->head->recordlen);
	if ( cnt < 0 )
		return errno;
	if ( cnt < fileinfo->head->recordlen )
		return EIO;
	cnt = _write(fileinfo->fd, &buf, 1);
	if ( cnt < 0 )
		return errno;
	if ( cnt < 1 )
		return EIO;
	err = dbasefile_updateheader(fileinfo, false);
	if ( err )
		return err;
#ifdef OK_SYS_WINDOWS
	if (!(fileinfo->isBatch))
	{
		if ( 0 > _commit( fileinfo->fd ) )
			return errno;
	}
#endif
	return 0;
}

errno_t __stdcall dbasefile_close(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	if ( NotPtrCheck(fileinfo->index) )
	{
		dbaseindex* p;
		dword ix;

		for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
		{
			p = fileinfo->index[ix];
			FBTreeClose(p->index);
			TFfree(p->fields);
			TFfree(p->keyData);
			TFfree(p);
		}
		TFfree(fileinfo->index);
		fileinfo->index = NULL;
	}
	if ( NotPtrCheck(fileinfo->memodatachunk) )
	{
		TFfree(fileinfo->memodatachunk);
		fileinfo->memodatachunk = NULL;
	}
	if ( NotPtrCheck(fileinfo->data) )
	{
		TFfree(fileinfo->data);
		fileinfo->data = NULL;
	}
	if ( NotPtrCheck(fileinfo->backup) )
	{
		TFfree(fileinfo->backup);
		fileinfo->backup = NULL;
	}
	if ( NotPtrCheck(fileinfo->fields) )
	{
		TFfree(fileinfo->fields);
		fileinfo->fields = NULL;
	}
	if ( NotPtrCheck(fileinfo->head) )
	{
		TFfree(fileinfo->head);
		fileinfo->head = NULL;
	}
	if ( fileinfo->fd >= 0 )
	{
		if ( 0 > _close( fileinfo->fd ) )
			return errno;
	}
	if ( fileinfo->memofd >= 0 )
	{
		if ( 0 > _close( fileinfo->memofd ) )
			return errno;
	}
	if ( fileinfo->indexfd >= 0 )
	{
		if ( 0 > _close( fileinfo->indexfd ) )
			return errno;
	}
	TFfree(fileinfo);
	return 0;
}

void __stdcall dbasefile_beginbatch(dbasefile* fileinfo)
{
	dword ix;
	dbaseindex* p;

	assert(fileinfo != NULL);
	fileinfo->isBatch = TRUE;
	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		p = fileinfo->index[ix];
		FBTreeBeginBatch(p->index);
	}
}

errno_t __stdcall dbasefile_endbatch(dbasefile* fileinfo)
{
	errno_t result;
	dword ix;
	dbaseindex* p;

	assert(fileinfo != NULL);
	fileinfo->isBatch = FALSE;
	result = dbasefile_updateheader(fileinfo, false);
	if ( result )
		return result;
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->fd))
		return errno;
#endif
	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		p = fileinfo->index[ix];
		result = FBTreeEndBatch(p->index);
		if ( result )
			return result;
	}
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->indexfd))
		return errno;
#endif
	return 0;
}

void __stdcall dbasefile_cleardata(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	assert(fileinfo->head != NULL);
    assert(fileinfo->data != NULL);
    assert(fileinfo->backup != NULL);
    s_memset(fileinfo->data, 0x20, fileinfo->head->recordlen);
    s_memset(fileinfo->backup, 0x20, fileinfo->head->recordlen);
}

bool __stdcall dbasefile_isdeleted(dbasefile* fileinfo)
{
	BPointer data;

	assert(fileinfo != NULL);
	assert(fileinfo->data != NULL);
	data = CastAny(BPointer, fileinfo->data);
	return (*data == '*');
}

sqword __stdcall dbasefile_getrecordcnt(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	return fileinfo->head->records;
}

dword __stdcall dbasefile_getfieldcnt(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	return fileinfo->fieldcnt;
}

int __stdcall dbasefile_getfieldno(dbasefile* fileinfo, CConstPointer fieldname)
{
	dword fieldno;
	byte* fn;
	CConstPointer name;
	dbasefield* p;

	assert(fileinfo != NULL);
	fieldno = 0;
	p = fileinfo->fields;
	while ( fieldno < fileinfo->fieldcnt )
	{
		fn = p->fname;
		name = fieldname;
		while ( *name != 0 )
		{
			if ( *fn++ != Castbyte(*name++) )
				break;
		}
		if ( *name == 0 )
			return fieldno;
		++fieldno;
		p = CastAnyPtr(dbasefield, _l_ptradd(p, szdbasefield));
	}
	return -1;
}

CPointer __stdcall dbasefile_getfieldname(dbasefile* fileinfo, dword fieldno)
{
    dbasefield* p;
    BPointer fname;
    dword fn_len;
    CPointer result;
    CPointer rp;
    
	assert(fileinfo != NULL);
    assert(fieldno < fileinfo->fieldcnt);
    p = fileinfo->fields + fieldno;
    fname = p->fname;
    fn_len = 0;
    while ( *fname++ )
        ++fn_len;
    result = CastAny(CPointer,TFalloc(fn_len * szchar));
    if ( PtrCheck(result) )
        return NULL;
    rp = result;
    fname = p->fname;
    while ( fn_len-- > 0 )
        *rp++ = *fname++;
    return result;
}

void __stdcall dbasefile_getfieldinfo(dbasefile* fileinfo, dword fieldno, byte* fieldtype, byte* fieldlen, byte* fielddec)
{
	dbasefield* p;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	*fieldtype = p->ftyp;
	*fieldlen = p->flen;
	*fielddec = p->fdec;
}

Pointer __stdcall dbasefile_getfielddata(dbasefile* fileinfo, dword fieldno)
{
	dbasefield* p;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	return _l_ptradd(fileinfo->data, p->fadr);
}

bool __stdcall dbasefile_getfielddata_bool(dbasefile* fileinfo, dword fieldno)
{
	dbasefield* p;
	Pointer d;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	return ((toupper(DerefAnyPtr(char, d)) == 'T') || (toupper(DerefAnyPtr(char, d)) == 'J'));
}

sdword __stdcall dbasefile_getfielddata_long(dbasefile* fileinfo, dword fieldno)
{
	dbasefield* p;
	Pointer d;
	char format[20];
	sdword result;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	format[0] = '%';
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	sprintf(&format[1], "%d", Cast(int, p->flen));
	strcat(format, "d");
	if (1 != sscanf(CastAnyPtr(char, d), format, &result))
		result = 0;
#endif
#ifdef OK_SYS_UNIX
	sprintf(&format[1], "%d", Cast(int, p->flen));
	strcat(format, "d");
	if (1 != sscanf(CastAnyPtr(char, d), format, &result))
		result = 0;
#endif
#endif
#ifdef OK_COMP_MSC
	sprintf_s(&format[1], 19, "%d", Cast(int, p->flen));
	strcat_s(format, 20, "d");
	if (1 != sscanf_s(CastAnyPtr(char, d), format, &result))
		result = 0;
#endif
	return result;
}

double __stdcall dbasefile_getfielddata_double(dbasefile* fileinfo, dword fieldno)
{
	dbasefield* p;
	Pointer d;
	char format[20];
	double result;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	format[0] = '%';
#ifdef OK_COMP_GNUC
	sprintf(&format[1], "%d", Cast(int, p->flen));
	strcat(format, "lf");
	if (1 != sscanf(CastAnyPtr(char, d), format, &result))
		result = 0.0;
#endif
#ifdef OK_COMP_MSC
	sprintf_s(&format[1], 19, "%d", Cast(int, p->flen));
	strcat_s(format, 20, "lf");
	if (1 != sscanf_s(CastAnyPtr(char, d), format, &result))
		result = 0.0;
#endif
	return result;
}

CPointer __stdcall dbasefile_getfielddata_string(dbasefile* fileinfo, dword fieldno)
{
	dbasefield* p;
	Pointer d;
	CPointer result;
	CPointer q;
	byte i;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	q = result = CastAnyPtr(mbchar, TFalloc((p->flen + 1) * szchar));
	for (i = 0; i < p->flen; ++i)
	{
		*q++ = DerefAnyPtr(char, d);
		d = _l_ptradd(d, 1);
	}
	*q = 0;
	return result;
}

bool _stdcall dbasefile_cmpfielddata(dbasefile* fileinfo, dword fieldno)
{
    dbasefield* p;
    Pointer p1;
    Pointer p2;

    assert(fileinfo != NULL);
    assert(fieldno < fileinfo->fieldcnt);
    p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
    p1 = _l_ptradd(fileinfo->data, p->fadr);
    p2 = _l_ptradd(fileinfo->backup, p->fadr);
    return (0 == s_memcmp(p1, p2, p->flen));
}

void __stdcall dbasefile_setfielddata(dbasefile* fileinfo, dword fieldno, Pointer data)
{
	dbasefield* p;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	s_memcpy_s(_l_ptradd(fileinfo->data,p->fadr), p->flen, data, p->flen);
}

void __stdcall dbasefile_setfielddata_bool(dbasefile* fileinfo, dword fieldno, bool data)
{
	dbasefield* p;
	Pointer d;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	if (data)
		DerefAnyPtr(char, d) = 'T';
	else
		DerefAnyPtr(char, d) = 'F';
}

void __stdcall dbasefile_setfielddata_long(dbasefile* fileinfo, dword fieldno, sdword data)
{
	dbasefield* p;
	Pointer d;
	char buf[256];

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	memset(buf, 0x20, 256);
#ifdef OK_COMP_GNUC
	sprintf(buf, "%*.*d", p->flen, 0, data);
#endif
#ifdef OK_COMP_MSC
	sprintf_s(buf, 256, "%*.*d", p->flen, 0, data);
#endif
	s_memcpy_s(d, p->flen, buf, p->flen);
}

void __stdcall dbasefile_setfielddata_double(dbasefile* fileinfo, dword fieldno, double data)
{
	dbasefield* p;
	Pointer d;
	char buf[256];

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	memset(buf, 0x20, 256);
#ifdef OK_COMP_GNUC
	sprintf(buf, "%*.*lf", p->flen, p->fdec, data);
#endif
#ifdef OK_COMP_MSC
	sprintf_s(buf, 256, "%*.*lf", p->flen, p->fdec, data);
#endif
	s_memcpy_s(d, p->flen, buf, p->flen);
}

void __stdcall dbasefile_setfielddata_string(dbasefile* fileinfo, dword fieldno, CPointer data, dword datalen)
{
	dbasefield* p;
	Pointer d;
	dword cnt, i;

	assert(fileinfo != NULL);
	assert(fieldno < fileinfo->fieldcnt);
	p = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	d = _l_ptradd(fileinfo->data, p->fadr);
	cnt = Min(datalen, p->flen);
	for (i = 0; i < cnt; ++i)
	{
		DerefAnyPtr(char, d) = Cast(char, *data++);
		d = _l_ptradd(d, 1);
	}
}

errno_t __stdcall dbasememo_create(CConstPointer filename, dbasefile* fileinfo)
{
	errno_t result;
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif

	assert(fileinfo != NULL);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fileinfo->memofd = _tsopen(filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( fileinfo->memofd < 0 )
		return errno;
	result = _chsize( fileinfo->memofd, (long)(DBASEMEMO_CHUNKSIZE * 2) );
	if ( result )
		return errno;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fileinfo->memofd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( fileinfo->memofd < 0 )
		return errno;
	result = chsize( fileinfo->memofd, DBASEMEMO_CHUNKSIZE * 2 );
	if ( result )
		return result;
	fpos = lseek( fileinfo->memofd, 0, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&(fileinfo->memofd), filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
	result = _chsize_s( fileinfo->memofd, DBASEMEMO_CHUNKSIZE * 2 );
	if ( result )
		return result;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != 0 )
		return EIO;
	fileinfo->memopointer = 1L	;
	cnt = _write(fileinfo->memofd, &(fileinfo->memopointer), sizeof(dword));
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < sizeof(dword) )
		return EIO;
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->memofd))
		return errno;
#endif
	fileinfo->memodatachunk = TFalloc(DBASEMEMO_CHUNKSIZE);
	if ( PtrCheck(fileinfo->memodatachunk) )
		return ENOMEM;
	return 0;
}

errno_t __stdcall dbasememo_open(CConstPointer filename, dbasefile* fileinfo)
{
#ifdef OK_COMP_MSC
	errno_t result;
#endif
	int cnt;
#ifdef OK_SYS_WINDOWS
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t fpos;
#endif

	assert(fileinfo != NULL);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fileinfo->memofd = _tsopen(filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( fileinfo->memofd < 0 )
		return errno;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	fileinfo->memofd = open(filename, _O_BINARY | _O_RDWR);
    if ( fileinfo->memofd < 0 )
		return errno;
	fpos = lseek( fileinfo->memofd, 0, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&(fileinfo->memofd), filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != 0 )
		return EIO;
	cnt = _read(fileinfo->memofd, &(fileinfo->memopointer), sizeof(dword));
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < sizeof(dword) )
		return EIO;
	fileinfo->memodatachunk = TFalloc(DBASEMEMO_CHUNKSIZE);
	if ( PtrCheck(fileinfo->memodatachunk) )
		return ENOMEM;
	return 0;
}

errno_t __stdcall dbasememo_readchunk(dbasefile* fileinfo, sqword recno)
{
#ifdef OK_SYS_WINDOWS
	sqword filepos = recno * DBASEMEMO_CHUNKSIZE;
	sqword filelength;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t filepos = recno * DBASEMEMO_CHUNKSIZE;
	off_t filelength;
	off_t fpos;
#endif
	int cnt;

	assert(fileinfo != NULL);
	assert(fileinfo->memofd >= 0);
#ifdef OK_SYS_WINDOWS
	filelength = _filelengthi64(fileinfo->memofd);
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
	fpos = _lseeki64( fileinfo->memofd, filepos, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	filelength = lseek( fileinfo->memofd, 0, SEEK_END );
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
	fpos = lseek( fileinfo->memofd, filepos, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != filepos )
		return EIO;
	cnt = _read(fileinfo->memofd, fileinfo->memodatachunk, DBASEMEMO_CHUNKSIZE);
	if ( cnt < 0 )
		return errno;
	if ( cnt < DBASEMEMO_CHUNKSIZE )
		return EIO;
	return 0;
}

errno_t __stdcall dbasememo_writechunk(dbasefile* fileinfo, sqword recno)
{
#ifdef OK_SYS_WINDOWS
	sqword filepos = recno * DBASEMEMO_CHUNKSIZE;
	sqword filelength;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t filepos = recno * DBASEMEMO_CHUNKSIZE;
	off_t filelength;
	off_t fpos;
#endif
	int cnt;

	assert(fileinfo != NULL);
	assert(fileinfo->memofd >= 0);
#ifdef OK_SYS_WINDOWS
	filelength = _filelengthi64(fileinfo->memofd);
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
	fpos = _lseeki64( fileinfo->memofd, filepos, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
	filelength = lseek( fileinfo->memofd, 0, SEEK_END );
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
	fpos = lseek( fileinfo->memofd, filepos, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != filepos )
		return EIO;
	cnt = _write(fileinfo->memofd, fileinfo->memodatachunk, DBASEMEMO_CHUNKSIZE);
	if ( cnt < 0 )
		return errno;
	if ( cnt < DBASEMEMO_CHUNKSIZE )
		return EIO;
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->memofd))
		return errno;
#endif
	return 0;
}

errno_t __stdcall dbasememo_appendchunk(dbasefile* fileinfo, sqword* recno)
{
	errno_t result;
#ifdef OK_SYS_WINDOWS
	sqword filepos;
	sqword filelength;
	sqword fpos;
#endif
#ifdef OK_SYS_UNIX
	off_t filepos;
	off_t filelength;
	off_t fpos;
#endif
	int cnt;

	assert(fileinfo != NULL);
	assert(fileinfo->memofd >= 0);
	filepos = fileinfo->memopointer * DBASEMEMO_CHUNKSIZE;
#ifdef OK_SYS_WINDOWS
	filelength = _filelengthi64(fileinfo->memofd);
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
#endif
#ifdef OK_SYS_UNIX
	filelength = lseek( fileinfo->memofd, 0, SEEK_END );
	if ( 0 > filelength )
		return errno;
	assert(filepos < filelength);
#endif	
	++(fileinfo->memopointer);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	result = _chsize( fileinfo->memofd, (long)((fileinfo->memopointer + 1L) * DBASEMEMO_CHUNKSIZE) );
	if ( result < 0 )
		return errno;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
#ifdef OK_SYS_UNIX
#else
	result = chsize( fileinfo->memofd, (fileinfo->memopointer + 1L) * DBASEMEMO_CHUNKSIZE );
	if ( result )
		return result;
	fpos = lseek( fileinfo->memofd, 0, SEEK_SET );
#endif
#endif
#ifdef OK_COMP_MSC
	result = _chsize_s(fileinfo->memofd, (fileinfo->memopointer + 1L) * DBASEMEMO_CHUNKSIZE);
	if ( result )
		return result;
	fpos = _lseeki64( fileinfo->memofd, 0LL, SEEK_SET );
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != 0 )
		return EIO;
	cnt = _write(fileinfo->memofd, &(fileinfo->memopointer), sizeof(dword));
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < sizeof(dword) )
		return EIO;
#ifdef OK_SYS_WINDOWS
	fpos = _lseeki64(fileinfo->memofd, (fileinfo->memopointer - 1L) * DBASEMEMO_CHUNKSIZE, SEEK_SET);
#endif
#ifdef OK_SYS_UNIX
	fpos = lseek(fileinfo->memofd, (fileinfo->memopointer - 1L) * DBASEMEMO_CHUNKSIZE, SEEK_SET);
#endif
	if ( 0 > fpos )
		return errno;
	if ( fpos != ((fileinfo->memopointer - 1L) * DBASEMEMO_CHUNKSIZE) )
		return EIO;
	cnt = _write(fileinfo->memofd, fileinfo->memodatachunk, DBASEMEMO_CHUNKSIZE);
	if ( cnt < 0 )
		return errno;
	if ( cnt < DBASEMEMO_CHUNKSIZE )
		return EIO;
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->memofd))
		return errno;
#endif
	*recno = fileinfo->memopointer - 1L;
	return 0;
}

Pointer __stdcall dbasememo_getdata(dbasefile* fileinfo)
{
	assert(fileinfo != NULL);
	return fileinfo->memodatachunk;
}

void __stdcall dbasememo_setdata(dbasefile* fileinfo, Pointer data)
{
	assert(fileinfo != NULL);
	s_memcpy_s(fileinfo->memodatachunk, DBASEMEMO_CHUNKSIZE, data, DBASEMEMO_CHUNKSIZE);
}

errno_t __stdcall dbaseindex_init(dbasefile* fileinfo, dword indexcnt)
{
	assert(fileinfo != NULL);
	assert(indexcnt > 0);
	fileinfo->indexcnt = indexcnt;
	fileinfo->index = CastAnyPtr(dbaseindex*, TFalloc(indexcnt * sizeof(dbaseindex*)));
	if ( PtrCheck(fileinfo->index) )
		return ENOMEM;
	fileinfo->head->MDXmarker = 0x01;
	return 0;
}

errno_t __stdcall dbaseindex_initfields(dbasefile* fileinfo, dword index, dword fieldcnt)
{
	dbaseindex* p;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	fileinfo->index[index] = CastAnyPtr(dbaseindex, TFalloc(szdbaseindex));
	p = fileinfo->index[index];
	if ( PtrCheck(p) )
		return ENOMEM;
	p->fieldcnt = fieldcnt;
	p->fields = CastAnyPtr(dbasefield, TFalloc(fieldcnt * szdbasefield));
	if ( PtrCheck(p->fields) )
		return ENOMEM;
	return 0;
}

void __stdcall dbaseindex_initfield(dbasefile* fileinfo, dword index, dword field, dword fieldno, byte fieldlen)
{
	dbaseindex* p;
	dbasefield* p1;
	dbasefield* p2;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	p = fileinfo->index[index];
	assert(field < p->fieldcnt);
	p1 = CastAnyPtr(dbasefield, _l_ptradd(p->fields, field * szdbasefield));
	assert(fieldno < fileinfo->fieldcnt);
	p2 = CastAnyPtr(dbasefield, _l_ptradd(fileinfo->fields, fieldno * sizeof(dbasefield)));
	s_memcpy_s(p1->fname, 11, p2->fname, 11);
	p1->fadr = p2->fadr;
	p1->ftyp = p2->ftyp;
	p1->flen = fieldlen;
	p1->fdec = p2->fdec;
}

dword __stdcall dbaseindex_getindexcnt(dbasefile* fileinfo)
{
    assert(fileinfo != NULL);
    return fileinfo->indexcnt;
}

dword __stdcall dbaseindex_getindexfieldcnt(dbasefile* fileinfo, dword index)
{
    dbaseindex* p;

    assert(fileinfo != NULL);
    assert(index < fileinfo->indexcnt);
    p = fileinfo->index[index];
    return p->fieldcnt;
}

void __stdcall dbaseindex_getindexfieldinfo(dbasefile* fileinfo, dword index, dword field, Ptr(dword) fieldno, Ptr(byte) fieldlen)
{
    dbaseindex* p;
    dbasefield* p1;
    dbasefield* p2;
    byte* fn1;
    byte* fn2;

    assert(fileinfo != NULL);
    assert(index < fileinfo->indexcnt);
    p = fileinfo->index[index];
    assert(field < p->fieldcnt);
    p1 = CastAnyPtr(dbasefield, _l_ptradd(p->fields, field * szdbasefield));

    *fieldno = 0;
    p2 = fileinfo->fields;
    while ( *fieldno < fileinfo->fieldcnt )
    {
        fn1 = p1->fname;
        fn2 = p2->fname;
        while ( (*fn1 != 0) && (*fn2 != 0) )
        {
            if ( *fn1++ != *fn2++ )
                break;
        }
        if ( (*fn1 == 0) && (*fn2 == 0) )
            break;
        ++(*fieldno);
        p2 = CastAnyPtr(dbasefield, _l_ptradd(p2, szdbasefield));
    }
    *fieldlen = p1->flen;
}

static dword __stdcall dbaseindex_fileheadersize(dbasefile* fileinfo)
{
	dword result = szdbaseindexheader;
	dbaseindex* p;
	dword ix;

	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		p = fileinfo->index[ix];
		result += szdbaseindexsubheader + (p->fieldcnt * szdbasefield);
	}
	return result;
}

static dword __stdcall dbaseindex_keysize(dbasefile* fileinfo, dword index)
{
	dbaseindex* p = fileinfo->index[index];
	dbasefield* p1;
	dword ix;
	dword result = 0;

	for ( ix = 0, p1 = p->fields; ix < p->fieldcnt; ++ix, p1 = CastAnyPtr(dbasefield, _l_ptradd(p1, szdbasefield)) )
		result += p1->flen;
	return result;
}

static void __stdcall dbaseindex_setfadr(dbasefile* fileinfo)
{
	dword ix;
	dword jx;
	dbaseindex* index;
	dbasefield* field;
	dword fieldno;
	dbasefield* field1;
	byte* fn;
	byte* fn1;

	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		index = fileinfo->index[ix];
		for ( jx = 0, field = index->fields; 
			jx < index->fieldcnt; 
			++jx, field = CastAnyPtr(dbasefield, _l_ptradd(field, szdbasefield)) )
		{
			fieldno = 0;
			field1 = fileinfo->fields;
			while ( fieldno < fileinfo->fieldcnt )
			{
				fn1 = field1->fname;
				fn = field->fname;
				while ( (*fn != 0) && (*fn1 != 0) )
				{
					if ( *fn++ != *fn1++ )
						break;
				}
				if ( (*fn == 0) && (*fn1 == 0) )
					break;
				++fieldno;
				field1 = CastAnyPtr(dbasefield, _l_ptradd(field1, szdbasefield));
			}
			assert( fieldno < fileinfo->fieldcnt );
			field->fadr = field1->fadr;
		}
	}
}

errno_t __stdcall dbaseindex_create(CConstPointer filename, dbasefile* fileinfo)
{
#ifdef OK_COMP_MSC
	errno_t result;
#endif
	int cnt;
	dbaseindexheader header;
	dbaseindexsubheader subheader;
	_FBTreeFileHead btreefilehead;
	byte* p;
	dbaseindex* p1;
	dword ix;
	dword filepos;
#ifdef OK_SYS_WINDOWS
	sqword filepos1;
#endif
#ifdef OK_SYS_UNIX
	off_t filepos1;
#endif
	dword minpagesize;

	assert(fileinfo != NULL);
	dbaseindex_setfadr(fileinfo);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fileinfo->indexfd = _tsopen(filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( fileinfo->indexfd < 0 )
		return errno;
#endif
#ifdef OK_SYS_UNIX
	fileinfo->indexfd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if ( fileinfo->indexfd < 0 )
		return errno;
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&(fileinfo->indexfd), filename, _O_BINARY | _O_CREAT | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
#endif
	s_memset(&header, 0, szdbaseindexheader);
	p = CastAnyPtr(byte, &(header.art));
	*p++ = 'M';
	*p++ = 'D';
	*p++ = 'X';
	*p = 0;
	header.headersize = dbaseindex_fileheadersize(fileinfo);
	filepos = header.headersize;
	header.indexcnt = fileinfo->indexcnt;
	cnt = _write(fileinfo->indexfd, &header, szdbaseindexheader);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szdbaseindexheader )
		return EIO;
	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		p1 = fileinfo->index[ix];
		s_memset(&subheader, 0, szdbaseindexsubheader);
		subheader.fieldcnt = p1->fieldcnt;
		subheader.rootNode = filepos;
		filepos += szFBTreeFileHead;
		cnt = _write(fileinfo->indexfd, &subheader, szdbaseindexsubheader);
		if ( cnt < 0 )
			return errno;
		if ( Cast(dword,cnt) < szdbaseindexsubheader )
			return EIO;
		cnt = _write(fileinfo->indexfd, p1->fields, p1->fieldcnt * szdbasefield );
		if ( cnt < 0 )
			return errno;
		if ( cnt < Cast(int, (p1->fieldcnt * szdbasefield)) )
			return EIO;
	}
	for ( ix = 0; ix < fileinfo->indexcnt; ++ix )
	{
		p1 = fileinfo->index[ix];
		s_memset(&btreefilehead, 0, szFBTreeFileHead);
		btreefilehead.keySize = Cast(word, dbaseindex_keysize(fileinfo, ix));
		minpagesize = szFBTreeFileNode + (4 * (btreefilehead.keySize + szFBTreeFileNodeEntry));
		if ( minpagesize < 4096L )
		{
			btreefilehead.pagesize = 4096L;
			btreefilehead.maxEntriesPerNode = (4096L - szFBTreeFileNode) / (btreefilehead.keySize + szFBTreeFileNodeEntry);
		}
		else if ( minpagesize < 16384L )
		{
			btreefilehead.pagesize = 16384L;
			btreefilehead.maxEntriesPerNode = (16384L - szFBTreeFileNode) / (btreefilehead.keySize + szFBTreeFileNodeEntry);
		}
		assert(minpagesize < 16384L);
#ifdef OK_SYS_WINDOWS
		filepos1 = _telli64(fileinfo->indexfd);
#endif
#ifdef OK_SYS_UNIX
		filepos1 = lseek(fileinfo->indexfd, 0, SEEK_CUR);
#endif
		if ( filepos1 < 0 )
			return errno;
		btreefilehead.fphead = Cast(word, filepos1);
		cnt = _write(fileinfo->indexfd, &btreefilehead, szFBTreeFileHead );
		if ( cnt < 0 )
			return errno;
		if ( Cast(dword,cnt) < szFBTreeFileHead )
			return EIO;
		p1->index = TFalloc(szFBTreeFileHead);
		if ( PtrCheck(p1->index) )
			return ENOMEM;
		btreefilehead.fd = fileinfo->indexfd;
		s_memcpy_s(p1->index, szFBTreeFileHead, &btreefilehead, szFBTreeFileHead);
		p1->keyData = TFalloc(btreefilehead.keySize + szFBTreeFileNodeEntry);
		if ( PtrCheck(p1->keyData) )
			return ENOMEM;
	}
#ifdef OK_SYS_WINDOWS
	if (0 > _commit(fileinfo->indexfd))
		return errno;
#endif
	return 0;
}

errno_t __stdcall dbaseindex_open(CConstPointer filename, dbasefile* fileinfo)
{
	errno_t result;
	int cnt;
	dbaseindexheader header;
	dbaseindexsubheader subheader;
	_FBTreeFileHead btreefilehead;
	dbaseindex* p1;
	int ix;

	assert(fileinfo != NULL);
#ifdef OK_COMP_GNUC
#ifdef OK_SYS_WINDOWS
	fileinfo->indexfd = _tsopen(filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
    if ( fileinfo->indexfd < 0 )
		return errno;
#endif
#ifdef OK_SYS_UNIX
	fileinfo->indexfd = open(filename, _O_BINARY | _O_RDWR);
    if ( fileinfo->indexfd < 0 )
		return errno;
#endif
#endif
#ifdef OK_COMP_MSC
	result = _tsopen_s(&(fileinfo->indexfd), filename, _O_BINARY | _O_RDWR, _SH_DENYWR, _S_IREAD | _S_IWRITE);
	if ( result )
		return result;
#endif
	cnt = _read(fileinfo->indexfd, &header, szdbaseindexheader);
	if ( cnt < 0 )
		return errno;
	if ( Cast(dword,cnt) < szdbaseindexheader )
		return EIO;
	result = dbaseindex_init(fileinfo, header.indexcnt);
	if ( result )
		return result;
	for ( ix = 0; ix < Cast(int, fileinfo->indexcnt); ++ix )
	{
		cnt = _read(fileinfo->indexfd, &subheader, szdbaseindexsubheader);
		if ( cnt < 0 )
			return errno;
		if ( Cast(dword,cnt) < szdbaseindexsubheader )
			return EIO;
		result = dbaseindex_initfields(fileinfo, ix, subheader.fieldcnt);
		if ( result )
			return result;
		p1 = fileinfo->index[ix];
		cnt = _read(fileinfo->indexfd, p1->fields, p1->fieldcnt * szdbasefield );
		if ( cnt < 0 )
			return errno;
		if ( cnt < Cast(int, (p1->fieldcnt * szdbasefield)) )
			return EIO;
	}
	dbaseindex_setfadr(fileinfo);
	for ( ix = 0; ix < Cast(int, fileinfo->indexcnt); ++ix )
	{
		cnt = _read(fileinfo->indexfd, &btreefilehead, szFBTreeFileHead );
		if ( cnt < 0 )
			return errno;
		if ( Cast(dword,cnt) < szFBTreeFileHead )
			return EIO;
		p1 = fileinfo->index[ix];
		p1->index = TFalloc(szFBTreeFileHead);
		if ( PtrCheck(p1->index) )
			return ENOMEM;
		btreefilehead.fd = fileinfo->indexfd;
		btreefilehead.root = NULL;
		s_memcpy_s(p1->index, szFBTreeFileHead, &btreefilehead, szFBTreeFileHead);
		p1->keyData = TFalloc(btreefilehead.keySize + szFBTreeFileNodeEntry);
		if ( PtrCheck(p1->keyData) )
			return ENOMEM;
	}
	return 0;
}

static void __stdcall dbaseindex_fillkey(dbasefile* fileinfo, dword index)
{
    dbaseindex* p = fileinfo->index[index];
    Pointer key = _l_ptradd(p->keyData, szFBTreeFileNodeEntry);
    dbasefield* p1;
    dword ix;

    for ( ix = 0, p1 = p->fields; ix < p->fieldcnt; ++ix, p1 = CastAnyPtr(dbasefield, _l_ptradd(p1, szdbasefield)) )
    {
        s_memcpy_s(key, p1->flen, _l_ptradd(fileinfo->data,p1->fadr), p1->flen);
        key = _l_ptradd(key, p1->flen);
    }
}

static void __stdcall dbaseindex_fillkey_from_backup(dbasefile* fileinfo, dword index)
{
    dbaseindex* p = fileinfo->index[index];
    Pointer key = _l_ptradd(p->keyData, szFBTreeFileNodeEntry);
    dbasefield* p1;
    Pointer p2;
    dword ix;

    for ( ix = 0, p1 = p->fields; ix < p->fieldcnt; ++ix, p1 = CastAnyPtr(dbasefield, _l_ptradd(p1, szdbasefield)) )
    {
        p2 = _l_ptradd(fileinfo->backup, p1->fadr);
        s_memcpy_s(key, p1->flen, p2, p1->flen);
        key = _l_ptradd(key, p1->flen);
    }
}

static sword __stdcall
dbaseindex_usersearchfunc( ConstPointer pa, ConstPointer pb, Pointer context )
{
    Pointer ppa = CastAnyPtr(byte, _l_ptradd(CastMutable(Pointer, pa), szFBTreeFileNodeEntry));
    Pointer ppb = CastAnyPtr(byte, _l_ptradd(CastMutable(Pointer, pb), szFBTreeFileNodeEntry));
	dbaseindex* pIndex = CastAnyPtr(dbaseindex, context);
	dword ix;
	dbasefield* p1;
	int result;
	
	for ( ix = 0, p1 = pIndex->fields; ix < pIndex->fieldcnt; ++ix, p1 = CastAnyPtr(dbasefield, _l_ptradd(p1, szdbasefield)) )
	{
        result = s_memcmp(ppa, ppb, p1->flen);
		if ( result < 0 )
			return -1;
		if ( result > 0 )
			return 1;
        ppa = _l_ptradd(ppa, p1->flen);
        ppb = _l_ptradd(ppb, p1->flen);
	}
	return 0;
}

errno_t __stdcall dbaseindex_insertkey(dbasefile* fileinfo, dword index, sqword recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;
	LSearchResultType result;
    errno_t err;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, pIndex->keyData);
	p1->fpnext = recno;
	dbaseindex_fillkey(fileinfo, index);
	err = FBTreeInsertSorted(pIndex->index, pIndex->keyData, dbaseindex_usersearchfunc, pIndex, &result);
    if ( err )
        return err;
	if ( LPtrCheck(result) )
		return EINVAL;
	// FBTreeSortedTester(pIndex->index, dbaseindex_usersearchfunc, pIndex);
	return 0;
}

errno_t __stdcall dbaseindex_removekey(dbasefile* fileinfo, dword index, sqword recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;
	errno_t err;
	bool result;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, pIndex->keyData);
	p1->fpnext = recno;
    dbaseindex_fillkey_from_backup(fileinfo, index);
	err = FBTreeRemoveSorted(pIndex->index, pIndex->keyData, dbaseindex_usersearchfunc, pIndex, &result);
	if ( err )
        return err;
	FBTreeSortedTester(pIndex->index, dbaseindex_usersearchfunc, pIndex);
	return 0;
}

errno_t __stdcall dbaseindex_getSearchedRecno(dbasefile* fileinfo, dword index, Ptr(sqword) recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;
    errno_t err;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	dbaseindex_fillkey(fileinfo, index);
	if ( FBTreeCount(pIndex->index) == 0 )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1;
		return 0;
	}
	err = FBTreeFindSorted(pIndex->index, pIndex->keyData, dbaseindex_usersearchfunc, pIndex, &(pIndex->curRecord));
    if ( err )
        return err;
	if ( LPtrCheck(pIndex->curRecord) || PtrCheck(FBTreeGetData(pIndex->curRecord)) || (dbaseindex_usersearchfunc(FBTreeGetData(pIndex->curRecord), pIndex->keyData, pIndex) != 0) )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1;
		return 0;
	}
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, FBTreeGetData(pIndex->curRecord));
    DerefPtr(sqword, recno) = p1->fpnext;
	return 0;
}

errno_t __stdcall dbaseindex_getFirstRecno(dbasefile* fileinfo, dword index, Ptr(sqword) recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;
    errno_t err;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	if ( FBTreeCount(pIndex->index) == 0 )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1;
		return 0;
	}
	err = FBTreeBegin(pIndex->index, &(pIndex->curRecord));
    if ( err )
        return err;
	if ( LPtrCheck(pIndex->curRecord) || PtrCheck(FBTreeGetData(pIndex->curRecord)) )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1;
		return 0;
	}
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, FBTreeGetData(pIndex->curRecord));
    DerefPtr(sqword, recno) = p1->fpnext;
	return 0;
}

errno_t __stdcall dbaseindex_getNextRecno(dbasefile* fileinfo, dword index, Ptr(sqword) recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;
    errno_t err;
    LSearchResultType result;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	if ( LPtrCheck(pIndex->curRecord) )
		return EINVAL;
	err = FBTreeNext(pIndex->curRecord, &result);
    if ( err )
        return err;
	if ( LPtrCheck(result) || PtrCheck(FBTreeGetData(result)) )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1;
		return 0;
	}
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, FBTreeGetData(result));
    pIndex->curRecord = result;
    DerefPtr(sqword, recno) = p1->fpnext;
	return 0;
}

errno_t __stdcall dbaseindex_getRecno(dbasefile* fileinfo, dword index, Ptr(sqword) recno)
{
	_pFBTreeFileNodeEntry p1;
	dbaseindex* pIndex;

	assert(fileinfo != NULL);
	assert(index < fileinfo->indexcnt);
	pIndex = fileinfo->index[index];
	if ( LPtrCheck(pIndex->curRecord) || PtrCheck(FBTreeGetData(pIndex->curRecord)) )
	{
		pIndex->curRecord = _LNULL;
        DerefPtr(sqword, recno) = -1LL;
		return EINVAL;
	}
	p1 = CastAnyPtr(_FBTreeFileNodeEntry, FBTreeGetData(pIndex->curRecord));
    DerefPtr(sqword, recno) = p1->fpnext;
	return 0;
}

