/*
 * LOCCounter.h
 * Author: Nicolás Pereira
 * Date: 2026-04-03
 * Description: Header file for LOCCounter class that counts lines of code in C++ source files, excluding comments and blank lines, and identifies classes and their methods.
 */

#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>

#ifndef LOCCOUNTER_H
#define LOCCOUNTER_H

struct ClassInfo
{
    std::string name;
    int methodCount = 0;
    int loc = 0;
};

struct FileInfo
{
    std::string path;
    std::vector<ClassInfo> classes;
    int loc = 0;
};

class LOCCounter
{
private:
    bool inBlockComment = false;
    bool inClass = false;
    std::vector<FileInfo> files;

    bool processFile(FileInfo &fileInfo)
    {
        std::ifstream file(fileInfo.path);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file '" << fileInfo.path << "'.\n";
            return false;
        }
        std::string line;
        while (std::getline(file, line))
        {
            if (isBlockComment(line))
                continue;
            if (isCommentLine(line))
                continue;
            if (isEmptyOrWhitespace(line))
                continue;
            if (isIgnoredCharacters(line))
                continue;

            fileInfo.loc++;
            processClasses(fileInfo, line);
        }
        file.close();
        return true;
    };

    bool isBlockComment(const std::string &line)
    {
        if (inBlockComment)
        {
            if (line.find("*/") != std::string::npos)
                inBlockComment = false;
            return true;
        }
        // Find /* in first non-whitespace position
        size_t pos = line.find_first_not_of(" \t\r\n");
        if (pos != std::string::npos && line.substr(pos, 2) == "/*")
        {
            if (line.find("*/") == std::string::npos)
                inBlockComment = true;
            return true;
        }
        return false;
    };

    bool isEmptyOrWhitespace(const std::string &line)
    {
        for (char c : line)
        {
            if (!std::isspace(c))
                return false;
        }
        return true;
    };

    bool isCommentLine(const std::string &line)
    {
        // Start with optional whitespace followed by //
        size_t pos = line.find_first_not_of(" \t\r\n");
        return pos != std::string::npos && line.substr(pos, 2) == "//";
    };

    bool isIgnoredCharacters(const std::string &line)
    {
        for (char c : line)
        {
            if (c != '(' && c != ')' && c != '{' && c != '}' &&
                c != '[' && c != ']' && c != ';' && !std::isspace(c))
            {
                return false;
            }
        }
        return true;
    };

    void processClasses(FileInfo &fileInfo, const std::string &line)
    {
        if (inClass)
        {
            ClassInfo &classInfo = fileInfo.classes.back();
            classInfo.loc++;

            // Regex to match method declarations
            std::regex methodRegex(R"(^\s*(?!(?:if|else|while|for|switch|return|do)\b)(?:(?:static|inline|extern|const|unsigned|signed|long|short|void|int|char|float|double|struct\s+\w+|\w+)\s+)+\**\s*\w+\s*\([^;]*\)\s*(?:\{|$))");
            if (std::regex_search(line, methodRegex))
            {
                classInfo.methodCount++;
            }
            return;
        }
        if (line.find("class ") != std::string::npos)
        {
            ClassInfo classInfo;
            size_t classPos = line.find("class ") + 6;
            size_t nameEnd = line.find_first_of(" :{", classPos);
            classInfo.name = line.substr(classPos, nameEnd - classPos);
            classInfo.methodCount = 0;
            classInfo.loc = 0;
            fileInfo.classes.push_back(classInfo);
            inClass = true;
        }
    };

public:
    bool getSourceFiles(const std::string &rootDir)
    {
        std::queue<std::string> dirQueue;
        dirQueue.push(rootDir);

        while (!dirQueue.empty())
        {
            std::string currentDir = dirQueue.front();
            dirQueue.pop();

            try
            {
                for (const auto &entry : std::filesystem::directory_iterator(currentDir))
                {
                    if (entry.is_directory())
                    {
                        dirQueue.push(entry.path().string());
                        continue;
                    }
                    if (entry.is_regular_file())
                    {
                        std::string ext = entry.path().extension().string();
                        if (ext == ".cpp" || ext == ".h")
                        {
                            FileInfo fileInfo;
                            fileInfo.path = entry.path().string();
                            files.push_back(fileInfo);
                        }
                    }
                }
            }
            catch (...)
            {
                // Ignore permission errors
            }
        }

        return true;
    };

    void processFiles()
    {
        for (auto &fileInfo : files)
        {

            if (!processFile(fileInfo))
            {
                std::cerr << "Error: Could not process file '" << fileInfo.path << "'.\n";
            }
        }
    };

    void printReport()
    {
        std::cout << "\n==================================================\n";
        for (const auto &fileInfo : files)
        {
            std::cout << "File: " << fileInfo.path << "\n";
            std::cout << "Total LOC: " << fileInfo.loc << "\n";
            for (const auto &classInfo : fileInfo.classes)
            {
                std::cout << "  Class: " << classInfo.name << "\n";
                std::cout << "    Methods: " << classInfo.methodCount << "\n";
                std::cout << "    LOC: " << classInfo.loc << "\n";
            }
            std::cout << "==================================================\n";
        }
    }
};

#endif