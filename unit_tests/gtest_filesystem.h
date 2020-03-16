#ifndef GTEST_FILESYSTEM_H
#define GTEST_FILESYSTEM_H

#include <ctime>
#include <ncine/FileSystem.h>
#include <ncine/IFile.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const char *DirectoryName = "TestDir";
const char *DirectoryRenamed = "TestDirRenamed";

const char *FileName = "TestFile";
const char *FileRenamed = "TestFileRenamed";

void touchFile(const char *path)
{
	nctl::UniquePtr<nc::IFile> file = nc::IFile::createFileHandle(path);
	file->open(nc::IFile::OpenMode::WRITE);
	file->close();
}

void fillFile(const char *path, int size)
{
	char buffer[11] = "1234567890";

	nctl::UniquePtr<nc::IFile> file = nc::IFile::createFileHandle(path);
	file->open(nc::IFile::OpenMode::WRITE);
	for (int i = 0; i < size; i++)
		file->write(&buffer[i % 10], 1);
	file->close();
}

}

#endif
