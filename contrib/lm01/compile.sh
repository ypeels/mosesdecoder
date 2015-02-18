g++ -I../../ -I../../boost/include -L../../boost/lib64 -o extract extract.cpp ../../phrase-extract/InputFileStream.cpp ../../phrase-extract/OutputFileStream.cpp -lz -lboost_iostreams

g++ -I../../ -I../../boost/include -L../../boost/lib64 -o extract-nonfactored extract-nonfactored.cpp ../../phrase-extract/InputFileStream.cpp ../../phrase-extract/OutputFileStream.cpp -lz -lboost_iostreams

g++ -I../../ -I../../boost/include -L../../boost/lib64 -o score score.cpp ../../phrase-extract/InputFileStream.cpp ../../phrase-extract/OutputFileStream.cpp -lz -lboost_iostreams

