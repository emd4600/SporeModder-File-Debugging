#pragma once

#include <Spore\Resource\IPFRecord.h>
#include <Spore\Resource\DatabasePackedFile.h>
#include <Spore\IO\FileStream.h>

using namespace Resource;

/// A special type of IPFRecord that allows us to read files from the disk, instead of a .package file.
class FilePFRecord : public IPFRecord, IO::IStream
{
public:
	FilePFRecord(DBPFItem& itemInfo, const ResourceKey& name, DatabasePackedFile* pParentDBPF);
	FilePFRecord(FilePFRecord* pOther, const ResourceKey& name, DatabasePackedFile* pParentDBPF);

	virtual ~FilePFRecord();
	virtual int AddRef() override;
	virtual int Release() override;

	virtual void SetPath(const char16_t* pPath);

	//// IPFRecord ////

	/* 10h */	virtual ResourceKey&	GetName() override;
	/* 14h */	virtual void			SetName(const ResourceKey& name) override;

	/* 18h */	virtual IStream* GetStream() override;

	/* 1Ch */	virtual DatabasePackedFile* GetParentDBPF() const override;

	/* 20h */	virtual bool Open() override;
	/* 24h */	virtual bool Close() override;
	/* 28h */	virtual int func28h() override;

	//// IStream ////

	/* 0Ch */	virtual uint32_t	GetType() const override;
	/* 10h */	virtual int			GetAccessFlags() const override;
	/* 14h */	virtual FileError	GetState() const override;

	/* 1Ch */	virtual size_type	GetSize() const override;
	/* 20h */	virtual bool		SetSize(size_type size) override;  // does nothing
	/* 24h */	virtual int			GetPosition(PositionType positionType = kPositionTypeBegin) const override;
	/* 28h */	virtual bool		SetPosition(int distance, PositionType positionType = kPositionTypeBegin) override;
	/* 2Ch */	virtual int			GetAvailable() const override;

	/* 30h */	virtual int		Read(void* pData, size_t nSize) override;
	/* 34h */	virtual bool	Flush() override;  // does nothing
	/* 38h */	virtual int		Write(const void* pData, size_t nSize) override;  // does nothing

	static const uint32_t kType = 0x4CF999A2;  // Hash::FNV("FilePFRecord")

protected:
	/// The disk file stream to read data from the disk.
	FileStream mFileStream;
	int mnStreamRefCount;
	bool mbStreamOpened;
	/// We must store the DBPFItem that the record represents.
	DBPFItem mItemInfo;
};

