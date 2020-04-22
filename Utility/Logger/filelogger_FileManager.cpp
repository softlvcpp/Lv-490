#include "Other/pch.h"
#include "Filelogger/filelogger.h"

#include <filesystem>
#include <cassert>
#include <iostream>

namespace fs = std::filesystem;

constexpr wchar_t dateTimeWRegex[] = LR"(_[0-9]{8} ([0-9]{2}-){2}[0-9]{2}(_\d*?){0,1})";
constexpr char noCounterRegex[] = R"(_[0-9]{8} ([0-9]{2}-){2}[0-9]{2})";
constexpr auto parentFolderRegex = R"(^((\.\/)|(\.\.\/))*?$)";
constexpr auto dateTimeTokenString = "%d%m%Y %H-%M-%S";
constexpr auto datePaddingSize = sizeof("ddmmyyyy");

namespace filelog
{

    FileLoggerFilestreamManager::FileLoggerFilestreamManager(FileLogger& logger) : mlogger{ logger }
    {
        currentFileSize = 0;
        std::ios_base::iostate exceptionMask = outputStream.exceptions() | std::ios::failbit;
        outputStream.exceptions(exceptionMask);
    }

    FileLoggerFilestreamManager::FileLoggerFilestreamManager(FileLoggerFilestreamManager&& source) noexcept
        : mlogger{ source.mlogger.get() }
    {
        moveToThis(std::move(source));
    }

    FileLoggerFilestreamManager& FileLoggerFilestreamManager::operator=(FileLoggerFilestreamManager&& right) noexcept
    {
        if (this != &right)
            moveToThis(std::move(right));
        return *this;
    }

    void FileLoggerFilestreamManager::moveToThis(FileLoggerFilestreamManager&& source)
    {
        this->currentFileSize = source.currentFileSize;
        this->maxFileSize = source.maxFileSize;
        this->outputStream = std::move(source.outputStream);

        this->currentFileName = std::move(source.currentFileName);
        this->filenameInfo = std::move(source.filenameInfo);
        this->filenameTemplateRegex = std::move(source.filenameTemplateRegex);
        this->filenameTNoCounterRegex = std::move(source.filenameTNoCounterRegex);
    }

    stream_base& FileLoggerFilestreamManager::getStream()
    {
        return outputStream;
    }

    bool FileLoggerFilestreamManager::getFileIsOpen()
    {
        return outputStream.is_open();
    }

    const std::string& FileLoggerFilestreamManager::getCurrentFilename() const
    {
        return currentFileName;
    }

    void FileLoggerFilestreamManager::setMaxFileSize(size_t value)
    {
        maxFileSize = value;
    }

    void FileLoggerFilestreamManager::setFilename(const char* newFileName)
    {
        currentFileName = newFileName;
    }


    bool FileLoggerFilestreamManager::checkedRotate(size_t bytesToWrite)
    {
        this->currentFileSize += bytesToWrite;
        if (currentFileSize >= maxFileSize)
        {
            outputStream << std::flush;
            outputStream.close();               /* Uses current file name to create a new one if time suffix is identical */
            if (!openFile(getCurrentFilename().c_str(), false, bytesToWrite))
                return false;

            currentFileSize = fs::file_size(getCurrentFilename()) + bytesToWrite;
        }
        return true;
    }

