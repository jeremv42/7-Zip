The whole purpose of this git is just to create some plugins to add new archive support in 7-Zip.

# Plugins
## Build requirements
I'm not sure of everything needed to build 7zip sources. For now, I identified:
- Visual Studio 2022 with C++ support and nmake

You can check the DOC/readme.txt file for more informations.

## How to create a plugin documentation
The development documentation of 7-Zip seems to be lacking, I only found some subject on SourceForge.net:
- https://sourceforge.net/p/sevenzip/discussion/45797/thread/1420eda5/
- https://sourceforge.net/p/sevenzip/discussion/45797/thread/6db98beb/
- https://sourceforge.net/p/sevenzip/discussion/45797/thread/fe7ff749/

So I will try to update this README with what I understand.

### Init an archive plugin
- Clone this git (or take the sources of 7-Zip)
- Create a folder for your plugin in `CPP/7-Zip/Archive`
- Add a `makefile` with (maybe some obj files are useless):
```makefile
PROG = <NAME>.dll
DEF_FILE = ../Archive.def
CFLAGS = $(CFLAGS) \
  -DEXTERNAL_CODECS \

AR_OBJS = \
  $O\ArchiveExports.obj \
  $O\DllExports.obj \

7Z_OBJS = \
  $O\<NAME>Handler.obj \

COMMON_OBJS = \
  $O\IntToString.obj \
  $O\MyString.obj \
  $O\StringConvert.obj \
  $O\StringToInt.obj \
  $O\MyVector.obj \
  $O\Wildcard.obj \

WIN_OBJS = \
  $O\DLL.obj \
  $O\FileDir.obj \
  $O\FileFind.obj \
  $O\FileIO.obj \
  $O\FileName.obj \
  $O\PropVariant.obj \
  $O\Synchronization.obj \
  $O\System.obj \

7ZIP_COMMON_OBJS = \
  $O\CreateCoder.obj \
  $O\InBuffer.obj \
  $O\InOutTempBuffer.obj \
  $O\FilterCoder.obj \
  $O\LimitedStreams.obj \
  $O\LockedStream.obj \
  $O\MethodId.obj \
  $O\MethodProps.obj \
  $O\OutBuffer.obj \
  $O\ProgressUtils.obj \
  $O\PropId.obj \
  $O\StreamBinder.obj \
  $O\StreamObjects.obj \
  $O\StreamUtils.obj \
  $O\VirtThread.obj \

COMPRESS_OBJS = \
  $O\CopyCoder.obj \

AR_COMMON_OBJS = \
  $O\CoderMixer2.obj \
  $O\HandlerOut.obj \
  $O\InStreamWithCRC.obj \
  $O\ItemNameUtils.obj \
  $O\MultiStream.obj \
  $O\OutStreamWithCRC.obj \
  $O\ParseProperties.obj \

C_OBJS = \
  $O\Alloc.obj \
  $O\CpuArch.obj \
  $O\Threads.obj \

!include "../../7zip.mak"
```
- Change `<NAME>` in the makefile
- 7-Zip makefiles use precompiled header so create StdAfx.h and StdAfx.cpp files
- In your handler file, you will be able to register the functions called by 7z.exe
  - The handler.h/cpp file need to define a `CHandler` class which implements `CMyUnknownImp`
  - In order to add the new fileformat in 7-Zip, `CHandler` also need to implements `IInArchive`
  - You should have a basic class like this one:
    ```cpp
    class CHandler: public IInArchive, public CMyUnknownImp
    {
    public:
        MY_UNKNOWN_IMP1(IInArchive)
        INTERFACE_IInArchive(;)
    };
    ```
    The 7-Zip codebase heavily use macros, `MY_UNKNOWN_IMP1` will implements COM-like methods (Ref management: AddRef, Release, etc) and `INTERFACE_IInArchive` will declare many things:
    ```cpp
    //// 2 methods which can be generated by some macros:
    static const Byte kArcProps[] = { kpidName, kpidCRC, }; // list the archive metadata by your format
    IMP_IInArchive_ArcProps
    static const Byte kProps[] = { kpidPath, kpidSize, kpidPackSize }; // list the metadata for each item in your archive format
    IMP_IInArchive_Props
    //// Methods
    // Every method returns a HRESULT with S_OK when it's ok, S_FALSE in other cases (maybe some other values in specific situation)
    // Since methods can't return the value directly, it's often returned by an argument pointer

    // Get the value of one archive metadata PROPID equals one of kpid, PROPVARIANT* is used to return the value
    STDMETHODIMP CHandler::GetArchiveProperty(PROPID propID, PROPVARIANT *value)
    // Open the archive, maxCheckStartPosition and openArchiveCallback can be ignored
    STDMETHODIMP CHandler::Open(IInStream *stream, const UInt64 *maxCheckStartPosition, IArchiveOpenCallback * openArchiveCallback)
    // Close the archive, you can release retained resources here (like the input stream from Open)
    STDMETHODIMP CHandler::Close()
    STDMETHODIMP CHandler::GetNumberOfItems(UInt32 *numItems)
    // Get the value of one item's metadata property, the index must go from 0 to NumberOfItems-1
    STDMETHODIMP CHandler::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
    // Extract one or more items:
    //   If numItems == -1: extract everything
    //   In other case, indices will contains each item index to extract
    // In order to extract an item, you will need to do some call on extractCallback:
    //   1. OPTIONAL extractCallback->SetTotal(UInt64 totalExtractedSizeInBytes)
    //   2. MUST     extractCallback->GetStream(UInt32 itemIndex, ISequentialOutStream **outStream, Int32 askExtractMode)
    //      outStream will give you the stream to write with the content of your file
    //   3. MUST     extractCallback->SetOperationResult(HRESULT result)
    //   4. At the end of extraction, you need to release the outStream
    STDMETHODIMP CHandler::Extract(const UInt32 *indices, UInt32 numItems, Int32 extractMode, IArchiveExtractCallback *extractCallback)
    ```

## MPAK (Dark Age of Camelot packages: mpk, npk)
You can build the dll in `CPP/7zip/Archive/Mpk`, just run nmake from a Visual Studio command prompt.

# Thanks

It's based on git source of https://github.com/mcmilk/7-Zip which itself is based on work of https://sourceforge.net/p/sevenzip.

Thanks everyone for 7-Zip!

## See Also

[7-Zip Homepage](https://www.7-zip.org/) \
[7-Zip @ sf.net](https://sourceforge.net/p/sevenzip/) \
[7-Zip mcmilk's GitHub](https://github.com/mcmilk/7-Zip)
