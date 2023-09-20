add_library(db
        BufferPool.cpp
        Catalog.cpp
        Database.cpp
        HeapFile.cpp
        HeapPage.cpp
        HeapPageId.cpp
        IntField.cpp
        RecordId.cpp
        SeqScan.cpp
        SkeletonFile.cpp
        StringField.cpp
        Tuple.cpp
        TupleDesc.cpp
        Type.cpp
        Utility.cpp
)

target_include_directories(db PUBLIC ../include)