    template <typename TChar>
    static void replaceAll(std::basic_string<TChar>& target, std::basic_string<TChar> from, std::basic_string<TChar> to)
    {
        size_t start_pos = 0;
        while ((start_pos = target.find(from, start_pos)) != std::string::npos) {
            target.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    bool FileLoggerFilestreamManager::setupFilenameTemplate(const std::string& temp)
    {
        fs::path p{ temp };
        auto folder = p.parent_path();

        static std::regex parentFolder{ parentFolderRegex };

        std::string pathNormalized = folder.string();
        replaceAll<char>(pathNormalized, R"(\)", R"(/)");

        if (!p.has_extension())
            return false;
        if (!fs::exists(pathNormalized))
        {
            std::error_code mkdirCode;
            fs::create_directories(pathNormalized, mkdirCode);
            if (mkdirCode)
                return false;
        }        

        filenameInfo.asString.folder = (folder.empty()) ? "./" : folder.string();       // Set containing folder info
        filenameInfo.asWstring.folder = (folder.empty()) ? L"./" : folder.c_str();

        filenameInfo.asString.fullPath = p.string();                                    // Set full path info
        filenameInfo.asWstring.fullPath = p.c_str();

        auto filename = p.filename();
        filenameInfo.asString.extension = filename.extension().string();                // Set extension info
        filenameInfo.asWstring.extension = filename.extension().c_str();

        filename.replace_extension();
        filenameInfo.asString.fileNameWithNoExt = filename.string();                    // Set filename without extension info
        filenameInfo.asWstring.fileNameWithNoExt = filename.c_str();

        p.replace_extension();
        filenameInfo.asString.pathWithNoExt = p.string();                               // Set path with no extension info
        filenameInfo.asWstring.pathWithNoExt = p.c_str();

        using namespace std::string_literals;

        auto pathWithNoExtEsc = filenameInfo.asWstring.pathWithNoExt;
        auto pathWithNeExtEscChar = filenameInfo.asString.pathWithNoExt;

        // Prepare path for regex syntax
        replaceAll<wchar_t>(pathWithNoExtEsc, LR"(\)", LR"(\\)");
        replaceAll<wchar_t>(pathWithNoExtEsc, L"/", LR"(\/)");
        replaceAll<wchar_t>(pathWithNoExtEsc, L".", LR"(\.)");

        replaceAll<char>(pathWithNeExtEscChar, R"(\)", R"(\\)");
        replaceAll<char>(pathWithNeExtEscChar, "/", R"(\/)");
        replaceAll<char>(pathWithNeExtEscChar, ".", R"(\.)");

        filenameTNoCounterRegex = std::regex{ "^"s + pathWithNeExtEscChar + noCounterRegex + "\\" + filenameInfo.asString.extension + "$" };
        filenameTemplateRegex = std::wregex{ L"^"s + pathWithNoExtEsc + dateTimeWRegex + L"\\" + filenameInfo.asWstring.extension + L"$" };

        return true;
    }

    bool FileLoggerFilestreamManager::getLastModifiedMatchesTemplate(std::string& path)
    {
        auto lastWrTime = fs::file_time_type();
        fs::path lastWrFile = "";

        for (auto dirIt : fs::directory_iterator{ filenameInfo.asWstring.folder })  // Search for last writen file
        {
            auto wrTime = dirIt.last_write_time();
            if (fileMatchesTemplate(dirIt.path().c_str()) && (wrTime > lastWrTime))
            {
                lastWrFile = dirIt.path();
                lastWrTime = wrTime;
            }
        }

        if (lastWrFile.empty())
            return false;
        else
        {
            std::wstring widePath = lastWrFile;
            path = std::string{ widePath.begin(), widePath.end() };
            return true;
        }
    }

    /* Checks if file is bigger that maxFileSize, taking into how much space left is needed */
    bool FileLoggerFilestreamManager::fileFull(const char* fileName, size_t shouldHaveLeft)
    {
        std::error_code c;
        const auto fileSize = fs::file_size(fileName, c);
        return !c && ((fileSize + shouldHaveLeft) > maxFileSize);
    }

    /* Checks file matches path/name_DATE TIME_COUNT.template */
    bool FileLoggerFilestreamManager::fileMatchesTemplate(const char_type* filename)
    {
        return std::regex_match(filename, filenameTemplateRegex);
    }

    /* Filenames can be created in a span of same second. In such case i decided to add counter to it's name */
    std::string FileLoggerFilestreamManager::filenameRegexCheck()
    {
        std::string newFileName = createNarrowFileName();
        if (!currentFileName.empty() && (currentFileName != filenameInfo.asString.fullPath))
        {
            if (!std::regex_match(currentFileName, filenameTNoCounterRegex))
            {
                // Filename has format log_DATE-TIME_COUNT.extension
                newFileName = createNarrowFileName();

                auto lastUnderInNew = newFileName.find_last_of('_');        // _DATE-TIME date-time check start
                auto lastDotInNew = newFileName.find_last_of('.');          // .extension insertion point

                auto lastUnderInOld = currentFileName.find_last_of('_');    // _COUNT count substring start
                auto lastDotInOld = currentFileName.find_last_of('.');      // COUNT.  count substring end

                bool sameTime = true;
                size_t i = lastUnderInNew + datePaddingSize;
                size_t maxInd = lastDotInNew;
                for (; i < lastDotInNew; ++i)
                    if (newFileName[i] != currentFileName[i])
                    {
                        sameTime = false;
                        break;
                    }
                if (sameTime)
                {
                    newFileName = currentFileName;
                    size_t n = std::stoull(newFileName.substr(lastUnderInOld + 1, lastDotInOld - lastUnderInOld));
                    ++n;
                    // Increase counter
                    std::string replacement = std::to_string(n);
                    newFileName.replace(lastUnderInOld + 1, lastDotInOld - lastUnderInOld - 1, replacement.c_str());
                }
            }
            else
            {
                // Filename has format log_DATE-TIME.extension
                newFileName = createNarrowFileName();
                if (newFileName == currentFileName)
                {
                    // Add counter
                    auto lastDot = newFileName.find_last_of('.');
                    newFileName.replace(lastDot, 1, "_1.");
                }
            }
        }

        return newFileName;
    }

    std::string FileLoggerFilestreamManager::createNarrowFileName()
    {
        time_t timeEnc = time(0);
        tm timeStruct;
        log490::Utils::getUTCTime(timeStruct, timeEnc);

        char timeSuffix[DATETIME_FORMAT_CHAR_COUNT + 1];
        strftime(timeSuffix, DATETIME_FORMAT_CHAR_COUNT, dateTimeTokenString, &timeStruct);

        using namespace std::string_literals;
        return filenameInfo.asString.pathWithNoExt + "_"        // path/name_DATE-TIME.ext
            + timeSuffix
            + filenameInfo.asString.extension;
    }

    bool FileLoggerFilestreamManager::openFile(const char* fileName, bool useExact, size_t neccessarySpace, std::ios::openmode mode)
    {
        if (useExact)
        {
            if (fs::exists(fileName))
            {
                std::string lastModified;
                if (getLastModifiedMatchesTemplate(lastModified))                       // Try to find last modified file
                    setFilename((fileFull(lastModified.c_str(), neccessarySpace)) ?     // If found then continue it(path/name_DATE-TIME_COUNT.extension)
                        filenameRegexCheck().c_str() :
                        lastModified.c_str());
                else
                    setFilename((fileFull(fileName, neccessarySpace)) ?                 // Else write in file of format path/name.extension
                        filenameRegexCheck().c_str() :
                        fileName);
            }
            else
                setFilename(fileName);
        }
        else
            setFilename(filenameRegexCheck().c_str());                                  // Create new file

        outputStream.open(currentFileName, mode);
        if (outputStream.is_open())
        {
            currentFileSize += fs::file_size(getCurrentFilename());
            return true;
        }
        else
            return false;
    }

    void FileLoggerFilestreamManager::closeFile()
    {
        if (outputStream.is_open())
        {
            outputStream << std::flush;
            outputStream.close();
        }
    }

}