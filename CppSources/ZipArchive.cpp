/******************************************************************************
    
	This file is part of CppSources, which is part of UserLib.

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
#include "CPPS_PCH.H"
#include "ZipArchive.h"
#include "ZipFilter.h"
#include "File.h"
#ifdef OK_SYS_WINDOWS
#include "DirectoryIterator.h"
#endif

class CPPSOURCES_LOCAL ZipCommon
{
public:
	enum
	{
		HEADER_SIZE = 4
	};

	enum CompressionMethod
	{
		CM_STORE   = 0,
		CM_SHRUNK  = 1,
		CM_FACTOR1 = 2,
		CM_FACTOR2 = 3,
		CM_FACTOR3 = 4,
		CM_FACTOR4 = 5,
		CM_IMPLODE = 6,
		CM_TOKENIZE= 7,
		CM_DEFLATE = 8,
		CM_ENHANCEDDEFLATE = 9,
		CM_DATECOMPRIMPLODING = 10,
		CM_UNUSED = 11
	};

	enum CompressionLevel
	{
		CL_NORMAL    = 0,
		CL_MAXIMUM   = 1,
		CL_FAST      = 2,
		CL_SUPERFAST = 3
	};

	enum HostSystem
	{
		HS_FAT = 0,  // + PKZIPW 2.50 VFAT, NTFS
		HS_AMIGA = 1,
		HS_VMS = 2,
		HS_UNIX = 3,
		HS_VM_CMS = 4,
		HS_ATARI = 5,
		HS_HPFS = 6,
		HS_MACINTOSH = 7,
		HS_ZSYSTEM = 8,
		HS_CP_M = 9,
		HS_TOPS20 = 10, // used by pkzip2.5 to indicate ntfs
		HS_NTFS = 11,
		HS_SMS_QDOS = 12,
		HS_ACORN = 13,
		HS_VFAT = 14,
		HS_MVS = 15,
		HS_BEOS = 16,
		HS_TANDEM = 17,
		HS_UNUSED = 18
	};

	enum FileType
	{
		FT_BINARY= 0,
		FT_ASCII = 1
	};
};

#ifdef OK_COMP_MSC
#pragma pack(push, 2)
#define HAVE_PRAGMA_PACK
#endif
#ifdef OK_COMP_GNUC
#undef HAVE_PRAGMA_PACK
#endif
struct CPPSOURCES_LOCAL TLocalFileHeader
{
//	dword signature; // 0x04034b50
	word version_needed_to_extract; // 0
	word general_purpose_bit_flag;  // 2
	word compression_method;        // 4
	word last_mod_file_time;        // 6
	word last_mod_file_date;        // 8
	dword crc32;                    // 10
	dword compressed_size;          // 14
	dword uncompressed_size;        // 18
	word file_name_length;          // 22
	word extra_field_length;        // 24
};            // 26

struct CPPSOURCES_LOCAL TFileHeader
{
//	dword signature; // 0x02014b50 
    word version_made_by;			// 0
    word version_needed_to_extract; // 2
    word general_purpose_bit_flag;  // 4
    word compression_method;        // 6
    word last_mod_file_time;        // 8
    word last_mod_file_date;        // 10
    dword crc32;                    // 12
    dword compressed_size;          // 16
    dword uncompressed_size;        // 20
    word file_name_length;          // 24
    word extra_field_length;        // 26
    word file_comment_length;       // 28
    word disk_number_start;         // 30
    word internal_file_attributes;  // 32
    dword external_file_attributes; // 34
    dword relative_offset_of_local_header; // 38
};                                  // 42

struct CPPSOURCES_LOCAL TArchiveInfo
{
//	dword signature; // 0x06054b50
    word number_of_this_disk;                        // 0
    word number_of_disk_of_central_dir_start;        // 2
    word number_entries_in_central_dir_on_this_disk; // 4
    word total_number_entries_in_central_dir;        // 6
    dword size_of_the_central_directory;             // 8
    dword central_dir_start_offset_in_disk;          // 12
    word zip_file_comment_length;                    // 16
};                                                   // 18

struct CPPSOURCES_LOCAL TNTFSExtraInfo
{
	word extratag; // 0000a                  // 0 
	word totalsize;                          // 2
	dword reserved0;                         // 4
	// tagdata
	struct __attrdata
	{
		word attrtag;                        // 8   0
		word attrsize;                       // 10  2
		qword mtime; // last modified time   // 12  4
		qword atime; // last access time     // 20  12
		qword ctime; // creation time        // 28  20
	} data;                                  //     28
};                                           // 36
#ifdef OK_COMP_MSC
#pragma pack(pop)
#endif

class CPPSOURCES_LOCAL ZipLocalFileHeader: public CCppObject
{
public:
	ZipLocalFileHeader()
	{
#ifdef HAVE_PRAGMA_PACK
		assert(26 == sizeof(TLocalFileHeader));
		m_header.set_BufferSize(__FILE__LINE__ sizeof(TLocalFileHeader));
#else
		m_header.set_BufferSize(__FILE__LINE__ 26);
#endif
	}

	void read(Ptr(CFile) file)
	{
		word sz;

		file->Read(m_header);
		sz = get_file_name_length();
		if ( 0 < sz )
		{
			m_filename.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_filename);
		}
		sz = get_extra_field_length();
		if ( 0 < sz )
		{
			m_extra.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_extra);
		}
	}

	void write(Ptr(CFile) file)
	{
		word sz;

		file->Write(m_header);
		sz = get_file_name_length();
		if ( 0 < sz )
			file->Write(m_filename);
		sz = get_extra_field_length();
		if ( 0 < sz )
			file->Write(m_extra);
	}

	static dword get_signature()
	{
		return 0x04034b50;
	}

	ZipCommon::HostSystem get_needed_host_system() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract;
#else
		word flags = DerefWPointer(m_header.get_Buffer());
#endif

		flags >>= 8;
		flags &= 0x00FF;
		return Cast(ZipCommon::HostSystem, flags);
	}

	void set_needed_host_system(ZipCommon::HostSystem v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0x00FF;
		word v1 = v & 0x00FF;

		v1 <<= 8;
		SETBIT(flags, v1);
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract = flags;
#else
		word flags = DerefWPointer(m_header.get_Buffer()) & 0x00FF;
		word v1 = v & 0x00FF;

		v1 <<= 8;
		SETBIT(flags, v1);
		DerefWPointer(m_header.get_Buffer()) = flags;
#endif
	}

	word get_version_needed_to_extract() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0x00FF;
#else
		return DerefWPointer(m_header.get_Buffer()) & 0x00FF;
#endif
	}

	void set_version_needed_to_extract(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0xFF00;

		SETBIT(flags, v & 0x00FF);
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->version_needed_to_extract = flags;
#else
		word flags = DerefWPointer(m_header.get_Buffer()) & 0xFF00;

		SETBIT(flags, v & 0x00FF);
		DerefWPointer(m_header.get_Buffer()) = flags;
#endif
	}

	bool get_encryption() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(),2));
#endif

		return TestBitN(flags,0);
	}

	void set_encryption(bool v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(),2));
#endif

		if ( v )
			SETBITN(flags,0);
		else
			DELBITN(flags,0);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(),2)) = flags;
#endif
	}

	ZipCommon::CompressionLevel get_compression_level() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2));
#endif

		return Cast(ZipCommon::CompressionLevel, ((flags >> 1) & 0x0003));
	}

	void set_compression_level(ZipCommon::CompressionLevel v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag & 0xFFF9;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) & 0xFFF9;
#endif
		word v1 = v & 0x0003;

		v1 <<= 1;
		SETBIT(flags,v1);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) = flags;
#endif
	}

	bool has_data_record() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2));
#endif

		return TestBitN(flags,3);
	}

	void set_data_record(bool v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2));
#endif

		if ( v ) 
			SETBITN(flags, 3);
		else
			DELBITN(flags, 3);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) = flags;
#endif
	}

	ZipCommon::CompressionMethod get_compression_method() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->compression_method;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		return Cast(ZipCommon::CompressionMethod, flags);
	}

	void set_compression_method(ZipCommon::CompressionMethod v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->compression_method = Cast(word, v);
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) = Cast(word, v);
#endif
	}

	CDateTime get_last_mod_file() const
	{
#ifdef HAVE_PRAGMA_PACK
		word file_time = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->last_mod_file_time;
		word file_date = CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->last_mod_file_date;
#else
		word file_time = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6));
		word file_date = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 8));
#endif

		return CDateTime(file_date, file_time);
	}

	void set_last_mod_file(ConstRef(CDateTime) t)
	{
		word file_time;
		word file_date;

		t.GetDosDateTime(file_date, file_time);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->last_mod_file_time = file_time;
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->last_mod_file_date = file_date;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6)) = file_time;
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 8)) = file_date;
#endif
	}

	dword get_crc32() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->crc32;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 10));
#endif
	}

	void set_crc32(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->crc32 = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 10)) = v;
#endif
	}

	dword get_compressed_size() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->compressed_size;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 14));
#endif
	}

	void set_compressed_size(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->compressed_size = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 14)) = v;
#endif
	}

	dword get_uncompressed_size() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->uncompressed_size;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 18));
#endif
	}

	void set_uncompressed_size(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->uncompressed_size = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 18)) = v;
#endif
	}

	word get_file_name_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->file_name_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 22));
#endif
	}

	word get_extra_field_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->extra_field_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 24));
#endif
	}

	CStringBuffer get_filename() const
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(m_filename);
		return tmp;
	}

	void set_filename(ConstRef(CStringBuffer) name)
	{
		name.convertToByteBuffer(m_filename);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->file_name_length = Castword(m_filename.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 22)) = Castword(m_filename.get_BufferSize());
#endif
	}

	ConstRef(CByteBuffer) get_extra_field() const
	{
		return m_extra;
	}

	void set_extra_field(ConstRef(CByteBuffer) extra)
	{
		m_extra = extra;
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TLocalFileHeader, m_header.get_Buffer())->extra_field_length = Castword(m_extra.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 24)) = Castword(m_extra.get_BufferSize());
#endif
	}

protected:
	CByteBuffer m_header;
	CByteBuffer m_filename;
	CByteBuffer m_extra;
};

class CPPSOURCES_LOCAL ZipFileHeader: public CCppObject
{
public:
	ZipFileHeader()
	{
#ifdef HAVE_PRAGMA_PACK
		assert(42 == sizeof(TFileHeader));
		m_header.set_BufferSize(__FILE__LINE__ sizeof(TFileHeader));
#else
		m_header.set_BufferSize(__FILE__LINE__ 42);
#endif
	}

	void read(Ptr(CFile) file)
	{
		word sz;

		file->Read(m_header);
		sz = get_file_name_length();
		if ( 0 < sz )
		{
			m_filename.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_filename);
		}
		sz = get_extra_field_length();
		if ( 0 < sz )
		{
			m_extra.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_extra);
		}
		sz = get_file_comment_length();
		if ( 0 < sz )
		{
			m_filecomment.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_filecomment);
		}
	}

	void write(Ptr(CFile) file)
	{
		word sz;

		file->Write(m_header);
		sz = get_file_name_length();
		if ( 0 < sz )
			file->Write(m_filename);
		sz = get_extra_field_length();
		if ( 0 < sz )
			file->Write(m_extra);
		sz = get_file_comment_length();
		if ( 0 < sz )
			file->Write(m_filecomment);
	}

	static dword get_signature()
	{
		return 0x02014b50;
	}

	ZipCommon::HostSystem get_made_host_system() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0));
#endif

		flags >>= 8;
		flags &= 0x00FF;
		return Cast(ZipCommon::HostSystem, flags);
	}

	void set_made_host_system(ZipCommon::HostSystem v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by & 0x00FF;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) & 0x00FF;
#endif
		word v1 = v & 0x00FF;

		v1 <<= 8;
		SETBIT(flags, v1);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) = flags;
#endif
	}

	word get_version_made_by() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by & 0x00FF;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) & 0x00FF;
#endif

		return flags;
	}

	void set_version_made_by(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by & 0x00FF;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) & 0x00FF;
#endif

		SETBIT(flags, v & 0x00FF);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_made_by = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) = flags;
#endif
	}

	ZipCommon::HostSystem get_needed_host_system() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2));
#endif

		flags >>= 8;
		flags &= 0x00FF;
		return Cast(ZipCommon::HostSystem, flags);
	}

	void set_needed_host_system(ZipCommon::HostSystem v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0x00FF;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) & 0x00FF;
#endif
		word v1 = v & 0x00FF;

		v1 <<= 8;
		SETBIT(flags, v1);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) = flags;
#endif
	}

	word get_version_needed_to_extract() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0x00FF;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) & 0x00FF;
#endif
		return flags;
	}

	void set_version_needed_to_extract(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract & 0xFF00;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) & 0xFF00;
#endif

		SETBIT(flags, v & 0x00FF);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->version_needed_to_extract = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) = flags;
#endif
	}

	bool get_encryption() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		return TestBitN(flags,0);
	}

	void set_encryption(bool v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		if ( v )
			SETBITN(flags,0);
		else
			DELBITN(flags,0);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) = flags;
#endif
	}

	ZipCommon::CompressionLevel get_compression_level() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		return Cast(ZipCommon::CompressionLevel, ((flags >> 1) & 0x0003));
	}

	void set_compression_level(ZipCommon::CompressionLevel v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag & 0xFFF9;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) & 0xFFF9;
#endif
		word v1 = v & 0x0003;

		v1 <<= 1;
		SETBIT(flags,v1);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) = flags;
#endif
	}

	bool has_data_record() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		return TestBitN(flags,3);
	}

	void set_data_record(bool v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif

		if ( v ) 
			SETBITN(flags, 3);
		else
			DELBITN(flags, 3);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->general_purpose_bit_flag = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) = flags;
#endif
	}

	ZipCommon::CompressionMethod get_compression_method() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->compression_method;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6));
#endif

		return Cast(ZipCommon::CompressionMethod, flags);
	}

	void set_compression_method(ZipCommon::CompressionMethod v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->compression_method = Cast(word, v);
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6)) = Cast(word, v);
#endif
	}

	CDateTime get_last_mod_file() const
	{
#ifdef HAVE_PRAGMA_PACK
		word file_time = CastAnyPtr(TFileHeader, m_header.get_Buffer())->last_mod_file_time;
		word file_date = CastAnyPtr(TFileHeader, m_header.get_Buffer())->last_mod_file_date;
#else
		word file_time = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 8));
		word file_date = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 10));
#endif

		return CDateTime(file_date, file_time);
	}

	void set_last_mod_file(ConstRef(CDateTime) t)
	{
		word file_time;
		word file_date;

		t.GetDosDateTime(file_date, file_time);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->last_mod_file_time = file_time;
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->last_mod_file_date = file_date;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 8)) = file_time;
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 10)) = file_date;
#endif
	}

	dword get_crc32() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->crc32;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 12));
#endif
	}

	void set_crc32(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->crc32 = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 12)) = v;
#endif
	}

	dword get_compressed_size() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->compressed_size;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 16));
#endif
	}

	void set_compressed_size(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->compressed_size = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 16)) = v;
#endif
	}

	dword get_uncompressed_size() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->uncompressed_size;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 20));
#endif
	}

	void set_uncompressed_size(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->uncompressed_size = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 20)) = v;
#endif
	}

	word get_file_name_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->file_name_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 24));
#endif
	}

	word get_extra_field_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->extra_field_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 26));
#endif
	}

	word get_file_comment_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->file_comment_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 28));
#endif
	}

	CStringBuffer get_filename() const
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(m_filename);
		return tmp;
	}

	void set_filename(ConstRef(CStringBuffer) name)
	{
		name.convertToByteBuffer(m_filename);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->file_name_length = Castword(m_filename.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 24)) = Castword(m_filename.get_BufferSize());
#endif
	}

	ConstRef(CByteBuffer) get_extra_field() const
	{
		return m_extra;
	}

	void set_extra_field(ConstRef(CByteBuffer) extra)
	{
		m_extra = extra;
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->extra_field_length = Castword(m_extra.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 26)) = Castword(m_extra.get_BufferSize());
#endif
	}

	CStringBuffer get_file_comment() const
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(m_filecomment);
		return tmp;
	}

	void set_file_comment(ConstRef(CStringBuffer) comment)
	{
		comment.convertToByteBuffer(m_filecomment);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->file_comment_length = Castword(m_filecomment.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 28)) = Castword(m_filecomment.get_BufferSize());
#endif
	}

	word get_disk_number_start() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TFileHeader, m_header.get_Buffer())->disk_number_start;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 30));
#endif
	}

	void set_disk_number_start(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->disk_number_start = v;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 30)) = v;
#endif
	}

	ZipCommon::FileType get_file_type() const
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->internal_file_attributes;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 32));
#endif
		return Cast(ZipCommon::FileType, flags & 0x0001);
	}

	void set_file_type(ZipCommon::FileType v)
	{
#ifdef HAVE_PRAGMA_PACK
		word flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->internal_file_attributes & 0xFFFE;
#else
		word flags = DerefWPointer(_l_ptradd(m_header.get_Buffer(), 32)) & 0xFFFE;
#endif

		SETBIT(flags, v);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->internal_file_attributes = flags;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 32)) = flags;
#endif
	}

	dword get_external_file_attributes() const
	{
#ifdef HAVE_PRAGMA_PACK
		dword flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->external_file_attributes;
#else
		dword flags = DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 34));
#endif

		return flags;
	}

	void set_external_file_attributes(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->external_file_attributes = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 34)) = v;
#endif
	}

	dword get_relative_offset_of_local_header() const
	{
#ifdef HAVE_PRAGMA_PACK
		dword flags = CastAnyPtr(TFileHeader, m_header.get_Buffer())->relative_offset_of_local_header;
#else
		dword flags = DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 38));
#endif

		return flags;
	}

	void set_relative_offset_of_local_header(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TFileHeader, m_header.get_Buffer())->relative_offset_of_local_header = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 38)) = v;
#endif
	}

	word get_entry_size() const
	{
		return 4 + Castword(m_header.get_BufferSize()) + get_file_name_length() + get_extra_field_length() + get_file_comment_length();
	}

protected:
	CByteBuffer m_header;
	CByteBuffer m_filename;
	CByteBuffer m_extra;
	CByteBuffer m_filecomment;
};

class CPPSOURCES_LOCAL ZipArchiveInfo: public CCppObject
{
public:
	ZipArchiveInfo()
	{
#ifdef HAVE_PRAGMA_PACK
		assert(18 == sizeof(TArchiveInfo));
		m_header.set_BufferSize(__FILE__LINE__ sizeof(TArchiveInfo));
#else
		m_header.set_BufferSize(__FILE__LINE__ 18);
#endif
	}

	void read(Ptr(CFile) file)
	{
		word sz;

		file->Read(m_header);
		sz = get_zip_file_comment_length();
		if ( 0 < sz )
		{
			m_zipfilecomment.set_BufferSize(__FILE__LINE__ sz);
			file->Read(m_zipfilecomment);
		}
	}

	void write(Ptr(CFile) file)
	{
		word sz;

		file->Write(m_header);
		sz = get_zip_file_comment_length();
		if ( 0 < sz )
			file->Write(m_zipfilecomment);
	}

	static dword get_signature()
	{
		return 0x06054b50;
	}

	word get_number_of_this_disk() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_of_this_disk;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0));
#endif
	}

	void set_number_of_this_disk(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_of_this_disk = v;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 0)) = v;
#endif
	}

	word get_number_of_disk_of_central_dir_start() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_of_disk_of_central_dir_start;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2));
#endif
	}

	void set_number_of_disk_of_central_dir_start(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_of_disk_of_central_dir_start = v;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 2)) = v;
#endif
	}

	word get_number_entries_in_central_dir_on_this_disk() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_entries_in_central_dir_on_this_disk;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4));
#endif
	}

	void set_number_entries_in_central_dir_on_this_disk(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->number_entries_in_central_dir_on_this_disk = v;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 4)) = v;
#endif
	}

	word get_total_number_entries_in_central_dir() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->total_number_entries_in_central_dir;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6));
#endif
	}

	void set_total_number_entries_in_central_dir(word v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->total_number_entries_in_central_dir = v;
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 6)) = v;
#endif
	}

	dword get_size_of_the_central_directory() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->size_of_the_central_directory;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 8));
#endif
	}

	void set_size_of_the_central_directory(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->size_of_the_central_directory = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 8)) = v;
#endif
	}

	dword get_central_dir_start_offset_in_disk() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->central_dir_start_offset_in_disk;
#else
		return DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 12));
#endif
	}

	void set_central_dir_start_offset_in_disk(dword v)
	{
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->central_dir_start_offset_in_disk = v;
#else
		DerefDWPointer(_l_ptradd(m_header.get_Buffer(), 12)) = v;
#endif
	}

	word get_zip_file_comment_length() const
	{
#ifdef HAVE_PRAGMA_PACK
		return CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->zip_file_comment_length;
#else
		return DerefWPointer(_l_ptradd(m_header.get_Buffer(), 16));
#endif
	}

	CStringBuffer get_zip_file_comment() const
	{
		CStringBuffer tmp;

		tmp.convertFromByteBuffer(m_zipfilecomment);
		return tmp;
	}

	void set_zip_file_comment(ConstRef(CStringBuffer) comment)
	{
		comment.convertToByteBuffer(m_zipfilecomment);
#ifdef HAVE_PRAGMA_PACK
		CastAnyPtr(TArchiveInfo, m_header.get_Buffer())->zip_file_comment_length = Castword(m_zipfilecomment.get_BufferSize());
#else
		DerefWPointer(_l_ptradd(m_header.get_Buffer(), 16)) = Castword(m_zipfilecomment.get_BufferSize());
#endif
	}

protected:
	CByteBuffer m_header;
	CByteBuffer m_zipfilecomment;
};

class CPPSOURCES_API CZipArchiveImpl : public CArchive
{
public:
	class TFileCacheItem: public CCppObject
	{
	public:
		TFileCacheItem() {}
		virtual ~TFileCacheItem() {}

		CCppObjectPtr<ZipLocalFileHeader> localFileHeader;
		CCppObjectPtr<ZipFileHeader> fileHeader;
		CByteLinkedBuffer fileContent;

		void Reserve(CFile::TFileSize sz, CFile::TFileSize parts)
		{
			fileContent.Clear();
			if (sz == 0)
				return;
			while (sz > parts)
			{
				fileContent.AddBufferItem(Cast(dword, parts));
				sz -= parts;
			}
			if (sz > 0)
				fileContent.AddBufferItem(Cast(dword, sz));
		}
	};

	class TFileCacheItemLessFunctor
	{
	public:
		bool operator()(ConstPtr(TFileCacheItem) pA, ConstPtr(TFileCacheItem) pB) const
		{
			return pA->localFileHeader->get_filename().LT(pB->localFileHeader->get_filename(), 0, CStringLiteral::cIgnoreCase);
		}
	};

	class TFileCacheItemEqualFunctor
	{
	public:
		bool operator()(ConstPtr(TFileCacheItem) pA, ConstPtr(TFileCacheItem) pB) const
		{
			Ptr(CStringBuffer) tmp = CastAnyPtr(CStringBuffer, CastMutablePtr(TFileCacheItem, pB));
			return pA->localFileHeader->get_filename().EQ(*tmp, 0, CStringLiteral::cIgnoreCase);
		}
	};

	typedef CDataVectorT<TFileCacheItem, TFileCacheItemLessFunctor> TFileCacheItems;

public:
	CZipArchiveImpl(Ptr(CFile) _file);
	virtual ~CZipArchiveImpl(void);

	virtual Ptr(CArchiveIterator) begin();

	virtual void AddOpen();
	virtual void AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props);
	virtual void AddDelete(ConstRef(CFilePath) fname);
	virtual void AddClose();

protected:
	CDataVectorT<ZipArchiveInfo> m_Disks;
	TFileCacheItems m_FileCache;
	bool m_opened;
	bool m_modified;

	friend class CZipArchiveIterator;
	friend class CZipArchiveFile;

private:
	CZipArchiveImpl();
	CZipArchiveImpl(ConstRef(CZipArchiveImpl));
	Ref(CZipArchiveImpl) operator=(ConstRef(CZipArchiveImpl));
};

CZipArchiveFile::CZipArchiveFile(Ref(CZipArchiveImpl) _archive, ConstRef(LSearchResultType) dataPtr) :
    CArchiveFile(_archive, 0), m_ziparchive(_archive), m_dataPtr(dataPtr)
{
	CZipArchiveImpl::TFileCacheItems::Iterator it(m_dataPtr);
	Ptr(CZipArchiveImpl::TFileCacheItem) pCacheItem = *it;

	m_length = pCacheItem->fileContent.GetTotalLength();
}

CZipArchiveFile::~CZipArchiveFile()
{
}

void CZipArchiveFile::Read(Ref(CByteBuffer) _buffer)
{
	CZipArchiveImpl::TFileCacheItems::Iterator it(m_dataPtr);
	Ptr(CZipArchiveImpl::TFileCacheItem) pCacheItem = *it;
	CByteLinkedBuffer::Iterator it2(pCacheItem->fileContent.Begin());
	dword bufSize = _buffer.get_BufferSize();
	dword restLen = m_length - m_offset;
	dword want = Min(bufSize, restLen);
	
	_buffer.set_BufferSize(__FILE__LINE__ want);
	if ( want )
	{
		it2 += m_offset;
		pCacheItem->fileContent.GetSubBuffer(it2, _buffer);
		if ( _buffer.get_BufferSize() < want )
			throw OK_NEW_OPERATOR CArchiveException(__FILE__LINE__ _T("unexpected eof"));
		m_offset += want;
	}
}

CZipArchiveIterator::CZipArchiveIterator(Ref(CZipArchiveImpl) _archive):
    CArchiveIterator(_archive), m_ziparchive(_archive), m_dataPtr(_LNULL)
{
	m_ziparchive.AddOpen();
}
	
CZipArchiveIterator::~CZipArchiveIterator()
{
}

CArchiveIterator::archive_file_t CZipArchiveIterator::GetType() const
{
	if ( LPtrCheck(m_dataPtr) )
		return ARCHIVE_FILE_INVALID;
	return ARCHIVE_FILE_REGULAR;
}

CStringBuffer CZipArchiveIterator::GetFileName() const
{
	CStringBuffer tmp;

	if ( LPtrCheck(m_dataPtr) )
		return tmp;

	CZipArchiveImpl::TFileCacheItems::Iterator it(m_dataPtr);
	Ptr(CZipArchiveImpl::TFileCacheItem) pCacheItem = *it;

	return pCacheItem->localFileHeader->get_filename();
}

Ptr(CArchiveFile) CZipArchiveIterator::GetFile()
{
	return OK_NEW_OPERATOR CZipArchiveFile(m_ziparchive, m_dataPtr);
}

bool CZipArchiveIterator::Next()
{
	CZipArchiveImpl::TFileCacheItems::Iterator it(m_dataPtr);

	for ( ;; )
	{
		if ( !it )
			it = m_ziparchive.m_FileCache.Begin();
		else
			++it;
		ClearProperties();
		m_dataPtr = it;
		if ( !it )
			return false;

		Ptr(CZipArchiveImpl::TFileCacheItem) pCacheItem = *it;
#ifdef HAVE_PRAGMA_PACK
		TNTFSExtraInfo info;
#endif
		CByteBuffer buf;
		CDateTime dt;
		time_t t;

		dt = pCacheItem->localFileHeader->get_last_mod_file();
		dt.GetTime(t);
		SetProperty(_T("DOSFILETIME"), Cast(sqword, t));
		buf = pCacheItem->localFileHeader->get_extra_field();
		if (buf.get_BufferSize() > 0)
		{
#ifdef HAVE_PRAGMA_PACK
			s_memcpy(&info, buf.get_Buffer(), sizeof(TNTFSExtraInfo));
			if ((info.extratag == 0x000a) && (info.totalsize == sizeof(TNTFSExtraInfo)) && (info.data.attrtag == 0x0001) && (info.data.attrsize == sizeof(TNTFSExtraInfo::__attrdata)))
			{
				SetProperty(_T("NTFSMTIME"), info.data.mtime);
				SetProperty(_T("NTFSATIME"), info.data.atime);
				SetProperty(_T("NTFSCTIME"), info.data.ctime);
			}
#else
			if ((DerefWPointer(_l_ptradd(buf.get_Buffer(), 0)) == 0x000a) && (DerefWPointer(_l_ptradd(buf.get_Buffer(), 2)) == 36) && 
				(DerefWPointer(_l_ptradd(buf.get_Buffer(), 8)) == 0x0001) && (DerefWPointer(_l_ptradd(buf.get_Buffer(), 10)) == 28))
			{
				SetProperty(_T("NTFSMTIME"), DerefQWPointer(_l_ptradd(buf.get_Buffer(), 12)));
				SetProperty(_T("NTFSATIME"), DerefQWPointer(_l_ptradd(buf.get_Buffer(), 20)));
				SetProperty(_T("NTFSCTIME"), DerefQWPointer(_l_ptradd(buf.get_Buffer(), 28)));
			}
#endif
		}
		SetProperty(_T("CRC32"), pCacheItem->localFileHeader->get_crc32());
		SetProperty(_T("UNCOMPRESSEDSIZE"), pCacheItem->localFileHeader->get_uncompressed_size());
		return true;
	}
	return false;
}

void CZipArchiveIterator::Skip()
{
}

CZipArchive::CZipArchive(Ptr(CFile) _file) : CArchive(_file), _impl(OK_NEW_OPERATOR CZipArchiveImpl(_file)) {}
CZipArchive::~CZipArchive(void) { if (_impl) _impl->release(); }

Ptr(CArchiveIterator) CZipArchive::begin() { if (_impl) return _impl->begin(); return nullptr; }

void CZipArchive::AddOpen() { if (_impl) _impl->AddOpen(); }
void CZipArchive::AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props)  { if (_impl) _impl->AddFile(fname, props); }
void CZipArchive::AddDelete(ConstRef(CFilePath) fname) { if (_impl) _impl->AddDelete(fname); }
void CZipArchive::AddClose() { if (_impl) _impl->AddClose(); }

CZipArchiveImpl::CZipArchiveImpl(Ptr(CFile) _file) :
CArchive(_file), m_Disks(__FILE__LINE__ 8, 8), m_FileCache(__FILE__LINE__ 256, 256), m_opened(false), m_modified(false)
{
}
	
CZipArchiveImpl::~CZipArchiveImpl(void)
{
}

Ptr(CArchiveIterator) CZipArchiveImpl::begin()
{
	return OK_NEW_OPERATOR CZipArchiveIterator(*this);
}

void CZipArchiveImpl::AddOpen()
{
	if (m_opened)
		return;

	CByteBuffer signature(__FILE__LINE__ 4);

	m_file->SetFilePos(0);
	m_file->Read(signature);
	while ( DerefAnyPtr(dword, signature.get_Buffer()) == ZipLocalFileHeader::get_signature() )
	{
		Ptr(ZipLocalFileHeader) pLocalFileHeader = OK_NEW_OPERATOR ZipLocalFileHeader();
		Ptr(TFileCacheItem) fci = OK_NEW_OPERATOR TFileCacheItem;

		pLocalFileHeader->read(m_file);
		fci->localFileHeader = pLocalFileHeader;
		fci->Reserve(pLocalFileHeader->get_compressed_size(), 8192);
		m_file->Read(fci->fileContent);
		m_FileCache.InsertSorted(fci);
		m_file->Read(signature);
	}
	while ( DerefAnyPtr(dword, signature.get_Buffer()) == ZipFileHeader::get_signature() )
	{
		Ptr(ZipFileHeader) pFileHeader = OK_NEW_OPERATOR ZipFileHeader();
		CStringBuffer tmp;

		pFileHeader->read(m_file);
		tmp = pFileHeader->get_filename();

		TFileCacheItems::Iterator itFileCacheItem = m_FileCache.Find<TFileCacheItemEqualFunctor>(CastAnyPtr(TFileCacheItem, &tmp));

		if (itFileCacheItem)
		{
			Ptr(TFileCacheItem) pCacheItem = *itFileCacheItem;

			pCacheItem->fileHeader = pFileHeader;
		}
		m_file->Read(signature);
	}
	m_opened = true;
}

class CPPSOURCES_LOCAL CZipArchiveFilterOutput: public CFilterOutput
{
public:
	CZipArchiveFilterOutput(CZipArchiveImpl* archive);
	virtual ~CZipArchiveFilterOutput();

	__inline Ptr(CZipArchiveImpl::TFileCacheItem) get_CacheItem()
	{ 
		if (!m_item)
			m_item = OK_NEW_OPERATOR CZipArchiveImpl::TFileCacheItem;
		return m_item; 
	}

	virtual void open();
	virtual void write(Ref(CByteBuffer) outputbuf);
	virtual void close();

protected:
	Ptr(CZipArchiveImpl) m_archive;
	Ptr(CZipArchiveImpl::TFileCacheItem) m_item;
};

CZipArchiveFilterOutput::CZipArchiveFilterOutput(CZipArchiveImpl* archive): m_archive(archive), m_item(nullptr) {}
CZipArchiveFilterOutput::~CZipArchiveFilterOutput() {}

void CZipArchiveFilterOutput::open() {}

void CZipArchiveFilterOutput::write(Ref(CByteBuffer) outputbuf) 
{
	get_CacheItem()->fileContent.AddBufferItem(outputbuf);
}

void CZipArchiveFilterOutput::close() {}

void CZipArchiveImpl::AddFile(ConstRef(CFilePath) fname, ConstRef(CArchiveProperties) props)
{
	CStringBuffer tmp;
	sqword vDateTime;
	sqword vFileSize;
	sqword vCreationTime;
	sqword vLastAccessTime;
	sqword vLastWriteTime;
	bool isNull;
	bool shouldFetchFileSize = false;
	bool hasNTFSData = true;
	CDateTime DateTime;

#ifdef OK_SYS_WINDOWS
	tmp = fname.get_Directory();
	tmp += fname.get_Filename();
	tmp.ReplaceString(CDirectoryIterator::WinPathSeparatorString(), CDirectoryIterator::UnixPathSeparatorString());
#endif
#ifdef OK_SYS_UNIX
	tmp = fname.get_Path();
#endif
	props.GetProperty(_T("FILETIME"), vDateTime, isNull);
	if (isNull)
		DateTime.Now();
	else
		DateTime.SetTime(vDateTime);
	props.GetProperty(_T("FILESIZE"), vFileSize, isNull);
	if (isNull)
		shouldFetchFileSize = true;
	props.GetProperty(_T("NTFSLWTIME"), vLastWriteTime, isNull);
	if (isNull)
		hasNTFSData = false;
	else
	{
		props.GetProperty(_T("NTFSLATIME"), vLastAccessTime, isNull);
		if (isNull)
			vLastAccessTime = 0;
		props.GetProperty(_T("NTFSCRTIME"), vCreationTime, isNull);
		if (isNull)
			vCreationTime = 0;
	}

	Ptr(ZipLocalFileHeader) pLocalFileHeader = OK_NEW_OPERATOR ZipLocalFileHeader;

	pLocalFileHeader->set_filename(tmp);
	pLocalFileHeader->set_compressed_size(0);
	pLocalFileHeader->set_compression_level(ZipCommon::CL_NORMAL);
	pLocalFileHeader->set_compression_method(ZipCommon::CM_DEFLATE);
	pLocalFileHeader->set_crc32(0);
	pLocalFileHeader->set_data_record(false);
	pLocalFileHeader->set_encryption(false);
	pLocalFileHeader->set_last_mod_file(DateTime);
	pLocalFileHeader->set_needed_host_system(ZipCommon::HS_FAT);
	if (!shouldFetchFileSize)
		pLocalFileHeader->set_uncompressed_size(Castdword(vFileSize));
	pLocalFileHeader->set_version_needed_to_extract(20);

	if (hasNTFSData)
	{
#ifdef HAVE_PRAGMA_PACK
		TNTFSExtraInfo info;
#endif
		CByteBuffer buf;

#ifdef HAVE_PRAGMA_PACK
		info.extratag = 0x000a;
		info.totalsize = sizeof(TNTFSExtraInfo);
		info.reserved0 = 0;
		info.data.attrtag = 0x0001;
		info.data.attrsize = sizeof(TNTFSExtraInfo::__attrdata);
		info.data.mtime = vLastWriteTime;
		info.data.atime = vLastAccessTime;
		info.data.ctime = vCreationTime;

		buf.set_BufferSize(__FILE__LINE__ info.totalsize);
		s_memcpy(buf.get_Buffer(), &info, info.totalsize);
#else
		buf.set_BufferSize(__FILE__LINE__ 36);
		DerefWPointer(_l_ptradd(buf.get_Buffer(), 0)) = 0x000a;
		DerefWPointer(_l_ptradd(buf.get_Buffer(), 2)) = 36;
		DerefWPointer(_l_ptradd(buf.get_Buffer(), 8)) = 0x0001;
		DerefWPointer(_l_ptradd(buf.get_Buffer(), 10)) = 28;
		DerefQWPointer(_l_ptradd(buf.get_Buffer(), 12)) = vLastWriteTime;
		DerefQWPointer(_l_ptradd(buf.get_Buffer(), 20)) = vLastAccessTime;
		DerefQWPointer(_l_ptradd(buf.get_Buffer(), 28)) = vCreationTime;
#endif
		pLocalFileHeader->set_extra_field(buf);
	}

	Ptr(ZipFileHeader) pFileHeader = OK_NEW_OPERATOR ZipFileHeader;

	pFileHeader->set_filename(tmp);
	pFileHeader->set_compressed_size(0);
	pFileHeader->set_compression_level(ZipCommon::CL_NORMAL);
	pFileHeader->set_compression_method(ZipCommon::CM_DEFLATE);
	pFileHeader->set_crc32(0);
	pFileHeader->set_data_record(false);
	pFileHeader->set_encryption(false);
	pFileHeader->set_last_mod_file(DateTime);
	pFileHeader->set_needed_host_system(ZipCommon::HS_FAT);
	if (!shouldFetchFileSize)
		pFileHeader->set_uncompressed_size(Castdword(vFileSize));
	pFileHeader->set_version_needed_to_extract(20);
	pFileHeader->set_disk_number_start(0);
	pFileHeader->set_external_file_attributes(0);
	pFileHeader->set_file_type(ZipCommon::FT_ASCII);
	pFileHeader->set_made_host_system(ZipCommon::HS_FAT);
	pFileHeader->set_relative_offset_of_local_header(0);
	pFileHeader->set_version_made_by(20);

	CCppObjectPtr<CFileFilterInput> pInput = OK_NEW_OPERATOR CFileFilterInput(fname);
	CCppObjectPtr<CZipArchiveFilterOutput> pOutput = OK_NEW_OPERATOR CZipArchiveFilterOutput(this);
	CCppObjectPtr<CZipCompressFilter> pFilter = OK_NEW_OPERATOR CZipCompressFilter(pInput, pOutput);

	pFilter->open();
	if (shouldFetchFileSize)
	{
		vFileSize = pInput->size();
		pLocalFileHeader->set_uncompressed_size(Castdword(vFileSize));
		pFileHeader->set_uncompressed_size(Castdword(vFileSize));
	}
	pFilter->do_filter();
	pFilter->close();

	pLocalFileHeader->set_crc32(pFilter->get_crc32());
	pFileHeader->set_crc32(pFilter->get_crc32());
	pLocalFileHeader->set_compressed_size(pFilter->get_compressedSize());
	pFileHeader->set_compressed_size(pFilter->get_compressedSize());

	pOutput->get_CacheItem()->localFileHeader = pLocalFileHeader;
	pOutput->get_CacheItem()->fileHeader = pFileHeader;

	TFileCacheItems::Iterator itFileCacheItems = m_FileCache.FindSorted(pOutput->get_CacheItem());

	if (m_FileCache.MatchSorted(itFileCacheItems, pOutput->get_CacheItem()))
		m_FileCache.Remove(itFileCacheItems);
	m_FileCache.InsertSorted(pOutput->get_CacheItem());
	m_modified = true;
}

void CZipArchiveImpl::AddDelete(ConstRef(CFilePath) fname)
{
	CStringBuffer tmp;

#ifdef OK_SYS_WINDOWS
	tmp = fname.get_Directory();
	tmp += fname.get_Filename();
	tmp.ReplaceString(CDirectoryIterator::WinPathSeparatorString(), CDirectoryIterator::UnixPathSeparatorString());
#endif
#ifdef OK_SYS_UNIX
	tmp = fname.get_Path();
#endif

	TFileCacheItems::Iterator itFileCacheItems = m_FileCache.Find<TFileCacheItemEqualFunctor>(CastAnyPtr(TFileCacheItem, &tmp));

	if (itFileCacheItems)
		m_FileCache.Remove(itFileCacheItems);
	m_modified = true;
}

void CZipArchiveImpl::AddClose()
{
	if (!m_modified)
		return;

	Ptr(ZipArchiveInfo) pArchiveInfo = OK_NEW_OPERATOR ZipArchiveInfo();
	TFileCacheItems::Iterator it = m_FileCache.Begin();
	CByteBuffer signature(__FILE__LINE__ 4);
	dword sizeofcentraldir = 0;

	m_file->SetFilePos(0);
	while ( it )
	{
		Ptr(TFileCacheItem) pCacheItem = CastAnyPtr(TFileCacheItem, *it);

		pCacheItem->fileHeader->set_relative_offset_of_local_header(Cast(dword, m_file->GetFilePos()));
		DerefAnyPtr(dword, signature.get_Buffer()) = ZipLocalFileHeader::get_signature();
		m_file->Write(signature);
		pCacheItem->localFileHeader->write(m_file);
		m_file->Write(pCacheItem->fileContent);
		++it;
	}

	pArchiveInfo->set_central_dir_start_offset_in_disk(Cast(dword, m_file->GetFilePos()));
	pArchiveInfo->set_number_entries_in_central_dir_on_this_disk(Cast(word, m_FileCache.Count()));
	pArchiveInfo->set_number_of_disk_of_central_dir_start(0);
	pArchiveInfo->set_number_of_this_disk(0);
	pArchiveInfo->set_size_of_the_central_directory(0);
	pArchiveInfo->set_total_number_entries_in_central_dir(Cast(word, m_FileCache.Count()));

	it = m_FileCache.Begin();
	while ( it )
	{
		Ptr(TFileCacheItem) pCacheItem = CastAnyPtr(TFileCacheItem, *it);

		DerefAnyPtr(dword, signature.get_Buffer()) = ZipFileHeader::get_signature();
		m_file->Write(signature);
		pCacheItem->fileHeader->write(m_file);
		sizeofcentraldir += pCacheItem->fileHeader->get_entry_size();
		++it;
	}

	pArchiveInfo->set_size_of_the_central_directory(sizeofcentraldir);

	DerefAnyPtr(dword, signature.get_Buffer()) = ZipArchiveInfo::get_signature();
	m_file->Write(signature);
	pArchiveInfo->write(m_file);
	m_file->SetSize(m_file->GetFilePos());

	m_Disks.Append(pArchiveInfo);
}
