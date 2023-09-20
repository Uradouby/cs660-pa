#include <db/DbFile.h>
#include <db/SkeletonFile.h>
#include <stdexcept>
#include <string>

using namespace db;

Page *SkeletonFile::readPage(const PageId &pid) {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

SkeletonFileIterator SkeletonFile::begin() const {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}

SkeletonFileIterator SkeletonFile::end() const {
    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " not implemented");
}
